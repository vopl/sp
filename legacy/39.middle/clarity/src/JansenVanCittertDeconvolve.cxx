/* 
 * Clarity is Copyright 2008 Center for Integrated Systems for Microscopy, 
 * Copyright 2008 University of North Carolina at Chapel Hill.
 *
 * Clarity is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Public License as published by the Free Software 
 * Foundation; either version 2 of the License, or (at your option) any 
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA. You can also find 
 * the GPL on the GNU web site (http://www.gnu.org/copyleft/gpl.html).
 *
 * File name: JansenVanCittertDeconvolve.cxx
 * Author: Cory Quammen <cquammen@cs.unc.edu>
 */


#include "Clarity.h"

#include <iostream>
#include <stdlib.h>
#include <omp.h>

#include "Convolve.h"
#include "FFT.h"
#include "Memory.h"

extern bool g_CUDACapable;

#ifdef TIME
#include <iostream>
#include "Stopwatch.h"

static Stopwatch totalTimer("JansenVanCittert filter (total time)");
static Stopwatch transferTimer("JansenVanCittert filter (transfer time)");
#endif

double
Clarity_GetImageMax(
   double *inImage, int numVoxels) {

   double max = inImage[0];
   double val = 0.0f;
   int tid = 0;
   int numThreads = 0;

#pragma omp parallel
   {
     numThreads = omp_get_num_threads();
   }
   double *threadMax = new double[numThreads];

#pragma omp parallel private(tid, val)
   {
      tid = omp_get_thread_num();
      threadMax[tid] = max;

#pragma omp for
      for (int i = 0; i < numVoxels; i++) {
         val = inImage[i];
         if (val > threadMax[tid]) threadMax[tid] = val;
      }
   }

   for (int i = 0; i < numThreads; i++) {
     if (threadMax[i] > max) max = threadMax[i];
   }
   delete[] threadMax;

   return max;
}


void
JansenVanCittertDeconvolveKernelCPU(
   int nx, int ny, int nz, double* in, double inMax, double invMaxSq,
   double* i_k, double* o_k, double* i_kNext) {

   int numVoxels = nx*ny*nz;

#pragma omp parallel for
   for (int j = 0; j < numVoxels; j++) {
      double diff = o_k[j] - inMax;
      double gamma = 1.0f - ((diff * diff) * invMaxSq);
      double val = i_k[j] + (gamma * (in[j] - o_k[j]));
      if (val < 0.0f) val = 0.0f;
      i_kNext[j] = val;
   }

}


ClarityResult_t 
Clarity_JansenVanCittertDeconvolveCPU(
   double* outImage, double* inImage, double* psfImage, 
   int nx, int ny, int nz, double max, unsigned iterations) {
   
   ClarityResult_t result = CLARITY_SUCCESS;
   double A = 0.5f * max;
   double invASq = 1.0f / (A * A);

   // Fourier transform of PSF.
   double* psfFT = NULL;
   result = Clarity_Complex_Malloc((void**) &psfFT, sizeof(double), nx, ny, nz);
   if (result != CLARITY_SUCCESS) {
      return result;
   }
   result = Clarity_FFT_R2C_double(nx, ny, nz, psfImage, psfFT);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psfFT);
      return result;
   }

   // Set up the array holding the current guess.
   double* iPtr = NULL;
   result = Clarity_Real_Malloc((void**)&iPtr, sizeof(double), 
      nx, ny, nz);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psfFT);
      return CLARITY_OUT_OF_MEMORY;
   }

   // Storage for convolution of current guess with the PSF.
   double* oPtr = NULL;
   result = Clarity_Real_Malloc((void**) &oPtr, sizeof(double), 
      nx, ny, nz);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psfFT); Clarity_Free(iPtr);
      return result;
   }

   // First iteration.
   result = Clarity_Convolve_OTF(nx, ny, nz, inImage, psfFT, oPtr);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psfFT); Clarity_Free(oPtr); Clarity_Free(iPtr);
      return result;
   }
   if (iterations > 1) {
      JansenVanCittertDeconvolveKernelCPU(nx, ny, nz, inImage, 
         A, invASq, inImage, oPtr, iPtr);
   } else {
      JansenVanCittertDeconvolveKernelCPU(nx, ny, nz, inImage, 
         A, invASq, inImage, oPtr, outImage);
   }

   // Iterate
   for (unsigned k = 1; k < iterations; k++) {
      result = Clarity_Convolve_OTF(nx, ny, nz, iPtr, psfFT, oPtr);
      if (result != CLARITY_SUCCESS) break;

      if (k < iterations - 1) {
         JansenVanCittertDeconvolveKernelCPU(nx, ny, nz, inImage,
            A, invASq, iPtr, oPtr, iPtr);
      } else {
         JansenVanCittertDeconvolveKernelCPU(nx, ny, nz, inImage,
            A, invASq, iPtr, oPtr, outImage);
      }
   }

   Clarity_Free(psfFT); Clarity_Free(oPtr); Clarity_Free(iPtr);

   return result;
}

#ifdef BUILD_WITH_CUDA

#include "JansenVanCittertDeconvolveGPU.h"


