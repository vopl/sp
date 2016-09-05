#include "sp/kernelTabled.hpp"
#include "sp/math.hpp"
#include "levmar.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
static const std::size_t phasesAmountForKernelApproximator = 2;//MAGIC



namespace sp
{
    KernelTabled::KernelTabled(real ppw, std::size_t samplesPerLevelSample, std::size_t samplesPerLevelPeriod, std::size_t convolverPolyOrder)
        : _ppw(ppw)
        , _samplesPerLevelSample(samplesPerLevelSample)
        , _samplesPerLevelPeriod(samplesPerLevelPeriod)
        , _convolverPolyOrder(convolverPolyOrder)
    {
        load();
    }

    KernelTabled::~KernelTabled()
    {
        if(_addedValuesAmount)
        {
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
        void evalLevmarFunc(double *p, double *hx, int m, int n, void *levmarParams)
        {
            LevmarParams *params = reinterpret_cast<LevmarParams *>(levmarParams);

            for(int i(0); i<n/2; i++)
            {
                real et = params->_et[i];
                Summator<real> re = 0;
                Summator<real> im = 0;

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

                re += -params->_ev[i].re();
                im += -params->_ev[i].im();

                hx[i*2+0] = double(re);
                hx[i*2+1] = double(im);
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void evalLevmarJaco(double *p, double *jx, int m, int n, void *levmarParams)
        {
            (void)p;
            LevmarParams *params = reinterpret_cast<LevmarParams *>(levmarParams);

            for(int i(0); i<n/2; i++)
            {
                real et = params->_et[i];

                for(int j(0); j<m/2; j++)
                {
                    real st = params->_st[j];

                    real rr,  ri,  ir,  ii;
                    params->_kernelTabled->evalKernel(et/st, rr, ri, ir, ii);

                    jx[(i*2+0)*m+j*2+0] = double(rr);
                    jx[(i*2+0)*m+j*2+1] = double(-ri);

                    jx[(i*2+1)*m+j*2+0] = double(ir);
                    jx[(i*2+1)*m+j*2+1] = double(-ii);
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void KernelTabled::deconvolve(
            size_t esize, const real *et, const complex *ev, //отклик
            size_t ssize, const real *st,       complex *sv, //спектр
            size_t &iters, //макс итераций
            real &error,
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
//         std::cerr<<"dlevmar_chkjac: "<<errVal<<std::endl;
//         exit(0);

        static double levmarOpts[LM_OPTS_SZ] =
        {
            1e-40,  //LM_INIT_MU,        //mu
            1e-140,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
            1e-300,  //LM_STOP_THRESH,    //||Dp||_2 and
            1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
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
            int(ssize*2),
            int(esize*2),
            int(iters),
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
        std::cerr<<std::endl;
        //exit(1);

        for(std::size_t i(0); i<ssize; ++i)
        {
            sv[i] = complex(d_sv[i*2+0], d_sv[i*2+1]);
        }

        iters = std::size_t(levmarInfo[5]+0.5);
        error = levmarInfo[1];
    }

    namespace
    {
        complex approxCos(TVReal &ys)
        {
            double levmarInfo[LM_INFO_SZ];
            std::vector<double> work;
            if(work.size() < LM_DIF_WORKSZ(2, ys.size()*2))
            {
                work.resize(LM_DIF_WORKSZ(2, ys.size()*2));
            }

            static double levmarOpts[LM_OPTS_SZ] =
            {
                1e-40,  //LM_INIT_MU,        //mu
                1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
                1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
                1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
            };

            double p[2]={0,0};

            std::vector<double> dys(ys.begin(), ys.end());

            int levmarResult = dlevmar_der(
                        [](double *p, double *hx, int m, int n, void *)->void{
                            (void)m;
                            for(int i(0); i<n; i++)
                            {
                                hx[i] = double(p[0]*cos(g_pi/2*i/n) - p[1]*sin(g_pi/2*i/n));
                            }
                        },
                        [](double *p, double *jx, int m, int n, void *){
                            (void)p;
                            (void)m;

                            for(int i(0); i<n; i++)
                            {
                                jx[i*2+0] = double(cos(g_pi/2*i/n));
                                jx[i*2+1] = double(-sin(g_pi/2*i/n));
                            }
                        },
                        &p[0],
                        &dys[0],
                        2,
                        int(ys.size()),
                        5,
                        levmarOpts,
                        levmarInfo,
                        &work[0],
                        NULL,
                        NULL);
            (void)levmarResult;

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

            return complex(p[0],p[1]);
        }
    }

    void KernelTabled::evalKernel(real t, real &rr, real &ri, real &ir, real &ii)
    {
        ValuesByPeriod::iterator iter = _valuesByPeriod.lower_bound(t);

        static const real maxDelta = std::numeric_limits<real>::epsilon()*20;

        bool found = false;

        if(fabs(iter->first/t-1) > maxDelta)
        {
            iter--;

            if(fabs(iter->first/t-1) > maxDelta)
            {
                std::cerr<<"add kernel value "<<t<<", "<<_valuesByPeriod.size()<<"...";
                std::cerr.flush();

                Value v;
                buildValue(t, v._re, v._im);
                iter = _valuesByPeriod.insert(std::make_pair(t, v)).first;

                std::cerr<<"ok"<<std::endl;

                if(++_addedValuesAmount >= 10)
                {
                    save();
                }

            }
            else
            {
                found = true;
                //std::cerr<<"same2 "<<t<<", "<<iter->first<<", "<<fabs(iter->first - t)<<std::endl;
            }
        }
        else
        {
            found = true;
            //std::cerr<<"same1 "<<t<<", "<<iter->first<<", "<<fabs(iter->first - t)<<std::endl;
        }

        if(found)
        {
            if(_addedValuesAmount && (++_addedValuesAmount >= 100))
            {
                save();
            }
        }

        Value &v = iter->second;

        rr = v._re.re();
        ri = v._re.im();
        ir = v._im.re();
        ii = v._im.im();
    }

    void KernelTabled::buildValue(const real &period, complex &re, complex &im)
    {
        const real sampleStep = period/_samplesPerLevelPeriod/_samplesPerLevelSample;
        real targetX = period*_ppw*2.0+sampleStep*3 + 10*period/_samplesPerLevelPeriod;
        TVReal signal(std::size_t(targetX/sampleStep+1.5));

        SignalConvolver &sc = getSignalConvolver();
        sc.setupSignal(period, sampleStep);

        TVReal hre(phasesAmountForKernelApproximator), him(phasesAmountForKernelApproximator);
        for(std::size_t phaseIndex(0); phaseIndex<phasesAmountForKernelApproximator; ++phaseIndex)
        {
            std::size_t startIdx = 0;
            std::size_t stopIdx = signal.size();
            for(std::size_t sindex(startIdx); sindex<stopIdx; sindex++)
            {
                signal[sindex] = cos(g_2pi*(sampleStep*sindex - targetX) + phaseIndex*g_pi/2/phasesAmountForKernelApproximator);
//                signal[sindex] += cos(g_2pi*(sampleStep*sindex - targetX)*2 + phaseIndex*g_pi/2/phasesAmountForKernelApproximator);
//                signal[sindex] += cos(g_2pi*(sampleStep*sindex - targetX)*0.5 + phaseIndex*g_pi/2/phasesAmountForKernelApproximator);
            }

            sc.pushSignal(&signal[0], signal.size());

            complex echo = sc.convolve(period);

            hre[phaseIndex] = echo.re();
            him[phaseIndex] = echo.im();

        }

        re = approxCos(hre);
        im = approxCos(him);
    }

    std::string KernelTabled::stateFileName()
    {
        char tmp[4096];
        sprintf(tmp, "kt_state_PPW%0.2f_CPO%zd_SPLS%zd_SPLP%zd.bin",
                double(_ppw),
                size_t(_convolverPolyOrder),
                size_t(_samplesPerLevelSample),
                size_t(_samplesPerLevelPeriod));

        return tmp;
    }

    bool KernelTabled::load()
    {
        std::cerr<<"load kernel: "<<stateFileName()<<"...";


        _valuesByPeriod.clear();
        //на краях добавить значения, чтобы была гарантия наличия +-одного элемента
        Value zv;
        _valuesByPeriod[0] = zv;
        _valuesByPeriod[-1] = zv;

        _valuesByPeriod[std::numeric_limits<real>::max()/2] = zv;
        _valuesByPeriod[std::numeric_limits<real>::max()] = zv;

        std::ifstream in(stateFileName().c_str(), std::ios::in|std::ios::binary);
        if(!in)
        {
            std::cerr<<0<<std::endl;
            return false;
        }

        real t;
        Value v;
        while(in)
        {
            in.read(reinterpret_cast<char *>(&t), sizeof(t));
            if(in.gcount() != sizeof(t))
            {
                break;
            }

            in.read(reinterpret_cast<char *>(&v), sizeof(v));
            if(in.gcount() != sizeof(v))
            {
                break;
            }

            _valuesByPeriod.insert(std::make_pair(t, v));
        }

        std::cerr<<_valuesByPeriod.size()<<std::endl;

        return true;
    }

    bool KernelTabled::save()
    {
        _addedValuesAmount = 0;

        std::string fname = stateFileName();
        std::cerr<<"save kernel: "<<fname<<"...";
        std::ofstream out((fname+".tmp").c_str(), std::ios::out|std::ios::binary);
        if(!out)
        {
            std::cerr<<"unable to open file: "<<fname<<std::endl;
            abort();
            return false;
        }

        for(const auto &vbp : _valuesByPeriod)
        {
            if(!out)
            {
                std::cerr<<"unable to write file: "<<fname<<std::endl;
                abort();
                return false;
            }
            out.write(reinterpret_cast<const char *>(&vbp.first), sizeof(vbp.first));
            out.write(reinterpret_cast<const char *>(&vbp.second), sizeof(vbp.second));
        }

        out.close();

        if(0 != rename((fname+".tmp").c_str(), (fname).c_str()))
        {
            std::cerr<<"unable to rename file: "<<fname<<".tmp -> "<<fname<<std::endl;
            abort();
            return false;
        }

        std::cerr<<_valuesByPeriod.size()<<std::endl;
        return true;
    }

    SignalConvolver &KernelTabled::getSignalConvolver()
    {
        if(!_scp)
        {
            _scp.reset(new SignalConvolver);
            _scp->setupFirs(_ppw, _samplesPerLevelPeriod, _convolverPolyOrder);
        }

        return *_scp;
    }

}
