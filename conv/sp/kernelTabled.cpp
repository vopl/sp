#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"
#include "levmar.h"

#include <algorithm>
#include <iostream>
#include <fstream>

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
static const std::size_t phasesAmountForKernelApproximator = 10;//MAGIC
static const std::size_t samplesOnSignalPeriod = 1000;//MAGIC сколько сэмплов сигнала брать на период при построении ядра. Больше-лучше
static const std::size_t extraValuesInGrid = 2;//сколько дополнительных значений прикрепить к концу таблицы (там значения возле t=1)



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
        _periodSteps = periodSteps/2;

        _steps01 = _periodSteps;
        _min01 = _periodMin;
        _max01 = 1.0;
        _step01 = (_max01 - _min01)/(_steps01-1);

        _steps1inf = _periodSteps;
        _min1inf = 1.0/_periodMax;
        _max1inf = 1.0;
        _step1inf = (_max1inf - _min1inf)/(_steps1inf-1);

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
        if(t<1)
        {
            //линейно по периоду
            std::int64_t idx = std::int64_t((t-_min01)/_step01);
            if(idx < 0)
            {
                rr = approx01(t/_min01, 0, 0, _kre01.front().re(), _kdre01.front().re());
                ri = approx01(t/_min01, 0, 0, _kre01.front().im(), _kdre01.front().im());
                ir = approx01(t/_min01, 0, 0, _kim01.front().re(), _kdim01.front().im());
                ii = approx01(t/_min01, 0, 0, _kim01.front().im(), _kdim01.front().im());
                return;
            }

            assert(idx < _kre01.size()-1);

            real x = (t-_step01*idx-_min01)/_step01;
            rr = approx01(x, _kre01[idx].re(), _kdre01[idx].re(), _kre01[idx+1].re(), _kdre01[idx+1].re());
            ri = approx01(x, _kre01[idx].im(), _kdre01[idx].im(), _kre01[idx+1].im(), _kdre01[idx+1].im());
            ir = approx01(x, _kim01[idx].re(), _kdim01[idx].re(), _kim01[idx+1].re(), _kdim01[idx+1].re());
            ii = approx01(x, _kim01[idx].im(), _kdim01[idx].im(), _kim01[idx+1].im(), _kdim01[idx+1].im());

            return;
        }

        //линейно по частоте
        t = 1.0/t;

        std::int64_t idx = std::int64_t((t-_min1inf)/_step1inf);
        if(idx < 0)
        {
            rr = approx01(t/_min1inf, 0, 0, _kre1inf.front().re(), _kdre1inf.front().re());
            ri = approx01(t/_min1inf, 0, 0, _kre1inf.front().im(), _kdre1inf.front().im());
            ir = approx01(t/_min1inf, 0, 0, _kim1inf.front().re(), _kdim1inf.front().im());
            ii = approx01(t/_min1inf, 0, 0, _kim1inf.front().im(), _kdim1inf.front().im());
            return;
        }

        assert(idx < _kre01.size()-1);

        real x = (t-_step1inf*idx-_min1inf)/_step1inf;
        rr = approx01(x, _kre1inf[idx].re(), _kdre1inf[idx].re(), _kre1inf[idx+1].re(), _kdre1inf[idx+1].re());
        ri = approx01(x, _kre1inf[idx].im(), _kdre1inf[idx].im(), _kre1inf[idx+1].im(), _kdre1inf[idx+1].im());
        ir = approx01(x, _kim1inf[idx].re(), _kdim1inf[idx].re(), _kim1inf[idx+1].re(), _kdim1inf[idx+1].re());
        ii = approx01(x, _kim1inf[idx].im(), _kdim1inf[idx].im(), _kim1inf[idx+1].im(), _kdim1inf[idx+1].im());

    }

    std::string KernelTabled::stateFileName()
    {
        char tmp[4096];
        sprintf(tmp, "kt_state_POW%0.15e_TMIN%0.15e_TMAX%0.15e_TSTEPS%zd.bin", double(_pow), double(_periodMin), double(_periodMax), _periodSteps);

        return tmp;
    }

    namespace
    {
        bool loadTable(std::ifstream &in, TVComplex &tbl, std::size_t amount)
        {
            tbl.resize(amount);

            std::streamsize bytesAmount = std::streamsize(tbl.size()*sizeof(tbl[0]));

            in.read(reinterpret_cast<char *>(&tbl[0]), bytesAmount);
            return in.gcount() == bytesAmount;
        }

        bool saveTable(std::ofstream &out, const TVComplex &tbl)
        {
            std::streamsize bytesAmount = std::streamsize(tbl.size()*sizeof(tbl[0]));

            std::size_t pos = out.tellp();
            out.write(reinterpret_cast<const char *>(&tbl[0]), bytesAmount);

            return out.tellp() == pos + bytesAmount;
        }

    }

    bool KernelTabled::load()
    {
        //std::cout<<stateFileName()<<std::endl;

        std::ifstream in(stateFileName().c_str(), std::ios::in|std::ios::binary);
        if(!in)
        {
            return false;
        }

        if(!loadTable(in, _kre01, _steps01+extraValuesInGrid)) return false;
        if(!loadTable(in, _kim01, _steps01+extraValuesInGrid)) return false;
        if(!loadTable(in, _kdre01, _steps01+extraValuesInGrid)) return false;
        if(!loadTable(in, _kdim01, _steps01+extraValuesInGrid)) return false;

        if(!loadTable(in, _kre1inf, _steps1inf+extraValuesInGrid)) return false;
        if(!loadTable(in, _kim1inf, _steps1inf+extraValuesInGrid)) return false;
        if(!loadTable(in, _kdre1inf, _steps1inf+extraValuesInGrid)) return false;
        if(!loadTable(in, _kdim1inf, _steps1inf+extraValuesInGrid)) return false;

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

        if(!saveTable(out, _kre01)) return false;
        if(!saveTable(out, _kim01)) return false;
        if(!saveTable(out, _kdre01)) return false;
        if(!saveTable(out, _kdim01)) return false;

        if(!saveTable(out, _kre1inf)) return false;
        if(!saveTable(out, _kim1inf)) return false;
        if(!saveTable(out, _kdre1inf)) return false;
        if(!saveTable(out, _kdim1inf)) return false;

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
                1e-30,  //LM_INIT_MU,        //mu
                1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
                1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
                1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
            };

            real p[2]={1,0};

            {
                real max = ys[0];
                std::size_t maxIdx = 0;

                for(std::size_t idx(1); idx<ys.size(); ++idx)
                {
                    if(max < ys[idx])
                    {
                        max = ys[idx];
                        maxIdx = idx;
                    }
                }

                p[0] = max;
                p[1] = -g_2pi * maxIdx / ys.size();
            }

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

    namespace
    {
        void buildValues(std::size_t samplesOnSignalPeriod, real pow, const PeriodGrid &grid, TVComplex &re, TVComplex &im)
        {
            std::size_t steps = grid.grid().size();
            re.resize(steps);
            im.resize(steps);

            Convolver c(pow);

            for(std::size_t periodIndex(0); periodIndex<steps; ++periodIndex)
            {
                const real &period = grid.grid()[periodIndex];

                real targetX = period*pow*2.5;
                const real sampleStep = period/samplesOnSignalPeriod;
                TVReal signal(std::size_t(targetX/sampleStep)+1000);

                TVReal hre(phasesAmountForKernelApproximator), him(phasesAmountForKernelApproximator);
                for(std::size_t phaseIndex(0); phaseIndex<phasesAmountForKernelApproximator; ++phaseIndex)
                {
                    std::size_t startIdx = 0;
                    std::size_t stopIdx = signal.size();
                    for(std::size_t sindex(startIdx); sindex<stopIdx; sindex++)
                    {
                        signal[sindex] = cos(g_2pi*(sampleStep*sindex - targetX) + phaseIndex*g_2pi/phasesAmountForKernelApproximator);
                    }

                    complex echo = c.execute(period, 0, sampleStep, signal, targetX);
                    hre[phaseIndex] = echo.re();
                    him[phaseIndex] = echo.im();

                }

                re[periodIndex] = approxCos(hre);
                im[periodIndex] = approxCos(him);

                std::cerr<<periodIndex<<"/"<<steps<<std::endl;
            }
        }

        void buildDerivatives(TVComplex &v, TVComplex &dv)
        {
            std::size_t steps = v.size();
            dv.resize(steps);

            for(std::size_t idx(1); idx<steps-1; ++idx)
            {
                dv[idx] = (v[idx+1] - v[idx-1])/2;
            }

            dv[0] = dv[1];

            dv[steps-1] = dv[steps-2];
        }

    }

    void KernelTabled::build()
    {

        std::cerr<<"build kernel t1"<<std::endl;
        //0-1, линейный период
        {
            PeriodGrid grid(_min01, _max01+_step01*extraValuesInGrid, _steps01+extraValuesInGrid, PeriodGridType::periodLin);

            buildValues(samplesOnSignalPeriod, _pow, grid, _kre01, _kim01);

            buildDerivatives(_kre01, _kdre01);
            buildDerivatives(_kim01, _kdim01);
        }

        std::cerr<<"build kernel t2"<<std::endl;
        //1-inf линейная частота
        {
            PeriodGrid grid(1.0/(1.0+extraValuesInGrid*_step1inf), _periodMax, _steps1inf+extraValuesInGrid, PeriodGridType::frequencyLin);

            buildValues(samplesOnSignalPeriod, _pow, grid, _kre1inf, _kim1inf);

            std::reverse(_kre1inf.begin(), _kre1inf.end());
            std::reverse(_kim1inf.begin(), _kim1inf.end());

            buildDerivatives(_kre1inf, _kdre1inf);
            buildDerivatives(_kim1inf, _kdim1inf);
        }
        std::cerr<<"kernel done"<<std::endl;
    }
}
