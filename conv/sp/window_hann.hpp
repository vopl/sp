#pragma once

#include "math.hpp"

namespace sp
{
    struct Window_hann
    {

        template <long pow>
        static complex convolve(real xCenter, real x0, real y0, real x1, real y1, real period);

        template <long pow>
        static void kernel(real period, real &rr, real &ri, real &ir, real &ii);
    };

    template <>
    inline complex Window_hann::convolve<10>(real xCenter, real x0, real y0, real x1, real y1, real period)
    {
        if(fabs(x0-x1) < std::numeric_limits<real>::epsilon()*10)
        {
            return 0;
        }
        static const real pi = g_pi;
        const real t = period;

        const real t_p_2 = t*t;
        const real pi_p_2 = pi*pi;

        real re, im;

        {
            const real _10 =  ( 8910*pi*t*x0-8910*pi*t*x1 ) ;
            const real _13 =  ( 78408*pi_p_2*x1-78408*pi_p_2*x0 ) ;
            const real _3 =  ( 11*pi*xCenter-11*pi*x0 ) ;
            const real _5 =  ( 9*pi*xCenter-9*pi*x1 ) ;
            const real _1 =  ( 11*pi*xCenter-11*pi*x1 ) ;
            const real _4 =  ( 10890*pi*t*x1-10890*pi*t*x0 ) ;
            const real _11 =  ( 10890*pi*t*x0-10890*pi*t*x1 ) ;
            const real _0 =  ( 8910*pi*t*x1-8910*pi*t*x0 ) ;
            const real _12 =  ( 19602*pi*t*x1-19602*pi*t*x0 ) ;
            const real _6 =  ( 9*pi*xCenter-9*pi*x0 ) ;
            const real _9 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _8 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _7 =  ( 19602*pi*t*x0-19602*pi*t*x1 ) ;
            const real _2 =  ( 5*t ) ;
            const real _16 = cos ( _3/_2 ) ;
            const real _14 = sin ( _1/_2 ) ;
            const real _21 = cos ( _8/t ) ;
            const real _18 = cos ( _5/_2 ) ;
            const real _19 = cos ( _6/_2 ) ;
            const real _15 = cos ( _1/_2 ) ;
            const real _17 = sin ( _5/_2 ) ;
            const real _25 = sin ( _9/t ) ;
            const real _24 = sin ( _6/_2 ) ;
            const real _22 = cos ( _9/t ) ;
            const real _23 = sin ( _3/_2 ) ;
            const real _20 = sin ( _8/t ) ;
            const real _28 =  ( 4050*t_p_2*_14+_0*_15-4050*t_p_2*_23+6050*t_p_2*_17+_4*_18-6050*t_p_2*_24-9801*t_p_2*_20+_7*_21+9801*t_p_2*_25 ) ;
            const real _29 =  ( -4050*t_p_2*_14+4050*t_p_2*_23+_10*_16-6050*t_p_2*_17+6050*t_p_2*_24+_11*_19+9801*t_p_2*_20-9801*t_p_2*_25+_12*_22 ) ;
            const real _26 =  ( _0*_14-4050*t_p_2*_15+4050*t_p_2*_16+_4*_17-6050*t_p_2*_18+6050*t_p_2*_19+_7*_20+9801*t_p_2*_21-9801*t_p_2*_22 ) ;
            const real _27 =  ( 4050*t_p_2*_15+_10*_23-4050*t_p_2*_16+6050*t_p_2*_18+_11*_24-6050*t_p_2*_19-9801*t_p_2*_21+_12*_25+9801*t_p_2*_22 ) ;
            const real _30 =  ( _26*y1+_27*y0 ) ;
            const real _31 =  ( _28*y1+_29*y0 ) ;
            //expr

            re = _30/_13;im = -_31/_13;

        }

        return complex(re, im)/t;
    }

    template <>
    inline void Window_hann::kernel<10>(real period, real &rr, real &ri, real &ir, real &ii)
    {
        static const real pi = g_pi;
        const real t = period;
        const real t_p_2 = t*t;
        const real t_p_3 = t*t*t;
        const real t_p_4 = t*t*t*t;
        const real t_p_5 = t*t*t*t*t;
        const real t_p_6 = t*t*t*t*t*t;
        const real t_p_7 = t*t*t*t*t*t*t;
        const real t_p_8 = t*t*t*t*t*t*t*t;
        const real t_p_9 = t*t*t*t*t*t*t*t*t;
        const real t_p_10 = t*t*t*t*t*t*t*t*t*t;
        const real t_p_11 = t*t*t*t*t*t*t*t*t*t*t;
        const real t_p_12 = t*t*t*t*t*t*t*t*t*t*t*t;
        const real t_p_13 = t*t*t*t*t*t*t*t*t*t*t*t*t;
        const real t_p_14 = t*t*t*t*t*t*t*t*t*t*t*t*t*t;

        const real ts = period;// < 1 ? period : 1;

        {
            const real _1 = sin ( 20*pi*t ) ;
            const real _2 =  ( 400000*pi*t_p_6-1208000*pi*t_p_4+1200040*pi*t_p_2-392040*pi ) ;
            const real _3 = cos ( 20*pi*t ) ;
            const real _0 =  ( 100*t_p_3+299*t ) ;
            const real _4 =  ( 300*t_p_2+99 ) ;
            const real _7 =  ( _4*_3-300*t_p_2-99 ) ;
            const real _6 =  ( _0*_3-100*t_p_3-299*t ) ;
            const real _5 =  ( _0*_1 ) ;
            const real _8 =  ( _4*_1 ) ;
            //expr

            rr = -_5/_2;ri = -_6/_2;ir = -_7/_2;ii = _8/_2;


            if(t>real(1.0)-std::numeric_limits<real>::epsilon()*10 &&
               t<real(1.0)+std::numeric_limits<real>::epsilon()*10)
            {
                rr = real(1)/4;
                ri = 0;
                ir = 0;
                ii = -real(1)/4;
            }
            else
            {
                rr = -_5/_2;ri = -_6/_2;ir = -_7/_2;ii = _8/_2;
            }
        }
    }

