#include "sp/signalConvolverLevel.hpp"
#include "sp/math.hpp"
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
        , _values(std::size_t(samplesPerPeriod*pow*2 + 0.5))
        , _valuesFiltered(std::size_t(samplesPerPeriod*pow + 0.5))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolverLevel::~SignalConvolverLevel()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolverLevel::update(const real *signal, std::size_t signalSize)
    {
        for(std::size_t valueIndex(0); valueIndex<_values.size(); ++valueIndex)
        {
            real startTime = _sampleStep*valueIndex;
            real stopTime = startTime+_sampleStep;

            std::size_t signalStartIdx = std::size_t(startTime/_signalSampleStep);
            std::size_t signalStopIdx = std::size_t(stopTime/_signalSampleStep);

            assert(signalStopIdx < signalSize-1);

            Summator<real> sum;
            real amount = 0;

            real firstSampleStart = signal[signalStartIdx];
            real firstSampleStop = signal[signalStartIdx+1];

            real lastSampleStart = signal[signalStopIdx];
            real lastSampleStop = signal[signalStopIdx+1];

            if(signalStartIdx == signalStopIdx)
            {
                //from single signal sample
                real first01x = (startTime - (signalStartIdx)*_signalSampleStep) / _signalSampleStep;
                real last01x = (stopTime - (signalStopIdx)*_signalSampleStep) / _signalSampleStep;

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

                real first01x = (startTime - (signalStartIdx)*_signalSampleStep) / _signalSampleStep;
                firstSampleStart = firstSampleStart + (firstSampleStop - firstSampleStart)*first01x;
                real amount1 = 1 - first01x;
                sum += amount1 * (firstSampleStart+firstSampleStop)/2;
                amount += amount1;

                real last01x = (stopTime - (signalStopIdx)*_signalSampleStep) / _signalSampleStep;
                lastSampleStop = lastSampleStart + (lastSampleStop - lastSampleStart)*last01x;
                real amount2 = last01x;
                sum += amount2 * (lastSampleStart+lastSampleStop)/2;
                amount += amount2;

            }

            if(amount>0)
            {
                _values[valueIndex] = sum/amount;
            }
            else
            {
                _values[valueIndex] = 0;
            }

            //std::cout<<_values[valueIndex]<<std::endl;
        }
        //exit(0);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolverLevel::filtrate(const std::vector<std::vector<real>> &halfFirs)
    {


        _valuesFiltered[0] = _values[0];

        for(std::size_t index(0); index<_valuesFiltered.size()-1; ++index)
        {
            if(index < std::size_t(_pow + 1.5))//попробовать без этого
            {
                _valuesFiltered[index+1] = _values[index+1];
                continue;
            }

            {
                assert(index < halfFirs.size());
                const std::vector<real> &halfFir = halfFirs[index];
                const std::size_t halfFirSize = halfFir.size();
                assert(halfFirSize>=2);
                const std::size_t firSize = (halfFirSize-1)*2+1;

                Summator<real> sum = 0;
                sum += (_values[0] + _values[firSize-1]) * halfFir[0] / 2;
                for(std::size_t i(1); i<halfFirSize-1; ++i)
                {
                    sum += (_values[i] + _values[firSize-1-i]) * halfFir[i];
                }
                sum += _values[halfFirSize-1] * halfFir[halfFirSize-1];

                _valuesFiltered[index+1] = sum;
            }
        }

//        for(std::size_t index(0); index<_valuesFiltered.size(); ++index)
//        {
//            std::cout<<_values[index]<<", "<<_valuesFiltered[index]<<std::endl;
//        }

//        exit(0);
    }

    namespace
    {
        complex evalSegment(real period, real x0, real y0, real x1, real y1)
        {
            const real pi2 = g_2pi;
            const real pi2_p_2 = pi2*pi2;

            const real t = period;
            const real t_p_2 = t*t;

            const real x1_minus_x0 = x1-x0;

            const real pi2_mul_t = pi2*t;

            real re, im;
            {
                const real _0_1 =  pi2/t;


                const real _4_1 = x1*_0_1;
                const real _4_2 = x0*_0_1;

                const real _6 = cos ( _4_1 ) ;
                const real _8 = sin ( _4_2 ) ;
                const real _5 = sin ( _4_1 ) ;
                const real _7 = cos ( _4_2 ) ;





                const real _0 =  pi2_mul_t * ( x1_minus_x0 ) ;

                const real _7_2 = t_p_2*(_7-_6);
                const real _9  =  ( _0*_5 - _7_2 ) ;
                const real _10 =  ( _8*_0 - _7_2 ) ;
                const real _13 =  ( _9*y1 - _10*y0 ) ;

                const real _3 =  pi2_p_2 * ( x1_minus_x0 ) ;
                re = _13/_3;

                const real _9_1 = t_p_2*(_5-_8);
                const real _11 =  ( _9_1 -_0*_6 ) ;
                const real _12 =  ( _9_1 -_7*_0 ) ;
                const real _14 =  ( _11*y1 - _12*y0 ) ;

                im = _14/_3;
            }


//            real re2, im2;
//            {
//                const real pi = g_pi;
//                const real pi_p_2 = pi*pi;

//                //TODO упростить
//                const real _1 =  ( 2*pi*x1 ) ;
//                const real _3 =  ( 4*pi_p_2*x1-4*pi_p_2*x0 ) ;
//                const real _0 =  ( 2*pi*t*x1-2*pi*t*x0 ) ;
//                const real _2 =  ( 2*pi*x0 ) ;
//                const real _4 =  ( 2*pi*t*x0-2*pi*t*x1 ) ;
//                const real _6 = cos ( _1/t ) ;
//                const real _8 = sin ( _2/t ) ;
//                const real _5 = sin ( _1/t ) ;
//                const real _7 = cos ( _2/t ) ;
//                const real _10 =  ( -t_p_2*_6-2*pi*t*_8*x1+2*pi*t*x0*_8+t_p_2*_7 ) ;
//                const real _9 =  ( _0*_5+t_p_2*_6-t_p_2*_7 ) ;
//                const real _11 =  ( t_p_2*_5+_4*_6-t_p_2*_8 ) ;
//                const real _12 =  ( -t_p_2*_5+2*pi*t*_7*x1+t_p_2*_8-2*pi*t*x0*_7 ) ;
//                const real _13 =  ( _9*y1+_10*y0 ) ;
//                const real _14 =  ( _11*y1+_12*y0 ) ;
//                //expr

//                re2 = _13/_3;im2 = _14/_3;
//            }

//            if(fabs(re-re2) > 1e-12 || fabs(im-im2) > 1e-12)
//            {
//                std::cerr<<fabs(re-re2)<<", "<<fabs(im-im2)<<std::endl;
//                exit(0);
//            }



            return complex(re, im);
        }
    }
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    complex SignalConvolverLevel::convolve()
    {
        const real step0 = 1 / _pow;

        real period_m1 = _period /(1.0-step0/2);
        real period_p1 = _period /(1.0+step0/2);

        Summator<complex> res_m1;
        Summator<complex> res_p1;

        for(std::size_t index(0); index<_valuesFiltered.size()-1; ++index)
        {
            real x0 = index * _sampleStep;
            real x1 = x0 + _sampleStep;

            real y0 = _valuesFiltered[index];
            real y1 = _valuesFiltered[index+1];

            res_m1 += evalSegment(period_m1, x0, y0, x1, y1);
            res_p1 + evalSegment(period_p1, x0, y0, x1, y1);
        }

        return (res_p1*(1.0+step0/2) - res_m1*(1.0-step0/2)) / (_period * _pow);
    }

}
