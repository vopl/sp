#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"
#include "levmar.h"

#include <iostream>
#include <fstream>

namespace sp
{
    KernelTabled::KernelTabled()
    {

    }

    KernelTabled::~KernelTabled()
    {

    }

    void KernelTabled::setup(real pow, real periodMin, real periodMax, std::size_t periodSteps)
    {
        _pow = pow;
        _periodMin = periodMin;
        _periodMax = periodMax;
        _periodSteps = periodSteps;
        _periodStep = (_periodMax - _periodMin) / (_periodSteps-1);

        if(!load())
        {
            build();
            save();
        }
    }

    complex KernelTabled::eval(real t, real st, const complex &sv)
    {
        real rr,ri, ir, ii;
        evalKernel(t/st, rr,ri, ir, ii);
        return complex(sv.re()*rr - sv.im()*ri, sv.re()*ir - sv.im()*ii);
    }

    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct LevmarParams
        {
            //сетка периода, должна быть задана всегда
            //отклика - длина n/2
            const real      *_et;

            //спектра - длина m/2
            const real      *_st;

            //значения отклика, если задано
            //длина n/2
            const complex   *_ev;

            KernelTabled    *_kernelTabled;
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
                    params->_kernelTabled->evalKernel(et/st, rr, ri, ir, ii);

                    //assert(std::isfinite(rr) && std::isfinite(ri));
                    //assert(std::isfinite(ir) && std::isfinite(ii));

                    real sr = p[j*2+0];
                    real si = p[j*2+1];

                    re += sr*rr - si*ri;
                    im += sr*ir - si*ii;
                }

                //assert(std::isfinite(re) && std::isfinite(im));

