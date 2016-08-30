#include <iostream>

#include "sp/config.hpp"
#include "sp/math.hpp"
#include "sp/kernel.hpp"
#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"
#include "sp/periodGrid.hpp"
#include "sp/signalConvolver.hpp"

#include "test/scaledData.h"

using namespace std;





//hann, hann2












#define POW 10.0


int main(int argc, char *argv[])
{
    cout.precision(20);
    cout.setf(std::ios::scientific);








    sp::PeriodGrid periodGrid(sp::g_periodMin, sp::g_periodMax, sp::g_periodSteps, sp::PeriodGridType::frequencyLog);



    std::cerr<<"make signal"<<std::endl;
    sp::TVReal signal;

    signal.resize(std::size_t(sp::g_periodMax*POW*2/sp::g_sampleStep+1));//чтобы уместился максимальный период
    for(size_t index(0); index<signal.size(); ++index)
    {
        sp::real x = index * sp::g_sampleStep;
        signal[index] = 0;

        for(std::size_t k(0); k<800; k+=4)
        {
            signal[index] += cos((x-1.5)*sp::g_2pi/periodGrid.grid()[k]);
        }


        //cout << x<<","<<signal[index]<< endl;

    }





    if(1)
    {
        sp::KernelTabled kt(POW);
        //sp::Kernel kt(POW);

        //чето с фиром
//        kt.eval(.63456, 1, sp::complex(.23452,1.3456));
//        exit(0);



        sp::TVComplex response(sp::g_periodSteps);
        sp::SignalConvolver c;
        c.setup(POW, periodGrid, sp::g_sampleStep, 800);

        std::cerr<<"push signal"<<std::endl;
        c.pushSignal(&signal[0], signal.size());

        std::cerr<<"convolve signal"<<std::endl;
        response = c.convolve();

        for(size_t i(0); i<periodGrid.grid().size(); ++i)
        {
//            std::cout<<response[i].re()<<", "<<response[i].im()<<", ";

//            response[i] = 0;

//            //sp::real t1 = (periodGrid.grid()[300] + periodGrid.grid()[300])/2;

//            for(std::size_t k(0); k<800; k+=4)
//            {
//                response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[k], sp::complex(0,1));
//            }



//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[200], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[210], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[220], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[230], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[240], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[250], sp::complex(1,0));

//            std::cout<<response[i].re()<<", "<<response[i].im();
//            std::cout<<std::endl;
        }

//        exit(0);

        std::vector<double> work;

        sp::PeriodGrid spectrPeriods(periodGrid.grid()[0], periodGrid.grid()[800], 401, sp::PeriodGridType::frequencyLog);
        //sp::PeriodGrid spectrPeriods(1.0/8000, 1.0/40, 1000, sp::PeriodGridType::frequencyLog);

        std::cerr<<"sfMin: "<<1.0/spectrPeriods.grid().back()<<", sfMax: "<<1.0/spectrPeriods.grid().front()<<std::endl;

        int iters = 10;
        //for(int iters0(1); iters0<20; iters0++)
        {
            sp::TVComplex spectr(spectrPeriods.grid().size());

            int res = kt.deconvolve(
                                 response.size(), &periodGrid.grid()[0], &response[0],
                                 spectr.size(), &spectrPeriods.grid()[0], &spectr[0],
                                 iters,
                                 work);

            cerr<<iters<<": "<<res<<endl;
            for(size_t i(0); i<spectr.size(); ++i)
            {
                std::cout<<spectr[i].re()<<", "<<spectr[i].im()<<std::endl;
            }
        }
    }

    return 0;
}
