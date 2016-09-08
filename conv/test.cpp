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








    sp::PeriodGrid echoPeriods(sp::g_periodMin, sp::g_periodMax, sp::g_periodSteps, sp::PeriodGridType::frequencyLog);
    std::cerr
          <<"efmin: "<<(1.0/echoPeriods.grid().back())
          <<", efmax: "<<(1.0/echoPeriods.grid().front())
          <<", efcount: "<<echoPeriods.grid().size()
          <<", efstep: "<<(echoPeriods.grid()[1]/echoPeriods.grid()[0])
          <<std::endl;

    sp::PeriodGrid spectrPeriods = sp::PeriodGrid(echoPeriods.grid()[0], echoPeriods.grid()[780], 780, sp::PeriodGridType::frequencyLog);
    std::cerr
            <<"sfmin: "<<(1.0/spectrPeriods.grid().back())
            <<", sfmax: "<<(1.0/spectrPeriods.grid().front())
            <<", sfcount: "<<spectrPeriods.grid().size()
            <<", sfstep: "<<(spectrPeriods.grid()[1]/spectrPeriods.grid()[0])
            <<std::endl;

    //exit(0);





#define POW 10.0

    std::size_t splp = 200;
    std::size_t cpo = 11;


    if(1)
    {
        sp::KernelTabled kt(POW, splp, cpo);
        //sp::Kernel kt(POW);

        kt.eval(1.2345234, 1, sp::complex(.23452,1.3456));
        exit(0);



        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        std::cerr<<"make signal"<<std::endl;
        sp::TVReal signal;

        signal.resize(std::size_t(sp::g_periodMax*POW*2.2/sp::g_sampleStep+1));//чтобы уместился максимальный период
        for(size_t index(0); index<signal.size(); ++index)
        {
            sp::real x = index * sp::g_sampleStep;
            sp::real xTarget = (signal.size()-2)*sp::g_sampleStep;

            signal[index] = 0;

            sp::real a = x/xTarget;

            for(std::size_t k(2); k<spectrPeriods.grid().size(); k+=2)
            {
                sp::real t = echoPeriods.grid()[k];
                signal[index] += sin((x-xTarget)*sp::g_2pi/t);
            }


            //cout << x<<","<<signal[index]<< endl;

        }


        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        sp::TVComplex response(sp::g_periodSteps);
        sp::SignalConvolver c;
        c.setup(POW, echoPeriods.grid(), sp::g_sampleStep, splp, cpo, sp::SignalApproxType::poly6p5o32x);

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
//                response[i] += kt.eval(echoPeriods.grid()[i], echoPeriods.grid()[k], sp::complex(0,1));
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
        std::vector<long double> work;
        //for(int iters0(1); iters0<20; iters0++)
        {
            sp::TVComplex spectr(spectrPeriods.grid().size());

            std::size_t iters = 6;
            sp::real error0 = 0;
            sp::real error1 = 0;
            kt.deconvolve(
                    response.size(), &echoPeriods.grid()[0], &response[0],
                    spectr.size(), &spectrPeriods.grid()[0], &spectr[0],
                    iters,
                    1e-40,
                    error0,
                    error1,
                    work);

            cerr<<iters<<": "<<error0<<"->"<<error1<<endl;
            for(size_t i(0); i<spectr.size(); ++i)
            {
                std::cout<<spectr[i].re()<<", "<<spectr[i].im()<<std::endl;
            }
        }
    }

    return 0;
}
