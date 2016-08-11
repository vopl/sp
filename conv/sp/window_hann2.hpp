#pragma once

#include "math.hpp"
#include <cmath>

namespace sp
{
    struct Window_hann2
    {

        template <long pow>
        static complex convolve(real xCenter, real x0, real y0, real x1, real y1, real period);

        template <long pow>
        static void kernel(real period, real &rr, real &ri, real &ir, real &ii);

    };

    template <>
    inline complex Window_hann2::convolve<10>(real xCenter, real x0, real y0, real x1, real y1, real period)
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
            const real _2 =  ( 5*t ) ;
            const real _17 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _9 =  ( 9*pi*xCenter-9*pi*x1 ) ;
            const real _0 =  ( 261360*pi*t*x1-261360*pi*t*x0 ) ;
            const real _12 =  ( 392040*pi*t*x1-392040*pi*t*x0 ) ;
            const real _19 =  ( 392040*pi*t*x0-392040*pi*t*x1 ) ;
            const real _20 =  ( 1881792*pi*t*x0-1881792*pi*t*x1 ) ;
            const real _5 =  ( 11*pi*xCenter-11*pi*x1 ) ;
            const real _10 =  ( 9*pi*xCenter-9*pi*x0-5*pi*t ) ;
            const real _1 =  ( 12*pi*xCenter-12*pi*x1 ) ;
            const real _4 =  ( 11*pi*xCenter-11*pi*x1+5*pi*t ) ;
            const real _13 =  ( 8*pi*xCenter-8*pi*x1 ) ;
            const real _16 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _21 =  ( 10036224*pi_p_2*x1-10036224*pi_p_2*x0 ) ;
            const real _18 =  ( 261360*pi*t*x0-261360*pi*t*x1 ) ;
            const real _3 =  ( 12*pi*xCenter-12*pi*x0 ) ;
            const real _7 =  ( 11*pi*xCenter-11*pi*x0 ) ;
            const real _14 =  ( 8*pi*xCenter-8*pi*x0 ) ;
            const real _11 =  ( 9*pi*xCenter-9*pi*x0 ) ;
            const real _8 =  ( 9*pi*xCenter-9*pi*x1-5*pi*t ) ;
            const real _15 =  ( 1881792*pi*t*x1-1881792*pi*t*x0 ) ;
            const real _6 =  ( 11*pi*xCenter-11*pi*x0+5*pi*t ) ;
            const real _34 = sin ( _10/_2 ) ;
            const real _30 = cos ( _7/_2 ) ;
            const real _29 = sin ( _7/_2 ) ;
            const real _28 = sin ( _6/_2 ) ;
            const real _44 = sin ( _14/_2 ) ;
            const real _46 = cos ( _4/_2 ) ;
            const real _38 = cos ( _13/_2 ) ;
            const real _27 = cos ( _5/_2 ) ;
            const real _35 = sin ( _11/_2 ) ;
            const real _49 = cos ( _10/_2 ) ;
            const real _48 = cos ( _8/_2 ) ;
            const real _31 = sin ( _8/_2 ) ;
            const real _43 = sin ( _3/_2 ) ;
            const real _23 = cos ( _1/_2 ) ;
            const real _32 = sin ( _9/_2 ) ;
            const real _33 = cos ( _9/_2 ) ;
            const real _47 = cos ( _6/_2 ) ;
            const real _37 = sin ( _13/_2 ) ;
            const real _42 = cos ( _17/t ) ;
            const real _22 = sin ( _1/_2 ) ;
            const real _26 = sin ( _5/_2 ) ;
            const real _39 = cos ( _14/_2 ) ;
            const real _41 = cos ( _16/t ) ;
            const real _25 = sin ( _4/_2 ) ;
            const real _45 = sin ( _17/t ) ;
            const real _36 = cos ( _11/_2 ) ;
            const real _24 = cos ( _3/_2 ) ;
            const real _40 = sin ( _16/t ) ;
            const real _53 =  ( -108900*t_p_2*_22+108900*t_p_2*_43+_18*_24-1140480*pi*t*x1*_46-518400*t_p_2*_26-1140480*pi*t*x1*_27+1140480*pi*t*x1*_47+518400*t_p_2*_29+1140480*pi*t*x0*_30-1393920*pi*t*x1*_48-774400*t_p_2*_32-1393920*pi*t*x1*_33+1393920*pi*t*x1*_49+774400*t_p_2*_35+1393920*pi*t*x0*_36-245025*t_p_2*_37+245025*t_p_2*_44+_19*_39-940896*t_p_2*_40+940896*t_p_2*_45+_20*_42 ) ;
            const real _52 =  ( 108900*t_p_2*_22+_0*_23-108900*t_p_2*_43+1140480*pi*t*x0*_46+518400*t_p_2*_26+1140480*pi*t*x1*_27-1140480*pi*t*x0*_47-518400*t_p_2*_29-1140480*pi*t*x0*_30+1393920*pi*t*x0*_48+774400*t_p_2*_32+1393920*pi*t*x1*_33-1393920*pi*t*x0*_49-774400*t_p_2*_35-1393920*pi*t*x0*_36+245025*t_p_2*_37+_12*_38-245025*t_p_2*_44+940896*t_p_2*_40+_15*_41-940896*t_p_2*_45 ) ;
            const real _51 =  ( 108900*t_p_2*_23+_18*_43-108900*t_p_2*_24-1140480*pi*t*x1*_25-1140480*pi*t*x1*_26+518400*t_p_2*_27+1140480*pi*t*x1*_28+1140480*pi*t*x0*_29-518400*t_p_2*_30-1393920*pi*t*x1*_31-1393920*pi*t*x1*_32+774400*t_p_2*_33+1393920*pi*t*x1*_34+1393920*pi*t*x0*_35-774400*t_p_2*_36+245025*t_p_2*_38+_19*_44-245025*t_p_2*_39+940896*t_p_2*_41+_20*_45-940896*t_p_2*_42 ) ;
            const real _50 =  ( _0*_22-108900*t_p_2*_23+108900*t_p_2*_24+1140480*pi*t*x0*_25+1140480*pi*t*x1*_26-518400*t_p_2*_27-1140480*pi*t*x0*_28-1140480*pi*t*x0*_29+518400*t_p_2*_30+1393920*pi*t*x0*_31+1393920*pi*t*x1*_32-774400*t_p_2*_33-1393920*pi*t*x0*_34-1393920*pi*t*x0*_35+774400*t_p_2*_36+_12*_37-245025*t_p_2*_38+245025*t_p_2*_39+_15*_40-940896*t_p_2*_41+940896*t_p_2*_42 ) ;
            const real _54 =  ( _50*y1+_51*y0 ) ;
            const real _55 =  ( _52*y1+_53*y0 ) ;
            //expr

