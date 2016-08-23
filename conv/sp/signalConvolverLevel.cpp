#include "signalConvolverLevel.hpp"
#include <cassert>
#include <iostream>

namespace sp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolverLevel::SignalConvolverLevel(real pow, real period, real signalSampleStep, std::size_t samplesPerPeriod)
        : _pow(pow)
        , _period(period)
        , _signalSampleStep(signalSampleStep)
        , _sampleStep(_period/samplesPerPeriod)
        , _values(std::size_t(samplesPerPeriod * pow * 2+0.5))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolverLevel::~SignalConvolverLevel()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolverLevel::update(const real *signal, std::size_t signalSize)
    {
        real totalStopTime = _signalSampleStep * signalSize;
        real totalStartTime = totalStopTime - _sampleStep*_values.size();

        for(std::size_t valueIndex(0); valueIndex<_values.size(); ++valueIndex)
        {
            real sum = 0, amount = 0;

            {
                real firstSample0,firstSample1;
                real lastSample0,lastSample1;

                real startTime = totalStartTime+_sampleStep*valueIndex;
                real stopTime = startTime+_sampleStep;

                std::size_t signalStartIdx = std::size_t(startTime/_signalSampleStep);
                std::size_t signalStopIdx = std::size_t(stopTime/_signalSampleStep);
                if(signalStopIdx >= signalSize)
                {
                    signalStopIdx--;
                }

                assert(signalStopIdx < signalSize);

                firstSample0 = signal[signalStartIdx];
                if(signalStartIdx < signalSize-1)
                {
                    firstSample1 = signal[signalStartIdx+1];
                }
                else
                {
                    firstSample1 = signal[signalStartIdx];
                }


                lastSample0 = signal[signalStopIdx];
                if(signalStopIdx < signalSize-1)
                {
                    lastSample1 = signal[signalStopIdx+1];
                }
                else
                {
                    lastSample1 = signal[signalStopIdx];
                }

                if(signalStartIdx < signalStopIdx)
                {
                    //from multiple signal samples

                    if(signalStartIdx < signalStopIdx-1)
                    {
                        sum += signal[signalStartIdx+1]/2;
                        for(std::size_t signalIndex(signalStartIdx+2); signalIndex<signalStopIdx-1; ++signalIndex)
                        {
                            sum += signal[signalIndex];
                        }
                        sum += signal[signalStopIdx]/2;
                    }

                    amount = signalStopIdx - signalStartIdx - 1;

                    real firstx = (startTime - (signalStartIdx)*_signalSampleStep) / _signalSampleStep;
                    real lastx = (stopTime - (signalStopIdx)*_signalSampleStep) / _signalSampleStep;

                    firstSample0 = firstSample0 + (firstSample1 - firstSample0)*firstx;

                    real amount1 = 1 - firstx;
                    sum += amount1 * (firstSample0+firstSample1)/2;

                    lastSample1 = lastSample0 + (lastSample1 - lastSample0)*lastx;

                    real amount2 = lastx;

                    sum += amount2 * (lastSample0+lastSample1)/2;

                    amount += amount1 + amount2;
                }
                else
                {
                    //from single signal sample
                    amount = 0;

                    real firstx = (startTime - (signalStartIdx)*_signalSampleStep) / _signalSampleStep;
                    real lastx = (stopTime - (signalStopIdx)*_signalSampleStep) / _signalSampleStep;

                    firstSample0 = firstSample0 + (firstSample1 - firstSample0)*firstx;
                    lastSample1 = lastSample0 + (lastSample1 - lastSample0)*lastx;

                    real amount12 = lastx - firstx;
                    assert(amount12>=0-std::numeric_limits<real>::epsilon()*100000 && amount12<=1+std::numeric_limits<real>::epsilon()*100000);

                    sum += amount12 * (firstSample0+lastSample1)/2;
                    amount += amount12;
                }
            }


            if(amount>0)
            {
                _values[valueIndex] = sum/amount;
            }
            else
            {
                _values[valueIndex] = 0;
            }
        }
    }

}
