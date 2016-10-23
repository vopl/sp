/*
////////////////////////////////////////////////////////////////////////////////////
//
//  Prototypes and definitions for the Levenberg - Marquardt minimization algorithm
//  Copyright (C) 2004  Manolis Lourakis (lourakis at ics forth gr)
//  Institute of Computer Science, Foundation for Research & Technology - Hellas
//  Heraklion, Crete, Greece.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
////////////////////////////////////////////////////////////////////////////////////
*/

#ifndef _LEVMAR_H_
#define _LEVMAR_H_


/************************************* Start of configuration options *************************************/
/* Note that when compiling with CMake, this configuration section is automatically generated
 * based on the user's input, see levmar.h.in
 */

/* specifies whether to use LAPACK or not. Using LAPACK is strongly recommended */
#define HAVE_LAPACK

/* specifies whether the PLASMA parallel library for multicore CPUs is available */
/* #undef HAVE_PLASMA */

/* to avoid the overhead of repeated mallocs(), routines in Axb.c can be instructed to
 * retain working memory between calls. Such a choice, however, renders these routines
 * non-reentrant and is not safe in a shared memory multiprocessing environment.
 * Bellow, an attempt is made to issue a warning if this option is turned on and OpenMP
 * is being used (note that this will work only if omp.h is included before levmar.h)
 */
//#define LINSOLVERS_RETAIN_MEMORY
#if (defined(_OPENMP))
# ifdef LINSOLVERS_RETAIN_MEMORY
#  ifdef _MSC_VER
#  pragma message("LINSOLVERS_RETAIN_MEMORY is not safe in a multithreaded environment and should be turned off!")
#  else
#  warning LINSOLVERS_RETAIN_MEMORY is not safe in a multithreaded environment and should be turned off!
#  endif /* _MSC_VER */
# endif /* LINSOLVERS_RETAIN_MEMORY */
#endif /* _OPENMP */

/* specifies whether long double precision routines will be compiled or not */
#define LM_EXT_PREC
/* specifies whether double precision routines will be compiled or not */
#define LM_DBL_PREC
/* specifies whether single precision routines will be compiled or not */
#define LM_SNGL_PREC

/****************** End of configuration options, no changes necessary beyond this point ******************/


#ifdef __cplusplus
//extern "C" {
#endif

/* work arrays size for ?levmar_der and ?levmar_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_DER_WORKSZ(npar, nmeas) (2*(nmeas) + 4*(npar) + (nmeas)*(npar) + (npar)*(npar))
#define LM_DIF_WORKSZ(npar, nmeas) (4*(nmeas) + 4*(npar) + (nmeas)*(npar) + (npar)*(npar))

/* work arrays size for ?levmar_bc_der and ?levmar_bc_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_BC_DER_WORKSZ(npar, nmeas) (2*(nmeas) + 4*(npar) + (nmeas)*(npar) + (npar)*(npar))
#define LM_BC_DIF_WORKSZ(npar, nmeas) LM_BC_DER_WORKSZ((npar), (nmeas)) /* LEVMAR_BC_DIF currently implemented using LEVMAR_BC_DER()! */

/* work arrays size for ?levmar_lec_der and ?levmar_lec_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_LEC_DER_WORKSZ(npar, nmeas, nconstr) LM_DER_WORKSZ((npar)-(nconstr), (nmeas))
#define LM_LEC_DIF_WORKSZ(npar, nmeas, nconstr) LM_DIF_WORKSZ((npar)-(nconstr), (nmeas))

/* work arrays size for ?levmar_blec_der and ?levmar_blec_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_BLEC_DER_WORKSZ(npar, nmeas, nconstr) LM_LEC_DER_WORKSZ((npar), (nmeas)+(npar), (nconstr))
#define LM_BLEC_DIF_WORKSZ(npar, nmeas, nconstr) LM_LEC_DIF_WORKSZ((npar), (nmeas)+(npar), (nconstr))

/* work arrays size for ?levmar_bleic_der and ?levmar_bleic_dif functions.
 * should be multiplied by sizeof(double) or sizeof(float) to be converted to bytes
 */
#define LM_BLEIC_DER_WORKSZ(npar, nmeas, nconstr1, nconstr2) LM_BLEC_DER_WORKSZ((npar)+(nconstr2), (nmeas)+(nconstr2), (nconstr1)+(nconstr2))
#define LM_BLEIC_DIF_WORKSZ(npar, nmeas, nconstr1, nconstr2) LM_BLEC_DIF_WORKSZ((npar)+(nconstr2), (nmeas)+(nconstr2), (nconstr1)+(nconstr2))

