#include <iostream>

#include "sp/config.hpp"
#include "sp/math.hpp"
#include "sp/kernel.hpp"
#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"
#include "sp/periodGrid.hpp"

#include "test/scaledData.h"

using namespace std;





//hann, hann2












#define POW 10.0


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

    signal.resize(5.1/sp::g_sampleStep);//10 сек
    for(size_t index(0); index<signal.size(); ++index)
    {
        sp::real x = index * sp::g_sampleStep;
        signal[index] = cos((x-1.5)*sp::g_2pi*200);//ровно посеридине нашей шкалы

        //cout << x<<","<<signal[index]<< endl;

    }

    if(0)
    {
        hel::ScaledData sd;

        hel::SettingsScaledData settings;

        settings._zeroPeriod = sp::g_sampleStep;
        settings._smoothLength = 0;
        settings._maxLength = 100000000;

        sd.setup(settings);

        for(std::size_t idx(0); idx<signal.size(); idx++)
        {
            hel::TimedValue tv;
            tv._time = sp::g_sampleStep * idx;
            tv._value = signal[idx];

            sd.pushData(&tv, 1);
        }

        hel::TDTimedValue signal2;
        sd.fillSmoothedDischarged(1.5, signal2, 100, POW*0.9620, false);

        sp::TVReal signal3;
        for(const auto &tv : signal2)
        {
            signal3.push_back(tv._value);
        }

//        for(std::size_t i(0); i<std::min(signal.size(), signal3.size()); i+=1)
//        {
//            if(i>1000) break;

//            std::cout<<signal[(1.0/sp::g_sampleStep + 0.5)-i]<<", "<<signal3[signal3.size()-1-i]<<std::endl;
//        }
//        exit(0);

        signal.swap(signal3);
    }

    if(1)
    {
        sp::KernelTabled kt(POW);
        //kt.setup(5, 0.1, 10, 1000);
        //kt.setup(10, 0.1, 10.0, 100*1000);
        //exit(0);


        {
//            sp::Kernel k(5);

//            sp::PeriodGrid periodGridc(0.1/200, 2.0/200, 1000, sp::PeriodGridType::frequencyLin);
//            sp::Convolver c(POW);
//            sp::TVComplex response(sp::g_periodSteps);
//            c.execute(periodGridc, 0, sp::g_sampleStep, signal, 1.5, response);

//            sp::PeriodGrid periodGrid(0.1, 1, 1000, sp::PeriodGridType::frequencyLin);

//            for(size_t i(0); i<periodGrid.grid().size(); ++i)
//            {

//                //std::cout<<periodGrid.grid()[i]<<", ";

////                sp::complex ck = k.eval(periodGrid.grid()[i], 1.0, sp::complex(1,0));
////                std::cout<<ck.re()<<", "<<ck.im()<<", ";

//                sp::complex r = response[i];
//                std::cout<<r.re()<<", "<<r.im()<<", ";

//                sp::complex ckt = kt.eval(periodGrid.grid()[i], 1.0, sp::complex(1,0));
//                std::cout<<ckt.re()<<", "<<ckt.im()<<", ";

//                std::cout<<std::endl;
//            }


            //exit(0);
        }

        sp::TVComplex response(sp::g_periodSteps);

        sp::PeriodGrid periodGrid(sp::g_periodMin, sp::g_periodMax, sp::g_periodSteps, sp::PeriodGridType::frequencyLog);

        sp::Convolver c(POW);
        //c.execute(periodGrid, 0, sp::g_sampleStep, signal, 1.5, response);

        for(size_t i(0); i<periodGrid.grid().size(); ++i)
        {
            response[i] = 0;
            response[i] += kt.eval(periodGrid.grid()[i], 1.0/200, sp::complex(1,0));
        }

//        std::cout<<"response"<<std::endl;
//        for(size_t i(0); i<periodGrid.grid().size(); ++i)
//        {
//            std::cout<<response[i].re()<<", "<<response[i].im();
//            std::cout<<std::endl;
//        }

//        exit(0);

        std::vector<double> work;

        //для инициализации спектра нулем - mu=1e-10 лучший. При меньших значениях начинают артифакты появляться, при больших - медленно сходится
        int iters = 5;
        //for(int iters(1); iters<200; iters++)
        {
            sp::TVComplex spectr(response.size());
            //spectr = response;
            //spectr[97] = sp::complex(1,0);

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
