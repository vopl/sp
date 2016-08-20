#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"
#include "levmar.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
static const std::size_t phasesAmountForKernelApproximator = 4;//MAGIC
static const std::size_t samplesOnSignalPeriod = 500;//MAGIC сколько сэмплов сигнала брать на период при построении ядра. Больше-лучше



namespace sp
{
    KernelTabled::KernelTabled(real pow)
        : _pow(pow)
    {

    }

    KernelTabled::~KernelTabled()
    {

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
        std::vector<double> &work)
    {
        LevmarParams params;
        params._et = et;
        params._ev = ev;
        params._st = st;
        params._kernelTabled = this;

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
            1e-15,  //LM_INIT_MU,        //mu
            1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
            1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
            1e-20,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
        };

        std::vector<double> d_sv(ssize*2);
        for(std::size_t i(0); i<ssize; ++i)
        {
            d_sv[i*2+0] = sv[i].re();
            d_sv[i*2+1] = sv[i].im();
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

    namespace
    {
        complex approxCos(std::vector<double> &ys)
        {
            double levmarInfo[LM_INFO_SZ];
            std::vector<double> work;
            if(work.size() < LM_DIF_WORKSZ(2, ys.size()*2))
            {
                work.resize(LM_DIF_WORKSZ(2, ys.size()*2));
            }

            static double levmarOpts[LM_OPTS_SZ] =
            {
                1e-30,  //LM_INIT_MU,        //mu
                1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
                1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
                1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
            };

            double p[2]={1,0};

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

        void buildValue(std::size_t samplesOnSignalPeriod, real pow, const real &period, complex &re, complex &im)
        {
            Convolver c(pow);

            real targetX = period*pow*2.5;
            const real sampleStep = period/samplesOnSignalPeriod;
            TVReal signal(std::size_t(targetX/sampleStep)+1000);

            std::vector<double> hre(phasesAmountForKernelApproximator), him(phasesAmountForKernelApproximator);
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

            re = approxCos(hre);
            im = approxCos(him);
        }
    }

    void KernelTabled::evalKernel(real t, real &rr, real &ri, real &ir, real &ii)
    {
        real key = t;
        ValuesByPeriod::iterator iter = _valuesByPeriod.find(key);

        if(_valuesByPeriod.end() == iter)
        {
            Value v;
            buildValue(samplesOnSignalPeriod, _pow, t, v._re, v._im);
            iter = _valuesByPeriod.insert(std::make_pair(key, v)).first;

            std::cout<<"build "<<t<<", "<<_valuesByPeriod.size()<<std::endl;
        }
        else
        {
            //std::cout<<"already "<<t<<" == "<<real(key)/100000000000<<std::endl;
        }

        Value &v = iter->second;

        rr = v._re.re();
        ri = v._re.im();
        ir = v._im.re();
        ii = v._im.im();
    }

    std::string KernelTabled::stateFileName()
    {
        char tmp[4096];
        sprintf(tmp, "kt_state_POW%0.15e.bin", double(_pow));

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

//        if(!loadTable(in, _kre, _periodSteps)) return false;
//        if(!loadTable(in, _kim, _periodSteps)) return false;
//        if(!loadTable(in, _kdre, _periodSteps)) return false;
//        if(!loadTable(in, _kdim, _periodSteps)) return false;

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

//        if(!saveTable(out, _kre)) return false;
//        if(!saveTable(out, _kim)) return false;
//        if(!saveTable(out, _kdre)) return false;
//        if(!saveTable(out, _kdim)) return false;

        return true;
    }

}
