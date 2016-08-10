#pragma once

#include "math.hpp"

namespace sp
{
    struct Window_hann2
    {

        template <long pow>
        static complex convolve(real xCenter, real x0, real y0, real x1, real y1, real period);

        template <long pow>
        static complex kernel(real period);

    };

    template <>
    inline complex Window_hann2::convolve<10>(real xCenter, real x0, real y0, real x1, real y1, real period)
    {
        static const real pi = g_pi;
        const real t = period;

        const real t_p_2 = t*t;
        const real pi_p_2 = pi*pi;

        real re, im;

        {
            const real _21 =  ( 10036224*pi_p_2*x1-10036224*pi_p_2*x0 ) ;
            const real _5 =  ( 11*pi*xCenter-11*pi*x1 ) ;
            const real _8 =  ( 9*pi*xCenter-9*pi*x1+5*pi*t ) ;
            const real _2 =  ( 5*t ) ;
            const real _7 =  ( 11*pi*xCenter-11*pi*x0 ) ;
            const real _14 =  ( 8*pi*xCenter-8*pi*x0 ) ;
            const real _1 =  ( 12*pi*xCenter-12*pi*x1 ) ;
            const real _17 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _9 =  ( 9*pi*xCenter-9*pi*x1 ) ;
            const real _4 =  ( 11*pi*xCenter-11*pi*x1-5*pi*t ) ;
            const real _19 =  ( 392040*pi*t*x0-392040*pi*t*x1 ) ;
            const real _10 =  ( 9*pi*xCenter-9*pi*x0+5*pi*t ) ;
            const real _12 =  ( 392040*pi*t*x1-392040*pi*t*x0 ) ;
            const real _13 =  ( 8*pi*xCenter-8*pi*x1 ) ;
            const real _6 =  ( 11*pi*xCenter-11*pi*x0-5*pi*t ) ;
            const real _0 =  ( 261360*pi*t*x1-261360*pi*t*x0 ) ;
            const real _3 =  ( 12*pi*xCenter-12*pi*x0 ) ;
            const real _18 =  ( 261360*pi*t*x0-261360*pi*t*x1 ) ;
            const real _15 =  ( 1881792*pi*t*x1-1881792*pi*t*x0 ) ;
            const real _11 =  ( 9*pi*xCenter-9*pi*x0 ) ;
            const real _20 =  ( 1881792*pi*t*x0-1881792*pi*t*x1 ) ;
            const real _16 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _40 = sin ( _16/t ) ;
            const real _23 = cos ( _1/_2 ) ;
            const real _35 = sin ( _11/_2 ) ;
            const real _44 = sin ( _14/_2 ) ;
            const real _24 = cos ( _3/_2 ) ;
            const real _22 = sin ( _1/_2 ) ;
            const real _43 = sin ( _3/_2 ) ;
            const real _29 = sin ( _7/_2 ) ;
            const real _46 = cos ( _4/_2 ) ;
            const real _49 = cos ( _10/_2 ) ;
            const real _39 = cos ( _14/_2 ) ;
            const real _36 = cos ( _11/_2 ) ;
            const real _34 = sin ( _10/_2 ) ;
            const real _47 = cos ( _6/_2 ) ;
            const real _27 = cos ( _5/_2 ) ;
            const real _38 = cos ( _13/_2 ) ;
            const real _41 = cos ( _16/t ) ;
            const real _45 = sin ( _17/t ) ;
            const real _26 = sin ( _5/_2 ) ;
            const real _25 = sin ( _4/_2 ) ;
            const real _31 = sin ( _8/_2 ) ;
            const real _30 = cos ( _7/_2 ) ;
            const real _33 = cos ( _9/_2 ) ;
            const real _48 = cos ( _8/_2 ) ;
            const real _42 = cos ( _17/t ) ;
            const real _37 = sin ( _13/_2 ) ;
            const real _28 = sin ( _6/_2 ) ;
            const real _32 = sin ( _9/_2 ) ;
            const real _51 =  ( 108900*t_p_2*_23+_18*_43-108900*t_p_2*_24-1140480*pi*t*x1*_25-1140480*pi*t*x1*_26+518400*t_p_2*_27+1140480*pi*t*x1*_28+1140480*pi*t*x0*_29-518400*t_p_2*_30-1393920*pi*t*x1*_31-1393920*pi*t*x1*_32+774400*t_p_2*_33+1393920*pi*t*x1*_34+1393920*pi*t*x0*_35-774400*t_p_2*_36+245025*t_p_2*_38+_19*_44-245025*t_p_2*_39+940896*t_p_2*_41+_20*_45-940896*t_p_2*_42 ) ;
            const real _53 =  ( -108900*t_p_2*_22+108900*t_p_2*_43+_18*_24-1140480*pi*t*x1*_46-518400*t_p_2*_26-1140480*pi*t*x1*_27+1140480*pi*t*x1*_47+518400*t_p_2*_29+1140480*pi*t*x0*_30-1393920*pi*t*x1*_48-774400*t_p_2*_32-1393920*pi*t*x1*_33+1393920*pi*t*x1*_49+774400*t_p_2*_35+1393920*pi*t*x0*_36-245025*t_p_2*_37+245025*t_p_2*_44+_19*_39-940896*t_p_2*_40+940896*t_p_2*_45+_20*_42 ) ;
            const real _52 =  ( 108900*t_p_2*_22+_0*_23-108900*t_p_2*_43+1140480*pi*t*x0*_46+518400*t_p_2*_26+1140480*pi*t*x1*_27-1140480*pi*t*x0*_47-518400*t_p_2*_29-1140480*pi*t*x0*_30+1393920*pi*t*x0*_48+774400*t_p_2*_32+1393920*pi*t*x1*_33-1393920*pi*t*x0*_49-774400*t_p_2*_35-1393920*pi*t*x0*_36+245025*t_p_2*_37+_12*_38-245025*t_p_2*_44+940896*t_p_2*_40+_15*_41-940896*t_p_2*_45 ) ;
            const real _50 =  ( _0*_22-108900*t_p_2*_23+108900*t_p_2*_24+1140480*pi*t*x0*_25+1140480*pi*t*x1*_26-518400*t_p_2*_27-1140480*pi*t*x0*_28-1140480*pi*t*x0*_29+518400*t_p_2*_30+1393920*pi*t*x0*_31+1393920*pi*t*x1*_32-774400*t_p_2*_33-1393920*pi*t*x0*_34-1393920*pi*t*x0*_35+774400*t_p_2*_36+_12*_37-245025*t_p_2*_38+245025*t_p_2*_39+_15*_40-940896*t_p_2*_41+940896*t_p_2*_42 ) ;
            const real _54 =  ( _50*y1+_51*y0 ) ;
            const real _55 =  ( _52*y1+_53*y0 ) ;
            //expr

            re = -_54/_21;im = _55/_21;
        }

        return complex(re, im);
    }