ClarityResult_t 
Clarity_JansenVanCittertDeconvolveGPU(
   double* outImage, double* inImage, double* psfImage, 
   int nx, int ny, int nz, double max, unsigned iterations) {

   ClarityResult_t result = CLARITY_SUCCESS;
   double A = 0.5f * max;
   double invASq = 1.0f / (A * A);

   // Copy over PSF and take its Fourier transform.
   double* psf = NULL;
   result = Clarity_Real_MallocCopy((void**) &psf, sizeof(double), 
      nx, ny, nz, psfImage);
   if (result != CLARITY_SUCCESS) {
      return result;
   }
   double* psfFT = NULL;
   result = Clarity_Complex_Malloc((void**) &psfFT, sizeof(double), 
      nx, ny, nz);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psf);
      return result;
   }
   result = Clarity_FFT_R2C_double(nx, ny, nz, psf, psfFT);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psf); Clarity_Free(psfFT);
      return result;
   }
   Clarity_Free(psf);

   // Copy over image.
   double* in = NULL;
   result = Clarity_Real_MallocCopy((void**) &in, sizeof(double), 
      nx, ny, nz, inImage);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psfFT);
      return result;
   }

   // Set up the array holding the current guess.
   double* iPtr = NULL;
   result = Clarity_Real_Malloc((void**)&iPtr, sizeof(double), 
      nx, ny, nz);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psfFT); Clarity_Free(in);
      return result;
   }

   // Storage for convolution of current guess with the PSF.
   double* oPtr = NULL;
   result = Clarity_Real_Malloc((void**)&oPtr, sizeof(double), 
      nx, ny, nz);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psfFT); Clarity_Free(in); Clarity_Free(iPtr);
      return result;
   }

   // First iteration.
   result = Clarity_Convolve_OTF(nx, ny, nz, in, psfFT, oPtr);
   if (result != CLARITY_SUCCESS) {
      Clarity_Free(psfFT); Clarity_Free(in);
      Clarity_Free(iPtr); Clarity_Free(oPtr);
      return result;
   }
   JansenVanCittertDeconvolveKernelGPU(nx, ny, nz, in, A, invASq, 
      in, oPtr, iPtr);

   // Iterate
   for (unsigned k = 1; k < iterations; k++) {
      result = Clarity_Convolve_OTF(nx, ny, nz, iPtr, psfFT, oPtr);
      if (result != CLARITY_SUCCESS) {
         break;
      }
      JansenVanCittertDeconvolveKernelGPU(nx, ny, nz, in, A, 
         invASq, iPtr, oPtr, iPtr);
   }

   // Copy result from device.
   result = Clarity_CopyFromDevice(nx, ny, nz, sizeof(double), 
      outImage, iPtr);

   Clarity_Free(psfFT); Clarity_Free(in);
   Clarity_Free(iPtr); Clarity_Free(oPtr);

   return result;
}

#endif // BUILD_WITH_CUDA

ClarityResult_t 
Clarity_JansenVanCittertDeconvolve(double* inImage, Clarity_Dim3 imageDim,
				   double* kernelImage, Clarity_Dim3 kernelDim,
				   double* outImage, int iterations) {
  ClarityResult_t result = CLARITY_SUCCESS;

#ifdef TIME
  totalTimer.Start();
#endif

  // Find maximum value in the input image.
  double max = Clarity_GetImageMax(inImage, imageDim.x*imageDim.y*imageDim.z);

  // Compute working dimensions. The working dimensions are the sum of the
  // image and kernel dimensions. This handles the cyclic nature of convolution
  // using multiplication in the Fourier domain.
  Clarity_Dim3 workDim;
  workDim.x = imageDim.x + kernelDim.x;
  workDim.y = imageDim.y + kernelDim.y;
  workDim.z = imageDim.z + kernelDim.z;
  int workVoxels = workDim.x*workDim.y*workDim.z;

  // Pad the input image to the working dimensions
  double *inImagePad = (double *) malloc(sizeof(double)*workVoxels);
  int zeroShift[] = {0, 0, 0};
  double fillValue = 0.0f;
  Clarity_ImagePadSpatialShift(inImagePad, workDim, inImage, imageDim,
			       zeroShift, fillValue);

  // Pad the kernel to the working dimensions and shift it so that the
  // center of the kernel is at the origin.
  double *kernelImagePad = (double *) malloc(sizeof(double)*workVoxels);
  int kernelShift[] = {-kernelDim.x/2, -kernelDim.y/2, -kernelDim.z/2};
  Clarity_ImagePadSpatialShift(kernelImagePad, workDim, kernelImage, kernelDim,
			       kernelShift, fillValue);

  // Allocate output array
  double *outImagePad = (double *) malloc(sizeof(double)*workVoxels);
  
#ifdef BUILD_WITH_CUDA
  if (g_CUDACapable) {
    result = Clarity_JansenVanCittertDeconvolveGPU(outImagePad, 
      inImagePad, kernelImagePad, workDim.x, workDim.y, workDim.z,
      max, iterations);
  } else
#endif // BUILD_WITH_CUDA
  {
    result = Clarity_JansenVanCittertDeconvolveCPU(outImagePad, 
      inImagePad, kernelImagePad, workDim.x, workDim.y, workDim.z,
      max, iterations);
  }

  // Clip the image to the original dimensions.
  Clarity_ImageClip(outImage, imageDim, outImagePad, workDim);

  // Free up memory.
  free(inImagePad);
  free(kernelImagePad);
  free(outImagePad);

#ifdef TIME
  totalTimer.Stop();
  std::cout << totalTimer << std::endl;
  std::cout << transferTimer << std::endl;
  totalTimer.Reset();
  transferTimer.Reset();
#endif

  return result;
}
