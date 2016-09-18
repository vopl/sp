#include <iostream>

#include "sp/config.hpp"
#include "sp/math.hpp"
#include "sp/kernel.hpp"
#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"
#include "sp/periodGrid.hpp"
#include "sp/signalConvolver.hpp"

using namespace std;












int test()
{
    cout.precision(20);
    cout.setf(std::ios::scientific);






    sp::real sampleStep = 1.0L/360000;

    sp::real tMin = 1.0/20000;
    sp::real tMax = 1.0/0.4;
    std::size_t tCount = 1000;

    sp::PeriodGrid echoPeriods(tMin, tMax, tCount, sp::PeriodGridType::frequencyLog);
    std::cerr
          <<"efmin: "<<(1.0/echoPeriods.grid().back())
          <<", efmax: "<<(1.0/echoPeriods.grid().front())
          <<", efcount: "<<echoPeriods.grid().size()
          <<", efstep: "<<(echoPeriods.grid()[1]/echoPeriods.grid()[0])
          <<std::endl;

    sp::PeriodGrid spectrPeriods = sp::PeriodGrid(
            echoPeriods.grid()[0],
            echoPeriods.grid()[600],
            600,
            sp::PeriodGridType::frequencyLog);
    std::cerr
            <<"sfmin: "<<(1.0/spectrPeriods.grid().back())
            <<", sfmax: "<<(1.0/spectrPeriods.grid().front())
            <<", sfcount: "<<spectrPeriods.grid().size()
            <<", sfstep: "<<(spectrPeriods.grid()[1]/spectrPeriods.grid()[0])
            <<std::endl;


    //exit(0);





#define POW 2.0

    std::size_t splp = 100;
    std::size_t cpo = 0;


    if(1)
    {
        sp::TVComplex response(echoPeriods.grid().size());
        sp::KernelTabled kt(POW, splp, cpo);
        //sp::Kernel kt(POW);

//        kt.eval(0.75223948756, 1, sp::complex(.23452,1.3456));
//        exit(0);



        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        std::cerr<<"make signal"<<std::endl;
        sp::TVReal signal;

        signal.resize(std::size_t(tMax*POW*1.2/sampleStep+1));//чтобы уместился максимальный период
        for(size_t index(0); index<signal.size(); ++index)
        {
            sp::real x = index * sampleStep;
            sp::real xTarget = (signal.size()-2)*sampleStep;

            signal[index] = 0;

            sp::real a = x/xTarget;

            //std::size_t k = spectrPeriods.grid().size()/2;
            for(std::size_t k(2); k<spectrPeriods.grid().size(); k+=5)
            {
                sp::real t = spectrPeriods.grid()[k];
                signal[index] += a*sin((x-xTarget)*sp::g_2pi/t);
            }


            //cout << x<<","<<signal[index]<< endl;

        }


        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        sp::SignalConvolver c;
        c.setup(POW, echoPeriods.grid(), sampleStep, splp, cpo, sp::SignalApproxType::poly6p5o32x);

        std::cerr<<"push signal"<<std::endl;
        c.pushSignal(&signal[0], signal.size());

        std::cerr<<"convolve signal"<<std::endl;
        response = c.convolve();




//        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
//        for(size_t i(0); i<echoPeriods.grid().size(); ++i)
//        {
//            //std::cerr<<"mk echo #"<<i<<std::endl;

//            response[i] = 0;
//            std::size_t k = 500;
//            //for(std::size_t k(2); k<spectrPeriods.grid().size(); k+=2)
//            {
//                //std::cerr<<(echoPeriods.grid()[i]/echoPeriods.grid()[k])<<std::endl;
//                response[i] += kt.eval(echoPeriods.grid()[i], echoPeriods.grid()[k], sp::complex(1,0));
//            }
//        }

//        for(size_t i(0); i<echoPeriods.grid().size(); ++i)
//        {
//            //std::cout<<response[i].re()<<", "<<response[i].im()<<", ";
//            std::cout<<(echoPeriods.grid()[i]/echoPeriods.grid()[500])<<", "<<response[i].re()<<", "<<response[i].im();
//            std::cout<<std::endl;
//        }

//        exit(0);

        std::cerr<<"deconvolve"<<std::endl;
        //for(int iters0(1); iters0<20; iters0++)
        {
            sp::TVComplex spectr(spectrPeriods.grid().size());

            std::size_t iters = 10;
            sp::real error0 = 0;
            sp::real error1 = 0;
            kt.deconvolve(
                    response.size(), &echoPeriods.grid()[0], &response[0],
                    spectr.size(), &spectrPeriods.grid()[0], &spectr[0],
                    iters,
                    1e-40,
                    error0,
                    error1);

            cerr<<iters<<": "<<error0<<"->"<<error1<<endl;
            for(size_t i(0); i<spectr.size(); ++i)
            {
                sp::complex s = spectr[i];
                std::cout<<s.re()<<", "<<s.im()<<std::endl;
            }
        }
    }

    return 0;
}