    template <>
    inline complex Window_hann2::kernel<10>(real period)
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
            const real _2 =  ( 4704480*t_p_9+3920400*t_p_8-21751230*t_p_7-18126025*t_p_6+36996750*t_p_5+30830625*t_p_4-27450000*t_p_3-22875000*t_p_2+7500000*t+6250000 ) ;
            const real _19 =  ( 8*pi*t-10*pi ) ;
            const real _6 =  ( 20528640*t_p_9+18662400*t_p_8-92204640*t_p_7-83822400*t_p_6+151151000*t_p_5+137410000*t_p_4-106975000*t_p_3-97250000*t_p_2+27500000*t+25000000 ) ;
            const real _24 =  ( -25090560*t_p_9-27878400*t_p_8+102454560*t_p_7+113838400*t_p_6-151389000*t_p_5-168210000*t_p_4+96525000*t_p_3+107250000*t_p_2-22500000*t-25000000 ) ;
            const real _13 =  ( 9*pi*t+10*pi ) ;
            const real _8 =  ( 33872256*t_p_9-33872256*t_p_8-146259000*t_p_7+146259000*t_p_6+228873750*t_p_5-228873750*t_p_4-153750000*t_p_3+153750000*t_p_2+37500000*t-37500000 ) ;
            const real _12 =  ( 25090560*t_p_9-27878400*t_p_8-102454560*t_p_7+113838400*t_p_6+151389000*t_p_5-168210000*t_p_4-96525000*t_p_3+107250000*t_p_2+22500000*t-25000000 ) ;
            const real _15 =  ( 9*pi*t-10*pi ) ;
            const real _0 =  ( 4704480*t_p_9-3920400*t_p_8-21751230*t_p_7+18126025*t_p_6+36996750*t_p_5-30830625*t_p_4-27450000*t_p_3+22875000*t_p_2+7500000*t-6250000 ) ;
            const real _22 =  ( -20528640*t_p_9-18662400*t_p_8+92204640*t_p_7+83822400*t_p_6-151151000*t_p_5-137410000*t_p_4+106975000*t_p_3+97250000*t_p_2-27500000*t-25000000 ) ;
            const real _18 =  ( 7056720*t_p_9+8820900*t_p_8-26501220*t_p_7-33126525*t_p_6+36744500*t_p_5+45930625*t_p_4-22300000*t_p_3-27875000*t_p_2+5000000*t+6250000 ) ;
            const real _25 =  ( -7056720*t_p_9-8820900*t_p_8+26501220*t_p_7+33126525*t_p_6-36744500*t_p_5-45930625*t_p_4+22300000*t_p_3+27875000*t_p_2-5000000*t-6250000 ) ;
            const real _10 =  ( 33872256*t_p_9+33872256*t_p_8-146259000*t_p_7-146259000*t_p_6+228873750*t_p_5+228873750*t_p_4-153750000*t_p_3-153750000*t_p_2+37500000*t+37500000 ) ;
            const real _20 =  ( 1806520320*pi*t_p_10-9607000320*pi*t_p_8+20007080000*pi*t_p_6-20406600000*pi*t_p_4+10200000000*pi*t_p_2-2000000000*pi ) ;
            const real _23 =  ( -33872256*t_p_9-33872256*t_p_8+146259000*t_p_7+146259000*t_p_6-228873750*t_p_5-228873750*t_p_4+153750000*t_p_3+153750000*t_p_2-37500000*t-37500000 ) ;
            const real _7 =  ( 11*pi*t-10*pi ) ;
            const real _9 =  ( 10*pi*t+10*pi ) ;
            const real _21 =  ( -4704480*t_p_9-3920400*t_p_8+21751230*t_p_7+18126025*t_p_6-36996750*t_p_5-30830625*t_p_4+27450000*t_p_3+22875000*t_p_2-7500000*t-6250000 ) ;
            const real _4 =  ( 20528640*t_p_9-18662400*t_p_8-92204640*t_p_7+83822400*t_p_6+151151000*t_p_5-137410000*t_p_4-106975000*t_p_3+97250000*t_p_2+27500000*t-25000000 ) ;
            const real _17 =  ( 8*pi*t+10*pi ) ;
            const real _11 =  ( 10*pi*t-10*pi ) ;
            const real _3 =  ( 12*pi*t-10*pi ) ;
            const real _16 =  ( 7056720*t_p_9-8820900*t_p_8-26501220*t_p_7+33126525*t_p_6+36744500*t_p_5-45930625*t_p_4-22300000*t_p_3+27875000*t_p_2+5000000*t-6250000 ) ;
            const real _1 =  ( 12*pi*t+10*pi ) ;
            const real _5 =  ( 11*pi*t+10*pi ) ;
            const real _14 =  ( 25090560*t_p_9+27878400*t_p_8-102454560*t_p_7-113838400*t_p_6+151389000*t_p_5+168210000*t_p_4-96525000*t_p_3-107250000*t_p_2+22500000*t+25000000 ) ;
            const real _32 =  ( _13*ts ) ;
            const real _31 =  ( _11*ts ) ;
            const real _26 =  ( _1*ts ) ;
            const real _34 =  ( _17*ts ) ;
            const real _35 =  ( _19*ts ) ;
            const real _27 =  ( _3*ts ) ;
            const real _29 =  ( _7*ts ) ;
            const real _30 =  ( _9*ts ) ;
            const real _28 =  ( _5*ts ) ;
            const real _33 =  ( _15*ts ) ;
            const real _45 = sin ( _35/t ) ;
            const real _39 = sin ( _29/t ) ;
            const real _43 = sin ( _33/t ) ;
            const real _42 = sin ( _32/t ) ;
            const real _38 = sin ( _28/t ) ;
            const real _44 = sin ( _34/t ) ;
            const real _37 = sin ( _27/t ) ;
            const real _40 = sin ( _30/t ) ;
            const real _36 = sin ( _26/t ) ;
            const real _41 = sin ( _31/t ) ;
            const real _46 =  ( _0*_36+_2*_37+_4*_38+_6*_39+_8*_40+_10*_41+_12*_42+_14*_43+_16*_44+_18*_45 ) ;
            const real _47 =  ( _0*_36+_21*_37+_4*_38+_22*_39+_8*_40+_23*_41+_12*_42+_24*_43+_16*_44+_25*_45 ) ;
            //expr

