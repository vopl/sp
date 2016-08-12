#include "responseModel.hpp"
#include "math.hpp"
#include <levmar.h>
#include <cstring>
#include <numeric>
#include <iostream>

#include <cmath>

#include "window_hann.hpp"
#include "window_hann2.hpp"
#include "window_rect.hpp"



// extern "C"
// {
//     int dAx_eq_b_Chol(double *A, double *B, double *x, int m);
//     int dAx_eq_b_QR(double *A, double *B, double *x, int m);
//     int dAx_eq_b_QRLS(double *A, double *B, double *x, int m, int n);
//     int dAx_eq_b_Chol(double *A, double *B, double *x, int m);
//     int dAx_eq_b_LU(double *A, double *B, double *x, int m);
//     int dAx_eq_b_SVD(double *A, double *B, double *x, int m);
// 
// 
// 
//     int dgesdd_(char *jobz, long *m, long *n, double *
//         a, long *lda, double *s, double *u, long *ldu, 
//         double *vt, long *ldvt, double *work, long *lwork, 
//         long *iwork, long *info);
// 
//     int min(int a, int b)
//     {
//         if(a<b) return a;
//         return b;
//     }
//     int max(int a, int b)
//     {
//         if(a>b) return a;
//         return b;
//     }
//     int dAx_eq_b_SVD2(double *A, double *B, double *x, long m, long n)
//     {
//         //column major a
//         {
//             double *a = (double *)malloc(sizeof(double)*m*n);
//             memcpy(a, A, sizeof(double)*m*n);
//             for(long row=0; row<m; row++)
//                 for(long col=0; col<n; col++)
//                     A[col*m+row] = a[row*n+col];
//             free(a);
//         }
// 
//         double *s = (double *)malloc(sizeof(double)*m);
//         double *u = (double *)malloc(sizeof(double)*m*m);
//         double *vt = (double *)malloc(sizeof(double)*n*n);
// 
//         long lwork = 3*min(m,n)*min(m,n) + max(max(m,n),4*min(m,n)*min(m,n)+4*min(m,n));
//         double *work = (double *)malloc(sizeof(double)*lwork);
//         long *iwork = (long *)malloc(sizeof(long)*8*min(m,n));
// 
//         long info;
// 
//         dgesdd_(
//             "A", 
//             (long *)&m, 
//             (long *)&n, 
//             A, 
//             (long *)&m, 
//             s, 
//             u, 
//             (long *)&m, 
//             vt, 
//             (long *)&n,
//             work, 
//             (long *)&lwork, 
//             iwork, 
//             &info);
// 
//         if(0 == info)
//         {
//             /* compute machine epsilon */
//             double eps, aux;
//             for(eps=1.0; aux=eps+1.0, aux-1.0>0.0; eps*=0.5);
//             eps*=2.0;
// 
//             /* compute the pseudoinverse in a */
//             memset(A, 0, sizeof(double)*m*n); /* initialize to zero */
// 
//             int rank, i, j;
//             double one_over_denom, thresh;
// 
//             for(rank=0, thresh=eps*s[0]; rank<m && s[rank]>thresh; rank++)
//             {
//                 one_over_denom=1.0/s[rank];
// 
//                 for(j=0; j<m; j++)
//                     for(i=0; i<n; i++)
//                         A[i*m+j]+=vt[rank+i*n]*u[j+rank*m]*one_over_denom;
//             }
// 
//             /* compute A^+ b in x */
//             double sum;
//             for(i=0; i<n; i++)
//             {
//                 for(j=0, sum=0.0; j<m; j++)
//                 {
//                     sum+=A[i*m+j]*B[j];
//                 }
//                 x[i]=sum;
//             }
//         }
// 
//         free(s);
//         free(u);
//         free(vt);
//         free(work);
//         free(iwork);
// 
//         return 0 == info;
//     }
// 
// };

using namespace sp;
namespace ResponseModel_impl
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
        return g_pi;
    }

//         //////////////////////////////////////////////////////////////////////////
//         template <> mpfr::mpreal sin_p_2<mpfr::mpreal>(const mpfr::mpreal &x)
//         {
//             real v = mpfr::sin(x);
//             return v*v;
//         }
//         template <> mpfr::mpreal cos_p_2<mpfr::mpreal>(const mpfr::mpreal &x)
//         {
//             real v = mpfr::cos(x);
//             return v*v;
//         }
//         template <> mpfr::mpreal mkPi<mpfr::mpreal>()
//         {
//             return mpfr::const_pi();
//         }

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

        if(fabs(_57)<1e-1)
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

