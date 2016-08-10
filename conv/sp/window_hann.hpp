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
        static const real pi = g_pi;
        const real t = period;

        const real t_p_2 = t*t;
        const real pi_p_2 = pi*pi;

        real re, im;

        {
            const real _15 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _14 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _11 =  ( 8*pi*xCenter-8*pi*x1 ) ;
            const real _6 =  ( 11*pi*xCenter-11*pi*x0 ) ;
            const real _5 =  ( 11*pi*xCenter-11*pi*x1 ) ;
            const real _16 =  ( 261360*pi*t*x0-261360*pi*t*x1 ) ;
            const real _0 =  ( 261360*pi*t*x1-261360*pi*t*x0 ) ;
            const real _9 =  ( 9*pi*xCenter-9*pi*x0 ) ;
            const real _3 =  ( 12*pi*xCenter-12*pi*x0 ) ;
            const real _18 =  ( 1393920*pi*t*x1-1393920*pi*t*x0 ) ;
            const real _2 =  ( 5*t ) ;
            const real _13 =  ( 1881792*pi*t*x1-1881792*pi*t*x0 ) ;
            const real _20 =  ( 1881792*pi*t*x0-1881792*pi*t*x1 ) ;
            const real _10 =  ( 392040*pi*t*x1-392040*pi*t*x0 ) ;
            const real _7 =  ( 1393920*pi*t*x0-1393920*pi*t*x1 ) ;
            const real _19 =  ( 392040*pi*t*x0-392040*pi*t*x1 ) ;
            const real _4 =  ( 1140480*pi*t*x0-1140480*pi*t*x1 ) ;
            const real _21 =  ( 10036224*pi_p_2*x1-10036224*pi_p_2*x0 ) ;
            const real _8 =  ( 9*pi*xCenter-9*pi*x1 ) ;
            const real _12 =  ( 8*pi*xCenter-8*pi*x0 ) ;
            const real _1 =  ( 12*pi*xCenter-12*pi*x1 ) ;
            const real _17 =  ( 1140480*pi*t*x1-1140480*pi*t*x0 ) ;
            const real _41 = sin ( _15/t ) ;
            const real _34 = sin ( _14/t ) ;
            const real _36 = cos ( _15/t ) ;
            const real _22 = sin ( _1/_2 ) ;
            const real _28 = sin ( _8/_2 ) ;
            const real _39 = sin ( _9/_2 ) ;
            const real _37 = sin ( _3/_2 ) ;
            const real _23 = cos ( _1/_2 ) ;
            const real _24 = cos ( _3/_2 ) ;
            const real _30 = cos ( _9/_2 ) ;
            const real _38 = sin ( _6/_2 ) ;
            const real _31 = sin ( _11/_2 ) ;
            const real _33 = cos ( _12/_2 ) ;
            const real _25 = sin ( _5/_2 ) ;
            const real _35 = cos ( _14/t ) ;
            const real _26 = cos ( _5/_2 ) ;
            const real _29 = cos ( _8/_2 ) ;
            const real _27 = cos ( _6/_2 ) ;
            const real _32 = cos ( _11/_2 ) ;
            const real _40 = sin ( _12/_2 ) ;
            const real _45 =  ( -108900*t_p_2*_22+108900*t_p_2*_37+_16*_24+518400*t_p_2*_25-518400*t_p_2*_38+_17*_27+774400*t_p_2*_28-774400*t_p_2*_39+_18*_30-245025*t_p_2*_31+245025*t_p_2*_40+_19*_33-940896*t_p_2*_34+940896*t_p_2*_41+_20*_36 ) ;
            const real _42 =  ( _0*_22-108900*t_p_2*_23+108900*t_p_2*_24+_4*_25+518400*t_p_2*_26-518400*t_p_2*_27+_7*_28+774400*t_p_2*_29-774400*t_p_2*_30+_10*_31-245025*t_p_2*_32+245025*t_p_2*_33+_13*_34-940896*t_p_2*_35+940896*t_p_2*_36 ) ;
            const real _44 =  ( 108900*t_p_2*_22+_0*_23-108900*t_p_2*_37-518400*t_p_2*_25+_4*_26+518400*t_p_2*_38-774400*t_p_2*_28+_7*_29+774400*t_p_2*_39+245025*t_p_2*_31+_10*_32-245025*t_p_2*_40+940896*t_p_2*_34+_13*_35-940896*t_p_2*_41 ) ;
            const real _43 =  ( 108900*t_p_2*_23+_16*_37-108900*t_p_2*_24-518400*t_p_2*_26+_17*_38+518400*t_p_2*_27-774400*t_p_2*_29+_18*_39+774400*t_p_2*_30+245025*t_p_2*_32+_19*_40-245025*t_p_2*_33+940896*t_p_2*_35+_20*_41-940896*t_p_2*_36 ) ;
            const real _47 =  ( _44*y1+_45*y0 ) ;
            const real _46 =  ( _42*y1+_43*y0 ) ;
            //expr

            re = -_46/_21;im = _47/_21;
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
            const real _8 = cos ( 20*pi*t ) ;
            const real _1 = sin ( 22*pi*t ) ;
            const real _9 = cos ( 18*pi*t ) ;
            const real _6 =  ( 7607920399200000*pi*t_p_14-55113774963984000*pi*t_p_12+169170965251207920*pi*t_p_10-285201481123175760*pi*t_p_8+285201481123175760*pi*t_p_6-169170965251207920*pi*t_p_4+55113774963984000*pi*t_p_2-7607920399200000*pi ) ;
            const real _11 =  ( -2852970149700*t_p_10+11110215609999*t_p_8-14506735429098*t_p_6+6077353580199*t_p_4+1107514960200*t_p_2-960596010000 ) ;
            const real _2 =  ( -950990049900*t_p_11+1173771670599*t_p_9+5850258430302*t_p_7-14363180399601*t_p_5+11166116920200*t_p_3-2901194010000*t ) ;
            const real _3 = sin ( 20*pi*t ) ;
            const real _12 =  ( 2174188500000*t_p_10-6758378770000*t_p_8+6789128008850*t_p_6-1582610081800*t_p_4-1109630506050*t_p_2+480298005000 ) ;
            const real _5 = sin ( 18*pi*t ) ;
            const real _10 =  ( 974308500000*t_p_10-4360681170000*t_p_8+6859786820850*t_p_6-4020071887800*t_p_4+60628495950*t_p_2+480298005000 ) ;
            const real _0 =  ( 357246450000*t_p_11-813564729000*t_p_9-1213904966355*t_p_7+5072421236310*t_p_5-5003585932455*t_p_3+1595656705500*t ) ;
            const real _4 =  ( 652256550000*t_p_11+114464669000*t_p_9-5494174063345*t_p_7+9281914833290*t_p_5-5867004137445*t_p_3+1305537304500*t ) ;
            const real _7 = cos ( 22*pi*t ) ;
            const real _16 =  ( _10*_1+_11*_3+_12*_5 ) ;
            const real _13 =  ( _0*_1+_2*_3+_4*_5 ) ;
            const real _14 =  ( _0*_7+_2*_8+_4*_9-58512950100*t_p_11-474671610599*t_p_9+857820599398*t_p_7+8844330001*t_p_5-295526850300*t_p_3 ) ;
            const real _15 =  ( _10*_7+_11*_8+_12*_9-295526850300*t_p_10+8844330001*t_p_8+857820599398*t_p_6-474671610599*t_p_4-58512950100*t_p_2 ) ;
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
                rr = _13/_6;ri = _14/_6;ir = _15/_6;ii = -_16/_6;
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
