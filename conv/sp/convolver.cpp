#include "convolver.hpp"
#include <iostream>
#include <boost/math/special_functions.hpp>

namespace sp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Convolver::Convolver(real pow, real periodMin, real periodMax, std::size_t periodSteps)
        : _pow(pow)
    {
        real minTLog = log(periodMin);
        real maxTLog = log(periodMax);
        real logtStep = (maxTLog - minTLog) / (periodSteps-1);
        _periodGrid.resize(periodSteps);
        for(std::size_t k(0); k<periodSteps; k++)
        {
            _periodGrid[k] = exp(minTLog+k*logtStep);
        }
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
            assert(indexStop < signal.size());

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
            real Fs,
            real Fd,
            std::size_t n,
            TVReal &A)
        {
            real f = Fs/Fd;
            real w = g_2pi*f;
            real q = 2*f;

            n+=2;
            n |= 1;
            A.resize(n-2);

            std::size_t mn = n/2-1;
            A[mn] = q;
            for(std::size_t k(1); k<n/2; k++)
            {
                //A[mn+k] = A[mn-k] = q*(sin(k*w)/(k*w));
                A[mn+k] = A[mn-k] = q* boost::math::sinc_pi(k*w);
            }

            real kaizerBeta = 10;
            TVReal _wnd(A.size());
            n = _wnd.size();
            mn = n/2;
            _wnd[mn+0] = kaizer(kaizerBeta, n/2, n);
            for(std::size_t k(1); k<=n/2; k++)
            {
                if(mn+k < _wnd.size())
                {
                    _wnd[mn+k] = kaizer(kaizerBeta, n/2+k, n);
                }
                _wnd[mn-k] = kaizer(kaizerBeta, n/2-k, n);
            }

            for(std::size_t i(0); i<A.size(); i++)
            {
                A[i] *= _wnd[i];
            }

        }

        //МНОГОПОТОК НЕ ПРОЙДЕТ
        //фиры тяжело вычисляются, в основном из за кайзера. Это прекэшированные значения
        using FirId = std::tuple<real, std::size_t, std::size_t>;
        FirId g_firId{0,0,0};
        std::vector<std::vector<real>> g_firs;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        real /*signalStartTime*/ prepareSignal(real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, real pow, real maxT, TVReal &preparedSignal)
        {
            std::size_t minLen = std::size_t(pow*10);//по 10 точек на период, иначе fir не справляется
            std::size_t maxLen = std::size_t(pow*(maxT/signalSampleLength)*2) + minLen + 2;

            std::size_t endIdx = std::size_t((targetTime - signalStartTime)/signalSampleLength) + minLen/2;
            assert(endIdx <= signal.size());

            preparedSignal.resize((maxLen - minLen)/2);

            FirId firId{pow, minLen, maxLen};
            if(firId != g_firId)
            {
                g_firs.clear();
                g_firs.resize((maxLen - minLen)/2);
                for(std::size_t len(minLen); len < maxLen; len+=2)
                {
                    //std::cerr<<"make fir "<<len<<"/"<<maxLen<<std::endl;
                    real bndT = len/(pow-0.5);
                    lowPassFir(1+1.0/pow, bndT/2, len, g_firs[(len-minLen)/2]);
                }

                g_firId = firId;
            }

            for(std::size_t len(minLen); len < maxLen; len+=2)
            {
                //TVReal fir;
                //real bndT = len/(pow-0.5);
                //lowPassFir(1+1.0/pow, bndT/2, len, fir);
                const TVReal &fir = g_firs[(len-minLen)/2];

                real preparedValue = 0;
                for(std::size_t i(0); i<fir.size(); i++)
                {
                    preparedValue += signal[endIdx-1-i] * fir[i];
                }
                preparedSignal[preparedSignal.size()-1 - (len-minLen)/2] = preparedValue;
            }

            return targetTime - preparedSignal.size()*signalSampleLength;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Convolver::execute(real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo)
    {
        TVReal preparedSignal;
        signalStartTime = prepareSignal(signalStartTime, signalSampleLength, signal, targetTime, _pow, _periodGrid.back(), preparedSignal);

//        for(std::size_t idx(0); idx<4000; ++idx)
//        {
//            std::cout<<signal[signal.size()-1-idx]<<", "<<preparedSignal[preparedSignal.size()-1-idx]<<std::endl;
//        }
//        exit(0);

        echo.resize(_periodGrid.size());
        for(std::size_t i(0); i<_periodGrid.size(); i++)
        {
            echo[i] = executeOne(signalStartTime, signalSampleLength, signal, targetTime, _periodGrid[i], _pow);
        }
    }

}
