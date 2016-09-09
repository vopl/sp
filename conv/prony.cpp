#include "sp/math.hpp"
#include "levmar.h"
#include <boost/math/special_functions.hpp>

using namespace sp;

namespace
{
    /////////-/////////-/////////-/////////-/////////-/////////-/////////-/////////-
    real updateOneLinear(const real *signal, std::size_t signalSize, real signalSampleStep, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx)
    {
        Summator<real> sum;
        real amount = 0;

        real firstSampleStart = signal[signalStartIdx];
        real firstSampleStop = signal[signalStartIdx+1];

        real lastSampleStart = signal[signalStopIdx];
        real lastSampleStop = signal[signalStopIdx+1];

        if(signalStartIdx == signalStopIdx)
        {
            //from single signal sample
            real first01x = (startTime - (signalStartIdx)*signalSampleStep) / signalSampleStep;
            real last01x = (stopTime - (signalStopIdx)*signalSampleStep) / signalSampleStep;

            firstSampleStart = firstSampleStart + (firstSampleStop - firstSampleStart)*first01x;
            lastSampleStop = lastSampleStart + (lastSampleStop - lastSampleStart)*last01x;

            real amount12 = last01x - first01x;
            assert(amount12>=0-std::numeric_limits<real>::epsilon()*100000 && amount12<=1+std::numeric_limits<real>::epsilon()*100000);

            sum += amount12 * (firstSampleStart+lastSampleStop)/2;
            amount = amount12;
        }
        else
        {
            //from multiple signal samples

            if(signalStartIdx < signalStopIdx-1)
            {
                sum += signal[signalStartIdx+1]/2;
                for(std::size_t signalIndex(signalStartIdx+2); signalIndex<signalStopIdx; ++signalIndex)
                {
                    sum += signal[signalIndex];
                }
                sum += signal[signalStopIdx]/2;
            }
            amount = signalStopIdx - signalStartIdx - 1;

            real first01x = (startTime - (signalStartIdx)*signalSampleStep) / signalSampleStep;
            firstSampleStart = firstSampleStart + (firstSampleStop - firstSampleStart)*first01x;
            real amount1 = 1 - first01x;
            sum += amount1 * (firstSampleStart+firstSampleStop)/2;
            amount += amount1;

            real last01x = (stopTime - (signalStopIdx)*signalSampleStep) / signalSampleStep;
            lastSampleStop = lastSampleStart + (lastSampleStop - lastSampleStart)*last01x;
            real amount2 = last01x;
            sum += amount2 * (lastSampleStart+lastSampleStop)/2;
            amount += amount2;
        }

        if(amount>0)
        {
            return sum/amount;
        }

        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void halfLowPassFir(
        real bndT,
        std::size_t n,
        std::vector<real> &A)
    {
        real w = g_2pi/bndT;
        real q = 1;//2/bndT;

        n+=2;
        n |= 1;
        std::size_t mn = n/2-1;
        A.resize(mn+1);

//            real kaizerBeta = 15;
//            real kaizerDenominator = kaizerDenom(kaizerBeta);

        A[mn] = q;
        Summator<real> sum = A[mn]/2;
        for(std::size_t k(1); k<n/2; k++)
        {

            real x = k*w;
            //real v = q * sin(x)/(x);// * kaizer(kaizerBeta, mn+k, n-2, kaizerDenominator);
            real v = q * boost::math::sinc_pi(x);// * kaizer(kaizerBeta, int(mn+k), int(n-2), kaizerDenominator);

            A[mn-k] = v;
            sum += v;
        }

        for(auto &v : A)
        {
            v /= sum*2;
        }
    }
}

void prony()
{
    std::cout.precision(20);
    std::cout.setf(std::ios::scientific);

    static const real c_ppw = 10;
    static const real c_signalSampleStep = (1.0/44000);

    static const real c_scaledSignalSampleStartLog = sp::log(c_signalSampleStep*100);
    static const real c_scaledSignalSampleStopLog = sp::log(2.0);
    static const std::size_t c_scaledSignalSampleSteps = 2000;

    static const real c_scaledSignalSampleStepLog = (c_scaledSignalSampleStopLog - c_scaledSignalSampleStartLog) / c_scaledSignalSampleSteps;

    //подготовить оригинальный сигнал
    TVReal signal(std::size_t(sp::exp(c_scaledSignalSampleStopLog)/c_signalSampleStep+0.5 + 2));
    for(std::size_t i(0); i<signal.size(); ++i)
    {
        signal[i] = 0;
        real x = c_signalSampleStep * i;

        for(real f(1/0.009990072835206058); f<101; f+=300)
        {
            if(x < c_ppw/f)
            {
                signal[i] += sp::cos(g_2pi*x*f + 0);
            }
        }
    }

    //сформировать отмасштабированный сигнал
    TVReal scaledSignal(c_scaledSignalSampleSteps);
    TVReal scaledSignalX(c_scaledSignalSampleSteps);

    for(std::size_t i(0); i<c_scaledSignalSampleSteps; ++i)
    {
        real startTime = sp::exp(c_scaledSignalSampleStartLog + c_scaledSignalSampleStepLog*i);
        real stopTime = sp::exp(c_scaledSignalSampleStartLog + c_scaledSignalSampleStepLog*(i+1));

        std::size_t signalStartIdx = std::size_t(startTime/c_signalSampleStep);
        std::size_t signalStopIdx = std::size_t(stopTime/c_signalSampleStep);

        assert(signalStartIdx >= 0);
        assert(signalStopIdx < signal.size()-1);

        scaledSignalX[i] = stopTime;
        scaledSignal[i] = updateOneLinear(&signal[0], signal.size(), c_signalSampleStep, startTime, stopTime, signalStartIdx, signalStopIdx);

//        std::cout<<scaledSignal[i]<<std::endl;
    }
//    exit(0);

    //отфильтровать
    static const std::size_t firSize = 999;
    TVReal halfFir;
    real lowBndT = (real(firSize-1))/(c_ppw*3.5)/2;
    halfLowPassFir(lowBndT, firSize, halfFir);

    //сформировать отмасштабированный сигнал
    TVReal scaledFilteredSignalX(scaledSignal.size() - halfFir.size());
    TVReal scaledFilteredSignal(scaledSignal.size() - halfFir.size());

//    std::size_t halfFirSize = halfFir.size();

//    for(std::size_t j(0); j<scaledFilteredSignal.size(); ++j)
//    {
//        Summator<real> sum = 0;
//        sum += (scaledSignal[j+0] + scaledSignal[j+firSize-1]) * halfFir[0] / 2;
//        for(std::size_t i(1); i<halfFirSize-1; ++i)
//        {
//            sum += (scaledSignal[j+i] + scaledSignal[j+firSize-1-i]) * halfFir[i];
//        }
//        sum += scaledSignal[j+halfFirSize-1] * halfFir[halfFirSize-1];

//        scaledFilteredSignalX[j] = scaledSignalX[j+halfFirSize-1];
//        scaledFilteredSignal[j] = sum;
//        scaledFilteredSignal[j] = scaledSignal[j+halfFirSize-1];
//        //std::cout<<scaledFilteredSignal[j]<<std::endl;
//    }

    scaledFilteredSignalX = scaledSignalX;
    scaledFilteredSignal = scaledSignal;


    //подготовить сетку периода
    TVReal periods(scaledFilteredSignal.size()*0.2+0.5);
    for(std::size_t i(0); i<periods.size(); ++i)
    {
        periods[i] = scaledFilteredSignalX[i/0.2+0.5]/c_ppw;
    }


    //аппроксимировать

    {
        sp::real levmarInfo[LM_INFO_SZ];

        static sp::real levmarOpts[LM_OPTS_SZ] =
        {
            1e-40,  //LM_INIT_MU,        //mu
            1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
            1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
            1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
        };

        static std::vector<sp::real> args;
        args.resize(periods.size()*2);
        std::fill(args.begin(), args.end(), (sp::real)(0));

        static std::vector<sp::real> work;
        if(work.size() < LM_DIF_WORKSZ(args.size(), scaledFilteredSignal.size()))
        {
            work.resize(LM_DIF_WORKSZ(args.size(), scaledFilteredSignal.size()));
        }

        struct LevmarParams
        {
            const real *_xs;
            const real *_ts;
        } levmarParams;

        levmarParams._xs = &scaledFilteredSignalX[0];
        levmarParams._ts = &periods[0];

        std::vector<sp::real> ys;
        ys.assign(scaledFilteredSignal.begin(), scaledFilteredSignal.end());


        int levmarResult = levmar_der(
                    [](sp::real *p, sp::real *hx, int m, int n, void *_levmarParams)->void{
                        LevmarParams *levmarParams = reinterpret_cast<LevmarParams*>(_levmarParams);
                        for(int i(0); i<n; i++)
                        {
                            hx[i] = 0;
                        }
                        for(int j(0); j<m/2; j++)
                        {
                            real t = levmarParams->_ts[j];

                            int startIdx = std::lower_bound(levmarParams->_xs, levmarParams->_xs+n, t * 0) - levmarParams->_xs;
                            int stopIdx = std::lower_bound(levmarParams->_xs, levmarParams->_xs+n, t * c_ppw) - levmarParams->_xs;

                            assert(stopIdx < n);

                            for(int i(startIdx); i<=stopIdx; ++i)
                            {
                                real x = levmarParams->_xs[i];
                                hx[i] += p[j*2+0]*sp::cos(g_2pi*x/t) + p[j*2+1]*sp::sin(g_2pi*x/t);
                            }
                        }
                    },
                    [](sp::real *p, sp::real *jx, int m, int n, void *_levmarParams){
                        LevmarParams *levmarParams = reinterpret_cast<LevmarParams*>(_levmarParams);

                        for(int i(0); i<n; i++)
                        {
                            for(int j(0); j<m/2; j++)
                            {
                                jx[i*m + j*2+0] = 0;
                                jx[i*m + j*2+1] = 0;
                            }
                        }

                        for(int j(0); j<m/2; j++)
                        {
                            real t = levmarParams->_ts[j];

                            int startIdx = std::lower_bound(levmarParams->_xs, levmarParams->_xs+n, t * 0) - levmarParams->_xs;
                            int stopIdx = std::lower_bound(levmarParams->_xs, levmarParams->_xs+n, t * c_ppw) - levmarParams->_xs;
                            assert(stopIdx < n);

                            for(int i(startIdx); i<=stopIdx; ++i)
                            {
                                real x = levmarParams->_xs[i];
                                jx[i*m + j*2+0] += sp::cos(g_2pi*x/t);
                                jx[i*m + j*2+1] += sp::sin(g_2pi*x/t);
                            }
                        }
                    },
                    &args[0],
                    &ys[0],
                    args.size(),
                    scaledFilteredSignal.size(),
                    10,
                    levmarOpts,
                    levmarInfo,
                    &work[0],
                    NULL,
                    &levmarParams);

            std::cerr<<"result: "<<levmarResult<<std::endl;
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

            for(std::size_t i(0); i<periods.size(); ++i)
            {
                std::cout<<periods[i]<<", "<<args[i*2+0]<<", "<<args[i*2+1]<<std::endl;
            }
    }


    return;
}
