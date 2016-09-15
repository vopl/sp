#include "sp/signalConvolverLevel.hpp"
#include "sp/math.hpp"
#include "levmar.h"
#include <cassert>
#include <iostream>

#include <boost/multiprecision/float128.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/constants/constants.hpp>


namespace sp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolverLevel::SignalConvolverLevel(real ppw, real period, real signalSampleStep, std::size_t samplesPerPeriod, std::size_t polyOrder)
        : _ppw(ppw)
        , _period(period)
        , _signalSampleStep(signalSampleStep)
        , _samplesPerPeriod(samplesPerPeriod)
        , _sampleStep(_period/samplesPerPeriod)
        , _polyOrder(polyOrder)
        , _values(std::size_t(_samplesPerPeriod*ppw + 0.5))
        , _valuesFiltered(_samplesPerPeriod/4)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolverLevel::~SignalConvolverLevel()
    {
    }

    namespace
    {
        real ermit(real t, real y0, real y1, real dy0, real dy1)
        {
            real t2 = t*t;
            real t3 = t2*t;

            return
                    ( 2*t3 - 3*t2 + 0*t + 1)*y0 +
                    ( 1*t3 - 2*t2 + 1*t + 0)*dy0 +
                    (-2*t3 + 3*t2 + 0*t + 0)*y1 +
                    ( 1*t3 - 1*t2 + 0*t + 0)*dy1;
        }

        real iermit(real tStart, real tStop, real y0, real y1, real dy0, real dy1)
        {
            real tStart2 = tStart*tStart;
            real tStop2 = tStop*tStop;

            real tStart3 = tStart2*tStart;
            real tStop3 = tStop2*tStop;

            real tStart4 = tStart3*tStart;
            real tStop4 = tStop3*tStop;

            return
                    ((6*tStart4-12*tStart3)*y1+(-6*tStart4+12*tStart3-12*tStart)*y0+(-3*dy1-3*dy0)*tStart4+(4*dy1+8*dy0)*tStart3-6*dy0*tStart2)/12-
                    ((6*tStop4-12*tStop3)*y1+(-6*tStop4+12*tStop3-12*tStop)*y0+(-3*dy1-3*dy0)*tStop4+(4*dy1+8*dy0)*tStop3-6*dy0*tStop2)/12;
        }

        real iermit(real y0, real y1, real dy0, real dy1)
        {
            return
                    (6*y1+6*y0-dy1+dy0)/12;
        }


        real integrateErmitSpline01_(real xStart, real xStop, const real *ys)
        {
            real y0 = ys[1];
            real y1 = ys[2];

            real dy0 = (ys[2]-ys[0])/2;
            real dy1 = (ys[3]-ys[1])/2;

            return iermit(xStart, xStop, y0, y1, dy0, dy1);
        }

        real integrateErmitSpline01_(const real *ys)
        {
            real y0 = ys[1];
            real y1 = ys[2];

            real dy0 = (ys[2]-ys[0])/2;
            real dy1 = (ys[3]-ys[1])/2;

            return iermit(y0, y1, dy0, dy1);
        }
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
    void SignalConvolverLevel::update(const real *signal, std::size_t signalSize, SignalApproxType sat)
    {
        switch(sat)
        {
        case SignalApproxType::linear:
            for(std::size_t valueIndex(0); valueIndex<_values.size(); ++valueIndex)
            {
                real startTime = _sampleStep*valueIndex;
                real stopTime = startTime+_sampleStep;

                std::size_t signalStartIdx = std::size_t(startTime/_signalSampleStep);
                std::size_t signalStopIdx = std::size_t(stopTime/_signalSampleStep);

                assert(signalStartIdx >= 0);
                assert(signalStopIdx < signalSize-1);

                _values[valueIndex] = updateOneLinear(signal, signalSize, startTime, stopTime, signalStartIdx, signalStopIdx);
            }
            break;

        case SignalApproxType::poly6p5o32x:
            for(std::size_t valueIndex(0); valueIndex<_values.size(); ++valueIndex)
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

                _values[valueIndex] = updateOnePoly(signal, signalSize, startTime, stopTime, signalStartIdx, signalStopIdx);
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
    void SignalConvolverLevel::updateIdentity(real period, real phase)
    {
        real stepMult = real(1)/g_2pi/_sampleStep;
        for(std::size_t valueIndex(0); valueIndex<_values.size(); ++valueIndex)
        {
            real startTime = _sampleStep*valueIndex;
            real stopTime = startTime+_sampleStep;

            _values[valueIndex] = stepMult * (sin(g_2pi*stopTime + phase) - sin(g_2pi*startTime + phase));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolverLevel::filtrate(const TVReal &halfFir)
    {
        auto cidx = [&](std::size_t idx)
        {
            return (idx+_samplesPerPeriod*100) % _samplesPerPeriod;
        };

        std::vector<Summator<real>> result(_values.size());
        for(std::size_t index(0); index<_values.size(); ++index)
        {
            result[index] += _values[index];
        }

        for(std::size_t i(0); i<1; ++i)
        {
            real phaseStep = real(_samplesPerPeriod)/4;

            for(std::size_t k(0); k<1; k++)
            {
                auto int_ = filtrate_int(result);
                for(std::size_t index(0); index<result.size(); ++index)
                {
                    int_[index] += result[cidx(std::size_t(index - phaseStep - 0.25))]/2;
                }
                result.swap(int_);
            }

            for(std::size_t k(0); k<1; k++)
            {
                auto dif = filtrate_dif(result);
                for(std::size_t index(0); index<result.size(); ++index)
                {
                    dif[index] += result[cidx(std::size_t(index + phaseStep + 0.25))]/2;
                }
                result.swap(dif);
            }
        }

        for(std::size_t index(0); index<_samplesPerPeriod; ++index)
        {
            for(std::size_t index2(1); index2<std::size_t(_ppw+0.5); ++index2)
            {
                result[index] += result[index2*_samplesPerPeriod + index];
            }
        }

        std::size_t samplesPerPeriodDiv4 = _samplesPerPeriod/4;
        for(std::size_t index(0); index<samplesPerPeriodDiv4; ++index)
        {
            result[index] +=  result[samplesPerPeriodDiv4*2-1 -index];
            result[index] += -result[samplesPerPeriodDiv4*2   +index];
            result[index] += -result[samplesPerPeriodDiv4*4-1 -index];

            _valuesFiltered[index] = result[index];
        }

    }

    std::vector<Summator<real>> SignalConvolverLevel::filtrate_int(const std::vector<Summator<real>> &src)
    {
        std::vector<Summator<real>> res(src);

        auto cidx = [&](std::size_t idx)
        {
            return (idx+src.size()) % src.size();
        };

        Summator<real> sum;
        for(std::size_t index(0); index<src.size(); ++index)
        {
            sum += src[cidx(index)];
            res[index] = sum.v()*(g_2pi*_ppw)/src.size();
        }

        return res;
    }

    std::vector<Summator<real>> SignalConvolverLevel::filtrate_dif(const std::vector<Summator<real>> &src)
    {
        std::vector<Summator<real>> res(src.size());

        auto cidx = [&](std::size_t idx)
        {
            return (idx+src.size()) % src.size();
        };

        for(std::size_t index(0); index<src.size()-1; ++index)
        {
            res[index] = (src[cidx(index+1)]-src[cidx(index)])*src.size()/(g_2pi*_ppw);
        }

        return res;
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


    namespace
    {
        complex approxCosPlusPoly(real period, TVReal &ys, std::size_t polyOrder)
        {
            real levmarInfo[LM_INFO_SZ];

            static real levmarOpts[LM_OPTS_SZ] =
            {
                1e-40,  //LM_INIT_MU,        //mu
                std::numeric_limits<real>::min(),  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
                std::numeric_limits<real>::min(),  //LM_STOP_THRESH,    //||Dp||_2 and
                std::numeric_limits<real>::min(),  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
            };

            static TVReal args;
            args.resize(2+polyOrder+1);
            std::fill(args.begin(), args.end(), real(0));

            static TVReal work;
            if(work.size() < LM_DIF_WORKSZ(args.size(), ys.size()))
            {
                work.resize(LM_DIF_WORKSZ(args.size(), ys.size()));
            }


            using bigreal = boost::multiprecision::float128;

            struct Params
            {
                real _period;
                std::vector<std::vector<bigreal>> _bigbasis;
                std::vector<TVReal> _basis;

                std::vector<bigreal> _ys;
            };

            static Params params;

            params._ys.assign(ys.begin(), ys.end());

            std::size_t n = ys.size();
            std::size_t m = args.size();
            if(
               period != params._period ||
               params._bigbasis.size() != n ||
               (!params._bigbasis.empty() && params._bigbasis[0].size()!=m))
            {
                params._period = period;

                using verybigreal = boost::multiprecision::cpp_dec_float_100;

                verybigreal pi = boost::math::constants::pi<verybigreal>();
                verybigreal pi2 = pi*2;

                params._bigbasis.resize(n);
                params._basis.resize(n);
                for(std::size_t i(0); i<n; i++)
                {
                    params._bigbasis[i].resize(m);
                    params._basis[i].resize(m);
                }

                /////
                for(std::size_t i(0); i<n; i++)
                {
                    verybigreal x = verybigreal(i)/(n);
                    verybigreal wnd = 1;//rect -- наилучшее пока что
                    //verybigreal wnd = (0.5 - 0.5*cos(pi2*x));//hann
                    //verybigreal wnd = 1-(0.5 - 0.5*cos(pi2*x));//rev hann
                    //verybigreal wnd = (0.54 - 0.46*cos(pi2*x));//hamming
                    //verybigreal wnd = (cos(pi/2*x-pi/4));//hann half
                    params._bigbasis[i][0] = verybigreal(cos(pi2*x/params._period)*wnd).convert_to<bigreal>();
                    params._bigbasis[i][1] = verybigreal(sin(pi2*x/params._period)*wnd).convert_to<bigreal>();
                }

                /////
                std::vector<verybigreal> poly(polyOrder+1);
                for(std::size_t i(0); i<n; i++)
                {
                    verybigreal time = verybigreal(i)/(n-1)*2 - 1;
                    verybigreal time2 = time*2;

                    if(polyOrder == 0)
                    {
                        poly[0] = 1;
                    }
                    else
                    {
                        poly[0] = 1;
                        poly[1] = time;

                        for(size_t o=2; o<=polyOrder; o++)
                        {
                            poly[o] = time2*poly[o-1] - poly[o-2];
                        }
                    }

                    for(size_t o=0; o<=polyOrder; o++)
                    {
                        params._bigbasis[i][o+2] = poly[o].convert_to<bigreal>();
                    }
                }

                /////
                for(std::size_t i(0); i<n; i++)
                {
                    for(std::size_t j(0); j<m; j++)
                    {
                        params._basis[i][j] = params._bigbasis[i][j].convert_to<real>();
                    }
                }
            }


            int levmarResult = levmar_der(
                        [](real *p, real *hx, int m, int n, void *_params)->void{
                            Params &params = *reinterpret_cast<Params*>(_params);

                            for(std::size_t i(0); i<std::size_t(n); i++)
                            {
                                //Summator<bigreal> sum(0);
                                bigreal sum(0);

                                for(std::size_t j(0); j<std::size_t(m); j++)
                                {
                                    sum += params._bigbasis[i][j] * p[j];
                                }

                                hx[i] = bigreal(sum - params._ys[i]).convert_to<real>();
                            }
                        },
                        [](real *p, real *jx, int m, int n, void *_params){

                            Params &params = *reinterpret_cast<Params*>(_params);

                            for(std::size_t i(0); i<std::size_t(n); i++)
                            {
                                for(std::size_t j(0); j<std::size_t(m); j++)
                                {
                                    jx[i*std::size_t(m)+j] = params._basis[i][j];
                                }
                            }
                        },
                        &args[0],
                        NULL, //&dys[0],
                        args.size(),
                        ys.size(),
                        150,
                        levmarOpts,
                        levmarInfo,
                        &work[0],
                        NULL,
                        &params);

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

            return complex(
                args[0]/*+args[2]+args[4]+args[6]+args[8]+args[10]*/,
                args[1]/*+args[3]+args[5]+args[7]+args[9]*/);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    complex SignalConvolverLevel::convolve()
    {
        if(_polyOrder)
        {
            return approxCosPlusPoly(real(_valuesFiltered.size())/_samplesPerPeriod, _valuesFiltered, _polyOrder);
        }

        Summator<complex> res;

        for(std::size_t mult(1); mult<=16; mult+=1)
        {
            real step01 = real(mult)/_samplesPerPeriod;

            real x0 = 0;
            real y0 = _valuesFiltered[0];

            for(std::size_t index(1); index<_valuesFiltered.size(); ++index)
            {
                real x1 = index * step01;
                real y1 = _valuesFiltered[index];
                res += evalSegment(x0, y0, x1, y1);

                x0 = x1;
                y0 = y1;
            }
        }

        return res;// / (_period);
    }

}
