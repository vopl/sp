#include "stdafx.h"
#include "hel/responseModel2.h"
#include "hel/math.h"
//#include "mpfrc++/mpreal.h"
//#include "hel/fft.h"
#include "levmar-2.5/levmar.h"
//#include "clarity/include/Clarity.h"
//#include <fftw3.h>
#include <complex>

extern "C"
{
	int dAx_eq_b_Chol(double *A, double *B, double *x, int m);
	int dAx_eq_b_QR(double *A, double *B, double *x, int m);
	int dAx_eq_b_QRLS(double *A, double *B, double *x, int m, int n);
	int dAx_eq_b_Chol(double *A, double *B, double *x, int m);
	int dAx_eq_b_LU(double *A, double *B, double *x, int m);
	int dAx_eq_b_SVD(double *A, double *B, double *x, int m);



	int dgesdd_(char *jobz, long *m, long *n, double *
		a, long *lda, double *s, double *u, long *ldu, 
		double *vt, long *ldvt, double *work, long *lwork, 
		long *iwork, long *info);

	int min(int a, int b)
	{
		if(a<b) return a;
		return b;
	}
	int max(int a, int b)
	{
		if(a>b) return a;
		return b;
	}
	int dAx_eq_b_SVD2(double *A, double *B, double *x, long m, long n)
	{
		//column major a
		{
			double *a = (double *)malloc(sizeof(double)*m*n);
			memcpy(a, A, sizeof(double)*m*n);
			for(long row=0; row<m; row++)
				for(long col=0; col<n; col++)
					A[col*m+row] = a[row*n+col];
			free(a);
		}

		double *s = (double *)malloc(sizeof(double)*m);
		double *u = (double *)malloc(sizeof(double)*m*m);
		double *vt = (double *)malloc(sizeof(double)*n*n);

		long lwork = 3*min(m,n)*min(m,n) + max(max(m,n),4*min(m,n)*min(m,n)+4*min(m,n));
		double *work = (double *)malloc(sizeof(double)*lwork);
		long *iwork = (long *)malloc(sizeof(long)*8*min(m,n));

		long info;

		dgesdd_(
			"A", 
			(long *)&m, 
			(long *)&n, 
			A, 
			(long *)&m, 
			s, 
			u, 
			(long *)&m, 
			vt, 
			(long *)&n,
			work, 
			(long *)&lwork, 
			iwork, 
			&info);

		if(NULL == B || NULL == x && m==n)
		{
			std::ofstream su("su");
			std::ofstream ss("ss");
			std::ofstream sv("sv");

			for(int i(0); i<n; i++)
			{
				for(int j(0); j<n; j++)
				{
					sv<<vt[i*n+j]<<"\t";
					su<<u[i*n+j]<<"\t";
				}
				sv<<std::endl;
				su<<std::endl;
				ss<<s[i]<<std::endl;
			}
			exit(0);
		}

		if(0 == info)
		{
			/* compute machine epsilon */
			double eps, aux;
			for(eps=1.0; aux=eps+1.0, aux-1.0>0.0; eps*=0.5);
			eps*=2.0;

			/* compute the pseudoinverse in a */
			memset(A, 0, sizeof(double)*m*n); /* initialize to zero */

			int rank, i, j;
			double one_over_denom, thresh;

			for(rank=0, thresh=eps*s[0]; rank<m && s[rank]>thresh; rank++)
			{
				one_over_denom=1.0/s[rank];

				for(j=0; j<m; j++)
					for(i=0; i<n; i++)
						A[i*m+j]+=vt[rank+i*n]*u[j+rank*m]*one_over_denom;
			}

			/* compute A^+ b in x */
			double sum;
			for(i=0; i<n; i++)
			{
				for(j=0, sum=0.0; j<m; j++)
				{
					sum+=A[i*m+j]*B[j];
				}
				x[i]=sum;
			}
		}

		free(s);
		free(u);
		free(vt);
		free(work);
		free(iwork);

		return 0 == info;
	}

};








//////////////////////////////////////////////////////////////////////////
const char *DeconvolutionRL(double *source, const double *response,
									   int ssize, int numberIterations,
									   int numberRepetitions, double boost ) 
{   

	/////////////////////////////////////////////////////////////////////////////
	//   ONE-DIMENSIONAL DECONVOLUTION FUNCTION                                //
	//   This function calculates deconvolution from source spectrum           //
	//   according to response spectrum using Richardson-Lucy algorithm        //
	//   The result is placed in the vector pointed by source pointer.         //
	//                                                                         //
	//   Function parameters:                                                  //
	//   source:  pointer to the vector of source spectrum                     //
	//   response:     pointer to the vector of response spectrum              //
	//   ssize:    length of source and response spectra                       //
	//   numberIterations, for details we refer to the reference given above   //
	//   numberRepetitions, for repeated boosted deconvolution                 //
	//   boost, boosting coefficient                                           //
	//                                                                         //
	/////////////////////////////////////////////////////////////////////////////

	if (ssize <= 0)
		return "Wrong Parameters";


	if (numberRepetitions <= 0)
		return "Wrong Parameters";


	//   working_space-pointer to the working vector
	//   (its size must be 4*ssize of source spectrum)

	double *working_space = new double[4 * ssize];
	int i, j, k, lindex, posit, lh_gold, repet, kmin, kmax;
	double lda, ldb, ldc, maximum;
	lh_gold = -1;
	posit = 0;
	maximum = 0;


	//read response vector
	for (i = 0; i < ssize; i++) {
		lda = response[i];
		if (lda != 0)
			lh_gold = i + 1;
		working_space[ssize + i] = lda;
		if (lda > maximum) {
			maximum = lda;
			posit = i;
		}
	}
	if (lh_gold == -1)
		return "ZERO RESPONSE VECTOR";


	//read source vector
	for (i = 0; i < ssize; i++)
		working_space[2 * ssize + i] = source[i];


	//initialization of resulting vector
	for (i = 0; i < ssize; i++){
		if (i <= ssize - lh_gold)      
			working_space[i] = 1;


		else
			working_space[i] = 0;   


	}
	//**START OF ITERATIONS**
	for (repet = 0; repet < numberRepetitions; repet++) {
		if (repet != 0) {
			for (i = 0; i < ssize; i++)
				working_space[i] = ::pow(working_space[i], boost);
		}       

		for (lindex = 0; lindex < numberIterations; lindex++) {
			for (i = 0; i <= ssize - lh_gold; i++){
				lda = 0;
				if (working_space[i] > 0){//x[i]
					for (j = i; j < i + lh_gold; j++){
						ldb = working_space[2 * ssize + j];//y[j]
						if (j < ssize){
							if (ldb > 0){//y[j]
								kmax = j;
								if (kmax > lh_gold - 1)
									kmax = lh_gold - 1;
								kmin = j + lh_gold - ssize;
								if (kmin < 0)
									kmin = 0;
								ldc = 0;
								for (k = kmax; k >= kmin; k--){
									ldc += working_space[ssize + k] * working_space[j - k];//h[k]*x[j-k]
								}
								if (ldc > 0)
									ldb = ldb / ldc;

								else
									ldb = 0;
							}
							ldb = ldb * working_space[ssize + j - i];//y[j]*h[j-i]/suma(h[j][k]x[k])
						}
						lda += ldb;
					}
					lda = lda * working_space[i];
				}
				working_space[3 * ssize + i] = lda;
			}
			for (i = 0; i < ssize; i++)
				working_space[i] = working_space[3 * ssize + i];
		}
	}

	//shift resulting spectrum
	for (i = 0; i < ssize; i++) {
		lda = working_space[i];
		j = i + posit;
		j = j % ssize;
		working_space[ssize + j] = lda;
	}

	//write back resulting spectrum
	for (i = 0; i < ssize; i++)
		source[i] = working_space[ssize + i];
	delete[]working_space;
	return 0;
}









//////////////////////////////////////////////////////////////////////////
typedef hel::Complex complex;
#define ERROR(x) abort();

