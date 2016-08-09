#include <iostream>
#include <vector>
#include <complex>
#include <cassert>

using namespace std;





namespace sp
{
    using real = long double;
    using complex = std::complex<real>;


    static const real g_pi = 3.141592653589793238462643383279502884L;
    static const real g_2pi = 2*g_pi;


    using TVReal = std::vector<real>;
    using TVComplex = std::vector<complex>;

    //частота в герцах
    static const real g_sampleStep = 1.0/8000;

    static const real g_periodMax = 1.0/100;//период для 100 герц
    static const real g_periodMin = 1.0/2000;//период для 2 килогерц
    static const long g_periodSteps = 1000;//размерность сетки

    //сетка периода, от максимума с шагом до минимума
    static const TVReal g_periodGrid = ([](){
        TVReal res;

        real minTLog = log(g_periodMin);
        real maxTLog = log(g_periodMax);
        real logtStep = (maxTLog - minTLog) / (g_periodSteps-1);
        res.resize(g_periodSteps);
        for(size_t k(0); k<g_periodSteps; k++)
        {
            res[k] = exp(minTLog+k*logtStep);
        }

        return res;
    })();






    //сворачивает сигнал input с окном и комплексной гармоникой, один раз
    template <class Window, int pow>//оконная функция (hann, gaussian), в ней математика интегрирования
    complex convolve(
            const real xCenter,          //на первом сэмпле время=0, каждый новый сэмпл это +1.0/g_frequency времени
            const TVReal &input,    //это вся дорожка, не кусок. В ней должно быть достаточно отсчетов чтобы
            const real period)      //целевой период
    {
        //итерировать сигнал, для каждого отчета накапливать свертку
        complex res;

        real xStart = xCenter-period*pow/2;
        real xStop = xCenter+period*pow/2;

        //левая граница. Найти максимальный отсчет, меньше старта
        size_t indexFirst = xStart/g_sampleStep;
        assert(indexFirst+1 < input.size());

        {
            real x0 = (indexFirst+0) * g_sampleStep;
            real x = xStart;
            real x1 = (indexFirst+1) * g_sampleStep;

            real y0 = input[indexFirst];
            real y1 = input[indexFirst+1];

            //линейная интерполяция от точки x0,y0 до x1,y1
            real y = ((y0-y1)*x + (x0*y1 - x1*y0)) / (x0-x1);

            res += Window::template integrate<pow>(
                       xCenter,
                       x, y,
                       x1, y1,
                       period);
        }

        //правая граница
        size_t indexLast = xStop/g_sampleStep;
        assert(indexLast > indexFirst);
        assert(indexLast+1 < input.size());

        {
            real x0 = (indexLast+0) * g_sampleStep;
            real x = xStop;
            real x1 = (indexLast+1) * g_sampleStep;

            real y0 = input[indexLast];
            real y1 = input[indexLast+1];

            //линейная интерполяция от точки x0,y0 до x1,y1
            real y = ((y0-y1)*x + (x0*y1 - x1*y0)) / (x0-x1);

            res += Window::template integrate<pow>(
                       xCenter,
                       x0, y0,
                       x, y,
                       period);
        }


        //центр
        for(size_t index(indexFirst+1); index<indexLast; ++index)
        {
            real x0 = (index+0) * g_sampleStep;
            real x1 = (index+1) * g_sampleStep;

            real y0 = input[index];
            real y1 = input[index+1];

            res += Window::template integrate<pow>(
                       xCenter,
                       x0, y0,
                       x1, y1,
                       period);
        }


        return res;
    }


    //сворачивает сигнал input с окном и комплексной гармоникой, по сетке периодов
    template <class Window, int pow>//оконная функция (hann, gaussian), в ней математика интегрирования
    void convolve(
            const real xCenter,          //на первом сэмпле время=0, каждый новый сэмпл это +1.0/g_frequency времени
            const TVReal &input,    //это вся дорожка, не кусок. В ней должно быть достаточно отсчетов чтобы
            TVComplex &output)      //результат свертки, размерность по сетке периодов
    {
        //итерировать сетку, для каждого периода делать свертку
        output.resize(g_periodSteps);

        for(size_t k(0); k<g_periodSteps; k++)
        {
            output[k] = convolve<Window, pow>(xCenter, input, g_periodGrid[k]);
        }
    }


