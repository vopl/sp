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
 * File name: IDivergenceDeconvolve.cxx
 * Author: Cory Quammen <cquammen@cs.unc.edu>
 */


#include "Clarity.h"

#include "math.h"
#include "omp.h"

static double IDIVFunctional(
   double* g, double* gHat, double* sHat, double alpha, 
   int nx, int ny, int nz) {

   double sum = 0.0f;
   int numVoxels = nx*ny*nz;

#pragma omp parallel for reduction(+:sum)
   for (int i = 0; i < numVoxels; i++) {
      sum += (g[i]*log(g[i]/gHat[i])) + gHat[i] - g[i] + (alpha*sHat[i]*sHat[i]);
   }
   return sum;
}


static void IDIVGradient(
   double* g, double* gHat, double* sHat, double* flippedPSFtf, 
   double alpha, double* gradient) {


}



ClarityResult_t
Clarity_IDivergenceDeconvolve(
  double* inImage, Clarity_Dim3 imageDim,
  double* kernelImage, Clarity_Dim3 kernelDim,
  double* outImage) {

   // Temporary code to produce something.
   int size = imageDim.x*imageDim.y*imageDim.z;
   for (int i = 0; i < size; i++) {
      outImage[i] = 0.0f;
   }

   return CLARITY_SUCCESS;
}