#define LM_OPTS_SZ       5 /* max(4, 5) */
#define LM_INFO_SZ       10
#define LM_ERROR         -1
#define LM_INIT_MU       1E-03
#define LM_STOP_THRESH   1E-17
#define LM_DIFF_DELTA    1E-06
#define LM_VERSION       "2.6 (November 2011) CUSTOM"

#ifdef LM_EXT_PREC
/* double precision LM, with & without Jacobian */
/* unconstrained minimization */
extern int elevmar_der(
      void (*func)(long double *p, long double *hx, int m, int n, void *adata),
      void (*jacf)(long double *p, long double *j, int m, int n, void *adata),
      long double *p, long double *x, int m, int n, int itmax, long double *opts,
      long double *info, long double *work, long double *covar, void *adata);

inline int levmar_der(
        void (*func)(long double *p, long double *hx, int m, int n, void *adata),
        void (*jacf)(long double *p, long double *j, int m, int n, void *adata),
        long double *p, long double *x, int m, int n, int itmax, long double *opts,
        long double *info, long double *work, long double *covar, void *adata)
{
    return elevmar_der(
                func,
                jacf,
                p, x, m, n, itmax, opts,
                info, work, covar, adata);
}

#endif /* LM_EXT_PREC */

#ifdef LM_DBL_PREC
/* double precision LM, with & without Jacobian */
/* unconstrained minimization */
extern int dlevmar_der(
      void (*func)(double *p, double *hx, int m, int n, void *adata),
      void (*jacf)(double *p, double *j, int m, int n, void *adata),
      double *p, double *x, int m, int n, int itmax, double *opts,
      double *info, double *work, double *covar, void *adata);

inline int levmar_der(
        void (*func)(double *p, double *hx, int m, int n, void *adata),
        void (*jacf)(double *p, double *j, int m, int n, void *adata),
        double *p, double *x, int m, int n, int itmax, double *opts,
        double *info, double *work, double *covar, void *adata)
{
    return dlevmar_der(
                func,
                jacf,
                p, x, m, n, itmax, opts,
                info, work, covar, adata);
}

#endif /* LM_DBL_PREC */


#ifdef LM_SNGL_PREC
/* single precision LM, with & without Jacobian */
/* unconstrained minimization */
extern int slevmar_der(
      void (*func)(float *p, float *hx, int m, int n, void *adata),
      void (*jacf)(float *p, float *j, int m, int n, void *adata),
      float *p, float *x, int m, int n, int itmax, float *opts,
      float *info, float *work, float *covar, void *adata);

inline int levmar_der(
        void (*func)(float *p, float *hx, int m, int n, void *adata),
        void (*jacf)(float *p, float *j, int m, int n, void *adata),
        float *p, float *x, int m, int n, int itmax, float *opts,
        float *info, float *work, float *covar, void *adata)
{
    return slevmar_der(
                func,
                jacf,
                p, x, m, n, itmax, opts,
                info, work, covar, adata);
}

#endif /* LM_SNGL_PREC */

/* linear system solvers */
#ifdef HAVE_LAPACK

#ifdef LM_EXT_PREC
extern int eAx_eq_b_Chol(long double *A, long double *B, long double *x, int m);
#endif /* LM_EXT_PREC */

#ifdef LM_DBL_PREC
extern int dAx_eq_b_Chol(double *A, double *B, double *x, int m);
#endif /* LM_DBL_PREC */

#ifdef LM_SNGL_PREC
extern int sAx_eq_b_Chol(float *A, float *B, float *x, int m);
#endif /* LM_SNGL_PREC */

#else /* no LAPACK */

#ifdef LM_EXT_PREC
extern int dAx_eq_b_LU_noLapack(long double *A, long double *B, long double *x, int n);
#endif /* LM_DBL_PREC */

#ifdef LM_DBL_PREC
extern int dAx_eq_b_LU_noLapack(double *A, double *B, double *x, int n);
#endif /* LM_DBL_PREC */

#ifdef LM_SNGL_PREC
extern int sAx_eq_b_LU_noLapack(float *A, float *B, float *x, int n);
#endif /* LM_SNGL_PREC */

#endif /* HAVE_LAPACK */


#ifdef __cplusplus
//}
#endif

#endif /* _LEVMAR_H_ */
