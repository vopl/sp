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
        if(t<=0)
        {
            rr=ri=ir=ii=0;
            return;
        }

        Window_hann2::kernel<10>(t, rr, ri, ir, ii);
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
        for(int i(0); i<nDiv2/2; i++)
        {
            logt = params->_logt[i];
            re = im = 0;

            for(int j(0); j<mDiv2; j++)
            {
                real slogt = params->_logt[j];
                real sr = p[j*2+0];
                real si = p[j*2+1];

                {
                    //params->_rm->evalVRaw(exp(logt-slogt), rr, ri, ir, ii);
                    Window_hann::kernel<10>(exp(logt-slogt), rr, ri, ir, ii);

                    //assert(std::isfinite(rr) && std::isfinite(ri));
                    //assert(std::isfinite(ir) && std::isfinite(ii));

                    re += sr*rr - si*ri;
                    im += sr*ir - si*ii;
                }
            }

            //assert(std::isfinite(re) && std::isfinite(im));

            hx[i*2+0] = re;
            hx[i*2+1] = im;

            re = im = 0;
            for(int j(0); j<mDiv2; j++)
            {
                real slogt = params->_logt[j];
                real sr = p[j*2+0];
                real si = p[j*2+1];

                {
                    //params->_rm->evalVRaw(exp(logt-slogt), rr, ri, ir, ii);
                    Window_hann2::kernel<10>(exp(logt-slogt), rr, ri, ir, ii);

                    //assert(std::isfinite(rr) && std::isfinite(ri));
                    //assert(std::isfinite(ir) && std::isfinite(ii));

                    re += sr*rr - si*ri;
                    im += sr*ir - si*ii;
                }
            }

            //assert(std::isfinite(re) && std::isfinite(im));

            hx[nDiv2/2*2 + i*2+0] = re;
            hx[nDiv2/2*2 + i*2+1] = im;

        }

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
        real rr1,  ri1,  ir1,  ii1;
        real rr2,  ri2,  ir2,  ii2;

        for(int i(0); i<n/2; i++)
        {
            logt = params->_logt[i/2];

            for(int j(0); j<m/2; j++)
            {
                real slogt = params->_logt[j];
                real sr = p[j*2+0];
                real si = p[j*2+1];

                Window_hann::kernel<10>(exp(logt-slogt), rr1, ri1, ir1, ii1);
                Window_hann2::kernel<10>(exp(logt-slogt), rr2, ri2, ir2, ii2);

                {
                    auto v = complex(sr, si) * complex(rr1, ii1);

                    jx[(i*m)+j*2+0] = v.re();
                    jx[(i*m)+j*2+1] = -v.im();

                    v = complex(sr, si) * complex(rr2, ii2);

                    jx[n/2*m + (i*m)+j*2+0] = v.re();
                    jx[n/2*m + (i*m)+j*2+1] = -v.im();
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


        if(1)
        {
            TVReal err(size);
            params._ev = NULL;
            dlevmar_chkjac(
                        evalFuncOpt,
                        jacoFuncOpt,
                        (real *)sv,
                        size*2/2,
                        size*2,
                        &params,
                        &err.front());
            real errVal = std::accumulate(err.begin(), err.end(), 0.0)/err.size();
            //std::cout<<"dlevmar_chkjac: "<<errVal<<std::endl;

            std::cout<<err[0]<<", "<<err[1]<<", "<<err[2]<<", "<<err[3]<<", "<<std::endl;
            exit(0);
        }

        real levmarOpts[LM_OPTS_SZ];
        memcpy(levmarOpts, g_levmarOpts_ResponseModel, sizeof(levmarOpts));

        levmarOpts[0] = mu;
//        levmarOpts[1] = 1e-20;
//        levmarOpts[2] = 1e-10;
//        levmarOpts[3] = 1e-40;

//        int res = dlevmar_der(
//            &evalFuncOpt,
//            &jacoFuncOpt,
//            (real *)sv,
//            NULL,
//            size*2/2,
//            size*2,
//            itMax,
//            levmarOpts,
//            levmarInfo,
//            &work[0],
//            NULL,
//            &params);

        int res = dlevmar_dif(
            &evalFuncOpt,
            (real *)sv,
            NULL,
            size*2/2,
            size*2,
            itMax,
            levmarOpts,
            levmarInfo,
            &work[0],
            NULL,
            &params);

//        std::cout<<"result: "<<res<<std::endl;
//        std::cout<<"||e||_2 at initial p.:"<<levmarInfo[0]<<std::endl;
//        std::cout<<"||e||_2:"<<levmarInfo[1]<<std::endl;
//        std::cout<<"||J^T e||_inf:"<<levmarInfo[2]<<std::endl;
//        std::cout<<"||Dp||_2:"<<levmarInfo[3]<<std::endl;
//        std::cout<<"\\mu/max[J^T J]_ii:"<<levmarInfo[4]<<std::endl;
//        std::cout<<"# iterations:"<<levmarInfo[5]<<std::endl;
//        std::cout<<"reason for terminating:";
//        switch(int(levmarInfo[6]+0.5))
//        {
//        case 1: std::cout<<" - stopped by small gradient J^T e"<<std::endl;break;
//        case 2: std::cout<<" - stopped by small Dp"<<std::endl;break;
//        case 3: std::cout<<" - stopped by itmax"<<std::endl;break;
//        case 4: std::cout<<" - singular matrix. Restart from current p with increased \\mu"<<std::endl;break;
//        case 5: std::cout<<" - no further error reduction is possible. Restart with increased mu"<<std::endl;break;
//        case 6: std::cout<<" - stopped by small ||e||_2"<<std::endl;break;
//        case 7: std::cout<<" - stopped by invalid (i.e. NaN or Inf) \"func\" values; a user error"<<std::endl;break;
//        }
//        std::cout<<"# function evaluations:"<<levmarInfo[7]<<std::endl;
//        std::cout<<"# Jacobian evaluations:"<<levmarInfo[8]<<std::endl;
//        std::cout<<"# linear systems solved:"<<levmarInfo[9]<<std::endl;


//        exit(0);

        return res;
    }


}
