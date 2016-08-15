#include <iostream>

#include "sp/config.hpp"
#include "sp/math.hpp"
#include "sp/kernel.hpp"
#include "sp/convolver.hpp"

using namespace std;





//hann, hann2















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
        signal[index] = cos((x-0.55)*sp::g_2pi/(sp::g_periodGrid[sp::g_periodGrid.size()/2]));//ровно посеридине нашей шкалы

        //cout << x<<","<<signal[index]<< endl;

    }

    if(0)
    {
        sp::Convolver c(10, sp::g_periodMin, sp::g_periodMax, sp::g_periodSteps);

        sp::TVComplex response1;
        c.execute(0, sp::g_sampleStep, signal, 0.55, response1);



        sp::Kernel rm(10);

        sp::TVComplex response2(sp::g_periodSteps);

        for(size_t i(0); i<sp::g_periodSteps; ++i)
        {
            response2[i] = 0;

            response2[i] += rm.eval(sp::g_periodGrid[i], sp::g_periodGrid[sp::g_periodGrid.size()/2], sp::complex(1,0));

            std::cout<<response1[i].im()<<", "<<response2[i].im()<<std::endl;
        }


        return 0;
    }

    if(1)
    {
        sp::Kernel rm(10);

        sp::TVComplex response(sp::g_periodSteps);
        //sp::convolve<sp::Window_hann, 10>(0.55, signal, response);

        for(size_t i(0); i<sp::g_periodSteps; ++i)
        {
            response[i] = 0;

            //response[i] += rm.eval(10, sp::g_periodGrid[i], sp::g_periodGrid[sp::g_periodGrid.size()/2], sp::complex(0,1));

            for(size_t j(80); j<sp::g_periodSteps-160; j+=50)
            {
                response[i] += rm.eval(sp::g_periodGrid[i], sp::g_periodGrid[j], sp::complex(1,0));
            }
        }

        sp::TVReal work;

        //для инициализации спектра нулем - mu=1e-10 лучший. При меньших значениях начинают артифакты появляться, при больших - медленно сходится
        int iters = 5;
        //for(int iters(1); iters<200; iters++)
        {
            sp::TVComplex spectr(response.size());
            //spectr = response;
            //spectr[250] = sp::complex(0.5,0);

            int res = rm.deconvolve(
                                 response.size(), &sp::g_periodGrid[0], &response[0],
                                 spectr.size(), &sp::g_periodGrid[0], &spectr[0],
                                 iters,
                                 work);

            cerr<<iters<<": "<<res<<endl;
            for(size_t i(0); i<sp::g_periodSteps; ++i)
            {
                std::cout<<spectr[i].a()<<" ";
            }
            std::cout<<endl;
        }
    }

    return 0;
}
