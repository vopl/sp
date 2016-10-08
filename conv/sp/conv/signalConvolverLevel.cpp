#include "sp/conv/signalConvolverLevel.hpp"
#include "sp/math.hpp"
#include "levmar.h"
#include <cassert>
#include <iostream>

#include <boost/multiprecision/float128.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/constants/constants.hpp>


namespace sp { namespace conv
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolverLevel::SignalConvolverLevel(real ppw, real period, real signalSampleStep, std::size_t samplesPerPeriod)
        : _ppw(ppw)
        , _period(period)
        , _signalSampleStep(signalSampleStep)
        , _samplesPerPeriod(samplesPerPeriod)
        , _sampleStep(_period/samplesPerPeriod)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolverLevel::~SignalConvolverLevel()
    {
    }

    namespace
    {
        //http://yehar.com/blog/wp-content/uploads/2009/08/deip.pdf
        //6 point 5th order 32x

        //[xStart, xStop]
        void integrate(Summator<real> &res, real xStart, real xStop, const real *y)
        {
            real even1 = y[3]+y[2];
            real even2 = y[4]+y[1];
            real even3 = y[5]+y[0];

            real odd1 = y[3]-y[2];
            real odd2 = y[4]-y[1];
            real odd3 = y[5]-y[0];


            real c0 = even1* real( 0.42685983409379380L)  + even2* real( 0.07238123511170030L) + even3* real( 0.00075893079450573L);
            real c1 = odd1 * real( 0.35831772348893259L)  + odd2 * real( 0.20451644554758297L) + odd3 * real( 0.00562658797241955L);
            real c2 = even1* real(-0.217009177221292431L) + even2* real( 0.20051376594086157L) + even3* real( 0.01649541128040211L);
            real c3 = odd1 * real(-0.25112715343740988L)  + odd2 * real( 0.04223025992200458L) + odd3 * real( 0.02488727472995134L);
            real c4 = even1* real( 0.04166946673533273L)  + even2* real(-0.06250420114356986L) + even3* real( 0.02083473440841799L);
            real c5 = odd1 * real( 0.08349799235675044L)  + odd2 * real(-0.04174912841630993L) + odd3 * real( 0.00834987866042734L);


//            assert(xStart>=0 && xStart<=1);
//            assert(xStop>=0 && xStop<=1);
//            assert(xStart<=xStop);

            real xStart2 = xStart *xStart;
            real xStart3 = xStart2*xStart;
            real xStart4 = xStart3*xStart;
            real xStart5 = xStart4*xStart;
            real xStart6 = xStart5*xStart;

            real xStop2 = xStop *xStop;
            real xStop3 = xStop2*xStop;
            real xStop4 = xStop3*xStop;
            real xStop5 = xStop4*xStop;
            real xStop6 = xStop5*xStop;

            res+=(80*c5)*xStop6/480;
            res+=(-240*c5+96*c4)*xStop5/480;
            res+=(300*c5-240*c4+120*c3)*xStop4/480;
            res+=(-200*c5+240*c4-240*c3+160*c2)*xStop3/480;
            res+=(75*c5-120*c4+180*c3-240*c2+240*c1)*xStop2/480;
            res+=(-15*c5+30*c4-60*c3+120*c2-240*c1+480*c0)*xStop/480;

            res+=(-80*c5)*xStart6/480;
            res+=(240*c5-96*c4)*xStart5/480;
            res+=(-300*c5+240*c4-120*c3)*xStart4/480;
            res+=(200*c5-240*c4+240*c3-160*c2)*xStart3/480;
            res+=(-75*c5+120*c4-180*c3+240*c2-240*c1)*xStart2/480;
            res+=(15*c5-30*c4+60*c3-120*c2+240*c1-480*c0)*xStart/480;
        }