    struct Window_hann
    {

        template <long pow>
        static complex integrate(real xCenter, real x0, real y0, real x1, real y1, real period);
    };

    template <>
    inline complex Window_hann::integrate<10>(real xCenter, real x0, real y0, real x1, real y1, real period)
    {
        static const real pi = g_pi;
        const real t = period;

        const real t_p_2 = t*t;
        const real t_p_3 = t*t*t;
        const real t_p_4 = t*t*t*t;
        const real t_p_5 = t*t*t*t*t;
        const real t_p_6 = t*t*t*t*t*t;

        const real pi_p_2 = pi*pi;

        real re, im;

        {
            const real _15 =  ( -5*pi*t_p_4-50*pi*t_p_3+500*pi*t_p_2+5000*pi*t ) ;
            const real _6 =  ( 5*pi*t_p_4-50*pi*t_p_3-500*pi*t_p_2+5000*pi*t ) ;
            const real _2 =  ( 5*t ) ;
            const real _23 =  ( 2*pi*xCenter-2*pi*x0 ) ;
            const real _22 =  ( -t_p_6+200*t_p_4-10000*t_p_2 ) ;
            const real _4 =  ( 2*pi*t+20*pi ) ;
            const real _5 =  ( -pi*t-10*pi ) ;
            const real _13 =  ( 10*pi*t_p_4+100*pi*t_p_3-1000*pi*t_p_2-10000*pi*t ) ;
            const real _17 =  ( -25*t_p_4-500*t_p_3-2500*t_p_2 ) ;
            const real _14 =  ( 25*t_p_4+500*t_p_3+2500*t_p_2 ) ;
            const real _12 =  ( 10*pi-pi*t ) ;
            const real _1 =  ( pi*t+10*pi ) ;
            const real _7 =  ( -25*t_p_4+500*t_p_3-2500*t_p_2 ) ;
            const real _18 =  ( -2*pi*t_p_5+400*pi*t_p_3-20000*pi*t ) ;
            const real _11 =  ( pi*t-10*pi ) ;
            const real _8 =  ( -10*pi*t_p_4+100*pi*t_p_3+1000*pi*t_p_2-10000*pi*t ) ;
            const real _10 =  ( -10*pi*t_p_4-100*pi*t_p_3+1000*pi*t_p_2+10000*pi*t ) ;
            const real _19 =  ( 2*pi*t_p_5-400*pi*t_p_3+20000*pi*t ) ;
            const real _20 =  ( 2*pi*xCenter-2*pi*x1 ) ;
            const real _16 =  ( 5*pi*t_p_4+50*pi*t_p_3-500*pi*t_p_2-5000*pi*t ) ;
            const real _0 =  ( 25*t_p_4-500*t_p_3+2500*t_p_2 ) ;
            const real _9 =  ( 10*pi*t_p_4-100*pi*t_p_3-1000*pi*t_p_2+10000*pi*t ) ;
            const real _21 =  ( t_p_6-200*t_p_4+10000*t_p_2 ) ;
            const real _3 =  ( -5*pi*t_p_4+50*pi*t_p_3+500*pi*t_p_2-5000*pi*t ) ;
            const real _24 =  ( 8*pi_p_2*t_p_4-1600*pi_p_2*t_p_2+80000*pi_p_2 ) ;
            const real _25 =  ( -8*pi_p_2*t_p_4+1600*pi_p_2*t_p_2-80000*pi_p_2 ) ;
            const real _28 =  ( _4*xCenter+_5*x0 ) ;
            const real _31 =  ( _11*xCenter+_12*x1-5*pi*t ) ;
            const real _43 =  ( _19*x1+_18*x0 ) ;
            const real _34 =  ( 2*pi*t*xCenter+_12*x0 ) ;
            const real _26 =  ( _1*xCenter ) ;
            const real _33 =  ( 2*pi*t*xCenter+_12*x1 ) ;
            const real _42 =  ( _1*x0 ) ;
            const real _35 =  ( 20*pi*xCenter+_11*x1 ) ;
            const real _30 =  ( _1*xCenter+_5*x0-5*pi*t ) ;
            const real _40 = cos ( _23/t ) ;
            const real _27 =  ( _4*xCenter+_5*x1 ) ;
            const real _32 =  ( _11*xCenter+_12*x0-5*pi*t ) ;
            const real _44 = sin ( _23/t ) ;
            const real _39 = cos ( _20/t ) ;
            const real _38 = sin ( _20/t ) ;
            const real _29 =  ( _1*xCenter+_5*x1-5*pi*t ) ;
            const real _36 =  ( 20*pi*xCenter+_11*x0 ) ;
            const real _37 =  ( _18*x1+_19*x0 ) ;
            const real _45 =  ( _24*x1+_25*x0 ) ;
            const real _41 =  ( _1*x1 ) ;
            const real _46 = sin ( _26/_2 ) ;
            const real _56 = sin ( _33/_2 ) ;
            const real _59 = cos ( _34/_2 ) ;
            const real _70 = cos ( _31/_2 ) ;
            const real _47 = cos ( _26/_2 ) ;
            const real _49 = cos ( _27/_2 ) ;
            const real _60 = sin ( _35/_2 ) ;
            const real _53 = sin ( _30/_2 ) ;
            const real _57 = cos ( _33/_2 ) ;
            const real _54 = sin ( _31/_2 ) ;
            const real _55 = sin ( _32/_2 ) ;
            const real _58 = sin ( _34/_2 ) ;
            const real _51 = cos ( _28/_2 ) ;
            const real _63 = cos ( _36/_2 ) ;
            const real _62 = sin ( _36/_2 ) ;
            const real _50 = sin ( _28/_2 ) ;
            const real _66 = sin ( _42/_2 ) ;
            const real _68 = cos ( _29/_2 ) ;
            const real _67 = cos ( _42/_2 ) ;
            const real _52 = sin ( _29/_2 ) ;
            const real _65 = cos ( _41/_2 ) ;
            const real _61 = cos ( _35/_2 ) ;
            const real _69 = cos ( _30/_2 ) ;
            const real _64 = sin ( _41/_2 ) ;
            const real _71 = cos ( _32/_2 ) ;
            const real _48 = sin ( _27/_2 ) ;
            const real _92 =  ( _14*_46+_15*x0*_47 ) ;
            const real _91 =  ( _15*x1*_46+_17*_47 ) ;
            const real _73 =  ( _6*x1*_46+_0*_47 ) ;
            const real _97 =  ( _15*x0*_46+_14*_47 ) ;
            const real _82 =  ( _17*_46+_15*x0*_47 ) ;
            const real _96 =  ( _14*_46+_16*x0*_47 ) ;
            const real _99 =  ( _3*x1*_64+_7*_65+_6*x0*_66+_0*_67 ) ;
            const real _81 =  ( _15*x1*_46+_14*_47 ) ;
            const real _74 =  ( _7*_46+_6*x0*_47 ) ;
            const real _79 =  ( _15*x0*_46+_17*_47 ) ;
            const real _77 =  ( _16*x1*_46+_14*_47 ) ;
            const real _98 =  ( _7*_64+_6*x1*_65+_0*_66+_3*x0*_67 ) ;
            const real _84 =  ( _0*_64+_3*x1*_65+_7*_66+_6*x0*_67 ) ;
            const real _87 =  ( _3*x1*_46+_7*_47 ) ;
            const real _78 =  ( _17*_46+_16*x0*_47 ) ;
            const real _93 =  ( _16*x0*_46+_14*_47 ) ;
            const real _83 =  ( _16*x0*_46+_17*_47 ) ;
            const real _88 =  ( _0*_46+_3*x0*_47 ) ;
            const real _75 =  ( _3*x0*_46+_7*_47 ) ;
            const real _85 =  ( _6*x1*_64+_0*_65+_3*x0*_66+_7*_67 ) ;
            const real _89 =  ( _6*x0*_46+_0*_47 ) ;
            const real _76 =  ( _14*_46+_15*x1*_47 ) ;
            const real _72 =  ( _0*_46+_3*x1*_47 ) ;
            const real _90 =  ( _17*_46+_16*x1*_47 ) ;
            const real _95 =  ( _16*x1*_46+_17*_47 ) ;
            const real _94 =  ( _17*_46+_15*x1*_47 ) ;
            const real _80 =  ( _14*_46+_16*x1*_47 ) ;
            const real _86 =  ( _7*_46+_6*x1*_47 ) ;
            const real _103 =  ( _87*_48+_72*_49+_89*_50+_74*_51+_8*x1*_68+_9*x1*_69+_13*x1*_70+_10*x1*_71+_77*_56+_90*_57+_79*_58+_92*_59+_95*_60+_80*_61+_97*_62+_82*_63+_22*_38+_21*_44+_37*_40+_99*_46+_84*_47 ) ;
            const real _100 =  ( _72*_48+_73*_49+_74*_50+_75*_51+_8*x0*_52+_9*x0*_53+_10*x0*_54+_13*x0*_55+_76*_56+_77*_57+_78*_58+_79*_59+_80*_60+_81*_61+_82*_62+_83*_63+_37*_38+_21*_39+_22*_40+_84*_46+_85*_47 ) ;
            const real _101 =  ( _86*_48+_87*_49+_88*_50+_89*_51+_9*x1*_52+_8*x1*_53+_13*x1*_54+_10*x1*_55+_90*_56+_91*_57+_92*_58+_93*_59+_94*_60+_95*_61+_96*_62+_97*_63+_22*_39+_43*_44+_21*_40+_98*_46+_99*_47 ) ;
            const real _102 =  ( _73*_48+_86*_49+_75*_50+_88*_51+_9*x0*_68+_8*x0*_69+_10*x0*_70+_13*x0*_71+_91*_56+_76*_57+_93*_58+_78*_59+_81*_60+_94*_61+_83*_62+_96*_63+_21*_38+_43*_39+_22*_44+_85*_46+_98*_47 ) ;
            const real _105 =  ( _102*y1+_103*y0 ) ;
            const real _104 =  ( _100*y1+_101*y0 ) ;
            //expr

            re = _104/_45;im = _105/_45;

        }


        return complex(re, im)/(x1-x0);
    }