            re = -_54/_21;im = _55/_21;


            assert(std::isfinite(re) && std::isfinite(im));
        }

        return complex(re, im)/t;
    }

    template <>
    inline void Window_hann2::kernel<10>(real period, real &rr, real &ri, real &ir, real &ii)
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
        const real t_p_10= t*t*t*t*t*t*t*t*t*t;

        const real ts = t < 1 ? t : 1;

        real re, im;

        {
            const real _1 = sin ( 10*pi*t ) ;
            const real _2 =  ( 500000000*pi*t_p_10-2550000000*pi*t_p_8+5101650000*pi*t_p_6-5001770000*pi*t_p_4+2401750080*pi*t_p_2-451630080*pi ) ;
            const real _3 =  ( 37500*t_p_4+73875*t_p_2+7128 ) ;
            const real _0 =  ( 7500*t_p_5+74625*t_p_3+36378*t ) ;
            const real _5 =  ( _3*_1 ) ;
            const real _4 =  ( _0*_1 ) ;
            //expr


            if(t>real(1.0)-std::numeric_limits<real>::epsilon()*10 &&
               t<real(1.0)+std::numeric_limits<real>::epsilon()*10)
            {
                rr = real(3)/16;
                ri = 0;
                ir = 0;
                ii = -real(3)/16;
            }
            else
            {
                rr = _4/_2;ii = -_5/_2;
                ri = ir = 0;
            }
        }
    }

    template <>
    inline complex Window_hann2::convolve<11>(real xCenter, real x0, real y0, real x1, real y1, real period)
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
            const real _14 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _12 =  ( 18*pi*xCenter-18*pi*x0 ) ;
            const real _13 =  ( 16426800*pi*t*x1-16426800*pi*t*x0 ) ;
            const real _16 =  ( 2316600*pi*t*x0-2316600*pi*t*x1 ) ;
            const real _17 =  ( 10038600*pi*t*x1-10038600*pi*t*x0 ) ;
            const real _19 =  ( 3346200*pi*t*x0-3346200*pi*t*x1 ) ;
            const real _10 =  ( 3346200*pi*t*x1-3346200*pi*t*x0 ) ;
            const real _7 =  ( 12046320*pi*t*x0-12046320*pi*t*x1 ) ;
            const real _3 =  ( 26*pi*xCenter-26*pi*x0 ) ;
            const real _6 =  ( 24*pi*xCenter-24*pi*x0 ) ;
            const real _11 =  ( 18*pi*xCenter-18*pi*x1 ) ;
            const real _5 =  ( 24*pi*xCenter-24*pi*x1 ) ;
            const real _1 =  ( 26*pi*xCenter-26*pi*x1 ) ;
            const real _8 =  ( 20*pi*xCenter-20*pi*x1 ) ;
            const real _20 =  ( 16426800*pi*t*x0-16426800*pi*t*x1 ) ;
            const real _21 =  ( 87609600*pi_p_2*x1-87609600*pi_p_2*x0 ) ;
            const real _4 =  ( 10038600*pi*t*x0-10038600*pi*t*x1 ) ;
            const real _0 =  ( 2316600*pi*t*x1-2316600*pi*t*x0 ) ;
            const real _2 =  ( 11*t ) ;
            const real _18 =  ( 12046320*pi*t*x1-12046320*pi*t*x0 ) ;
            const real _15 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _9 =  ( 20*pi*xCenter-20*pi*x0 ) ;
            const real _39 = sin ( _9/_2 ) ;
            const real _38 = sin ( _6/_2 ) ;
            const real _27 = cos ( _6/_2 ) ;
            const real _41 = sin ( _15/t ) ;
            const real _33 = cos ( _12/_2 ) ;
            const real _25 = sin ( _5/_2 ) ;
            const real _23 = cos ( _1/_2 ) ;
            const real _22 = sin ( _1/_2 ) ;
            const real _32 = cos ( _11/_2 ) ;
            const real _28 = sin ( _8/_2 ) ;
            const real _31 = sin ( _11/_2 ) ;
            const real _24 = cos ( _3/_2 ) ;
            const real _34 = sin ( _14/t ) ;
            const real _30 = cos ( _9/_2 ) ;
            const real _29 = cos ( _8/_2 ) ;
            const real _40 = sin ( _12/_2 ) ;
            const real _35 = cos ( _14/t ) ;
            const real _36 = cos ( _15/t ) ;
            const real _26 = cos ( _5/_2 ) ;
            const real _37 = sin ( _3/_2 ) ;
            const real _42 =  ( _0*_22-980100*t_p_2*_23+980100*t_p_2*_24+_4*_25+4601025*t_p_2*_26-4601025*t_p_2*_27+_7*_28+6625476*t_p_2*_29-6625476*t_p_2*_30+_10*_31-2044900*t_p_2*_32+2044900*t_p_2*_33+_13*_34-8213400*t_p_2*_35+8213400*t_p_2*_36 ) ;
            const real _44 =  ( 980100*t_p_2*_22+_0*_23-980100*t_p_2*_37-4601025*t_p_2*_25+_4*_26+4601025*t_p_2*_38-6625476*t_p_2*_28+_7*_29+6625476*t_p_2*_39+2044900*t_p_2*_31+_10*_32-2044900*t_p_2*_40+8213400*t_p_2*_34+_13*_35-8213400*t_p_2*_41 ) ;
            const real _45 =  ( -980100*t_p_2*_22+980100*t_p_2*_37+_16*_24+4601025*t_p_2*_25-4601025*t_p_2*_38+_17*_27+6625476*t_p_2*_28-6625476*t_p_2*_39+_18*_30-2044900*t_p_2*_31+2044900*t_p_2*_40+_19*_33-8213400*t_p_2*_34+8213400*t_p_2*_41+_20*_36 ) ;
            const real _43 =  ( 980100*t_p_2*_23+_16*_37-980100*t_p_2*_24-4601025*t_p_2*_26+_17*_38+4601025*t_p_2*_27-6625476*t_p_2*_29+_18*_39+6625476*t_p_2*_30+2044900*t_p_2*_32+_19*_40-2044900*t_p_2*_33+8213400*t_p_2*_35+_20*_41-8213400*t_p_2*_36 ) ;
            const real _47 =  ( _44*y1+_45*y0 ) ;
            const real _46 =  ( _42*y1+_43*y0 ) ;
            //expr

            re = -_46/_21;im = _47/_21;

        }

        return complex(re, im)/t;
    }

    template <>
    inline void Window_hann2::kernel<11>(real period, real &rr, real &ri, real &ir, real &ii)
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
        const real t_p_10= t*t*t*t*t*t*t*t*t*t;

        const real ts = t < 1 ? t : 1;

        real re, im;

        {
            const real _3 = cos ( 22*pi*t ) ;
            const real _1 = sin ( 22*pi*t ) ;
            const real _2 =  ( 9431790764*pi*t_p_10-47938440660*pi*t_p_8+95898140052*pi*t_p_6-94340667740*pi*t_p_4+45622527984*pi*t_p_2-8673350400*pi ) ;
            const real _4 =  ( 219615*t_p_4+433785*t_p_2+42120 ) ;
            const real _0 =  ( 43923*t_p_5+437415*t_p_3+214182*t ) ;
            const real _7 =  ( _4*_3-219615*t_p_4-433785*t_p_2-42120 ) ;
            const real _6 =  ( _0*_3-43923*t_p_5-437415*t_p_3-214182*t ) ;
            const real _5 =  ( _0*_1 ) ;
            //expr

            if(t>real(1.0)-std::numeric_limits<real>::epsilon()*10 &&
               t<real(1.0)+std::numeric_limits<real>::epsilon()*10)
            {
                rr = real(3)/16;
                ri = 0;
                ir = 0;
                ii = -real(3)/16;
            }
            else
            {
                rr = _5/_2;ri = _6/_2;ir = _7/_2;ii = _5/_2;
            }
        }
    }


}
