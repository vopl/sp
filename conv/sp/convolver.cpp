#include "convolver.hpp"

namespace sp
{
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

    Convolver::~Convolver()
    {

    }

    namespace
    {
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

    void Convolver::execute(real signalStartTime, real signalSampleLength, const TVReal &signal, real targetTime, TVComplex &echo)
    {
        echo.resize(_periodGrid.size());
        for(std::size_t i(0); i<_periodGrid.size(); i++)
        {
            echo[i] = executeOne(signalStartTime, signalSampleLength, signal, targetTime, _periodGrid[i], _pow);
        }
    }

}