    template <>
    inline complex Window_hann::integrate<11>(real xCenter, real x0, real y0, real x1, real y1, real period)
    {
        static const real pi = g_pi;
        const real t = period;

        const real t_p_2 = t*t;
        const real t_p_3 = t*t*t;
        const real t_p_4 = t*t*t*t;
        const real t_p_5 = t*t*t*t*t;
        const real t_p_6 = t*t*t*t*t*t;

        const real pi_p_2 = pi*pi;

        real re, im;

        {
            assert(0);
        }


        return complex(re, im);
    }

}










int main(int argc, char *argv[])
{
    cout.precision(20);
    cout.setf(std::ios::scientific);

//    sp::real x = 1.0/3;
//    cout << "Hello World! "<< sizeof(x)*8<< endl;

//    for(const auto &x : sp::g_periodGrid)
//    {
//        cout << x<< endl;
//    }

    sp::TVReal signal;

    signal.resize(100000);
    for(size_t index(0); index<signal.size(); ++index)
    {
        sp::real x = index * sp::g_sampleStep;
        signal[index] = sin(x*sp::g_2pi*500);//500 герц

        //cout << x<<","<<signal[index]<< endl;

    }


    sp::TVComplex response;

    sp::convolve<sp::Window_hann, 10>(0.456, signal, response);

    for(size_t index(0); index<sp::g_periodSteps; ++index)
    {
        const auto t = sp::g_periodGrid[index];
        const auto r = response[index];
        cout << t<<", "<<::sqrt(r.real()*r.real() + r.imag()*r.imag())<< endl;
    }

    return 0;
}