                hx[i*2+0] = re;
                hx[i*2+1] = im;

            }

            for(int i(0); i<n/2; i++)
            {
                hx[i*2+0] -= params->_ev[i].re();
                hx[i*2+1] -= params->_ev[i].im();
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
                    params->_kernelTabled->evalKernel(et/st, rr, ri, ir, ii);

                    jx[(i*2+0)*m+j*2+0] = rr;
                    jx[(i*2+0)*m+j*2+1] = -ri;

                    jx[(i*2+1)*m+j*2+0] = ir;
                    jx[(i*2+1)*m+j*2+1] = -ii;
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    int KernelTabled::deconvolve(
        size_t esize, const real *et, const complex *ev,//отклик
        size_t ssize, const real *st,       complex *sv,//спектр
        size_t itMax,//макс итераций
        TVReal &work)
    {
        LevmarParams params;
        params._et = et;
        params._ev = ev;
        params._st = st;
        params._kernelTabled = this;

        real levmarInfo[LM_INFO_SZ];
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

        static real levmarOpts[LM_OPTS_SZ] =
        {
            1e-30,  //LM_INIT_MU,        //mu
            1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
            1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
            1e-20,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
        };

        int res = dlevmar_der(
            &evalLevmarFunc,
            &evalLevmarJaco,
            (real *)sv,
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

        return res;
    }

    namespace
    {
        real approx01(real x, real y0, real dy0, real y1, real dy1)
        {
            assert(x>=0 && x<=1);

            //кубическим сплайном по двум точкам с производными
            real x2 = x*x;
            real x3 = x2*x;
            return  (+2*x3 - 3*x2 + 1)*y0 +
                    (+1*x3 - 2*x2 + x)*dy0 +
                    (-2*x3 + 3*x2 + 0)*y1 +
                    (+1*x3 - 1*x2 + 0)*dy1;


//            //линейно
//            return y0*(1-x) + y1*(x);
        }
    }

    void KernelTabled::evalKernel(real t, real &rr, real &ri, real &ir, real &ii)
    {
        if(t <= _periodMin)
        {
            rr = approx01(t/_periodMin, 0, 0, _kre.front().re(), _kdre.front().re());
            ri = approx01(t/_periodMin, 0, 0, _kre.front().im(), _kdre.front().im());
            ir = approx01(t/_periodMin, 0, 0, _kim.front().re(), _kdim.front().im());
            ii = approx01(t/_periodMin, 0, 0, _kim.front().im(), _kdim.front().im());
            return;
        }

        if(t >= _periodMax*100)
        {
            rr = 0;
            ri = 0;
            ir = 0;
            ii = 0;
            return;
        }

        if(t >= _periodMax)
        {
            rr = approx01((t-_periodMax)/100, _kre.back().re(), _kdre.back().re(), 0, 0);
            ri = approx01((t-_periodMax)/100, _kre.back().im(), _kdre.back().im(), 0, 0);
            ir = approx01((t-_periodMax)/100, _kim.back().re(), _kdim.back().re(), 0, 0);
            ii = approx01((t-_periodMax)/100, _kim.back().im(), _kdim.back().im(), 0, 0);
            return;
        }

        std::size_t idx = (t-_periodMin)/_periodStep;
        assert(idx<_periodSteps-1);

        real x = (t-_periodStep*idx-_periodMin)/_periodStep;
        rr = approx01(x, _kre[idx].re(), _kdre[idx].re(), _kre[idx+1].re(), _kdre[idx+1].re());
        ri = approx01(x, _kre[idx].im(), _kdre[idx].im(), _kre[idx+1].im(), _kdre[idx+1].im());
        ir = approx01(x, _kim[idx].re(), _kdim[idx].re(), _kim[idx+1].re(), _kdim[idx+1].re());
        ii = approx01(x, _kim[idx].im(), _kdim[idx].im(), _kim[idx+1].im(), _kdim[idx+1].im());
    }

    std::string KernelTabled::stateFileName()
    {
        char tmp[4096];
        sprintf(tmp, "kt_state_POW%0.15e_TMIN%0.15e_TMAX%0.15e_TSTEPS%zd.bin", double(_pow), double(_periodMin), double(_periodMax), _periodSteps);

        return tmp;
    }

    bool KernelTabled::load()
    {
        //std::cout<<stateFileName()<<std::endl;

        std::ifstream in(stateFileName().c_str(), std::ios::in|std::ios::binary);
        if(!in)
        {
            return false;
        }

        _kre.resize(_periodSteps);
        _kim.resize(_periodSteps);
        _kdre.resize(_periodSteps);
        _kdim.resize(_periodSteps);

        std::streamsize bytesAmount = std::streamsize(_periodSteps*sizeof(complex));

        in.read(reinterpret_cast<char *>(&_kre[0]), bytesAmount);
        if(in.gcount() != bytesAmount)
        {
            return false;
        }

        in.read(reinterpret_cast<char *>(&_kim[0]), bytesAmount);
        if(in.gcount() != bytesAmount)
        {
            return false;
        }

        in.read(reinterpret_cast<char *>(&_kdre[0]), bytesAmount);
        if(in.gcount() != bytesAmount)
        {
            return false;
        }

        in.read(reinterpret_cast<char *>(&_kdim[0]), bytesAmount);
        if(in.gcount() != bytesAmount)
        {
            return false;
        }
        in.close();
        return true;
    }

    bool KernelTabled::save()
    {
        std::ofstream out(stateFileName().c_str(), std::ios::out|std::ios::binary);
        if(!out)
        {
            std::cerr<<"unable to open file: "<<stateFileName();
            abort();
            return false;
        }

        std::streamsize bytesAmount = std::streamsize(_periodSteps*sizeof(complex));

        out.write(reinterpret_cast<char *>(&_kre[0]), bytesAmount);
        if(out.tellp() != bytesAmount)
        {
            return false;
        }

        out.write(reinterpret_cast<char *>(&_kim[0]), bytesAmount);
        if(out.tellp() != bytesAmount*2)
        {
            return false;
        }

        out.write(reinterpret_cast<char *>(&_kdre[0]), bytesAmount);
        if(out.tellp() != bytesAmount*3)
        {
            return false;
        }

        out.write(reinterpret_cast<char *>(&_kdim[0]), bytesAmount);
        if(out.tellp() != bytesAmount*4)
        {
            return false;
        }
        out.close();
        return true;
    }

    namespace
    {
        complex approxCos(TVReal &ys)
        {
            real levmarInfo[LM_INFO_SZ];
            TVReal work;
            if(work.size() < LM_DIF_WORKSZ(2, ys.size()*2))
            {
                work.resize(LM_DIF_WORKSZ(2, ys.size()*2));
            }

            static real levmarOpts[LM_OPTS_SZ] =
            {
                1e-3,  //LM_INIT_MU,        //mu
                1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
                1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
                1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
            };

            real p[2]={1,0};

            int levmarResult = dlevmar_der(
                        [](double *p, double *hx, int m, int n, void *_levmarParams)->void{
                            for(int i(0); i<n; i++)
                            {
                                hx[i] = p[0]*cos(g_2pi*i/n + p[1]);
                            }
                        },
                        [](double *p, double *jx, int m, int n, void *_levmarParams){
                            for(int i(0); i<n; i++)
                            {
                                jx[i*2+0] = cos(g_2pi*i/n + p[1]);
                                jx[i*2+1] = -p[0]*sin(g_2pi*i/n + p[1]);
                            }
                        },
                        &p[0],
                        &ys[0],
                        2,
                        ys.size(),
                        50,
                        levmarOpts,
                        levmarInfo,
                        &work[0],
                        NULL,
                        NULL);

//            std::cerr<<"result: "<<levmarResult<<std::endl;
//            std::cerr<<"||e||_2 at initial p.:"<<levmarInfo[0]<<std::endl;
//            std::cerr<<"||e||_2:"<<levmarInfo[1]<<std::endl;
//            std::cerr<<"||J^T e||_inf:"<<levmarInfo[2]<<std::endl;
//            std::cerr<<"||Dp||_2:"<<levmarInfo[3]<<std::endl;
//            std::cerr<<"\\mu/max[J^T J]_ii:"<<levmarInfo[4]<<std::endl;
//            std::cerr<<"# iterations:"<<levmarInfo[5]<<std::endl;
//            std::cerr<<"reason for terminating:";
//            switch(int(levmarInfo[6]+0.5))
//            {
//            case 1: std::cerr<<" - stopped by small gradient J^T e"<<std::endl;break;
//            case 2: std::cerr<<" - stopped by small Dp"<<std::endl;break;
//            case 3: std::cerr<<" - stopped by itmax"<<std::endl;break;
//            case 4: std::cerr<<" - singular matrix. Restart from current p with increased \\mu"<<std::endl;break;
//            case 5: std::cerr<<" - no further error reduction is possible. Restart with increased mu"<<std::endl;break;
//            case 6: std::cerr<<" - stopped by small ||e||_2"<<std::endl;break;
//            case 7: std::cerr<<" - stopped by invalid (i.e. NaN or Inf) \"func\" values; a user error"<<std::endl;break;
//            }
//            std::cerr<<"# function evaluations:"<<levmarInfo[7]<<std::endl;
//            std::cerr<<"# Jacobian evaluations:"<<levmarInfo[8]<<std::endl;
//            std::cerr<<"# linear systems solved:"<<levmarInfo[9]<<std::endl;
//            exit(1);

            return complex(p[0],p[1],CAP);
        }
    }

    void KernelTabled::build()
    {
        std::cerr<<"build kernel"<<std::endl;
        _kre.clear();
        _kim.clear();
        _kdre.clear();
        _kdim.clear();

        Convolver c(_pow, _periodMin, _periodMax, _periodSteps);

        real targetX = _periodMax*_pow*2.5;
        const real sampleStep = _periodMin/10;//10 сэмплов на минимальный период
        TVReal signal(std::size_t(targetX/sampleStep)+1000);

        const std::size_t phasesAmount = 30;

        std::vector<TVComplex> echos(phasesAmount);
        for(std::size_t phaseIndex(0); phaseIndex<phasesAmount; ++phaseIndex)
        {
            //fill signal
            std::cerr<<phaseIndex<<" signal"<<std::endl;

//            std::size_t startIdx = (targetX-10.0)/sampleStep;
//            std::size_t stopIdx = (targetX)/sampleStep+1;
            std::size_t startIdx = 0;
            std::size_t stopIdx = signal.size();
            for(std::size_t sindex(startIdx); sindex<stopIdx; sindex++)
            {
                signal[sindex] = cos(g_2pi*(sampleStep*sindex - targetX) + phaseIndex*g_2pi/phasesAmount);
            }

            std::cerr<<phaseIndex<<" echo"<<std::endl;
            //fill echo
            c.execute(0, sampleStep, signal, targetX, echos[phaseIndex]);

            //std::cout<<echos[phaseIndex][10].re()<<", "<<echos[phaseIndex][10].im()<<std::endl;
        }

//        //проверить отклонение, должно быть на уровне машинной точности
//        for(std::size_t i(0); i<periodSteps; ++i)
//        {
//            real re=0, im=0;
//            for(std::size_t phaseIndex(0); phaseIndex<phasesAmount; ++phaseIndex)
//            {
//                re += echos[phaseIndex][i].re();
//                im += echos[phaseIndex][i].im();
//            }
//            std::cout<<re<<", "<<im<<std::endl;
//        }


        _kre.resize(_periodSteps);
        _kim.resize(_periodSteps);

        TVReal hre(phasesAmount), him(phasesAmount);
        for(std::size_t periodIndex(0); periodIndex<_periodSteps; ++periodIndex)
        {
            for(std::size_t phaseIndex(0); phaseIndex<phasesAmount; ++phaseIndex)
            {
                hre[phaseIndex] = echos[phaseIndex][periodIndex].re();
                him[phaseIndex] = echos[phaseIndex][periodIndex].im();
            }

            _kre[periodIndex] = approxCos(hre);
            _kim[periodIndex] = approxCos(him);
        }

        _kdre.resize(_periodSteps);
        _kdim.resize(_periodSteps);

        for(std::size_t periodIndex(1); periodIndex<_periodSteps-1; ++periodIndex)
        {
            _kdre[periodIndex] = (_kre[periodIndex+1] - _kre[periodIndex-1])/2;
            _kdim[periodIndex] = (_kim[periodIndex+1] - _kim[periodIndex-1])/2;
        }

        _kdre[0] = _kdre[1];
        _kdim[0] = _kdim[1];

        _kdre[_periodSteps-1] = _kdre[_periodSteps-2];
        _kdim[_periodSteps-1] = _kdim[_periodSteps-2];
        std::cerr<<"kernel done"<<std::endl;
    }

}
