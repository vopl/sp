#include "kernel.hpp"
#include "math.hpp"
#include "levmar.h"
#include <iostream>

using namespace sp;

namespace
{

    //////////////////////////////////////////////////////////////////////////
    void evalKernelPart(real pow,
        real t, real tw, real ts, real &rr, real &ri, real &ir, real &ii)
    {
        using TReal = real;
        TReal pi = g_pi;

        TReal t_p_2 = t*t;

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
        TReal _12 = sp::cos ( _7 ) ;
        TReal _10 = sp::sin ( _7 ) ;
        TReal _13 = sp::cos ( _8 ) ;
        TReal _11 = sp::sin ( _8 ) ;
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

        if(sp::fabs(_9)<1e-7)
        {
            TReal _0 = sp::sin ( 4*pi*pow*tw ) ;
            TReal _1 =  ( 8*pi*pow*ts ) ;
            TReal _2 = sp::cos ( 4*pi*pow*tw ) ;
            TReal _4 =  ( _2-1 ) ;
            TReal _3 =  ( _0+4*pi*pow*tw ) ;
            TReal _5 =  ( _0-4*pi*pow*tw ) ;
            //expr

            rr=_3/_1;ri=_4/_1;ir=-_4/_1;ii=_5/_1;
        }
    }


    //////////////////////////////////////////////////////////////////////////
    void evalKernel(real pow,
        real t, real &rr, real &ri, real &ir, real &ii)
    {
        if(t<=0)
        {
            rr=ri=ir=ii=0;
            return;
        }

        rr=ri=ir=ii=0;
        real ts = t>1?1:t;
        real mp = (1.0+0.5/pow);
        real mm = (1.0-0.5/pow);

        real rr_1,ri_1,ir_1,ii_1;
        evalKernelPart(pow, t/mp, t, ts, rr_1, ri_1, ir_1, ii_1);

        real rr_2,ri_2,ir_2,ii_2;
        evalKernelPart(pow, t/mm, t, ts, rr_2, ri_2, ir_2, ii_2);

        rr += (rr_1*mp - rr_2*mm);
        ri += (ri_1*mp - ri_2*mm);
        ir += (ir_1*mp - ir_2*mm);
        ii += (ii_1*mp - ii_2*mm);
    }


    //////////////////////////////////////////////////////////////////////////
    struct LevmarParams
    {
        real            _pow;

        //сетка периода, должна быть задана всегда
        //отклика - длина n/2
        const real      *_et;

        //спектра - длина m/2
        const real      *_st;

        //значения отклика, если задано
        //длина n/2
        const complex   *_ev;
    };

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void evalLevmarFunc(double *p, double *hx, int m, int n, void *_levmarParams)
    {
        LevmarParams *params = (LevmarParams *)_levmarParams;

        for(int i(0); i<n/2; i++)
        {
            real et = params->_et[i];
            real re = 0;
            real im = 0;

            for(int j(0); j<m/2; j++)
            {
                real st = params->_st[j];

                real rr, ri, ir, ii;
                evalKernel(params->_pow, et/st, rr, ri, ir, ii);

                //assert(std::isfinite(rr) && std::isfinite(ri));
                //assert(std::isfinite(ir) && std::isfinite(ii));

                real sr = p[j*2+0];
                real si = p[j*2+1];

                re += sr*rr - si*ri;
                im += sr*ir - si*ii;
            }

            //assert(std::isfinite(re) && std::isfinite(im));

            hx[i*2+0] = double(re);
            hx[i*2+1] = double(im);

        }

        for(int i(0); i<n/2; i++)
        {
            hx[i*2+0] -= double(params->_ev[i].re());
            hx[i*2+1] -= double(params->_ev[i].im());
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void evalLevmarJaco(double *p, double *jx, int m, int n, void *_levmarParams)
    {
        LevmarParams *params = (LevmarParams *)_levmarParams;

        for(int i(0); i<n/2; i++)
        {
            real et = params->_et[i];

            for(int j(0); j<m/2; j++)
            {
                real st = params->_st[j];

                real rr,  ri,  ir,  ii;
                evalKernel(params->_pow, et/st, rr, ri, ir, ii);

                jx[(i*2+0)*m+j*2+0] = double(rr);
                jx[(i*2+0)*m+j*2+1] = double(-ri);

                jx[(i*2+1)*m+j*2+0] = double(ir);
                jx[(i*2+1)*m+j*2+1] = double(-ii);
            }
        }
    }
}

namespace sp
{
    Kernel::Kernel(real pow)
        : _pow(pow)
    {
    }

    Kernel::~Kernel()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    complex Kernel::eval(real t, real st, const complex &sv)
    {
        real rr,ri, ir, ii;
        evalKernel(_pow, t/st, rr,ri, ir, ii);
        return complex(sv.re()*rr - sv.im()*ri, sv.re()*ir - sv.im()*ii);
    }

    //////////////////////////////////////////////////////////////////////////
    int Kernel::deconvolve(
        size_t esize, const real *et, const complex *ev,//отклик
        size_t ssize, const real *st,       complex *sv,//спектр
        size_t itMax,//макс итераций
        std::vector<double> &work)
    {
        LevmarParams params;
        params._et = et;
        params._ev = ev;
        params._st = st;
        params._pow = _pow;

        double levmarInfo[LM_INFO_SZ];
        if(work.size() < LM_DER_WORKSZ(ssize*2, esize*2))
        {
            work.resize(LM_DER_WORKSZ(ssize*2, esize*2));
        }

        assert(sizeof(double) == sizeof(complex)/2);//хак Complex[n] используется как double[n*2]


//         TVReal err(size*2);
//         params._ev = NULL;
//         dlevmar_chkjac(
//             evalLevmarFunc,
//             evalLevmarJaco,
//             (real *)sv,
//             ssize*2,
//             esize*2,
//             &params,
//             &err.front());
//         real errVal = std::accumulate(err.begin(), err.end(), 0.0)/err.size();
//         std::cout<<"dlevmar_chkjac: "<<errVal<<std::endl;
//         exit(0);

        static double levmarOpts[LM_OPTS_SZ] =
        {
            1e-3,  //LM_INIT_MU,        //mu
            1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
            1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
            1e-20,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
        };

        std::vector<double> d_sv(ssize*2);
        for(std::size_t i(0); i<ssize; ++i)
        {
            d_sv[i*2+0] = double(sv[i].re());
            d_sv[i*2+1] = double(sv[i].im());
        }

        int res = dlevmar_der(
            &evalLevmarFunc,
            &evalLevmarJaco,
            &d_sv[0],
            NULL,
            ssize*2,
            esize*2,
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
         std::cerr<<"# linear systems solved:"<<levmarInfo[9]<<std::endl;
         //exit(1);

         for(std::size_t i(0); i<ssize; ++i)
         {
             sv[i] = complex(d_sv[i*2+0], d_sv[i*2+1]);
         }

        return res;
    }


}
