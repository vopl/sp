#include "responseModel.h"
#include "math.h"
#include "levmar.h"
#include <iostream>
#include <fstream>



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

using namespace hel;
//////////////////////////////////////////////////////////////////////////
void lsq(size_t cols, size_t rows, TVReal &A, TVReal &b, TVReal &x)
{
    x.resize(cols);
    int res;

    res = dAx_eq_b_QRLS(&A[0], &b[0], &x[0], rows, cols);
    //          res = dAx_eq_b_SVD2(&A[0], &b[0], &x[0], rows, cols);
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


namespace hel
{
    //////////////////////////////////////////////////////////////////////////
    ResponseModel::ResponseModel()
    {

    }

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
            return c_pi;
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


    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::setup(
        real maxT,
        size_t stepsT_l1,
        size_t stepsT_ge1,
        size_t pow)
    {
        _pow = pow;

        //////////////////////////////////////////////////////////////////////////
        //t<1
        _l1_t.resize(stepsT_l1);
        _l1_vds.resize(stepsT_l1);
        _l1_tStep = 1.0 / (stepsT_l1-1);

        for(size_t i(0); i<stepsT_l1-1; i++)
        {
            real tm,t,tp;
            t = (i)*_l1_tStep;
            tm = i?(i-1)*_l1_tStep:t;
            tp = (i+1)*_l1_tStep;
            _l1_t[i] = t;
            evalVDRaw(tm,t,tp,
                _l1_vds[i]._rr,
                _l1_vds[i]._ri,
                _l1_vds[i]._ir,
                _l1_vds[i]._ii,
                _l1_vds[i]._drr,
                _l1_vds[i]._dri,
                _l1_vds[i]._dir,
                _l1_vds[i]._dii);
        }
        _l1_t[stepsT_l1-1] = 1;
        evalVDRaw(_l1_t[stepsT_l1-2], _l1_t[stepsT_l1-1], (stepsT_l1)*_l1_tStep,
            _l1_vds[stepsT_l1-1]._rr,
            _l1_vds[stepsT_l1-1]._ri,
            _l1_vds[stepsT_l1-1]._ir,
            _l1_vds[stepsT_l1-1]._ii,
            _l1_vds[stepsT_l1-1]._drr,
            _l1_vds[stepsT_l1-1]._dri,
            _l1_vds[stepsT_l1-1]._dir,
            _l1_vds[stepsT_l1-1]._dii);

        //////////////////////////////////////////////////////////////////////////
        //t>=1
        _ge1_loglogt.resize(stepsT_ge1);
        _ge1_vds.resize(stepsT_ge1);
        real maxTLog = log(log(maxT)+1);
        _ge1_loglogtStep = maxTLog / stepsT_ge1;

        _ge1_loglogt[0] = 0;
        _ge1_vds[0]._rr=_l1_vds[stepsT_l1-1]._rr;
        _ge1_vds[0]._ri=_l1_vds[stepsT_l1-1]._ri;
        _ge1_vds[0]._ir=_l1_vds[stepsT_l1-1]._ir;
        _ge1_vds[0]._ii=_l1_vds[stepsT_l1-1]._ii;
        _ge1_vds[0]._drr=_l1_vds[stepsT_l1-1]._drr;
        _ge1_vds[0]._dri=_l1_vds[stepsT_l1-1]._dri;
        _ge1_vds[0]._dir=_l1_vds[stepsT_l1-1]._dir;
        _ge1_vds[0]._dii=_l1_vds[stepsT_l1-1]._dii;

        for(size_t i(1); i<stepsT_ge1; i++)
        {
            _ge1_loglogt[i] = (i)*_ge1_loglogtStep;

            real tm,t,tp;
            t = exp(exp(_ge1_loglogt[i])-1);
            tm = exp(exp(_ge1_loglogt[i-1])-1);
            tp = exp(exp((i+1)*_ge1_loglogtStep)-1);

            evalVDRaw(tm,t,tp,
                _ge1_vds[i]._rr,
                _ge1_vds[i]._ri,
                _ge1_vds[i]._ir,
                _ge1_vds[i]._ii,
                _ge1_vds[i]._drr,
                _ge1_vds[i]._dri,
                _ge1_vds[i]._dir,
                _ge1_vds[i]._dii);
        }
    }



    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::evalVRaw(
        real t, real &rr, real &ri, real &ir, real &ii)
    {
        if(t<=0)
        {
            rr=ri=ir=ii=0;
            return;
        }

        rr=ri=ir=ii=0;
        real ts = t>1?1:t;
        real mp = (1.0+0.5/_pow);
        real mm = (1.0-0.5/_pow);
        real rr_1,ri_1,ir_1,ii_1;
        ResponseModel_impl::eval_simple<real>(_pow, t/mp, t, ts, rr_1, ri_1, ir_1, ii_1);
        //ResponseModel_impl::eval_cosian<real>(_pow, t/mp, t, ts, rr_1, ri_1, ir_1, ii_1);

        real rr_2,ri_2,ir_2,ii_2;
        ResponseModel_impl::eval_simple<real>(_pow, t/mm, t, ts, rr_2, ri_2, ir_2, ii_2);
        //ResponseModel_impl::eval_cosian<real>(_pow, t/mm, t, ts, rr_2, ri_2, ir_2, ii_2);
        rr += (rr_1*mp - rr_2*mm);
        ri += (ri_1*mp - ri_2*mm);
        ir += (ir_1*mp - ir_2*mm);
        ii += (ii_1*mp - ii_2*mm);
    }


//     //////////////////////////////////////////////////////////////////////////
//     void ResponseModel::evalVRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii)
//     {
//         rr.resize(t.size());
//         ri.resize(t.size());
//         ir.resize(t.size());
//         ii.resize(t.size());
//         for(size_t i(0); i<t.size(); i++)
//         {
//             evalVRaw(t[i], rr[i], ri[i], ir[i], ii[i]);
//         }
//     }

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

//         drr = (rrp-rrm) / (ltp-ltm);
//         dri = (rip-rim) / (ltp-ltm);
//         dir = (irp-irm) / (ltp-ltm);
//         dii = (iip-iim) / (ltp-ltm);
        //

//         {
//             delta = lt-ltm;
//             real rrm3, rim3, irm3, iim3;
//             evalVRaw(exp(lt-3*delta), rrm3, rim3, irm3, iim3);
//             real rrm2, rim2, irm2, iim2;
//             evalVRaw(exp(lt-2*delta), rrm2, rim2, irm2, iim2);
//             real rrm1, rim1, irm1, iim1;
//             evalVRaw(exp(lt-1*delta), rrm1, rim1, irm1, iim1);
//             real rrm0, rim0, irm0, iim0;
//             evalVRaw(exp(lt-0*delta), rrm0, rim0, irm0, iim0);
//             real rrp1, rip1, irp1, iip1;
//             evalVRaw(exp(lt+1*delta), rrp1, rip1, irp1, iip1);
//             real rrp2, rip2, irp2, iip2;
//             evalVRaw(exp(lt+2*delta), rrp2, rip2, irp2, iip2);
//             real rrp3, rip3, irp3, iip3;
//             evalVRaw(exp(lt+3*delta), rrp3, rip3, irp3, iip3);
//
//             drr = (rrp3-9*rrp2+45*rrp1-45*rrm1+9*rrm2-rrm3)/(60*delta);
//             dri = (rip3-9*rip2+45*rip1-45*rim1+9*rim2-rim3)/(60*delta);
//             dir = (irp3-9*irp2+45*irp1-45*irm1+9*irm2-irm3)/(60*delta);
//             dii = (iip3-9*iip2+45*iip1-45*iip1+9*iim2-iim3)/(60*delta);
//         }
    }

//     //////////////////////////////////////////////////////////////////////////
//     void ResponseModel::evalDRaw(const TVReal &t, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii)
//     {
//         drr.resize(t.size());
//         dri.resize(t.size());
//         dir.resize(t.size());
//         dii.resize(t.size());
//         for(size_t i(0); i<t.size(); i++)
//         {
//             real rr,ri, ir, ii;
//             evalVDRaw(t[i], rr, ri, ir, ii, drr[i], dri[i], dir[i], dii[i]);
//         }
//     }
//
//     //////////////////////////////////////////////////////////////////////////
//     void ResponseModel::evalVDRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii)
//     {
//         rr.resize(t.size());
//         ri.resize(t.size());
//         ir.resize(t.size());
//         ii.resize(t.size());
//         drr.resize(t.size());
//         dri.resize(t.size());
//         dir.resize(t.size());
//         dii.resize(t.size());
//         for(size_t i(0); i<t.size(); i++)
//         {
//             evalVDRaw(t[i], rr[i], ri[i], ir[i], ii[i], drr[i], dri[i], dir[i], dii[i]);
//         }
//     }

    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::evalVRaw_tabled(
        real logt,
        real &rr, real &ri, real &ir, real &ii)
    {
        real key;
        real step;
        TVReal *pkeys;
        TVVDS *pvalues;
        if(logt < 0)
        {
            key = exp(logt);
            pkeys = &_l1_t;
            pvalues = &_l1_vds;
            step = _l1_tStep;
        }
        else
        {
            key = log(logt+1);
            pkeys = &_ge1_loglogt;
            pvalues = &_ge1_vds;
            step = _ge1_loglogtStep;

            if(key>=_ge1_loglogt.back())
            {
                rr=ri=ir=ii=0;
                return;
            }
        }

        TVReal &keys(*pkeys);
        TVVDS &values(*pvalues);

        size_t steps = keys.size();
        size_t idx = size_t((key - keys[0]) / step+1);
        while(idx >= steps)
        {
            idx--;
        }

        while(keys[idx] < key)
        {
            idx++;
        }
        while(keys[idx-1] > key)
        {
            idx--;
        }

        if(key != keys[idx])
        {
            real t1 = keys[idx-1];
            real t2 = keys[idx];
            real w = (key-t1)/(t2-t1);
            real oneMinusW = 1-w;

            VDS &vds1 = values[idx-1];
            VDS &vds2 = values[idx];
            rr = (vds1._rr*oneMinusW + vds2._rr*w);
            ri = (vds1._ri*oneMinusW + vds2._ri*w);
            ir = (vds1._ir*oneMinusW + vds2._ir*w);
            ii = (vds1._ii*oneMinusW + vds2._ii*w);
        }
        else
        {
            VDS &vds1 = values[idx];
            rr = vds1._rr;
            ri = vds1._ri;
            ir = vds1._ir;
            ii = vds1._ii;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::evalVDRaw_tabled(
        real logt,
        real &rr, real &ri, real &ir, real &ii,
        real &drr, real &dri, real &dir, real &dii)
    {
        real key;
        real step;
        TVReal *pkeys;
        TVVDS *pvalues;
        if(logt < 0)
        {
            key = exp(logt);
            pkeys = &_l1_t;
            pvalues = &_l1_vds;
            step = _l1_tStep;
        }
        else
        {
            key = log(logt+1);
            pkeys = &_ge1_loglogt;
            pvalues = &_ge1_vds;
            step = _ge1_loglogtStep;

            if(key>=_ge1_loglogt.back())
            {
                rr=ri=ir=ii=0;
                drr=dri=dir=dii=0;
                return;
            }
        }

        TVReal &keys(*pkeys);
        TVVDS &values(*pvalues);

        size_t steps = keys.size();
        size_t idx = size_t((key - keys[0]) / step+1);
        while(idx >= steps)
        {
            idx--;
        }

        while(keys[idx] < key)
        {
            idx++;
        }
        while(keys[idx-1] > key)
        {
            idx--;
        }

        if(key != keys[idx])
        {
            real t1 = keys[idx-1];
            real t2 = keys[idx];
            real w = (key-t1)/(t2-t1);
            real oneMinusW = 1-w;

            VDS &vds1 = values[idx-1];
            VDS &vds2 = values[idx];
            rr = (vds1._rr*oneMinusW + vds2._rr*w);
            ri = (vds1._ri*oneMinusW + vds2._ri*w);
            ir = (vds1._ir*oneMinusW + vds2._ir*w);
            ii = (vds1._ii*oneMinusW + vds2._ii*w);
            drr = (vds1._drr*oneMinusW + vds2._drr*w);
            dri = (vds1._dri*oneMinusW + vds2._dri*w);
            dir = (vds1._dir*oneMinusW + vds2._dir*w);
            dii = (vds1._dii*oneMinusW + vds2._dii*w);
        }
        else
        {
            VDS &vds1 = values[idx];
            rr = vds1._rr;
            ri = vds1._ri;
            ir = vds1._ir;
            ii = vds1._ii;
            drr = vds1._drr;
            dri = vds1._dri;
            dir = vds1._dir;
            dii = vds1._dii;
        }    }

    //////////////////////////////////////////////////////////////////////////
    Complex ResponseModel::eval(real t, real st, const Complex &sv)
    {
        real rr,ri, ir, ii;
        evalVRaw(t/st, rr,ri, ir, ii);
        return Complex(sv.re()*rr - sv.im()*ri, sv.re()*ir - sv.im()*ii);
    }

    //////////////////////////////////////////////////////////////////////////
    Complex ResponseModel::eval_tabled(real logt, real slogt, const Complex &sv)
    {
        real rr,ri, ir, ii;
        evalVRaw_tabled(logt-slogt, rr,ri, ir, ii);
        return Complex(sv.re()*rr - sv.im()*ri, sv.re()*ir - sv.im()*ii);
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

        if(params->_slogt)
        {//значения периода спектра. Если задано то период спектра НЕ оптимизируется
            int mDiv2 = m/2;
            for(int i(0); i<nDiv2; i++)
            {
                logt = params->_elogt[i];
                re = im = 0;

                for(int j(0); j<mDiv2; j++)
                {
                    real slogt = params->_slogt[j];
                    real sr = p[j*2+0];
                    real si = p[j*2+1];

                    //assert(!"вычисления переделать на таблицу");
                    //params->_rm->evalVRaw(exp(logt-slogt), rr, ri, ir, ii);
                    params->_rm->evalVRaw_tabled(logt-slogt, rr, ri, ir, ii);

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
                logt = params->_elogt[i];
                re = im = 0;

                for(int j(0); j<mDiv3; j++)
                {
                    real slogt = p[j*3+0];
                    real sr = p[j*3+1];
                    real si = p[j*3+2];

                    //assert(!"вычисления переделать на таблицу");
                    //params->_rm->evalVRaw(exp(logt-slogt), rr, ri, ir, ii);
                    params->_rm->evalVRaw_tabled(logt-slogt, rr, ri, ir, ii);

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
    void ResponseModel::jacoFuncOpt(double *p, double *jx, int m, int n, void *_SFuncOptParams)
    {
        SFuncOptParams *params = (SFuncOptParams *)_SFuncOptParams;

        real logt;
        real rr,  ri,  ir,  ii;
        int nDiv2 = n/2;

        if(params->_slogt)
        {//значения периода спектра. Если задано то период спектра НЕ оптимизируется
            int mDiv2 = m/2;
            for(int i(0); i<nDiv2; i++)
            {
                logt = params->_elogt[i];

                for(int j(0); j<mDiv2; j++)
                {
                    real slogt = params->_slogt[j];
                    real sr = p[j*2+0];
                    real si = p[j*2+1];

                    //assert(!"вычисления переделать на таблицу");
                    //params->_rm->evalVRaw(exp(logt-slogt), rr, ri, ir, ii);
                    params->_rm->evalVRaw_tabled(logt-slogt, rr, ri, ir, ii);

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
            for(int i(0); i<nDiv2; i++)
            {
                logt = params->_elogt[i];

                for(int j(0); j<mDiv3; j++)
                {
                    real slogt = p[j*3+0];
                    real sr = p[j*3+1];
                    real si = p[j*3+2];

                    //assert(!"вычисления переделать на таблицу");
                    //params->_rm->evalVDRaw(exp(logt-slogt), rr, ri, ir, ii, drr, dri, dir, dii);
                    params->_rm->evalVDRaw_tabled(logt-slogt, rr, ri, ir, ii, drr, dri, dir, dii);


                    jx[(i*2+0)*m+j*3+0] = si*dri - sr*drr;
                    jx[(i*2+0)*m+j*3+1] = rr;
                    jx[(i*2+0)*m+j*3+2] = -ri;

                    jx[(i*2+1)*m+j*3+0] = si*dii - sr*dir;
                    jx[(i*2+1)*m+j*3+1] = ir;
                    jx[(i*2+1)*m+j*3+2] = -ii;
                }
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
    int ResponseModel::solve_tv(
        size_t eSize, const real *elogt, const Complex *ev,//отклик
        size_t sSize,       real *slogt,       Complex *sv,//спектр
        size_t itMax,//макс итераций
        TVReal &work)
    {
        SFuncOptParams params;
        params._elogt = elogt;
        params._ev = ev;
        params._slogt = NULL;
        params._rm = this;

        real levmarInfo[LM_INFO_SZ];
        if(work.size() < LM_DER_WORKSZ(sSize*3, eSize*2))
        {
            work.resize(LM_DER_WORKSZ(sSize*3, eSize*2));
        }

        TVReal args(sSize*3);
        for(size_t i(0); i<sSize; i++)
        {
            args[i*3+0] = slogt[i];
            args[i*3+1] = sv[i].re();
            args[i*3+2] = sv[i].im();
        }


//         TVReal err(eSize*2);
//         params._ev=NULL;
//         dlevmar_chkjac(
//             evalFuncOpt,
//             jacoFuncOpt,
//             &args[0],
//             sSize*3,
//             eSize*2,
//             &params,
//             &err.front());
//         real errVal = std::accumulate(err.begin(), err.end(), 0.0)/err.size();
//         std::cout<<"dlevmar_chkjac: "<<errVal<<std::endl;
//         exit(0);

        real levmarOpts[LM_OPTS_SZ];
        memcpy(levmarOpts, g_levmarOpts_ResponseModel, sizeof(levmarOpts));

        int res = dlevmar_der(
            &evalFuncOpt,
            &jacoFuncOpt,
            &args[0],
            NULL,
            sSize*3,
            eSize*2,
            itMax,
            levmarOpts,
            levmarInfo,
            &work[0],
            NULL,
            &params);

        for(size_t i(0); i<sSize; i++)
        {
            slogt[i] = args[i*3+0];
            sv[i].re() = args[i*3+1];
            sv[i].im() = args[i*3+2];
        }

        return res;
    }

    //////////////////////////////////////////////////////////////////////////
    int ResponseModel::solve_v(
        size_t eSize, const real *elogt, const Complex *ev,//отклик
        size_t sSize, const real *slogt,       Complex *sv,//спектр
        size_t itMax,//макс итераций
        TVReal &work)
    {
        SFuncOptParams params;
        params._elogt = elogt;
        params._ev = ev;
        params._slogt = slogt;
        params._rm = this;

        real levmarInfo[LM_INFO_SZ];
        if(work.size() < LM_DER_WORKSZ(sSize*2, eSize*2))
        {
            work.resize(LM_DER_WORKSZ(sSize*2, eSize*2));
        }

        assert(sizeof(double) == sizeof(Complex)/2);//хак Complex[n] используется как double[n*2]


//         TVReal err(eSize*2);
//         params._ev = NULL;
//         dlevmar_chkjac(
//             evalFuncOpt,
//             jacoFuncOpt,
//             (real *)sv,
//             sSize*2,
//             eSize*2,
//             &params,
//             &err.front());
//         real errVal = std::accumulate(err.begin(), err.end(), 0.0)/err.size();
//         std::cout<<"dlevmar_chkjac: "<<errVal<<std::endl;
//         exit(0);

        real levmarOpts[LM_OPTS_SZ];
        memcpy(levmarOpts, g_levmarOpts_ResponseModel, sizeof(levmarOpts));

        levmarOpts[0] = 1e-30;
        levmarOpts[1] = 1e-20;
        levmarOpts[2] = 1e-10;
        levmarOpts[3] = 1e-40;

        int res = dlevmar_der(
            &evalFuncOpt,
            &jacoFuncOpt,
            (real *)sv,
            NULL,
            sSize*2,
            eSize*2,
            itMax,
            levmarOpts,
            levmarInfo,
            &work[0],
            NULL,
            &params);

//         std::cout<<"result: "<<res<<std::endl;
//         std::cout<<"||e||_2 at initial p.:"<<levmarInfo[0]<<std::endl;
//         std::cout<<"||e||_2:"<<levmarInfo[1]<<std::endl;
//         std::cout<<"||J^T e||_inf:"<<levmarInfo[2]<<std::endl;
//         std::cout<<"||Dp||_2:"<<levmarInfo[3]<<std::endl;
//         std::cout<<"\\mu/max[J^T J]_ii:"<<levmarInfo[4]<<std::endl;
//         std::cout<<"# iterations:"<<levmarInfo[5]<<std::endl;
//         std::cout<<"reason for terminating:";
//         switch(int(levmarInfo[6]+0.5))
//         {
//         case 1: std::cout<<" - stopped by small gradient J^T e"<<std::endl;break;
//         case 2: std::cout<<" - stopped by small Dp"<<std::endl;break;
//         case 3: std::cout<<" - stopped by itmax"<<std::endl;break;
//         case 4: std::cout<<" - singular matrix. Restart from current p with increased \\mu"<<std::endl;break;
//         case 5: std::cout<<" - no further error reduction is possible. Restart with increased mu"<<std::endl;break;
//         case 6: std::cout<<" - stopped by small ||e||_2"<<std::endl;break;
//         case 7: std::cout<<" - stopped by invalid (i.e. NaN or Inf) \"func\" values; a user error"<<std::endl;break;
//         }
//         std::cout<<"# function evaluations:"<<levmarInfo[7]<<std::endl;
//         std::cout<<"# Jacobian evaluations:"<<levmarInfo[8]<<std::endl;
//         std::cout<<"# linear systems solved:"<<levmarInfo[9]<<std::endl;

        return res;
    }

    //////////////////////////////////////////////////////////////////////////
    void ResponseModel::dump(const char *fname)
    {
        std::ofstream out(fname);
        out.precision(16);
        out.setf(std::ios::scientific);

        for(size_t i(0); i<_l1_t.size(); i++)
        {
            out<<_l1_t[i]<<"\t";
            out<<_l1_vds[i]._rr<<"\t";
            out<<_l1_vds[i]._ri<<"\t";
            out<<_l1_vds[i]._ir<<"\t";
            out<<_l1_vds[i]._ii<<"\t";
            out<<_l1_vds[i]._drr<<"\t";
            out<<_l1_vds[i]._dri<<"\t";
            out<<_l1_vds[i]._dir<<"\t";
            out<<_l1_vds[i]._dii<<"\t";
            out<<std::endl;
        }
        for(size_t i(0); i<_ge1_loglogt.size(); i++)
        {
            out<<exp(exp(_ge1_loglogt[i])-1)<<"\t";
            out<<_ge1_vds[i]._rr<<"\t";
            out<<_ge1_vds[i]._ri<<"\t";
            out<<_ge1_vds[i]._ir<<"\t";
            out<<_ge1_vds[i]._ii<<"\t";
            out<<_ge1_vds[i]._drr<<"\t";
            out<<_ge1_vds[i]._dri<<"\t";
            out<<_ge1_vds[i]._dir<<"\t";
            out<<_ge1_vds[i]._dii<<"\t";
            out<<std::endl;
        }
    }

}

int f()
{
    ResponseModel rm;

    TVReal _optimizerWeights;
    TVSize_t _optimizerPows;
    _optimizerPows.push_back(10); _optimizerWeights.push_back(1);
    _optimizerPows.push_back(5); _optimizerWeights.push_back(-2);

    rm.setup(
        1e2,    // real maxT,
        1000,    // size_t stepsT_l1,
        1000,    // size_t stepsT_ge1,
        5);    //pow


    rm.dump("out");

    std::ofstream out("model");
    out.precision(16);
    out.setf(std::ios::scientific);

    real midTlog = log(60*60.0);
    real minTlog = log(60.0) - midTlog;
    real maxTlog = log(60*60*24*5.0) - midTlog;
    size_t stepsPeriod = 884;
    real stepTlog = (maxTlog - minTlog)/stepsPeriod;

    for(size_t i(0); i<stepsPeriod; i++)
    {
        real t = exp(minTlog +i*stepTlog);
        Complex v = rm.eval_tabled(log(t), log(1), Complex(1,0, CAP));
        out<<t*60*60<<"\t";
        out<<v.re()<<"\t";
        out<<v.im()<<"\n";
    }
    out.flush();



    exit(0);
    return rand();
}
//static int i=f();
