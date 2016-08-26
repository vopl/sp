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



    sp::TVReal signal;

    signal.resize(std::size_t(sp::g_periodMax*POW*2/sp::g_sampleStep+1));//чтобы уместился максимальный период
    for(size_t index(0); index<signal.size(); ++index)
    {
        sp::real x = index * sp::g_sampleStep;
        signal[index] =
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[200]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[210]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[220]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[230]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[240]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[250]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[260]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[270]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[280]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[290]) +
//                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[300]) +

                cos((x-1.5)*sp::g_2pi/periodGrid.grid()[450]) +
                0;

        //cout << x<<","<<signal[index]<< endl;

    }





    if(1)
    {
        sp::KernelTabled kt(POW);
        //sp::Kernel kt(POW);

        //чето с фиром
//        kt.eval(60, 1, sp::complex(1,0));
//        exit(0);



        sp::TVComplex response(sp::g_periodSteps);
        sp::SignalConvolver c;
        //c.setup(POW, periodGrid, sp::g_sampleStep, 400);
        //c.pushSignal(&signal[0], signal.size());
        //response = c.convolve();

        for(size_t i(0); i<periodGrid.grid().size(); ++i)
        {
//            std::cout<<response[i].re()<<", "<<response[i].im()<<", ";

            response[i] = 0;

            //sp::real t1 = (periodGrid.grid()[300] + periodGrid.grid()[300])/2;

            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[50], sp::complex(1,0));


//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[300], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[310], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[320], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[330], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[340], sp::complex(1,0));
//            response[i] += kt.eval(periodGrid.grid()[i], periodGrid.grid()[350], sp::complex(1,0));

//            std::cout<<response[i].re()<<", "<<response[i].im();
//            std::cout<<std::endl;
        }

//        exit(0);

        std::vector<double> work;

        //для инициализации спектра нулем - mu=1e-10 лучший. При меньших значениях начинают артифакты появляться, при больших - медленно сходится

        int iters = 20;
        //for(int iters0(1); iters0<20; iters0++)
        {
            sp::TVComplex spectr(response.size());

            int res = kt.deconvolve(
                                 response.size(), &periodGrid.grid()[0], &response[0],
                                 spectr.size(), &periodGrid.grid()[0], &spectr[0],
                                 iters,
                                 work);

            cerr<<iters<<": "<<res<<endl;
            for(size_t i(0); i<sp::g_periodSteps; ++i)
            {
                std::cout<<spectr[i].re()<<", "<<spectr[i].im()<<std::endl;
            }
        }
    }

    return 0;
}
