#include "sp/signalConvolverLevel.hpp"
#include "sp/math.hpp"
#include "levmar.h"
#include <cassert>
#include <iostream>

namespace sp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolverLevel::SignalConvolverLevel(real ppw, real period, real signalSampleStep, std::size_t samplesPerPeriod)
        : _ppw(ppw)
        , _period(period)
        , _signalSampleStep(signalSampleStep)
        , _sampleStep(_period/samplesPerPeriod)
        , _values(std::size_t(samplesPerPeriod*ppw*2 + 0.5))
        , _valuesFiltered(samplesPerPeriod)
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
        real integrate(real xStart, real xStop, const real *y)
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

            return
                    (
                        +(80*c5)*xStop6
                        +(-240*c5+96*c4)*xStop5
                        +(300*c5-240*c4+120*c3)*xStop4
                        +(-200*c5+240*c4-240*c3+160*c2)*xStop3
                        +(75*c5-120*c4+180*c3-240*c2+240*c1)*xStop2
                        +(-15*c5+30*c4-60*c3+120*c2-240*c1+480*c0)*xStop

                        +(-80*c5)*xStart6
                        +(240*c5-96*c4)*xStart5
                        +(-300*c5+240*c4-120*c3)*xStart4
                        +(200*c5-240*c4+240*c3-160*c2)*xStart3
                        +(-75*c5+120*c4-180*c3+240*c2-240*c1)*xStart2
                        +(15*c5-30*c4+60*c3-120*c2+240*c1-480*c0)*xStart
                    )/480;
        }

        //[0, 1]
        real integrate(const real *y)
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


            Summator<real> res(0);

            res += even1 * (real(0.42685983409379380L) + real(-0.217009177221292431L/12) + real( 0.04166946673533273L/80));
            res += even2 * (real(0.07238123511170030L) + real( 0.20051376594086157L /12) + real(-0.06250420114356986L/80));
            res += even3 * (real(0.00075893079450573L) + real( 0.01649541128040211L /12) + real( 0.02083473440841799L/80));

            return res;
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
            return sum/amount;
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

            sum += integrate(first01x, last01x, signal+signalStartIdx-2);
            amount = last01x - first01x;
        }
        else
        {
            //from multiple signal samples

            if(signalStartIdx < signalStopIdx-1)
            {
                for(std::size_t signalIndex(signalStartIdx+1); signalIndex<signalStopIdx; ++signalIndex)
                {
                    sum += integrate(signal+signalIndex-2);
                }
            }
            amount = signalStopIdx - signalStartIdx - 1;

            real first01x = (startTime - (signalStartIdx)*_signalSampleStep) / _signalSampleStep;
            sum += integrate(first01x, 1, signal+signalStartIdx-2);
            amount += 1 - first01x;

            real last01x = (stopTime - (signalStopIdx)*_signalSampleStep) / _signalSampleStep;
            sum += integrate(0, last01x, signal+signalStopIdx-2);
            amount += last01x;
        }

        if(amount>0)
        {
            return sum/amount;
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
    void SignalConvolverLevel::filtrate(const std::vector<std::vector<real>> &halfFirs)
    {
        for(std::size_t index(0); index<_valuesFiltered.size(); ++index)
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

            _valuesFiltered[index] = sum;
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

    namespace
    {
        complex approxCosPlusAxPlusB(TVReal &ys)
        {
            double levmarInfo[LM_INFO_SZ];
            static std::vector<double> work;
            if(work.size() < LM_DIF_WORKSZ(4, ys.size()*2))
            {
                work.resize(LM_DIF_WORKSZ(4, ys.size()*2));
            }

            static double levmarOpts[LM_OPTS_SZ] =
            {
                1e-40,  //LM_INIT_MU,        //mu
                1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
                1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
                1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
            };

            double p[4]={0,0,0,0};

            static std::vector<double> dys;
            dys.assign(ys.begin(), ys.end());

            int levmarResult = dlevmar_der(
                        [](double *p, double *hx, int m, int n, void *_levmarParams)->void{
                            for(int i(0); i<n; i++)
                            {
                                real x = g_2pi*i/n;
                                hx[i] = double(p[0]*cos(x) - p[1]*sin(x) + p[2]*x + p[3]);

                                int k = 1;
                            }
                        },
                        [](double *p, double *jx, int m, int n, void *_levmarParams){
                            for(int i(0); i<n; i++)
                            {
                                real x = g_2pi*i/n;

                                jx[i*4+0] = double(cos(x));
                                jx[i*4+1] = double(-sin(x));
                                jx[i*4+2] = double(x);
                                jx[i*4+3] = double(1);
                            }
                        },
                        &p[0],
                        &dys[0],
                        4,
                        ys.size(),
                        5,
                        levmarOpts,
                        levmarInfo,
                        &work[0],
                        NULL,
                        NULL);

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
//            exit(1);

            return complex(p[0],p[1]);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    complex SignalConvolverLevel::convolve()
    {
//        Summator<complex> res;

//        for(std::size_t index(0); index<_valuesFiltered.size()-1; ++index)
//        {
//            real x0 = index * _sampleStep;
//            real x1 = x0 + _sampleStep;

//            real y0 = _valuesFiltered[index];
//            real y1 = _valuesFiltered[index+1];

//            res += evalSegment(_period, x0, y0, x1, y1);
//        }

//        //return res / (_period * _ppw);

//        complex r1 = res / (_period * _ppw);

        complex r2 = approxCosPlusAxPlusB(_valuesFiltered);

        return r2;
    }

}