/* 2D matrix shift, kinda brute force, left along each dimension. */
/* Note: Assumes matrix memory is contiguous & in row-major order */
/*
Could benefit from more generalization ...
*/
void
shiftl(complex *arr,int *dims,int ncols,int nrows)
{
	complex *tmp;
	register int big; /* Small is really < big only when */
	register int small; /* ncols|nrows < dims[rows|cols]/2 */
	/* Wrap by modulus */
	ncols %= dims[0];
	nrows %= dims[1];
	/* Shift the rows (quick, but could be memory hungry for large arr) */
	if (nrows > 0)
	{
		small = nrows*dims[0]*sizeof(complex);
		big
			= (dims[1]-nrows)*dims[0]*sizeof(complex);
		tmp = (complex*)malloc(small);
		/* Store the smaller chunk (which would be overwritten) */
		memcpy(tmp,arr,small);
		/* Move the larger chunk to where the smaller chunk was */
		memmove(arr,((char*)arr)+small,big);
		/* And put the smaller chunk in its place */
		memmove(((char*)arr)+big,tmp,small);
		free(tmp);
	}
	/* Shift the cols (slower due to iteration, but uses less memory) */
	if (ncols > 0)
	{
		register int row;
		small = ncols*sizeof(complex);
		big
			= (dims[0]-ncols)*sizeof(complex);
		tmp
			= (complex*)malloc(small);
		for (row=0; row<dims[1]; arr += dims[0],row++)
		{
			memcpy(tmp,arr,small);
			memmove(arr,arr+ncols,big);
			memmove(((char*)arr)+big,tmp,small);
		}
		free(tmp);
	}
}

/* multvec: Multiply two vectors of length abs(n). The multiplication is */
/*
done in the complex field when n>0, and real field when n<0. */
/*
The result will be scaled by a real factor when scale != NULL */
/*
Having slightly longer code (because code for the scaled mults*/
/*
is essentially duplicated) results in fewer operations in the */
/*
common unscaled case, and fewer memory refs in the scaled case*/
void multvec(complex *a, complex *b, complex *result, int n, double *scale)
{
	if (--n>=0)
	{
		/* use a temp var, to permit result array to be same as a or b */
		register double tmp;
		/* perform complex vector multiplication */
		if (!scale)
			do
			{
				tmp = a[n].re()*b[n].re() - a[n].im()*b[n].im();
				result[n].im() = a[n].re()*b[n].im() + a[n].im()*b[n].re();
				result[n].re() = tmp;
			} while (--n > -1);
		else
		{
			register double scal = *scale;
			do
			{
				tmp = (a[n].re()*b[n].re() - a[n].im()*b[n].im())*scal;
				result[n].im() = (a[n].re()*b[n].im() + a[n].im()*b[n].re())*scal;
				result[n].re() = tmp;
			} while (--n > -1);
		}
	}
	else
	{
		/* perform real vector multiplication */
		n = n*-1 - 1;
		if (!scale)
			do { result[n].re() = a[n].re()*b[n].re(); } while (--n > -1);
		else
		{
			register double scal = *scale;
			do { result[n].re() = a[n].re()*b[n].re()*scal; } while (--n > -1);
		}
	}
}


// void rld(
// 		 complex *image, int imsize,
// 		 complex *psf, int psfsize,
// 		 int iters)
// {
// 	double scale=1e-6;//TRATATA что это?
// #define EPSILON 1e-6
// 	int psfdims[2] = {psfsize,1};
// 	int imdims[2] = {imsize,1};
// 
// 	/* Resize the PSF to match the image, for convenient arithmetic */
// 	if (psfsize < imsize)
// 	{
// 		register int i;
// 		register int oldncols = psfdims[0];
// 		register int oldrowsize = oldncols*sizeof(complex);
// 		register int newncols = imdims[0];
// 		complex *tmp = psf;
// 		/* Resize PSF and zero out new pixels */
// 		psf = (complex *)calloc(imsize,sizeof(complex));
// 		for(i=0; i<psfdims[0]; i++)
// 			memmove(psf+i*newncols,tmp+i*oldncols,oldrowsize);
// 		free(tmp);
// 	}
// 	else if (psfsize > imsize)
// 		ERROR("psf cannot be larger than image to be restored!");
// 	/* Shift center of psf to upper left edge */
// 	shiftl(psf,imdims,psfdims[0]/2,psfdims[1]/2);
// 	psfdims[0] = imdims[0]; psfdims[1] = imdims[1];
// 	/* Create FFT plans for transforming the data to/from Fourier space */
// 	hel::TVComplex tmp(imdims[0]*imdims[1]);
// 	fftw_plan fromfspace = fftw_plan_dft(2,imdims, (fftw_complex*)&tmp[0], (fftw_complex*)&tmp[0], FFTW_FORWARD,0);
// 	fftw_plan tofspace = fftw_plan_dft(2,imdims, (fftw_complex*)&tmp[0], (fftw_complex*)&tmp[0], FFTW_BACKWARD,0);
// 	if (!fromfspace || !tofspace)
// 		ERROR("could not compute either fromfspace or tofspace FFT plan");
// 	/* Transform the PSF into Fourier space */
// 	fftw_execute_dft(tofspace,(fftw_complex*)psf, (fftw_complex*)psf);
// 	/* Generate the complex conjugate of the transformed PSF */
// 	complex *psfconj = (complex*)calloc(imsize,sizeof(complex));
// 	int i;
// 	for (i=0; i < imsize; i++)
// 	{
// 		psfconj[i].re() = psf[i].re();
// 		psfconj[i].im() = -1.0 * psf[i].im();
// 	}
// 	/* Finally, before iterating the heart of the RL */
// 	/* algorithm, save a copy of the original image */
// 	complex *orig = (complex*)malloc(imsize*sizeof(complex));
// 	memmove(orig,image,imsize*sizeof(complex));
// 	/* And allocate some scratch space */
// 	complex *scratch = (complex*)malloc(imsize*sizeof(complex));
// 	printf("Beginning RL algorithm, will perform %d iterations\n",iters);
// 	/* Iterate the Richardson-Lucy algorithm */
// 	int iter;
// 	for (iter=0; iter<=iters; iter++)
// 	{
// 		printf("lucy %d\n",iter);
// 		/* Transform the current image iteration into Fourier space */
// 		fftw_execute_dft(tofspace,(fftw_complex*)image,(fftw_complex*)image);
// 		/* Multiply by the PSF (Note that the FFTW library requires
// 		*/
// 		/* that we doing our own scaling when coming back from fspace) */
// 		multvec(psf,image,scratch,imsize,&scale);
// 		/* Transform result back to real space ... */
// 		fftw_execute_dft(fromfspace,(fftw_complex*)scratch,(fftw_complex*)scratch);
// 		/* ... and divide original image by the result */
// 
// 			for (i=0; i<imsize; i++)
// 			{
// 				scratch[i].re() = orig[i].re()/(scratch[i].re()+EPSILON);
// 				scratch[i].im() = orig[i].im()/(scratch[i].re()+EPSILON);
// 			}
// 			/* Go back to Fourier space and multiply by conjugate(PSF) */
// 			fftw_execute_dft(tofspace,(fftw_complex*)scratch,(fftw_complex*)scratch);
// 			multvec(scratch,psfconj,scratch,imsize,&scale);
// 			/* Go back to real space ... */
// 			fftw_execute_dft(fromfspace,(fftw_complex*)scratch,(fftw_complex*)scratch);
// 			fftw_execute_dft(fromfspace,(fftw_complex*)image,(fftw_complex*)image);
// 			/* ... and perform the final multiplication) */
// 			multvec(image,scratch,image,imsize,&scale);
// 			printf("After %dth iteration ..., ah(0:19) is \n",iter);
// 			//dumpdata(imdims,image,-20);
// 	}
// }





namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	ResponseModel2::ResponseModel2()
		: _pow(10)
	{

	}

	namespace ResponseModel2_impl
	{

		template <class TReal> TReal sin_p_2(const TReal &x);
		template <class TReal> TReal cos_p_2(const TReal &x);
		template <class TReal> TReal mkPi();

		//////////////////////////////////////////////////////////////////////////
		template <> real sin_p_2<real>(const real &x)
		{
			real v = ::sin(x);
			return v*v;
		}
		template <> real cos_p_2<real>(const real &x)
		{
			real v = ::cos(x);
			return v*v;
		}
		template <> real mkPi<real>()
		{
			return c_pi;
		}

// 		//////////////////////////////////////////////////////////////////////////
// 		template <> mpfr::mpreal sin_p_2<mpfr::mpreal>(const mpfr::mpreal &x)
// 		{
// 			real v = mpfr::sin(x);
// 			return v*v;
// 		}
// 		template <> mpfr::mpreal cos_p_2<mpfr::mpreal>(const mpfr::mpreal &x)
// 		{
// 			real v = mpfr::cos(x);
// 			return v*v;
// 		}
// 		template <> mpfr::mpreal mkPi<mpfr::mpreal>()
// 		{
// 			return mpfr::const_pi();
// 		}

		//////////////////////////////////////////////////////////////////////////
		template <class TReal> 
		void evalm(size_t POW, 
			real T, real TW, real &mr, real &mi, real &dr, real &di)
		{
			TReal pi = mkPi<TReal>();
			TReal pow = POW;
			TReal t = T;
			TReal tw = TW>1?1:TW;

			TReal _2 =  ( 4*pi*pow ) ;
			TReal _0 =  ( 2*pi*pow*tw ) ;
			TReal _1 =  ( t+1 ) ;
			TReal _3 =  ( t-1 ) ;
			TReal _4 =  ( _0*_1 ) ;
			TReal _7 =  ( _2*_3 ) ;
			TReal _5 =  ( _2*_1 ) ;
			TReal _6 =  ( _0*_3 ) ;
			TReal _10 = sin ( _6/t ) ;
			TReal _9 = cos ( _4/t ) ;
			TReal _8 = sin ( _4/t ) ;
			TReal _11 = cos ( _6/t ) ;
			TReal _13 =  ( _11-1 ) ;
			TReal _12 =  ( _9-1 ) ;
			//expr

			mr=_8/_5;di=_12/_5;dr=_10/_7;mi=_13/_7;

			if(fabs(_7) < 1e-8)
			{
				mr=0;di=0;dr=0.5;mi=0;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		template <class TReal> 
		void eval_simple(size_t POW, 
			real T, real TW, real TS, real &rr, real &ri, real &ir, real &ii)
		{
			TReal pi = mkPi<TReal>();

			TReal pow = POW;

			TReal t = T;
			TReal t_p_2 = t*t;

			TReal tw = TW;
			TReal ts = TS;


			TReal _2 =  ( t+1 ) ;
			TReal _0 =  ( t_p_2-t ) ;
			TReal _4 =  ( t-1 ) ;
			TReal _5 =  ( t_p_2-1 ) ;
			TReal _3 =  ( t_p_2+t ) ;
			TReal _6 =  ( -t_p_2-t ) ;
			TReal _1 =  ( 2*pi*pow*ts/t ) ;
			TReal _9 =  ( 4*pi*pow*tw*_5 ) ;
			TReal _7 =  ( _1*_2 ) ;
			TReal _8 =  ( _1*_4 ) ;
			TReal _12 = cos ( _7 ) ;
			TReal _10 = sin ( _7 ) ;
			TReal _13 = cos ( _8 ) ;
			TReal _11 = sin ( _8 ) ;
			TReal _16 =  ( _0*_12 ) ;
			TReal _17 =  ( _3*_13 ) ;
			TReal _15 =  ( _3*_11 ) ;
			TReal _19 =  ( _6*_11 ) ;
			TReal _14 =  ( _0*_10 ) ;
			TReal _18 =  ( _6*_13 ) ;
			TReal _22 =  ( _16+_18+2*t ) ;
			TReal _20 =  ( _14+_15 ) ;
			TReal _21 =  ( _16+_17-2*t_p_2 ) ;
			TReal _23 =  ( _14+_19 ) ;
			//expr

			rr=_20/_9;ri=_21/_9;ir=-_22/_9;ii=_23/_9;

			TReal chk = _9 / tw;
			if(fabs(chk)<1e-5)
			{
				if(t)
				{
					TReal _0 = sin ( 4*pi*pow*tw ) ;
					TReal _1 =  ( 8*pi*pow*ts ) ;
					TReal _2 = cos ( 4*pi*pow*tw ) ;
					TReal _4 =  ( _2-1 ) ;
					TReal _3 =  ( _0+4*pi*pow*tw ) ;
					TReal _5 =  ( _0-4*pi*pow*tw ) ;
					//expr

					rr=_3/_1;ri=_4/_1;ir=-_4/_1;ii=_5/_1;
				}
				else
				{
					rr=0;ri=0;ir=0;ii=0;
				}
			}

		}

		//////////////////////////////////////////////////////////////////////////
		template <class TReal> 
		void eval_cosian(size_t POW, 
			real T, real TW, real TS, real &rr, real &ri, real &ir, real &ii)
		{
			TReal pi = mkPi<TReal>();

			TReal pow = POW;
			TReal pow_p_2 = pow*pow;
			TReal pow_p_3 = pow*pow*pow;
			TReal pow_p_4 = pow*pow*pow*pow;
			TReal pow_p_5 = pow*pow*pow*pow*pow;

			TReal t = T;
			TReal t_p_2 = t*t;
			TReal t_p_3 = t*t*t;
			TReal t_p_4 = t*t*t*t;
			TReal t_p_5 = t*t*t*t*t;
			TReal t_p_6 = t*t*t*t*t*t;

			TReal tw = TW;
			TReal ts = TS;

			TReal _27 =  ( -8*pow_p_4+4*pow_p_3+2*pow_p_2-pow ) ;
			TReal _8 =  ( 8*pow_p_4+4*pow_p_3-2*pow_p_2-pow ) ;
			TReal _29 =  ( -8*pow_p_4-4*pow_p_3 ) ;
			TReal _22 =  ( 128*pi*pow_p_5-64*pi*pow_p_3+8*pi*pow ) ;
			TReal _28 =  ( 16*pow_p_4-2*pow_p_2+pow ) ;
			TReal _17 =  ( -16*pow_p_4+8*pow_p_2-1 ) ;
			TReal _21 =  ( 2*pi*pow*t-2*pi*pow ) ;
			TReal _7 =  ( -16*pow_p_4+8*pow_p_3-2*pow_p_2 ) ;
			TReal _6 =  ( 8*pow_p_4-8*pow_p_3+2*pow_p_2 ) ;
			TReal _1 =  ( -8*pow_p_4+8*pow_p_3-2*pow_p_2 ) ;
			TReal _4 =  ( 8*pow_p_4+4*pow_p_3 ) ;
			TReal _30 =  ( -8*pow_p_4-4*pow_p_3+2*pow_p_2+pow ) ;
			TReal _18 =  ( -32*pow_p_4-8*pow_p_2 ) ;
			TReal _33 =  ( 64*pow_p_4-8*pow_p_2+2 ) ;
			TReal _20 =  ( 2*pi*pow*t+2*pi*pow ) ;
			TReal _13 =  ( 2*pi*pow-pi ) ;
			TReal _14 =  ( 8*pow_p_4+8*pow_p_3+2*pow_p_2 ) ;
			TReal _5 =  ( 2*pi*pow+pi ) ;
			TReal _3 =  ( 16*pow_p_4-8*pow_p_3+2*pow_p_2 ) ;
			TReal _16 =  ( 16*pow_p_4-8*pow_p_2+1 ) ;
			TReal _25 =  ( -64*pow_p_4+24*pow_p_2-2 ) ;
			TReal _9 =  ( -8*pow_p_4-8*pow_p_3-2*pow_p_2 ) ;
			TReal _31 =  ( 16*pow_p_4-2*pow_p_2-pow ) ;
			TReal _23 =  ( -384*pi*pow_p_5-8*pi*pow ) ;
			TReal _26 =  ( 128*pow_p_4+8*pow_p_2 ) ;
			TReal _10 =  ( -16*pow_p_4+2*pow_p_2+pow ) ;
			TReal _19 =  ( 32*pow_p_4+8*pow_p_2 ) ;
			TReal _24 =  ( 384*pi*pow_p_5+64*pi*pow_p_3 ) ;
			TReal _34 =  ( -128*pow_p_4-24*pow_p_2 ) ;
			TReal _12 =  ( 8*pow_p_4-4*pow_p_3 ) ;
			TReal _11 =  ( 16*pow_p_4+8*pow_p_3+2*pow_p_2 ) ;
			TReal _15 =  ( -16*pow_p_4-8*pow_p_3-2*pow_p_2 ) ;
			TReal _2 =  ( -16*pow_p_4+2*pow_p_2-pow ) ;
			TReal _0 =  ( 8*pow_p_4-4*pow_p_3-2*pow_p_2+pow ) ;
			TReal _32 =  ( 4*pow_p_3-8*pow_p_4 ) ;
			TReal _44 =  ( _20*ts ) ;
			TReal _39 =  ( _8*t_p_6+_9*t_p_5+_10*t_p_4+_11*t_p_3+_12*t_p_2-8*pow_p_4*t ) ;
			TReal _46 =  ( _21*ts ) ;
			TReal _41 =  ( _8*t_p_6+_14*t_p_5+_10*t_p_4+_15*t_p_3+_12*t_p_2+8*pow_p_4*t ) ;
			TReal _48 =  ( _27*t_p_6+_1*t_p_5+_28*t_p_4+_3*t_p_3+_29*t_p_2-8*pow_p_4*t ) ;
			TReal _38 =  ( _5*t-2*pi*pow ) ;
			TReal _49 =  ( _30*t_p_6+_9*t_p_5+_31*t_p_4+_11*t_p_3+_32*t_p_2-8*pow_p_4*t ) ;
			TReal _47 =  ( _22*t_p_6+_23*t_p_4+_24*t_p_2-128*pi*pow_p_5 ) ;
			TReal _45 =  ( _16*t_p_6+_16*t_p_5+_18*t_p_4+_18*t_p_3+16*pow_p_4*t_p_2+16*pow_p_4*t ) ;
			TReal _42 =  ( _13*t-2*pi*pow ) ;
			TReal _35 =  ( _0*t_p_6+_1*t_p_5+_2*t_p_4+_3*t_p_3+_4*t_p_2-8*pow_p_4*t ) ;
			TReal _43 =  ( _16*t_p_6+_17*t_p_5+_18*t_p_4+_19*t_p_3+16*pow_p_4*t_p_2-16*pow_p_4*t ) ;
			TReal _37 =  ( _0*t_p_6+_6*t_p_5+_2*t_p_4+_7*t_p_3+_4*t_p_2+8*pow_p_4*t ) ;
			TReal _36 =  ( _5*t+2*pi*pow ) ;
			TReal _40 =  ( _13*t+2*pi*pow ) ;
			TReal _50 =  ( _17*t_p_6+_17*t_p_5+_19*t_p_4+_19*t_p_3-16*pow_p_4*t_p_2-16*pow_p_4*t ) ;
			TReal _56 = sin ( _46/t ) ;
			TReal _53 =  ( _40*ts ) ;
			TReal _52 =  ( _38*ts ) ;
			TReal _59 = cos ( _46/t ) ;
			TReal _51 =  ( _36*ts ) ;
			TReal _57 =  ( _47*tw ) ;
			TReal _55 = sin ( _44/t ) ;
			TReal _58 = cos ( _44/t ) ;
			TReal _54 =  ( _42*ts ) ;
			TReal _67 = cos ( _54/t ) ;
			TReal _65 = cos ( _52/t ) ;
			TReal _61 = sin ( _52/t ) ;
			TReal _62 = sin ( _53/t ) ;
			TReal _64 = cos ( _51/t ) ;
			TReal _66 = cos ( _53/t ) ;
			TReal _60 = sin ( _51/t ) ;
			TReal _63 = sin ( _54/t ) ;
			TReal _69 =  ( _35*_64+_37*_65+_39*_66+_41*_67+_43*_58+_45*_59+_25*t_p_6+_26*t_p_4-64*pow_p_4*t_p_2 ) ;
			TReal _68 =  ( _35*_60+_37*_61+_39*_62+_41*_63+_43*_55+_45*_56 ) ;
			TReal _70 =  ( _35*_64+_48*_65+_39*_66+_49*_67+_43*_58+_50*_59+_33*t_p_5+_34*t_p_3+64*pow_p_4*t ) ;
			TReal _71 =  ( _35*_60+_48*_61+_39*_62+_49*_63+_43*_55+_50*_56 ) ;
			//expr

			rr=_68/_57;ri=_69/_57;ir=-_70/_57;ii=_71/_57;

			if(fabs(_57)<1e-5)
			{

				if(fabs(t-1) < (2.0*pow/(2*pow-1) -1)/2)
				{//около 1
					TReal _5 = sin ( 4*pi*pow*tw ) ;
					TReal _2 =  ( 8*pow_p_2+2*pow ) ;
					TReal _10 = cos ( 4*pi*pow*tw ) ;
					TReal _9 =  ( 256*pi*pow_p_3-16*pi*pow ) ;
					TReal _8 =  ( 64*pi*pow_p_3-4*pi*pow ) ;
					TReal _11 =  ( 4*pow-64*pow_p_3 ) ;
					TReal _3 =  ( 4*pi*pow-pi ) ;
					TReal _0 =  ( 8*pow_p_2-2*pow ) ;
					TReal _1 =  ( 4*pi*pow+pi ) ;
					TReal _7 = sin ( pi*tw ) ;
					TReal _12 =  ( 4*pi*pow-64*pi*pow_p_3 ) ;
					TReal _6 =  ( 64*pow_p_3-4*pow ) ;
					TReal _4 =  ( 16*pow_p_2-1 ) ;
					TReal _15 =  ( _9*ts ) ;
					TReal _13 = sin ( _1*tw ) ;
					TReal _17 = cos ( _3*tw ) ;
					TReal _16 = cos ( _1*tw ) ;
					TReal _14 = sin ( _3*tw ) ;
					TReal _20 =  ( _0*_13+_2*_14+_4*_5+_11*_7+_12*tw ) ;
					TReal _18 =  ( _0*_13+_2*_14+_4*_5+_6*_7+_8*tw ) ;
					TReal _19 =  ( _0*_16+_2*_17+_4*_10-32*pow_p_2+1 ) ;
					//expr

					rr=_18/_15;ri=_19/_15;ir=-_19/_15;ii=_20/_15;
				}
				else if(t<1)
				{//около 2*pow/(2*pow+1)
					TReal _11 =  ( 256*pi*pow_p_3+192*pi*pow_p_2+32*pi*pow ) ;
					TReal _5 = sin ( 4*pi*pow*tw ) ;
					TReal _13 =  ( -16*pow_p_3-12*pow_p_2-2*pow ) ;
					TReal _12 = cos ( 4*pi*pow*tw ) ;
					TReal _1 =  ( 4*pi*pow+2*pi ) ;
					TReal _3 =  ( 4*pi*pow+pi ) ;
					TReal _17 =  ( -32*pi*pow_p_3-24*pi*pow_p_2-4*pi*pow ) ;
					TReal _10 =  ( 32*pi*pow_p_3+24*pi*pow_p_2+4*pi*pow ) ;
					TReal _9 = sin ( pi*tw ) ;
					TReal _16 = cos ( pi*tw ) ;
					TReal _0 =  ( 8*pow_p_2+2*pow ) ;
					TReal _14 = cos ( 2*pi*tw ) ;
					TReal _15 =  ( -64*pow_p_3-48*pow_p_2-8*pow ) ;
					TReal _7 = sin ( 2*pi*tw ) ;
					TReal _2 =  ( 16*pow_p_2+8*pow ) ;
					TReal _8 =  ( 64*pow_p_3+48*pow_p_2+8*pow ) ;
					TReal _6 =  ( 16*pow_p_3+12*pow_p_2+2*pow ) ;
					TReal _4 =  ( 8*pow_p_2+6*pow+1 ) ;
					TReal _18 = sin ( _1*tw ) ;
					TReal _22 = cos ( _3*tw ) ;
					TReal _21 = cos ( _1*tw ) ;
					TReal _20 =  ( _11*ts ) ;
					TReal _19 = sin ( _3*tw ) ;
					TReal _24 =  ( _0*_21+_2*_22+_4*_12+_13*_14+_15*_16+80*pow_p_3+28*pow_p_2-6*pow-1 ) ;
					TReal _26 =  ( _0*_18+_2*_19+_4*_5+_13*_7+_15*_9+_17*tw ) ;
					TReal _25 =  ( _0*_21+_2*_22+_4*_12+_6*_14+_8*_16-80*pow_p_3-92*pow_p_2-26*pow-1 ) ;
					TReal _23 =  ( _0*_18+_2*_19+_4*_5+_6*_7+_8*_9+_10*tw ) ;
					//expr

					rr=_23/_20;ri=_24/_20;ir=-_25/_20;ii=_26/_20;
				}
				else
				{//около 2*pow/(2*pow-1)
					TReal _5 = sin ( 4*pi*pow*tw ) ;
					TReal _12 = cos ( 4*pi*pow*tw ) ;
					TReal _10 =  ( 32*pi*pow_p_3-24*pi*pow_p_2+4*pi*pow ) ;
					TReal _8 =  ( 64*pow_p_3-48*pow_p_2+8*pow ) ;
					TReal _2 =  ( 8*pow_p_2-2*pow ) ;
					TReal _1 =  ( 4*pi*pow-pi ) ;
					TReal _9 = sin ( pi*tw ) ;
					TReal _14 = cos ( pi*tw ) ;
					TReal _15 =  ( -16*pow_p_3+12*pow_p_2-2*pow ) ;
					TReal _4 =  ( 8*pow_p_2-6*pow+1 ) ;
					TReal _13 = cos ( 2*pi*tw ) ;
					TReal _16 =  ( -64*pow_p_3+48*pow_p_2-8*pow ) ;
					TReal _6 =  ( 16*pow_p_3-12*pow_p_2+2*pow ) ;
					TReal _7 = sin ( 2*pi*tw ) ;
					TReal _3 =  ( 4*pi*pow-2*pi ) ;
					TReal _0 =  ( 16*pow_p_2-8*pow ) ;
					TReal _11 =  ( 256*pi*pow_p_3-192*pi*pow_p_2+32*pi*pow ) ;
					TReal _17 =  ( -32*pi*pow_p_3+24*pi*pow_p_2-4*pi*pow ) ;
					TReal _18 = sin ( _1*tw ) ;
					TReal _22 = cos ( _3*tw ) ;
					TReal _21 = cos ( _1*tw ) ;
					TReal _20 =  ( _11*ts ) ;
					TReal _19 = sin ( _3*tw ) ;
					TReal _26 =  ( _0*_18+_2*_19+_4*_5+_15*_7+_16*_9+_17*tw ) ;
					TReal _25 =  ( _0*_21+_2*_22+_4*_12+_15*_13+_16*_14+80*pow_p_3-92*pow_p_2+26*pow-1 ) ;
					TReal _24 =  ( _0*_21+_2*_22+_4*_12+_6*_13+_8*_14-80*pow_p_3+28*pow_p_2+6*pow-1 ) ;
					TReal _23 =  ( _0*_18+_2*_19+_4*_5+_6*_7+_8*_9+_10*tw ) ;
					//expr

					rr=_23/_20;ri=_24/_20;ir=-_25/_20;ii=_26/_20;
				}


			}
		}

	}

	//////////////////////////////////////////////////////////////////////////
	void lsq(size_t cols, size_t rows, TVReal &A, TVReal &b, TVReal &x)
	{
		x.resize(cols);
		int res;

		res = dAx_eq_b_QRLS(&A[0], &b[0], &x[0], rows, cols);
//  		res = dAx_eq_b_SVD2(&A[0], &b[0], &x[0], rows, cols);
 		return;

		TVReal normalA(cols*cols);
		TVReal normalb(cols);

		for(size_t col(0); col<cols; col++)
		{
			real sumb=0;
			for(size_t row(0); row<rows; row++)
			{
				sumb += b[row]*A[row*cols+col];
			}
			normalb[col] = sumb;

			for(size_t col1(0); col1<=col; col1++)
			{
				real suma=0;
				for(size_t row(0); row<rows; row++)
				{
					suma += A[row*cols+col1]*A[row*cols+col];
				}
				normalA[col*cols+col1] = suma;
				normalA[col1*cols+col] = suma;
			}
		}

		
		//res = dAx_eq_b_QRLS(&normalA[0], &normalb[0], &x[0], cols, cols);
		//res = dAx_eq_b_QR(&normalA[0], &normalb[0], &x[0], cols);
		//res = dAx_eq_b_Chol(&normalA[0], &normalb[0], &x[0], cols);
		//res = dAx_eq_b_LU(&normalA[0], &normalb[0], &x[0], cols);
		res = dAx_eq_b_SVD(&normalA[0], &normalb[0], &x[0], cols);
	}


	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::setupPeriod(
		size_t pow,
		real minT,
		real maxT,
		size_t stepsT)
	{
		_pow = pow;

		real minTLog = log(minT);
		real maxTLog = log(maxT);
		real stepTLog = (maxTLog - minTLog) / (stepsT);
		//minTLog = floor(minTLog/stepTLog)*stepTLog;

		_t.resize(stepsT);
		for(size_t k(0); k<_t.size(); k++)
		{
			_t[k] = exp(minTLog+k*stepTLog);
		}
		evalVDRaw(_t, _rr,_ri,_ir,_ii,_drr,_dri,_dir,_dii);
	}

	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::setup(
		real minT,
		real maxT,
		size_t stepsT,
		real minTd,
		real maxTd,
		size_t pow)
	{
		assert(minT<=1);
		assert(maxT>=1);
		assert(minTd<=minT);
		assert(maxTd>=maxT);

		_pow = pow;

		real minTLog = log(minT);
		real maxTLog = log(maxT);
		real stepTLog = (maxTLog - minTLog) / (stepsT);
		minTLog = floor(minTLog/stepTLog)*stepTLog;
		real minTdLog = log(minTd);
		real maxTdLog = log(maxTd);
		minTdLog = floor(minTdLog/stepTLog)*stepTLog;

		size_t M = stepsT;
		size_t N = size_t((maxTdLog - minTdLog) / stepTLog+0.5);

		assert(M*6/4<=N);

		size_t M0 = size_t(-minTLog / stepTLog+0.5);
		if(M0 >= M) M0=M-1;
		size_t N0 = size_t(-minTdLog / stepTLog+0.5);
		if(N0 >= N) N0=N-1;

		//////////////////////////////////////////////////////////////////////////
		TVReal t(M);
		for(size_t k(0); k<t.size(); k++)
		{
			t[k] = exp(minTLog+k*stepTLog);
		}
		TVReal td(N);
		for(size_t k(0); k<td.size(); k++)
		{
			td[k] = exp(minTdLog+k*stepTLog);
		}

		//////////////////////////////////////////////////////////////////////////
		TVReal err,eri,eir,eii;
		evalVRaw(t, err,eri,eir,eii);
		evalVRaw(td, err,eri,eir,eii);

		//////////////////////////////////////////////////////////////////////////
		//уравнений
		size_t rows = N-M;	//строка -> 2уравнения*3режима=6штук
		size_t row0 = N0-M/2;
		size_t cols = M;	//колонка -> 4элемента=4штуки
		size_t modes = 3;
		size_t eqVars = 2;
		size_t eqs = 1;

		//матрицы
		size_t rows_m = modes*rows*eqs;
		size_t cols_m = cols*eqVars;

		TVReal m(rows_m * cols_m);
		TVReal s(rows_m);

		for(size_t row(0); row<rows; row++)
		{
			//s
			if(row == row0)
			{
				s[row*modes+0] = 1;//re mode1
				s[row*modes+1] = 1;//re mode1
				s[row*modes+2] = 1;//re mode1
			}
			else
			{
				s[row*eqs+0] = 0;//re mode1
				s[row*modes+0] = 0;//re mode1
				s[row*modes+0] = 0;//re mode1
			}

			//m
			real *mrLine_m1 = &m[(row*3+0) * cols_m];//re mode1
			real *mrLine_m2 = &m[(row*3+1) * cols_m];//re mode1
			real *mrLine_m3 = &m[(row*3+2) * cols_m];//re mode1
			for(size_t col(0); col<cols; col++)
			{
				real sr,si;

				sr=1;
				si=0;
				mrLine_m1[col*2+0] = err[row+col] * sr - eri[row+col] * si;
				mrLine_m1[col*2+1] = eri[row+col] * sr - eii[row+col] * si;
				sr=0;
				si=1;
				mrLine_m2[col*2+0] = err[row+col] * sr - eri[row+col] * si;
				mrLine_m2[col*2+1] = eri[row+col] * sr - eii[row+col] * si;
				sr=1;
				si=1;
				mrLine_m3[col*2+0] = err[row+col] * sr - eri[row+col] * si;
				mrLine_m3[col*2+1] = eri[row+col] * sr - eii[row+col] * si;
			}
		}

		TVReal h;
		lsq(cols_m, rows_m, m, s, h);

		TVReal hrr(M), hri(M), hir(M), hii(M);
		for(size_t i(0); i<M; i++)
		{
			hrr[i] = h[(i*1+0)*2+0];
			hri[i] = h[(i*1+0)*2+1];
		}


		//////////////////////////////////////////////////////////////////////////
		//check
		real mult = td[1]/td[0];
		mult = exp(log(mult)/2);

		TVReal td2=td;
		for(size_t k(0); k<td2.size(); k++)
		{
			td2[k] = td[k]*mult;
		}
		evalVRaw(td2, err,eri,eir,eii);


		TVReal res_re(N-M);
		//TVReal res_im(N-M);

		real sre0 = 0.5;
		real sim0 = -1;

		for(size_t offset(0); offset<res_re.size(); offset++)
		{
			real re(0);
			real im(0);

			for(size_t i(0); i<M; i++)
			{
				real er = err[offset+i] * sre0 - eri[offset+i] * sim0;
				real ei = eir[offset+i] * sre0 - eii[offset+i] * sim0;
				re += er * hrr[i] + ei * hri[i];
				//im += er * hir[i] + ei * hii[i];
			}

			res_re[offset] = re;
			//res_im[offset] = im;
		}

	}


	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::setup2(
		real minT,
		real maxT,
		size_t stepsT,
		real minTd,
		real maxTd,
		size_t pow)
	{
		assert(minT<=1);
		assert(maxT>=1);
		assert(minTd<=minT);
		assert(maxTd>=maxT);

		_pow = pow;

		real minTLog = log(minT);
		real maxTLog = log(maxT);
		real stepTLog = (maxTLog - minTLog) / (stepsT);
		size_t M = stepsT;

		//////////////////////////////////////////////////////////////////////////
		TVReal t(M);
		for(size_t k(0); k<t.size(); k++)
		{
			t[k] = exp(minTLog+k*stepTLog);
		}
		_t = t;
		//evalVRaw(_t, _rr,_ri,_ir,_ii);
		{//посмотреть собственные векторы

			TVReal matrix(stepsT*stepsT);

			for(size_t i(0); i<stepsT; i++)
			{
				real x = _t[i];
				for(size_t j(0); j<stepsT; j++)
				{
					real t = _t[j];

					real rr,ri,ir,ii;
					evalVRaw(x/t, rr, ri,ir,ii);
					matrix[i*stepsT+j] = rr;
				}
			}
			dAx_eq_b_SVD2(&matrix[0], NULL, NULL, stepsT, stepsT);
		}
		return;

		//minTLog = floor(minTLog/stepTLog)*stepTLog;
		real minTdLog = log(minTd);
		real maxTdLog = log(maxTd);
		minTdLog = floor(minTdLog/stepTLog)*stepTLog;

		size_t N = size_t((maxTdLog - minTdLog) / stepTLog+0.5);

		assert(M*6/4<=N);

		size_t M0 = size_t(-minTLog / stepTLog+0.5);
		if(M0 >= M) M0=M-1;
		size_t N0 = size_t(-minTdLog / stepTLog+0.5);
		if(N0 >= N) N0=N-1;


		TVReal td(N);
		for(size_t k(0); k<td.size(); k++)
		{
			td[k] = exp(minTdLog+k*stepTLog);
		}


		//////////////////////////////////////////////////////////////////////////
		TVReal err,eri,eir,eii;
		evalVRaw(t, err,eri,eir,eii);
		evalVRaw(td, err,eri,eir,eii);

		_t = t;
		evalVRaw(_t, _rr,_ri,_ir,_ii);

		//DeconvolutionRL(&_ri[0], &_rr[0], _t.size(), 1, 1, 1.0);
		TVComplex image(_rr.size());
		TVComplex psf(_rr.size());
		for(size_t i(0); i<_ri.size(); i++)
		{
			image[i] = Complex(_rr[i], _ri[i]);
			psf[i] = Complex(_rr[i], _ri[i]);
		}
//		rld(&image[0], image.size(), &psf[0], psf.size(), 30000);
		return;

// 		Clarity_Dim3 d3;
// 		d3.x = _rr.size();
// 		d3.y = 1;
// 		d3.z = 1;
// 
// 		TVReal in(_rr), out(_rr), kernel(_rr);
// 
// 		size_t ks = kernel.size()/2;
// 		kernel.erase(kernel.begin(), kernel.begin()+ks/2);
// 		kernel.erase(kernel.end()-ks/2, kernel.end());
// //		ClarityResult_t ress = Clarity_BlindMaximumLikelihoodDeconvolve(&out[0], &in[0], &psf[0], d3, 10);
// 
// 		Clarity_Dim3 d3_in = {in.size(), 1, 1};
// 		Clarity_Dim3 d3_out = {out.size(), 1, 1};
// 		Clarity_Dim3 d3_kernel = {kernel.size(), 1, 1};
// 		//ClarityResult_t ress = Clarity_MaximumLikelihoodDeconvolve(&in[0], d3_in, &kernel[0], d3_kernel, &out[0], 20);
// 		//ClarityResult_t ress = Clarity_JansenVanCittertDeconvolve(&in[0], d3_in, &kernel[0], d3_kernel, &out[0], 5);
// 		//ClarityResult_t ress = Clarity_WienerDeconvolve(&in[0], d3_in, &kernel[0], d3_kernel, &out[0], 0.01);
		return;

		//////////////////////////////////////////////////////////////////////////
		//уравнений
		size_t rows = N-M;	//строка -> 2уравнения*3режима=6штук
		size_t row0 = N0-M0;
		size_t cols = M;	//колонка -> 4элемента=4штуки
		size_t col0 = M0;
		size_t modes = 3;
		size_t eqVars = 4;
		size_t eqs = 2;

		//матрицы
		size_t rows_m = modes*rows*eqs;
		size_t cols_m = cols*eqVars;

		TVReal m(rows_m * cols_m);
		TVReal s(rows_m);

		for(size_t row(0); row<rows; row++)
		{
			//s
// 			if(row == row0)
// 			{
// 				s[row*6+0] = 1;//re mode1
// 				s[row*6+1] = 0;//im mode1
// 				s[row*6+2] = 0;//re mode2
// 				s[row*6+3] = 1;//im mode2
// 				s[row*6+4] = 1;//re mode3
// 				s[row*6+5] = 1;//im mode3
// 			}
// 			else
// 			{
// 				s[row*6+0] = 0;//re mode1
// 				s[row*6+1] = 0;//im mode1
// 				s[row*6+2] = 0;//re mode2
// 				s[row*6+3] = 0;//im mode2
// 				s[row*6+4] = 0;//re mode3
// 				s[row*6+5] = 0;//im mode3
// 			}

			real sigma = 100;
			s[row*6+0] = 1*gaussian1(row, row0, sigma);//re mode1
			s[row*6+1] = 0*gaussian1(row, row0, sigma);//im mode1
			s[row*6+2] = 0*gaussian1(row, row0, sigma);//re mode2
			s[row*6+3] = 1*gaussian1(row, row0, sigma);//im mode2
			s[row*6+4] = 1*gaussian1(row, row0, sigma);//re mode3
			s[row*6+5] = 1*gaussian1(row, row0, sigma);//im mode3

			//m
			real *mrLine_m1 = &m[(row*6+0) * cols_m];//re mode1
			real *miLine_m1 = &m[(row*6+1) * cols_m];//im mode1
			real *mrLine_m2 = &m[(row*6+2) * cols_m];//re mode2
			real *miLine_m2 = &m[(row*6+3) * cols_m];//im mode2
			real *mrLine_m3 = &m[(row*6+4) * cols_m];//re mode3
			real *miLine_m3 = &m[(row*6+5) * cols_m];//im mode3
			for(size_t col(0); col<cols; col++)
			{
				real sr,si;

				sr=1;
				si=0;
				mrLine_m1[col*4+0] = err[row+M-col] * sr - eri[row+M-col] * si;
				mrLine_m1[col*4+1] = eir[row+M-col] * sr - eii[row+M-col] * si;
				mrLine_m1[col*4+2] = 0 * sr - 0 * si;
				mrLine_m1[col*4+3] = 0 * sr - 0 * si;

				miLine_m1[col*4+0] = 0 * sr - 0 * si;
				miLine_m1[col*4+1] = 0 * sr - 0 * si;
				miLine_m1[col*4+2] = err[row+M-col] * sr - eri[row+M-col] * si;
				miLine_m1[col*4+3] = eir[row+M-col] * sr - eii[row+M-col] * si;



				sr=0;
				si=1;
				mrLine_m2[col*4+0] = err[row+M-col] * sr - eri[row+M-col] * si;
				mrLine_m2[col*4+1] = eir[row+M-col] * sr - eii[row+M-col] * si;
				mrLine_m2[col*4+2] = 0 * sr - 0 * si;
				mrLine_m2[col*4+3] = 0 * sr - 0 * si;

				miLine_m2[col*4+0] = 0 * sr - 0 * si;
				miLine_m2[col*4+1] = 0 * sr - 0 * si;
				miLine_m2[col*4+2] = err[row+M-col] * sr - eri[row+M-col] * si;
				miLine_m2[col*4+3] = eir[row+M-col] * sr - eii[row+M-col] * si;


				sr=1;
				si=1;
				mrLine_m3[col*4+0] = err[row+M-col] * sr - eri[row+M-col] * si;
				mrLine_m3[col*4+1] = eir[row+M-col] * sr - eii[row+M-col] * si;
				mrLine_m3[col*4+2] = 0 * sr - 0 * si;
				mrLine_m3[col*4+3] = 0 * sr - 0 * si;

				miLine_m3[col*4+0] = 0 * sr - 0 * si;
				miLine_m3[col*4+1] = 0 * sr - 0 * si;
				miLine_m3[col*4+2] = err[row+M-col] * sr - eri[row+M-col] * si;
				miLine_m3[col*4+3] = eir[row+M-col] * sr - eii[row+M-col] * si;
			}
		}

		TVReal h;
		lsq(cols_m, rows_m, m, s, h);

		TVReal hrr(M), hri(M), hir(M), hii(M);
		for(size_t i(0); i<M; i++)
		{
			hrr[i] = h[(i*2+0)*2+0];
			hri[i] = h[(i*2+0)*2+1];
			hir[i] = h[(i*2+1)*2+0];
			hii[i] = h[(i*2+1)*2+1];
		}


		//////////////////////////////////////////////////////////////////////////
		//check
		size_t parts = 10;
		std::ofstream out_res_re("out_res_re");
		out_res_re.precision(16);
		out_res_re.setf(std::ios::scientific);
		std::ofstream out_res_im("out_res_im");
		out_res_im.precision(16);
		out_res_im.setf(std::ios::scientific);
		for(size_t part=0; part<parts; part++)
		{
			real mult = td[1]/td[0];
			mult = exp(log(mult)*part/parts);

			TVReal td2=td;
			for(size_t k(0); k<td2.size(); k++)
			{
				td2[k] = td[k]*mult;
			}
			evalVRaw(td2, err,eri,eir,eii);


			TVReal res_re(N-M);
			TVReal res_im(N-M);

			real sre0 = 1;
			real sim0 = 1;

			for(size_t offset(0); offset<res_re.size(); offset++)
			{
				real re(0);
				real im(0);

				for(size_t i(0); i<M; i++)
				{
					real er = err[offset+M-i] * sre0 - eri[offset+M-i] * sim0;
					real ei = eir[offset+M-i] * sre0 - eii[offset+M-i] * sim0;
					re += er * hrr[i] + ei * hri[i];
					im += er * hir[i] + ei * hii[i];
				}

				res_re[offset] = re;
				res_im[offset] = im;

				out_res_re<<res_re[offset]<<"\t";
				out_res_im<<res_im[offset]<<"\t";
			}
			out_res_re<<std::endl;
			out_res_im<<std::endl;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::evalVRaw(
		real t, real &rr, real &ri, real &ir, real &ii)
	{
//   		ResponseModel2_impl::eval_cosian<real>(_pow, t/(1.0+0.5/_pow), t, t>1?1:t, rr, ri, ir, ii);

		real ts = t>1?1:t;
		real mp = (1.0+0.5/_pow);
		real mm = (1.0-0.5/_pow);
		ResponseModel2_impl::eval_cosian<real>(_pow, t/mp, t, ts, rr , ri , ir , ii );

		real rr_,ri_,ir_,ii_;
		ResponseModel2_impl::eval_cosian<real>(_pow, t/mm, t, ts, rr_, ri_, ir_, ii_);
		rr = rr*mp - rr_*mm;
		ri = ri*mp - ri_*mm;
		ir = ir*mp - ir_*mm;
		ii = ii*mp - ii_*mm;
	}


	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::evalVRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii)
	{
		rr.resize(t.size());
		ri.resize(t.size());
		ir.resize(t.size());
		ii.resize(t.size());
		for(size_t i(0); i<t.size(); i++)
		{
			evalVRaw(t[i], rr[i], ri[i], ir[i], ii[i]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::evalVDRaw(
		real t, 
		real & rr, real & ri, real & ir, real & ii,
		real &drr, real &dri, real &dir, real &dii)
	{
		//для t взять небольшой плюс минус -> 3 точки
		//по ним построить параболу и взять ее производную в точке t
		real delta = t/_pow/100;
		real tm = t-delta;
		real tp = t+delta;

		real rrm, rim, irm, iim;
		evalVRaw(tm, rrm, rim, irm, iim);

		//real rr, ri, ir, ii;
		evalVRaw(t, rr, ri, ir, ii);

		real rrp, rip, irp, iip;
		evalVRaw(tp, rrp, rip, irp, iip);


		real a, b, c;
		parabolicArgs(tm, rrm, t, rr, tp, rrp, a, b, c); drr = 2*a*t + b;
		parabolicArgs(tm, rim, t, ri, tp, rip, a, b, c); dri = 2*a*t + b;
		parabolicArgs(tm, irm, t, ir, tp, irp, a, b, c); dir = 2*a*t + b;
		parabolicArgs(tm, iim, t, ii, tp, iip, a, b, c); dii = 2*a*t + b;
	}
	
	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::evalDRaw(const TVReal &t, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii)
	{
		drr.resize(t.size());
		dri.resize(t.size());
		dir.resize(t.size());
		dii.resize(t.size());
		for(size_t i(0); i<t.size(); i++)
		{
			real rr,ri, ir, ii;
			evalVDRaw(t[i], rr, ri, ir, ii, drr[i], dri[i], dir[i], dii[i]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::evalVDRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii)
	{
		rr.resize(t.size());
		ri.resize(t.size());
		ir.resize(t.size());
		ii.resize(t.size());
		drr.resize(t.size());
		dri.resize(t.size());
		dir.resize(t.size());
		dii.resize(t.size());
		for(size_t i(0); i<t.size(); i++)
		{
			evalVDRaw(t[i], rr[i], ri[i], ir[i], ii[i], drr[i], dri[i], dir[i], dii[i]);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	Complex ResponseModel2::eval(real t, real st, const Complex &sv)
	{
		real rr,ri, ir, ii;
		evalVRaw(t/st, rr,ri, ir, ii);
		return Complex(sv.re()*rr - sv.im()*ri, sv.re()*ir - sv.im()*ii);
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::evalFuncOpt(double *p, double *hx, int m, int n, void *_SFuncOptParams)
	{
		SFuncOptParams *params = (SFuncOptParams *)_SFuncOptParams;

		real t, re, im;
		real rr, ri, ir, ii;
		int nDiv2 = n/2;

		if(params->_st)
		{//значения периода спектра. Если задано то период спектра НЕ оптимизируется
			int mDiv2 = m/2;
			for(int i(0); i<nDiv2; i++)
			{
				t = params->_et[i];
				re = im = 0;

				for(int j(0); j<mDiv2; j++)
				{
					real st = params->_st[j];
					real sr = p[j*2+0];
					real si = p[j*2+1];

					params->_rm->evalVRaw(t/st, rr, ri, ir, ii);

					re += sr*rr - si*ri;
					im += sr*ir - si*ii;
				}

				hx[i*2+0] = re;
				hx[i*2+1] = im;

			}
		}
		else
		{//значения периода спектра. Если НЕ задано то период спектра оптимизируется
			int mDiv3 = m/3;
			for(int i(0); i<nDiv2; i++)
			{
				t = params->_et[i];
				re = im = 0;

				for(int j(0); j<mDiv3; j++)
				{
					real st = p[j*3+0];
					real sr = p[j*3+1];
					real si = p[j*3+2];

					params->_rm->evalVRaw(t/st, rr, ri, ir, ii);

					re += sr*rr - si*ri;
					im += sr*ir - si*ii;
				}

				hx[i*2+0] = re;
				hx[i*2+1] = im;

			}
		}

		//значения отклика, если задано, то будет вычтено из вычесленного (можно не задавать в dlrvmar_der)
		if(params->_ev)
		{
			for(int i(0); i<nDiv2; i++)
			{
				hx[i*2+0] -= params->_ev[i].re();
				hx[i*2+1] -= params->_ev[i].im();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void ResponseModel2::jacoFuncOpt(double *p, double *jx, int m, int n, void *_SFuncOptParams)
	{
		SFuncOptParams *params = (SFuncOptParams *)_SFuncOptParams;

		real t;
		real rr,  ri,  ir,  ii;
		int nDiv2 = n/2;

		if(params->_st)
		{//значения периода спектра. Если задано то период спектра НЕ оптимизируется
			int mDiv2 = m/2;
			for(int i(0); i<nDiv2; i++)
			{
				t = params->_et[i];

				for(int j(0); j<mDiv2; j++)
				{
					real st = params->_st[j];
					real sr = p[j*2+0];
					real si = p[j*2+1];

					params->_rm->evalVRaw(t/st, rr, ri, ir, ii);

					jx[(i*2+0)*m+j*2+0] = rr;
					jx[(i*2+0)*m+j*2+1] = -ri;

					jx[(i*2+1)*m+j*2+0] = ir;
					jx[(i*2+1)*m+j*2+1] = -ii;
				}
			}
		}
		else
		{//значения периода спектра. Если НЕ задано то период спектра оптимизируется
			real drr, dri, dir, dii;
			int mDiv3 = m/3;
			for(int i(0); i<n/2; i++)
			{
				t = params->_et[i];

				for(int j(0); j<mDiv3; j++)
				{
					real st = p[j*3+0];
					real sr = p[j*3+1];
					real si = p[j*3+2];

					params->_rm->evalVDRaw(t/st, rr, ri, ir, ii, drr, dri, dir, dii);

					real dt = -t/sqr(st);

					jx[(i*2+0)*m+j*3+0] = sr*drr*dt - si*dri*dt;
					jx[(i*2+0)*m+j*3+1] = rr;
					jx[(i*2+0)*m+j*3+2] = -ri;

					jx[(i*2+1)*m+j*3+0] = sr*dir*dt - si*dii*dt;
					jx[(i*2+1)*m+j*3+1] = ir;
					jx[(i*2+1)*m+j*3+2] = -ii;
				}
			}
		}
	}


	static real g_levmarOpts_ResponseModel2[LM_OPTS_SZ] = 
	{
		LM_INIT_MU,//mu
		1e-40,//LM_STOP_THRESH,	//stopping thresholds for ||J^T e||_inf, 
		1e-40,//LM_STOP_THRESH,	//||Dp||_2 and 
		1e-40,//LM_STOP_THRESH,	//||e||_2. Set to NULL for defaults to be used.
	};

	//////////////////////////////////////////////////////////////////////////
	int ResponseModel2::solve_tv(
		size_t eSize, const real *et, const Complex *ev,//отклик
		size_t sSize,       real *st,       Complex *sv,//спектр
		size_t itMax,//макс итераций
		TVReal &work)
	{
		SFuncOptParams params;
		params._et = et;
		params._ev = ev;
		params._st = NULL;
		params._rm = this;

		real levmarInfo[LM_INFO_SZ];
		if(work.size() < LM_DER_WORKSZ(sSize*3, eSize*2))
		{
			work.resize(LM_DER_WORKSZ(sSize*3, eSize*2));
		}

		TVReal args(sSize*3);
		for(size_t i(0); i<sSize; i++)
		{
			args[i*3+0] = st[i];
			args[i*3+1] = sv[i].re();
			args[i*3+2] = sv[i].im();
		}


// 		TVReal err(args.size());
// 		params._ev=NULL;
// 		dlevmar_chkjac(
// 			evalFuncOpt, 
// 			jacoFuncOpt, 
// 			&args[0], 
// 			sSize*3, 
// 			eSize*2,
// 			&params, 
// 			&err.front());
// 		real errVal = std::accumulate(err.begin(), err.end(), 0.0)/err.size();
// 		std::cout<<"dlevmar_chkjac: "<<errVal<<std::endl;
// 		exit(0);


		int res = dlevmar_der(
			&evalFuncOpt,
			&jacoFuncOpt,
			&args[0],
			NULL,
			sSize*3,
			eSize*2,
			itMax, 
			g_levmarOpts_ResponseModel2,
			levmarInfo,
			&work[0],
			NULL,
			&params);

		for(size_t i(0); i<sSize; i++)
		{
			st[i] = args[i*3+0];
			sv[i].re() = args[i*3+1];
			sv[i].im() = args[i*3+2];
		}

		return res;
	}

	//////////////////////////////////////////////////////////////////////////
	int ResponseModel2::solve_v(
		size_t eSize, const real *et, const Complex *ev,//отклик
		size_t sSize, const real *st,       Complex *sv,//спектр
		size_t itMax,//макс итераций
		TVReal &work)
	{
		SFuncOptParams params;
		params._et = et;
		params._ev = ev;
		params._st = st;
		params._rm = this;

		real levmarInfo[LM_INFO_SZ];
		if(work.size() < LM_DER_WORKSZ(sSize*2, eSize*2))
		{
			work.resize(LM_DER_WORKSZ(sSize*2, eSize*2));
		}

		assert(sizeof(double) == sizeof(Complex)/2);//хак Complex используется как double[2]


// 		TVReal err(eSize*2);
// 		params._ev = NULL;
// 		dlevmar_chkjac(
// 			evalFuncOpt, 
// 			jacoFuncOpt, 
// 			(real *)sv, 
// 			sSize*2, 
// 			eSize*2,
// 			&params, 
// 			&err.front());
// 		real errVal = std::accumulate(err.begin(), err.end(), 0.0)/err.size();
// 		std::cout<<"dlevmar_chkjac: "<<errVal<<std::endl;
// 		exit(0);


		int res = dlevmar_der(
			&evalFuncOpt,
			&jacoFuncOpt,
			(real *)sv,
			NULL,
			sSize*2,
			eSize*2,
			itMax, 
			g_levmarOpts_ResponseModel2,
			levmarInfo,
			&work[0],
			NULL,
			&params);

		return res;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	const TVReal &ResponseModel2::getTable_t()
	{
		return _t;
	}

	//////////////////////////////////////////////////////////////////////////
	const TVReal &ResponseModel2::getTable_rr()
	{
		return _rr;
	}
	
	//////////////////////////////////////////////////////////////////////////
	const TVReal &ResponseModel2::getTable_ri()
	{
		return _ri;
	}
	
	//////////////////////////////////////////////////////////////////////////
	const TVReal &ResponseModel2::getTable_ir()
	{
		return _ir;
	}
	
	//////////////////////////////////////////////////////////////////////////
	const TVReal &ResponseModel2::getTable_ii()
	{
		return _ii;
	}


}



using namespace hel;
int f()
{
	ResponseModel2 rm2;

	if(0)
	{
		TVReal et;
		TVComplex ev;

		for(real t(0.1); t<1.0/0.1; t/=0.99)
		{
			et.push_back(t);
			ev.push_back(rm2.eval(t, 1, Complex(1,0, CAP)));
		}

		TVReal st;
		TVComplex sv;
		TVComplex svg;
		for(real t(0.9); t<1.0/0.9; t/=0.999)
		{
			st.push_back(t);
			sv.push_back(rm2.eval(t, 1, Complex(1,0, CAP)));
			svg.push_back(Complex(1,0, CAP) * gaussian1(t,1, 0.05));
		}

		//solve
		TVReal work;
		int res;
		res = rm2.solve_v(
			et.size(), &et[0], &ev[0],
			st.size(), &st[0], &sv[0],
			1000, work);
// 		res = rm2.solve_tv(
// 			et.size(), &et[0], &ev[0],
// 			st.size(), &st[0], &sv[0],
// 			100, work);

		//check
		real err=0;
		TVComplex ev2(ev.size());
		TVComplex ev2g(ev.size());
		TVReal ev2ga(ev.size());
		for(size_t i(0); i<ev2.size(); i++)
		{
			ev2[i] = 0;
			for(size_t j(0); j<st.size(); j++)
			{
				ev2[i] += rm2.eval(et[i], st[j], sv[j]);
				for(size_t i2(0); i2<ev2g.size(); i2++)
				{
					ev2g[i2] += sv[j]*gaussian1(et[i2], st[j], 0.01);
				}
				ev2ga[j]++;
			}
			err += fabs(ev[i].re() - ev2[i].re());
			err += fabs(ev[i].im() - ev2[i].im());
		}
		for(size_t i2(0); i2<ev2g.size(); i2++)
		{
			if(ev2ga[i2])
			{
				ev2g[i2] /= ev2ga[i2];
			}
		}

		exit(0);

	}

// 	rm2.setup2(
// 		1.0/(60*30),	//minT
// 		60*30,	//maxT
// 		100000,	//stepsT
// 		0.00001,	//minTd
// 		100000,		//maxTd
// 		10);	//pow

	rm2.setup2(
		0.01,	//minT
		1/0.01,	//maxT
		5000,	//stepsT
		0.01,	//minTd
		100,	//maxTd
		10);	//pow

	std::ofstream out("out");
	out.precision(16);
	out.setf(std::ios::scientific);

	for(size_t idx(0); idx<rm2.getTable_t().size(); idx++)
	{
		out<<rm2.getTable_t()[idx]<<"\t";
		out<<rm2.getTable_rr()[idx]<<"\t";
		out<<rm2.getTable_ri()[idx]<<"\t";
		out<<rm2.getTable_ir()[idx]<<"\t";
		out<<rm2.getTable_ii()[idx]<<"\t";

		real g = gaussian1(log(rm2.getTable_t()[idx]), 0, 0.001);
		out<<g<<"\t";
		out<<std::endl;
	}


	exit(0);
	return rand();
}
//static int i=f();