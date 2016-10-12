#include "sp/conv/kernelTabled.hpp"
#include "sp/math.hpp"
#include "levmar.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
static const std::size_t phasesAmountForKernelApproximator = 7;//MAGIC



namespace sp { namespace conv
{
    KernelTabled::KernelTabled(real ppw, std::size_t samplesPerLevelPeriod)
        : _ppw(ppw)
        , _samplesPerLevelPeriod(samplesPerLevelPeriod)
    {
        load();
    }

    KernelTabled::~KernelTabled()
    {
        flush();
    }

    EchoPoint KernelTabled::eval(real t, real st, const SpectrPoint &sv)
    {
        KernelPoint k;
        evalKernel(t/st, k);
        return sv * k;
    }

    void KernelTabled::flush()
    {
        if(_addedValuesAmount)
        {
            save();
        }
    }

    namespace
    {
        using real4deconv = real;
        complex approxCos(const TVReal &ys)
        {
            real4deconv levmarInfo[LM_INFO_SZ];
            std::vector<real4deconv> work;
            if(work.size() < LM_DER_WORKSZ(2, ys.size()*2))
            {
                work.resize(LM_DER_WORKSZ(2, ys.size()*2));
            }

            static real4deconv levmarOpts[LM_OPTS_SZ] =
            {
                1e-40,  //LM_INIT_MU,        //mu
                1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
                1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
                1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
            };

            real4deconv p[2]={0,0};

            std::vector<real4deconv> dys(ys.begin(), ys.end());

            int levmarResult = levmar_der(
                        [](real4deconv *p, real4deconv *hx, int m, int n, void *)->void{
                            (void)m;
                            for(int i(0); i<n; i++)
                            {
                                hx[i] = real4deconv(p[0]*cos(g_pi*2*i/n) + p[1]*sin(g_pi*2*i/n));
                            }
                        },
                        [](real4deconv *p, real4deconv *jx, int m, int n, void *){
                            (void)p;
                            (void)m;

                            for(int i(0); i<n; i++)
                            {
                                jx[i*2+0] = real4deconv(cos(g_pi*2*i/n));
                                jx[i*2+1] = real4deconv(sin(g_pi*2*i/n));
                            }
                        },
                        &p[0],
                        &dys[0],
                        2,
                        int(ys.size()),
                        50,
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
//            //exit(1);

            return complex(p[0],p[1]);
        }
    }

    void KernelTabled::evalKernel(real t, KernelPoint &kv)
    {
        ValuesByPeriod::iterator iter = _valuesByPeriod.lower_bound(t);

        static const real maxDelta = std::numeric_limits<real>::epsilon()*20;

        bool found = false;

        if(fabs(iter->first/t-1) > maxDelta)
        {
            iter--;

            if(fabs(iter->first/t-1) > maxDelta)
            {
                //std::cerr<<"add kernel value "<<t<<", "<<_valuesByPeriod.size()<<"...";
                //std::cerr.flush();

                buildValue(t, kv);
                iter = _valuesByPeriod.insert(std::make_pair(t, kv)).first;

                //std::cerr<<"ok"<<std::endl;

                if(++_addedValuesAmount >= 100000)
                {
                    save();
                }

            }
            else
            {
                found = true;
                //std::cerr<<"same2 "<<t<<", "<<iter->first<<", "<<fabs(iter->first - t)<<std::endl;

                kv = iter->second;
            }
        }
        else
        {
            found = true;
            //std::cerr<<"same1 "<<t<<", "<<iter->first<<", "<<fabs(iter->first - t)<<std::endl;

            kv = iter->second;
        }
    }

    void KernelTabled::buildValue(const real &period, KernelPoint &kv)
    {
        SignalConvolver &sc = getSignalConvolver();

        std::vector<TVReal> harmKernel(EchoPoint::RowsAtCompileTime);

        for(std::size_t d(0); d<EchoPoint::RowsAtCompileTime; ++d)
        {
            harmKernel[d].resize(phasesAmountForKernelApproximator);
        }

        for(std::size_t phaseIndex(0); phaseIndex<phasesAmountForKernelApproximator; ++phaseIndex)
        {
            EchoPoint echo = sc.convolveIdentity(period, phaseIndex*g_2pi/phasesAmountForKernelApproximator);

            for(std::size_t d(0); d<EchoPoint::RowsAtCompileTime; ++d)
            {
                harmKernel[d][phaseIndex] = echo(d);
            }
        }

        for(std::size_t d(0); d<EchoPoint::RowsAtCompileTime; ++d)
        {
            complex cv = approxCos(harmKernel[d]);
            kv(0, d) = cv.re();
            kv(1, d) = -cv.im();
        }
    }

    std::string KernelTabled::stateFileName()
    {
        char tmp[4096];
        sprintf(tmp, "kt_state_PPW%0.3f_SPLP%zd.bin",
                double(_ppw),
                size_t(_samplesPerLevelPeriod));

        return tmp;
    }

    bool KernelTabled::load()
    {
        std::cerr<<"load kernel: "<<stateFileName()<<"...";


        _valuesByPeriod.clear();
        //на краях добавить значения, чтобы была гарантия наличия +-одного элемента
        KernelPoint zv;
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
        KernelPoint v;
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
        //return true;

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
            _scp->setup(_ppw, _samplesPerLevelPeriod);
        }

        return *_scp;
    }

}}
