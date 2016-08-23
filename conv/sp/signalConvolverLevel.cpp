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
    void SignalConvolverLevel::update(
            const real *first, std::size_t firstSize,
            const real *second, std::size_t secondSize)
    {
        real totalStopTime = _signalSampleStep * (firstSize + secondSize);
        real totalStartTime = totalStopTime - _sampleStep*_values.size();

        for(std::size_t valueindex(0); valueindex<_values.size(); ++valueindex)
        {
            real startTime = totalStartTime+_sampleStep*valueindex;
            real stopTime = totalStartTime+_sampleStep*(valueindex+1);

            std::size_t signalStartIdx = std::size_t(startTime/_signalSampleStep);
            std::size_t signalStopIdx = std::size_t(stopTime/_signalSampleStep);

            real firstSample, lastSample;

            real sum = 0;
            if(signalStopIdx < firstSize)
            {
                //from first
                firstSample = first[signalStartIdx];
                lastSample = first[signalStopIdx];

                for(std::size_t signalIndex(signalStartIdx+1); signalIndex<signalStopIdx; ++signalIndex)
                {
                    sum += first[signalIndex];
                }
            }
            else if(signalStartIdx >= firstSize)
            {
                //from second
                firstSample = second[signalStartIdx-firstSize];
                lastSample = second[signalStopIdx-firstSize];

                for(std::size_t signalIndex(signalStartIdx-firstSize+1); signalIndex<signalStopIdx-firstSize; ++signalIndex)
                {
                    sum += second[signalIndex];
                }
            }
            else
            {
                //from both
                firstSample = first[signalStartIdx];
                lastSample = second[signalStopIdx-firstSize];

                for(std::size_t signalIndex(signalStartIdx+1); signalIndex<firstSize; ++signalIndex)
                {
                    sum += first[signalIndex];
                }

                for(std::size_t signalIndex(0); signalIndex<signalStopIdx-firstSize; ++signalIndex)
                {
                    sum += second[signalIndex];
                }
            }

            real amount;

            if(signalStopIdx == signalStartIdx)
            {
                //from single signal sample
                amount = 0;

                real amount12 = (stopTime - startTime) / _signalSampleStep;
                assert(amount12>=0-std::numeric_limits<real>::epsilon()*100000 && amount12<=1+std::numeric_limits<real>::epsilon()*100000);

                sum += firstSample * amount12;
                amount += amount12;
            }
            else
            {
                //from multiple signal samples
                amount = signalStopIdx - signalStartIdx - 1;

                real amount1 = ((signalStartIdx+1)*_signalSampleStep - startTime) / _signalSampleStep;
                assert(amount1>=0-std::numeric_limits<real>::epsilon()*100000 && amount1<=1+std::numeric_limits<real>::epsilon()*100000);
                sum += firstSample * amount1;

                real amount2 = (stopTime - (signalStopIdx)*_signalSampleStep) / _signalSampleStep;
                assert(amount2>=0-std::numeric_limits<real>::epsilon()*100000 && amount2<=1+std::numeric_limits<real>::epsilon()*100000);
                sum += lastSample * amount2;

                amount += amount1 + amount2;
            }

            if(amount>0)
            {
                _values[valueindex] = sum/amount;
            }
            else
            {
                _values[valueindex] = 0;
            }

            std::cout<<_values[valueindex]<<std::endl;
        }
    }

}
