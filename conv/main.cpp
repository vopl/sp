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











    if(0)
    {
        sp::SignalConvolver sc;

        sp::PeriodGrid periodGrid(sp::g_periodMin, sp::g_periodMax, sp::g_periodSteps, sp::PeriodGridType::frequencyLog);
        sc.setup(POW, periodGrid, sp::g_sampleStep, 100);

        for(;;)
        {
            sp::real x0 = sc.getTime();

            sp::TVReal signal(1000000);
            for(size_t index(0); index<signal.size(); ++index)
            {
                signal[index] = cos((x0 + sp::g_sampleStep*(1+index))*sp::g_2pi*500 + 0.5);
            }
            sc.pushSignal(&signal[0], signal.size());

            sp::Convolver c(POW);
            sp::TVComplex response1;
            c.execute(periodGrid, 0, sp::g_sampleStep, signal, 1.4, response1);

            sp::TVComplex response2 = sc.convolve();
            for(size_t i(0); i<response1.size(); ++i)
            {
                auto v1 = response1[i];
                auto v2 = response2[i];

                std::cout<<v1.re()<<", "<<v1.im()<<", "<<v1.a()<<", "<<v2.re()<<", "<<v2.im()<<", "<<v2.a()<<std::endl;
            }
            exit(0);
        }
        exit(0);
    }





    sp::TVReal signal;

    signal.resize(std::size_t(5.1/sp::g_sampleStep));//10 сек
    for(size_t index(0); index<signal.size(); ++index)
    {
        sp::real x = index * sp::g_sampleStep;
        signal[index] = cos((x-1.5)*sp::g_2pi*800);//ровно посеридине нашей шкалы

        //cout << x<<","<<signal[index]<< endl;

    }





    if(1)
    {
        sp::KernelTabled kt(POW);
        //sp::Kernel k(POW);
        //kt.setup(5, 0.1, 10, 1000);
        //kt.setup(10, 0.1, 10.0, 100*1000);



        sp::TVComplex response(sp::g_periodSteps);

        sp::PeriodGrid periodGrid(sp::g_periodMin, sp::g_periodMax, sp::g_periodSteps, sp::PeriodGridType::frequencyLog);
        sp::SignalConvolver c;
        c.setup(POW, periodGrid, sp::g_sampleStep, 500);
        c.pushSignal(&signal[0], signal.size());
        response = c.convolve();

        for(size_t i(0); i<periodGrid.grid().size(); ++i)
        {
//            std::cout<<response[i].re()<<", "<<response[i].im()<<", ";

//            response[i] = 0;

//            sp::real t1 = (periodGrid.grid()[200] + periodGrid.grid()[201])/2;
//            response[i] += kt.eval(periodGrid.grid()[i], t1, sp::complex(1,0));

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