    template <>
    inline complex Window_hann::convolve<11>(real xCenter, real x0, real y0, real x1, real y1, real period)
    {
        static const real pi = g_pi;
        const real t = period;

        const real t_p_2 = t*t;
        const real pi_p_2 = pi*pi;

        real re, im;

        {
            const real _8 =  ( 20*pi*xCenter-20*pi*x0 ) ;
            const real _3 =  ( 24*pi*xCenter-24*pi*x0-11*pi*t ) ;
            const real _0 =  ( 24*pi*xCenter-24*pi*x1-11*pi*t ) ;
            const real _4 =  ( 24*pi*xCenter-24*pi*x0 ) ;
            const real _9 =  ( 14400*pi*t*x1-14400*pi*t*x0 ) ;
            const real _7 =  ( 20*pi*xCenter-20*pi*x0+11*pi*t ) ;
            const real _6 =  ( 20*pi*xCenter-20*pi*x1 ) ;
            const real _12 =  ( 14400*pi*t*x0-14400*pi*t*x1 ) ;
            const real _1 =  ( 11*t ) ;
            const real _11 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _10 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _2 =  ( 24*pi*xCenter-24*pi*x1 ) ;
            const real _13 =  ( 57600*pi_p_2*x1-57600*pi_p_2*x0 ) ;
            const real _5 =  ( 20*pi*xCenter-20*pi*x1+11*pi*t ) ;
            const real _25 = cos ( _8/_1 ) ;
            const real _17 = sin ( _3/_1 ) ;
            const real _27 = cos ( _10/t ) ;
            const real _26 = sin ( _10/t ) ;
            const real _19 = cos ( _4/_1 ) ;
            const real _29 = sin ( _11/t ) ;
            const real _16 = cos ( _2/_1 ) ;
            const real _30 = cos ( _0/_1 ) ;
            const real _31 = cos ( _3/_1 ) ;
            const real _20 = sin ( _5/_1 ) ;
            const real _15 = sin ( _2/_1 ) ;
            const real _21 = sin ( _6/_1 ) ;
            const real _18 = sin ( _4/_1 ) ;
            const real _33 = cos ( _7/_1 ) ;
            const real _32 = cos ( _5/_1 ) ;
            const real _22 = cos ( _6/_1 ) ;
            const real _14 = sin ( _0/_1 ) ;
            const real _28 = cos ( _11/t ) ;
            const real _23 = sin ( _7/_1 ) ;
            const real _24 = sin ( _8/_1 ) ;
            const real _37 =  ( -6600*pi*t*x1*_30-3025*t_p_2*_15-6600*pi*t*x1*_16+6600*pi*t*x1*_31+3025*t_p_2*_18+6600*pi*t*x0*_19-7920*pi*t*x1*_32-4356*t_p_2*_21-7920*pi*t*x1*_22+7920*pi*t*x1*_33+4356*t_p_2*_24+7920*pi*t*x0*_25-7200*t_p_2*_26+7200*t_p_2*_29+_12*_28 ) ;
            const real _36 =  ( 6600*pi*t*x0*_30+3025*t_p_2*_15+6600*pi*t*x1*_16-6600*pi*t*x0*_31-3025*t_p_2*_18-6600*pi*t*x0*_19+7920*pi*t*x0*_32+4356*t_p_2*_21+7920*pi*t*x1*_22-7920*pi*t*x0*_33-4356*t_p_2*_24-7920*pi*t*x0*_25+7200*t_p_2*_26+_9*_27-7200*t_p_2*_29 ) ;
            const real _34 =  ( 6600*pi*t*x0*_14+6600*pi*t*x1*_15-3025*t_p_2*_16-6600*pi*t*x0*_17-6600*pi*t*x0*_18+3025*t_p_2*_19+7920*pi*t*x0*_20+7920*pi*t*x1*_21-4356*t_p_2*_22-7920*pi*t*x0*_23-7920*pi*t*x0*_24+4356*t_p_2*_25+_9*_26-7200*t_p_2*_27+7200*t_p_2*_28 ) ;
            const real _35 =  ( -6600*pi*t*x1*_14-6600*pi*t*x1*_15+3025*t_p_2*_16+6600*pi*t*x1*_17+6600*pi*t*x0*_18-3025*t_p_2*_19-7920*pi*t*x1*_20-7920*pi*t*x1*_21+4356*t_p_2*_22+7920*pi*t*x1*_23+7920*pi*t*x0*_24-4356*t_p_2*_25+7200*t_p_2*_27+_12*_29-7200*t_p_2*_28 ) ;
            const real _38 =  ( _34*y1+_35*y0 ) ;
            const real _39 =  ( _36*y1+_37*y0 ) ;
            //expr

            re = -_38/_13;im = _39/_13;
        }

        return complex(re, im);
    }

}
