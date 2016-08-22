#include "convolver.hpp"
#include <iostream>
#include <boost/math/special_functions.hpp>

namespace sp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Convolver::Convolver(real pow)
        : _pow(pow)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Convolver::~Convolver()
    {

    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        complex evalSegment(real period, real xTarget, real x0, real y0, real x1, real y1, real xStart, real xStop)
        {
            real _2pi_div_period = g_2pi/period;
            real _period_pow_2 = period*period;
            real _period_mul_2pi = period*g_2pi;


            real xStart_mul_2pi_div_period = (xStart-xTarget)*_2pi_div_period;
            real xStop_mul_2pi_div_period = (xStop-xTarget)*_2pi_div_period;

            real sinStop = sin(xStop_mul_2pi_div_period);
            real cosStop = cos(xStop_mul_2pi_div_period);

            real sinStart = sin(xStart_mul_2pi_div_period);
            real cosStart = cos(xStart_mul_2pi_div_period);

            real x1_mul_y0_minus_x0_mul_y1 = (x1*y0-x0*y1);

            real y1_minus_y0 = y1-y0;
            real x1_minus_x0 = x1-x0;

            //TRATATA тут можно еще закэшировать
            real re =
                (
                    +y1_minus_y0*_period_pow_2*cosStop
                    +(y1_minus_y0*xStop+x1_mul_y0_minus_x0_mul_y1)*_period_mul_2pi*sinStop

                    -y1_minus_y0*_period_pow_2*cosStart
                    -(y1_minus_y0*xStart+x1_mul_y0_minus_x0_mul_y1)*_period_mul_2pi*sinStart
                )/
                (x1_minus_x0*g_2piPow2);

            real im =
                -(

                    +y1_minus_y0*_period_pow_2*sinStop
                    -(y1_minus_y0*xStop+x1_mul_y0_minus_x0_mul_y1)*_period_mul_2pi*cosStop

                    -y1_minus_y0*_period_pow_2*sinStart
                    +(y1_minus_y0*xStart+x1_mul_y0_minus_x0_mul_y1)*_period_mul_2pi*cosStart
                )/
                (x1_minus_x0*g_2piPow2);

            return complex(re, im);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        complex executeOne(real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, real period, real pow)
        {
            const real _winLen = period * pow;
            const real _step0 = 1.0 / pow;

            real period_m1 = period /(1.0-_step0/2);
            real period_p1 = period /(1.0+_step0/2);

            real xStop = targetTime;
            real xStart = xStop - _winLen;
            if(xStart < signalStartTime)
            {
                assert(0);//окно всегда должно помещатся в сигнал
                xStart = signalStartTime;
            }

            assert(xStart <= xStop);
            assert(xStart >= signalStartTime);
            assert(xStop <= signalStartTime + signalSampleLength*signal.size());

            complex res_m1;
            complex res_p1;

            std::size_t indexStart = std::size_t((xStart - signalStartTime) / signalSampleLength);
            assert(indexStart < signal.size());

            {
                real x0 = signalStartTime + indexStart * signalSampleLength;
                real x1 = x0 + signalSampleLength;

                res_m1 += evalSegment(period_m1, targetTime, x0, signal[indexStart], x1, signal[indexStart+1], xStart, x1);
                res_p1 += evalSegment(period_p1, targetTime, x0, signal[indexStart], x1, signal[indexStart+1], xStart, x1);
            }


            std::size_t indexStop = std::size_t((xStop - signalStartTime) / signalSampleLength);
            assert(indexStart <= indexStop);
            assert(indexStop < signal.size()-2);

            if(indexStop != indexStart)
            {
                real x0 = signalStartTime + indexStop * signalSampleLength;
                real x1 = x0 + signalSampleLength;

                res_m1 += evalSegment(period_m1, targetTime, x0, signal[indexStop], x1, signal[indexStop+1], x0, xStop);
                res_p1 += evalSegment(period_p1, targetTime, x0, signal[indexStop], x1, signal[indexStop+1], x0, xStop);

                for(std::size_t index(indexStart+1); index<indexStop; ++index)
                {
                    real x0 = signalStartTime + index * signalSampleLength;
                    real x1 = x0 + signalSampleLength;

                    res_m1 += evalSegment(period_m1, targetTime, x0, signal[index], x1, signal[index+1], x0, x1);
                    res_p1 += evalSegment(period_p1, targetTime, x0, signal[index], x1, signal[index+1], x0, x1);
                }
            }
            else
            {
                real x0 = signalStartTime + indexStop * signalSampleLength;
                real x1 = x0 + signalSampleLength;

                res_m1 -= evalSegment(period_m1, targetTime, x0, signal[indexStop], x1, signal[indexStop+1], xStop, x1);
                res_p1 -= evalSegment(period_p1, targetTime, x0, signal[indexStop], x1, signal[indexStop+1], xStop, x1);
            }

            return (res_p1*(1.0+_step0/2) - res_m1*(1.0-_step0/2)) / _winLen;
        }
    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        real kaizer(real beta, int n, int N)
        {
            real nom = boost::math::cyl_bessel_i(0, beta*sqrt(1.0-sqr(real(2*n-N+1)/(N-1))));
            real denom = boost::math::cyl_bessel_i(0, beta);

            return nom / denom;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void lowPassFir(
            real bndT,
            std::size_t n,
            TVReal &A)
        {
            real w = g_2pi/bndT;
            real q = 2/bndT;

            n+=2;
            n |= 1;
            A.resize(n-2);

            std::size_t mn = n/2-1;
            real kaizerBeta = 5;

            A[mn] = q;// * kaizer(kaizerBeta, mn, n-2);
            for(std::size_t k(1); k<n/2; k++)
            {
                A[mn+k] = A[mn-k] = q* boost::math::sinc_pi(k*w);// * kaizer(kaizerBeta, mn+k, n-2);
            }
        }

        //МНОГОПОТОК НЕ ПРОЙДЕТ
        //фиры тяжело вычисляются, в основном из за кайзера. Это прекэшированные значения
        using FirId = std::tuple<real, std::size_t, std::size_t>;
        FirId g_firId{0,0,0};
        std::vector<std::vector<real>> g_firs;

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real /*signalStartTime*/ Convolver::prepareSignal(real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, real pow, real maxT, TVReal &preparedSignal)
    {
        size_t extraSamples = 4;

        std::size_t minLen = std::size_t(pow*5+0.5);//по 10 точек на период, иначе fir не справляется

        std::size_t maxLen = std::size_t(pow*(maxT/signalSampleLength)*2 + 1.5);
        if(maxLen&1)
        {
            maxLen+=1;
        }

        std::size_t endIdx = std::size_t((targetTime - signalStartTime)/signalSampleLength+0.5);
        assert(endIdx <= signal.size());
        assert(endIdx > maxLen/2);

        preparedSignal.reserve(maxLen/2 + extraSamples);
        preparedSignal.resize(maxLen/2);

        FirId firId{pow, minLen, maxLen};
        if(firId != g_firId)
        {
            std::cerr<<"make fir"<<std::endl;
            g_firs.clear();
            g_firs.resize(maxLen/2);
            for(std::size_t len(1); len < maxLen; len+=2)
            {
                if(len >= minLen)
                {
                    //std::cerr<<len<<"/"<<maxLen<<std::endl;
                    real bndT = real(len)/(pow*2)/2;
                    lowPassFir(bndT, len, g_firs[(len-minLen)/2]);
                }
            }

            g_firId = firId;
            std::cerr<<"fir done"<<std::endl;
        }

        for(std::size_t len(1); len < maxLen; len+=2)
        {
            if(len >= minLen)
            {
//                    TVReal fir;
//                    real bndT = real(len)/(pow*2)/2;
//                    lowPassFir(bndT, len, fir);

                const TVReal &fir = g_firs[(len-minLen)/2];

                real preparedValue = 0;
                const std::size_t firSize = fir.size();

                const real *signalPart = &signal[endIdx-firSize];
                for(std::size_t i(0); i<firSize; i++)
                {
                    preparedValue += signalPart[i] * fir[i];
                }
                preparedSignal[preparedSignal.size()-1-len/2] = preparedValue;
            }
            else
            {
                preparedSignal[preparedSignal.size()-1-len/2] = signal[endIdx-1-len/2];
            }
        }

        real preparedSignalStartTime = targetTime - preparedSignal.size()*signalSampleLength;

        for(std::size_t i(0); i<extraSamples; i++)
        {
            preparedSignal.push_back(signal[endIdx+i]);
        }

        return preparedSignalStartTime;

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Convolver::execute(const PeriodGrid &periodGrid, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo)
    {
        TVReal preparedSignal;
        real preparedSignalStartTime = prepareSignal(signalStartTime, signalSampleLength, signal, targetTime, _pow, periodGrid.grid().back(), preparedSignal);

//        std::size_t signalTargetIdx = (targetTime - signalStartTime)/signalSampleLength+0.5;
//        std::size_t preparedSignalTargetIdx = (targetTime - preparedSignalStartTime)/signalSampleLength+0.5;
//        for(std::size_t idx(0); idx<preparedSignal.size()-preparedSignalTargetIdx-1; ++idx)
//        {
//            std::cout<<signal[signalTargetIdx-idx]<<", "<<preparedSignal[preparedSignalTargetIdx-idx]<<std::endl;
//        }
//        exit(0);

        echo.resize(periodGrid.grid().size());
        for(std::size_t i(0); i<periodGrid.grid().size(); i++)
        {
            echo[i] = executeOne(preparedSignalStartTime, signalSampleLength, preparedSignal, targetTime, periodGrid.grid()[i], _pow);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    complex /*echo*/ Convolver::execute(const real &period, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime)
    {
        TVReal preparedSignal;
        real preparedSignalStartTime = prepareSignal(signalStartTime, signalSampleLength, signal, targetTime, _pow, period, preparedSignal);

//        std::size_t signalTargetIdx = (targetTime - signalStartTime)/signalSampleLength+0.5;
//        std::size_t preparedSignalTargetIdx = (targetTime - preparedSignalStartTime)/signalSampleLength+0.5;
//        for(std::size_t idx(0); idx<preparedSignal.size()-preparedSignalTargetIdx-1; ++idx)
//        {
//            std::cout<<signal[signalTargetIdx-idx]<<", "<<preparedSignal[preparedSignalTargetIdx-idx]<<std::endl;
//        }
//        exit(0);

        return executeOne(preparedSignalStartTime, signalSampleLength, preparedSignal, targetTime, period, _pow);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Convolver::executeOnPrepared(const PeriodGrid &periodGrid, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo)
    {
        echo.resize(periodGrid.grid().size());
        for(std::size_t i(0); i<periodGrid.grid().size(); i++)
        {
            echo[i] = executeOne(signalStartTime, signalSampleLength, signal, targetTime, periodGrid.grid()[i], _pow);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    complex /*echo*/ Convolver::executeOnPrepared(const real &period, real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime)
    {
        return executeOne(signalStartTime, signalSampleLength, signal, targetTime, period, _pow);
    }

}
