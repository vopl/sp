#pragma once

#include "math.hpp"

namespace sp
{
    struct Window_rect
    {

        template <long pow>
        static complex convolve(real xCenter, real x0, real y0, real x1, real y1, real period);

        template <long pow>
        static void kernel(real period, real &rr, real &ri, real &ir, real &ii);

    };

    template <>
    inline void Window_rect::kernel<1>(real period, real &rr, real &ri, real &ir, real &ii)
    {
        static const real pi = g_pi;
        const real t = period;
        const real t_p_2 = t*t;
        const real ts = period;// < 1 ? period : 1;

        {
            const real _0 = sin ( 2*pi*t ) ;
            const real _2 = cos ( 2*pi*t ) ;
            const real _1 =  ( 2*pi*t_p_2-2*pi ) ;
            const real _3 =  ( t*_0 ) ;
            const real _5 =  ( _2-1 ) ;
            const real _4 =  ( t*_2-t ) ;
            //expr

            if(t>real(1.0)-std::numeric_limits<real>::epsilon()*10 &&
               t<real(1.0)+std::numeric_limits<real>::epsilon()*10)
            {
                rr = real(1)/2;
                ri = 0;
                ir = 0;
                ii = -real(1)/2;
            }
            else
            {
                rr = _3/_1;ri = _4/_1;ir = _5/_1;ii = -_0/_1;
            }
        }
    }

    template <>
    inline complex Window_rect::convolve<10>(real xCenter, real x0, real y0, real x1, real y1, real period)
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
            const real _4 =  ( 4*pi_p_2*x1-4*pi_p_2*x0 ) ;
            const real _0 =  ( 2*pi*t*x1-2*pi*t*x0 ) ;
            const real _1 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _3 =  ( 2*pi*t*x0-2*pi*t*x1 ) ;
            const real _2 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _6 = cos ( _1/t ) ;
            const real _7 = cos ( _2/t ) ;
            const real _5 = sin ( _1/t ) ;
            const real _8 = sin ( _2/t ) ;
            const real _9 =  ( _0*_5-t_p_2*_6+t_p_2*_7 ) ;
            const real _11 =  ( t_p_2*_5+_0*_6-t_p_2*_8 ) ;
            const real _10 =  ( t_p_2*_6+_3*_8-t_p_2*_7 ) ;
            const real _12 =  ( -t_p_2*_5+t_p_2*_8+_3*_7 ) ;
            const real _14 =  ( _11*y1+_12*y0 ) ;
            const real _13 =  ( _9*y1+_10*y0 ) ;
            //expr

            re = -_13/_4;im = _14/_4;
        }

        return complex(re, im)/t;
    }

    template <>
    inline void Window_rect::kernel<10>(real period, real &rr, real &ri, real &ir, real &ii)
    {
        static const real pi = g_pi;
        const real t = period;
        const real t_p_2 = t*t;
        const real ts = period;// < 1 ? period : 1;

        {
            const real _2 =  ( t+1 ) ;
            const real _0 =  ( t-1 ) ;
            const real _5 =  ( -t-1 ) ;
            const real _4 =  ( 40*pi*t_p_2-40*pi ) ;
            const real _3 =  ( 20*pi*t-20*pi ) ;
            const real _1 =  ( 20*pi*t+20*pi ) ;
            const real _6 =  ( _1*ts ) ;
            const real _7 =  ( _3*ts ) ;
            const real _8 = sin ( _6/t ) ;
            const real _11 = cos ( _7/t ) ;
            const real _10 = cos ( _6/t ) ;
            const real _9 = sin ( _7/t ) ;
            const real _15 =  ( _0*_8+_5*_9 ) ;
            const real _14 =  ( _0*_10+_5*_11+2 ) ;
            const real _13 =  ( _0*_10+_2*_11-2*t ) ;
            const real _12 =  ( _0*_8+_2*_9 ) ;
            //expr

            if(t>real(1.0)-std::numeric_limits<real>::epsilon()*10 &&
               t<real(1.0)+std::numeric_limits<real>::epsilon()*10)
            {
                rr = real(1)/2;
                ri = 0;
                ir = 0;
                ii = -real(1)/2;
            }
            else
            {
                rr = _12/_4;
                ri = _13/_4;
                ir = -_14/_4;
                ii = _15/_4;
            }
        }
    }

    template <>
    inline complex Window_rect::convolve<11>(real xCenter, real x0, real y0, real x1, real y1, real period)
    {
        static const real pi = g_pi;
        const real t = period;

        const real t_p_2 = t*t;
        const real pi_p_2 = pi*pi;

        real re, im;

        {
            const real _0 =  ( 2*pi*t*x1-2*pi*t*x0 ) ;
            const real _3 =  ( 2*pi*t*x0-2*pi*t*x1 ) ;
            const real _1 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _2 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _4 =  ( 4*pi_p_2*x1-4*pi_p_2*x0 ) ;
            const real _6 = cos ( _1/t ) ;
            const real _8 = sin ( _2/t ) ;
            const real _7 = cos ( _2/t ) ;
            const real _5 = sin ( _1/t ) ;
            const real _10 =  ( t_p_2*_6+_3*_8-t_p_2*_7 ) ;
            const real _11 =  ( t_p_2*_5+_0*_6-t_p_2*_8 ) ;
            const real _12 =  ( -t_p_2*_5+t_p_2*_8+_3*_7 ) ;
            const real _9 =  ( _0*_5-t_p_2*_6+t_p_2*_7 ) ;
            const real _13 =  ( _9*y1+_10*y0 ) ;
            const real _14 =  ( _11*y1+_12*y0 ) ;
            //expr

            re = -_13/_4;im = _14/_4;
        }

        return complex(re, im);
    }

}