            if(t>real(1.0)-std::numeric_limits<real>::epsilon()*10 &&
               t<real(1.0)+std::numeric_limits<real>::epsilon()*10)
            {
                re = real(3)/16; im=-real(3)/16;
            }
            else
            {
                re = _46/_20;im = _47/_20;
            }
        }

        return complex(re, im);
    }

    template <>
    inline complex Window_hann2::convolve<11>(real xCenter, real x0, real y0, real x1, real y1, real period)
    {
        static const real pi = g_pi;
        const real t = period;

        const real t_p_2 = t*t;
        const real pi_p_2 = pi*pi;

        real re, im;

        {
            const real _4 =  ( 24*pi*xCenter-24*pi*x1-11*pi*t ) ;
            const real _8 =  ( 20*pi*xCenter-20*pi*x1+11*pi*t ) ;
            const real _1 =  ( 26*pi*xCenter-26*pi*x1 ) ;
            const real _7 =  ( 24*pi*xCenter-24*pi*x0 ) ;
            const real _20 =  ( 16426800*pi*t*x0-16426800*pi*t*x1 ) ;
            const real _2 =  ( 11*t ) ;
            const real _10 =  ( 20*pi*xCenter-20*pi*x0+11*pi*t ) ;
            const real _9 =  ( 20*pi*xCenter-20*pi*x1 ) ;
            const real _5 =  ( 24*pi*xCenter-24*pi*x1 ) ;
            const real _18 =  ( 2316600*pi*t*x0-2316600*pi*t*x1 ) ;
            const real _6 =  ( 24*pi*xCenter-24*pi*x0-11*pi*t ) ;
            const real _17 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _14 =  ( 18*pi*xCenter-18*pi*x0 ) ;
            const real _11 =  ( 20*pi*xCenter-20*pi*x0 ) ;
            const real _19 =  ( 3346200*pi*t*x0-3346200*pi*t*x1 ) ;
            const real _0 =  ( 2316600*pi*t*x1-2316600*pi*t*x0 ) ;
            const real _12 =  ( 3346200*pi*t*x1-3346200*pi*t*x0 ) ;
            const real _21 =  ( 87609600*pi_p_2*x1-87609600*pi_p_2*x0 ) ;
            const real _15 =  ( 16426800*pi*t*x1-16426800*pi*t*x0 ) ;
            const real _16 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _3 =  ( 26*pi*xCenter-26*pi*x0 ) ;
            const real _13 =  ( 18*pi*xCenter-18*pi*x1 ) ;
            const real _34 = sin ( _10/_2 ) ;
            const real _47 = cos ( _6/_2 ) ;
            const real _22 = sin ( _1/_2 ) ;
            const real _36 = cos ( _11/_2 ) ;
            const real _42 = cos ( _17/t ) ;
            const real _44 = sin ( _14/_2 ) ;
            const real _49 = cos ( _10/_2 ) ;
            const real _39 = cos ( _14/_2 ) ;
            const real _33 = cos ( _9/_2 ) ;
            const real _26 = sin ( _5/_2 ) ;
            const real _24 = cos ( _3/_2 ) ;
            const real _23 = cos ( _1/_2 ) ;
            const real _35 = sin ( _11/_2 ) ;
            const real _43 = sin ( _3/_2 ) ;
            const real _25 = sin ( _4/_2 ) ;
            const real _41 = cos ( _16/t ) ;
            const real _30 = cos ( _7/_2 ) ;
            const real _46 = cos ( _4/_2 ) ;
            const real _28 = sin ( _6/_2 ) ;
            const real _40 = sin ( _16/t ) ;
            const real _29 = sin ( _7/_2 ) ;
            const real _32 = sin ( _9/_2 ) ;
            const real _48 = cos ( _8/_2 ) ;
            const real _38 = cos ( _13/_2 ) ;
            const real _37 = sin ( _13/_2 ) ;
            const real _45 = sin ( _17/t ) ;
            const real _31 = sin ( _8/_2 ) ;
            const real _27 = cos ( _5/_2 ) ;
            const real _52 =  ( 980100*t_p_2*_22+_0*_23-980100*t_p_2*_43+10038600*pi*t*x0*_46+4601025*t_p_2*_26+10038600*pi*t*x1*_27-10038600*pi*t*x0*_47-4601025*t_p_2*_29-10038600*pi*t*x0*_30+12046320*pi*t*x0*_48+6625476*t_p_2*_32+12046320*pi*t*x1*_33-12046320*pi*t*x0*_49-6625476*t_p_2*_35-12046320*pi*t*x0*_36+2044900*t_p_2*_37+_12*_38-2044900*t_p_2*_44+8213400*t_p_2*_40+_15*_41-8213400*t_p_2*_45 ) ;
            const real _51 =  ( 980100*t_p_2*_23+_18*_43-980100*t_p_2*_24-10038600*pi*t*x1*_25-10038600*pi*t*x1*_26+4601025*t_p_2*_27+10038600*pi*t*x1*_28+10038600*pi*t*x0*_29-4601025*t_p_2*_30-12046320*pi*t*x1*_31-12046320*pi*t*x1*_32+6625476*t_p_2*_33+12046320*pi*t*x1*_34+12046320*pi*t*x0*_35-6625476*t_p_2*_36+2044900*t_p_2*_38+_19*_44-2044900*t_p_2*_39+8213400*t_p_2*_41+_20*_45-8213400*t_p_2*_42 ) ;
            const real _50 =  ( _0*_22-980100*t_p_2*_23+980100*t_p_2*_24+10038600*pi*t*x0*_25+10038600*pi*t*x1*_26-4601025*t_p_2*_27-10038600*pi*t*x0*_28-10038600*pi*t*x0*_29+4601025*t_p_2*_30+12046320*pi*t*x0*_31+12046320*pi*t*x1*_32-6625476*t_p_2*_33-12046320*pi*t*x0*_34-12046320*pi*t*x0*_35+6625476*t_p_2*_36+_12*_37-2044900*t_p_2*_38+2044900*t_p_2*_39+_15*_40-8213400*t_p_2*_41+8213400*t_p_2*_42 ) ;
            const real _53 =  ( -980100*t_p_2*_22+980100*t_p_2*_43+_18*_24-10038600*pi*t*x1*_46-4601025*t_p_2*_26-10038600*pi*t*x1*_27+10038600*pi*t*x1*_47+4601025*t_p_2*_29+10038600*pi*t*x0*_30-12046320*pi*t*x1*_48-6625476*t_p_2*_32-12046320*pi*t*x1*_33+12046320*pi*t*x1*_49+6625476*t_p_2*_35+12046320*pi*t*x0*_36-2044900*t_p_2*_37+2044900*t_p_2*_44+_19*_39-8213400*t_p_2*_40+8213400*t_p_2*_45+_20*_42 ) ;
            const real _55 =  ( _52*y1+_53*y0 ) ;
            const real _54 =  ( _50*y1+_51*y0 ) ;
            //expr

            re = -_54/_21;im = _55/_21;
        }

        return complex(re, im);
    }

}