namespace sp
{
    //////////////////////////////////////////////////////////////////////////
    ResponseModel::ResponseModel()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::evalVRaw(
        real t, real &rr, real &ri, real &ir, real &ii)
    {
//        Window_rect::kernel<1>(t, rr, ri, ir, ii);
//        return;

        if(t<=0)
        {
            rr=ri=ir=ii=0;
            return;
        }

        size_t _pow = 1;

        rr=ri=ir=ii=0;
        real ts = t>1?1:t;
        real mp = (1.0+0.5/_pow);
        real mm = (1.0-0.5/_pow);
        real rr_1,ri_1,ir_1,ii_1;
        ResponseModel_impl::eval_simple<real>(_pow, t/mp, t, ts, rr_1, ri_1, ir_1, ii_1);
        //ResponseModelLegacy_impl::eval_cosian<real>(_pow, t/mp, t, ts, rr_1, ri_1, ir_1, ii_1);

        real rr_2,ri_2,ir_2,ii_2;
        ResponseModel_impl::eval_simple<real>(_pow, t/mm, t, ts, rr_2, ri_2, ir_2, ii_2);
        //ResponseModelLegacy_impl::eval_cosian<real>(_pow, t/mm, t, ts, rr_2, ri_2, ir_2, ii_2);
        rr += (rr_1*mp - rr_2*mm);
        ri += (ri_1*mp - ri_2*mm);
        ir += (ir_1*mp - ir_2*mm);
        ii += (ii_1*mp - ii_2*mm);

    }


    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        //Если для уравнения y = ax^2 + bx + c известны координаты 3-х различных точек его графика (x1;y1), (x2;y2), (x3;y3), то его коэффициенты могут быть найдены так:
        inline void parabolicArgs(real x1, real y1, real x2, real y2, real x3, real y3, real &a, real &b, real &c)
        {
            a = (y3- (x3*(y2-y1) + x2*y1 - x1*y2)/(x2-x1)) / (x3*(x3-x1-x2)+x1*x2);
            b = (y2-y1)/(x2-x1) - a*(x1+x2);
            c = (x2*y1-x1*y2)/(x2-x1) + a*x1*x2;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::evalVDRaw(
        real tm, real t, real tp,
        real & rr, real & ri, real & ir, real & ii,
        real &drr, real &dri, real &dir, real &dii)
    {
        if(t<=0)
        {
            rr=ri=ir=ii=0;
            drr=dri=dir=dii=0;
            return;
        }
        if(!tm)
        {
            tm = t/2;
        }

        real rrm, rim, irm, iim;
        evalVRaw(tm, rrm, rim, irm, iim);

        //real rr, ri, ir, ii;
        evalVRaw(t, rr, ri, ir, ii);

        real rrp, rip, irp, iip;
        evalVRaw(tp, rrp, rip, irp, iip);

        real lt = log(t);
        real ltm = log(tm);
        real ltp = log(tp);

        real a, b, c;
        parabolicArgs(ltm, rrm, lt, rr, ltp, rrp, a, b, c); drr = (2*a*lt + b);
        parabolicArgs(ltm, rim, lt, ri, ltp, rip, a, b, c); dri = (2*a*lt + b);
        parabolicArgs(ltm, irm, lt, ir, ltp, irp, a, b, c); dir = (2*a*lt + b);
        parabolicArgs(ltm, iim, lt, ii, ltp, iip, a, b, c); dii = (2*a*lt + b);
    }
    


    //////////////////////////////////////////////////////////////////////////
    complex ResponseModel::eval(real t, real st, const complex &sv)
    {
        real rr,ri, ir, ii;
        evalVRaw(t/st, rr,ri, ir, ii);
        return complex(sv.re()*rr - sv.im()*ri, sv.re()*ir - sv.im()*ii);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::evalFuncOpt(double *p, double *hx, int m, int n, void *_SFuncOptParams)
    {
        SFuncOptParams *params = (SFuncOptParams *)_SFuncOptParams;

        real logt, re, im;
        real rr, ri, ir, ii;
        int nDiv2 = n/2;

        int mDiv2 = m/2;
        for(int i(0); i<nDiv2; i++)
        {
            logt = params->_logt[i];
            re = im = 0;

            for(int j(0); j<mDiv2; j++)
            {
                real slogt = params->_logt[j];
                real sr = p[j*2+0];
                real si = p[j*2+1];

                params->_rm->evalVRaw(exp(logt-slogt), rr, ri, ir, ii);


                //assert(std::isfinite(rr) && std::isfinite(ri));
                //assert(std::isfinite(ir) && std::isfinite(ii));

                re += sr*rr - si*ri;
                im += sr*ir - si*ii;
            }

            //assert(std::isfinite(re) && std::isfinite(im));

            hx[i*2+0] = re;
            hx[i*2+1] = im;

        }

        for(int i(0); i<nDiv2; i++)
        {
            hx[i*2+0] -= params->_ev[i].re();
            hx[i*2+1] -= params->_ev[i].im();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::jacoFuncOpt(double *p, double *jx, int m, int n, void *_SFuncOptParams)
    {
        SFuncOptParams *params = (SFuncOptParams *)_SFuncOptParams;

        real logt;
        real rr,  ri,  ir,  ii;
        int nDiv2 = n/2;

        int mDiv2 = m/2;
        for(int i(0); i<nDiv2; i++)
        {
            logt = params->_logt[i];

            for(int j(0); j<mDiv2; j++)
            {
                real slogt = params->_logt[j];
//                real sr = p[j*2+0];
//                real si = p[j*2+1];

                real t = exp(logt-slogt);
                params->_rm->evalVRaw(t, rr, ri, ir, ii);

                jx[(i*2+0)*m+j*2+0] = rr;
                jx[(i*2+0)*m+j*2+1] = -ri;

                jx[(i*2+1)*m+j*2+0] = ir;
                jx[(i*2+1)*m+j*2+1] = -ii;
            }
        }
    }


    static real g_levmarOpts_ResponseModel[LM_OPTS_SZ] = 
    {
        LM_INIT_MU,//mu
        LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf, 
        LM_STOP_THRESH,    //||Dp||_2 and 
        LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
    };

    //////////////////////////////////////////////////////////////////////////
    int ResponseModel::solve_v(real mu,
        size_t size, const real *logt, const complex *ev,//отклик
        complex *sv,//спектр
        size_t itMax,//макс итераций
        TVReal &work)
    {
        SFuncOptParams params;
        params._logt = logt;
        params._ev = ev;
        params._rm = this;

        real levmarInfo[LM_INFO_SZ];
        if(work.size() < LM_DER_WORKSZ(size*2, size*2))
        {
            work.resize(LM_DER_WORKSZ(size*2, size*2));
        }

        assert(sizeof(double) == sizeof(complex)/2);//хак Complex[n] используется как double[n*2]


//         TVReal err(size*2);
//         params._ev = NULL;
//         dlevmar_chkjac(
//             evalFuncOpt,
//             jacoFuncOpt,
//             (real *)sv,
//             size*2,
//             size*2,
//             &params,
//             &err.front());
//         real errVal = std::accumulate(err.begin(), err.end(), 0.0)/err.size();
//         std::cout<<"dlevmar_chkjac: "<<errVal<<std::endl;
//         exit(0);

        real levmarOpts[LM_OPTS_SZ];
        memcpy(levmarOpts, g_levmarOpts_ResponseModel, sizeof(levmarOpts));

        levmarOpts[0] = mu;
        levmarOpts[1] = 1e-40;
        levmarOpts[2] = 1e-40;
        levmarOpts[3] = 1e-40;

        int res = dlevmar_der(
            &evalFuncOpt,
            &jacoFuncOpt,
            (real *)sv,
            NULL,
            size*2,
            size*2,
            itMax, 
            levmarOpts,
            levmarInfo,
            &work[0],
            NULL,
            &params);

         std::cerr<<"result: "<<res<<std::endl;
         std::cerr<<"||e||_2 at initial p.:"<<levmarInfo[0]<<std::endl;
         std::cerr<<"||e||_2:"<<levmarInfo[1]<<std::endl;
         std::cerr<<"||J^T e||_inf:"<<levmarInfo[2]<<std::endl;
         std::cerr<<"||Dp||_2:"<<levmarInfo[3]<<std::endl;
         std::cerr<<"\\mu/max[J^T J]_ii:"<<levmarInfo[4]<<std::endl;
         std::cerr<<"# iterations:"<<levmarInfo[5]<<std::endl;
         std::cerr<<"reason for terminating:";
         switch(int(levmarInfo[6]+0.5))
         {
         case 1: std::cerr<<" - stopped by small gradient J^T e"<<std::endl;break;
         case 2: std::cerr<<" - stopped by small Dp"<<std::endl;break;
         case 3: std::cerr<<" - stopped by itmax"<<std::endl;break;
         case 4: std::cerr<<" - singular matrix. Restart from current p with increased \\mu"<<std::endl;break;
         case 5: std::cerr<<" - no further error reduction is possible. Restart with increased mu"<<std::endl;break;
         case 6: std::cerr<<" - stopped by small ||e||_2"<<std::endl;break;
         case 7: std::cerr<<" - stopped by invalid (i.e. NaN or Inf) \"func\" values; a user error"<<std::endl;break;
         }
         std::cerr<<"# function evaluations:"<<levmarInfo[7]<<std::endl;
         std::cerr<<"# Jacobian evaluations:"<<levmarInfo[8]<<std::endl;
         //std::cerr<<"# linear systems solved:"<<levmarInfo[9]<<std::endl;
         //exit(1);

        return res;
    }


}