        //[0, 1]
        void integrate(Summator<real> &res, const real *y)
        {
            real even1 = y[3]+y[2];
            real even2 = y[4]+y[1];
            real even3 = y[5]+y[0];

//            real odd1 = y[3]-y[2];
//            real odd2 = y[4]-y[1];
//            real odd3 = y[5]-y[0];


//            real c0 = even1* 0.42685983409379380  + even2* 0.07238123511170030 + even3*0.00075893079450573;
////            real c1 = odd1 * 0.35831772348893259  + odd2 * 0.20451644554758297 + odd3 *0.00562658797241955;
//            real c2 = even1*-0.217009177221292431 + even2* 0.20051376594086157 + even3*0.01649541128040211;
////            real c3 = odd1 *-0.25112715343740988  + odd2 * 0.04223025992200458 + odd3 *0.02488727472995134;
//            real c4 = even1* 0.04166946673533273  + even2*-0.06250420114356986 + even3*0.02083473440841799;
////            real c5 = odd1 * 0.08349799235675044  + odd2 *-0.04174912841630993 + odd3 *0.00834987866042734;

//            return
//                    c4/80+c2/12+c0;


            res += even1 * (real(0.42685983409379380L) + real(-0.217009177221292431L/12) + real( 0.04166946673533273L/80));
            res += even2 * (real(0.07238123511170030L) + real( 0.20051376594086157L /12) + real(-0.06250420114356986L/80));
            res += even3 * (real(0.00075893079450573L) + real( 0.01649541128040211L /12) + real( 0.02083473440841799L/80));
        }
    }

    /////////-/////////-/////////-/////////-/////////-/////////-/////////-/////////-
    real SignalConvolverLevel::updateOneLinear(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx)
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
            return sum.v()/amount;
        }

        return 0;
    }

    real SignalConvolverLevel::updateOnePoly(const real *signal, std::size_t signalSize, real startTime, real stopTime, std::size_t signalStartIdx, std::size_t signalStopIdx)
    {
        Summator<real> sum;
        real amount = 0;

        if(signalStartIdx == signalStopIdx)
        {
            //from single signal sample
            real first01x = (startTime - (signalStartIdx)*_signalSampleStep) / _signalSampleStep;
            real last01x = (stopTime - (signalStopIdx)*_signalSampleStep) / _signalSampleStep;

            integrate(sum, first01x, last01x, signal+signalStartIdx-2);
            amount = last01x - first01x;
        }
        else
        {
            //from multiple signal samples

            if(signalStartIdx < signalStopIdx-1)
            {
                for(std::size_t signalIndex(signalStartIdx+1); signalIndex<signalStopIdx; ++signalIndex)
                {
                    integrate(sum, signal+signalIndex-2);
                }
            }
            amount = signalStopIdx - signalStartIdx - 1;

            real first01x = (startTime - (signalStartIdx)*_signalSampleStep) / _signalSampleStep;
            integrate(sum, first01x, 1, signal+signalStartIdx-2);
            amount += 1 - first01x;

            real last01x = (stopTime - (signalStopIdx)*_signalSampleStep) / _signalSampleStep;
            integrate(sum, 0, last01x, signal+signalStopIdx-2);
            amount += last01x;
        }

        if(amount>0)
        {
            return sum.v()/amount;
        }

        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolverLevel::update(TVReal &values, const real *signal, std::size_t signalSize, SignalApproxType sat)
    {
        switch(sat)
        {
        case SignalApproxType::linear:
            for(std::size_t valueIndex(0); valueIndex<values.size(); ++valueIndex)
            {
                real startTime = _sampleStep*valueIndex;
                real stopTime = startTime+_sampleStep;

                std::size_t signalStartIdx = std::size_t(startTime/_signalSampleStep);
                std::size_t signalStopIdx = std::size_t(stopTime/_signalSampleStep);

                assert(signalStartIdx >= 0);
                assert(signalStopIdx < signalSize-1);

                values[valueIndex] = updateOneLinear(signal, signalSize, startTime, stopTime, signalStartIdx, signalStopIdx);
            }
            break;

        case SignalApproxType::poly6p5o32x:
            for(std::size_t valueIndex(0); valueIndex<values.size(); ++valueIndex)
            {
                real startTime = _sampleStep*valueIndex+ 2*_signalSampleStep;
                real stopTime = startTime+_sampleStep;

                std::size_t signalStartIdx = std::size_t(startTime/_signalSampleStep);
                std::size_t signalStopIdx = std::size_t(stopTime/_signalSampleStep);

                if(signalStartIdx < 2)
                {
                    signalStartIdx++;
                    signalStopIdx++;
                }

                assert(signalStartIdx >= 2);
                assert(signalStopIdx < signalSize-1-3);

                values[valueIndex] = updateOnePoly(signal, signalSize, startTime, stopTime, signalStartIdx, signalStopIdx);
            }
            break;

//        default:
//            assert(0);
//            abort();
        }

//        for(const auto &v: _values)
//        {
//            std::cout<<v<<std::endl;
//        }

//        exit(0);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolverLevel::updateIdentity(TVReal &values, real period, real phase)
    {
        real stepMult = real(1)/g_2pi/_sampleStep;
        for(std::size_t valueIndex(0); valueIndex<values.size(); ++valueIndex)
        {
            real startTime = _sampleStep*valueIndex;
            real stopTime = startTime+_sampleStep;

            values[valueIndex] = stepMult * (sin(g_2pi*stopTime + phase) - sin(g_2pi*startTime + phase));
        }
    }


    namespace
    {
        complex evalSegment(real x0, real y0, real x1, real y1)
        {
            const real pi2 = g_2pi;
            const real pi2_p_2 = pi2*pi2;

            const real x1_minus_x0 = x1-x0;

            real re, im;
            {
                const real _4_1 = x1*pi2;
                const real _4_2 = x0*pi2;

                const real _6 = cos ( _4_1 ) ;
                const real _8 = sin ( _4_2 ) ;
                const real _5 = sin ( _4_1 ) ;
                const real _7 = cos ( _4_2 ) ;





                const real _0 =  pi2 * ( x1_minus_x0 ) ;

                const real _7_2 = (_7-_6);
                const real _9  =  ( _0*_5 - _7_2 ) ;
                const real _10 =  ( _8*_0 - _7_2 ) ;
                const real _13 =  ( _9*y1 - _10*y0 ) ;

                const real _3 =  pi2_p_2 * ( x1_minus_x0 ) ;
                re = _13/_3;

                const real _9_1 = (_5-_8);
                const real _11 =  ( _9_1 -_0*_6 ) ;
                const real _12 =  ( _9_1 -_7*_0 ) ;
                const real _14 =  ( _11*y1 - _12*y0 ) ;

                im = _14/_3;
            }

            return complex(re, im);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    complex SignalConvolverLevel::convolve(const real *signal, std::size_t signalSize, SignalApproxType sat, TVReal &valuesWorkArray)
    {
        assert(valuesWorkArray.size() == std::size_t(_samplesPerPeriod*_ppw + 0.5));
        update(valuesWorkArray, signal, signalSize, sat);

        return convolve(valuesWorkArray);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    complex SignalConvolverLevel::convolveIdentity(real period, real phase)
    {
        TVReal values(std::size_t(_samplesPerPeriod*_ppw + 0.5));
        updateIdentity(values, period, phase);

        return convolve(values);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    complex SignalConvolverLevel::convolve(const TVReal &values)
    {
        Summator<complex> res;

        {
            real step01 = real(1.0)/_samplesPerPeriod;
            real x0 = 0;
            real y0 = values[0];

            for(std::size_t index(1); index<values.size(); ++index)
            {
                real x1 = index * step01;
                real y1 = values[index];
                res += evalSegment(x0, y0, x1, y1);

                x0 = x1;
                y0 = y1;
            }
        }

        return res;
    }

}}
