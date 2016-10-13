#include <iostream>

#include "sp/math.hpp"
#include "sp/conv/kernelTabled.hpp"
#include "sp/conv/periodGrid.hpp"
#include "sp/conv/signalConvolver.hpp"

using namespace std;
using namespace sp;
using namespace sp::conv;


#if 1




int testI(std::size_t splp);

int test()
{
    testI(3000);
}


int testI(std::size_t splp)
{
    sp::real ppw = 1;

    cout.precision(20);
    cout.setf(std::ios::scientific);






    sp::real sampleStep = 1.0L/360000;

    sp::real minT = 1.0/20000;
    sp::real maxT = 1.0/0.02;


    PeriodGrid echoPeriodsGrid(
        minT,
        maxT,
        1000,
        PeriodGridType::frequencyLog);

    TVReal echoPeriods = echoPeriodsGrid.grid();

    cerr
        <<"efmin: "<<(sp::real(1)/echoPeriods.back())
        <<", efmax: "<<(sp::real(1)/echoPeriods.front())
        <<", efcount: "<<echoPeriods.size()
        <<", efstep: "<<echoPeriods[1]/echoPeriods[0]
        <<endl;

    TVReal spectrPeriods;
    {

        TVReal::const_iterator sfBegin = std::lower_bound(echoPeriods.begin(), echoPeriods.end(), echoPeriods.front());
        TVReal::const_iterator sfEnd = std::lower_bound(echoPeriods.begin(), echoPeriods.end(), echoPeriods.back()/1);


        std::size_t sfCountMult = 1;

        spectrPeriods.clear();
        spectrPeriods.resize(std::size_t(sp::real(sfEnd-sfBegin)/sfCountMult+0.5));
        auto sfIter = sfBegin;
        for(std::size_t idx(0); idx<spectrPeriods.size(); ++idx)
        {
            spectrPeriods[idx] = *sfIter;
            sfIter += std::ptrdiff_t(sfCountMult);
        }

    }

    cerr
        <<"sfmin: "<<(sp::real(1)/spectrPeriods.back())
        <<", sfmax: "<<(sp::real(1)/spectrPeriods.front())
        <<", sfcount: "<<spectrPeriods.size()
        <<", sfstep: "<<spectrPeriods[1]/spectrPeriods[0]
        <<endl;


    //exit(0);






    if(1)
    {
        TVEchoPoint echo(echoPeriods.size());
        KernelTabled kt(ppw, splp);
        //sp::Kernel kt(POW);

//        kt.eval(0.75223948756, 1, complex(.23452,1.3456));
//        exit(0);



//        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
//        std::cerr<<"make signal"<<std::endl;
//        TVReal signal;

//        signal.resize(std::size_t(tMax*POW*1.2/sampleStep+1));//чтобы уместился максимальный период
//        for(size_t index(0); index<signal.size(); ++index)
//        {
//            sp::real x = index * sampleStep;
//            sp::real xTarget = (signal.size()-2)*sampleStep;

//            signal[index] = 0;

//            sp::real a = x/xTarget;

//            //std::size_t k = spectrPeriods.size()/2;
//            for(std::size_t k(2); k<spectrPeriods.size(); k+=5)
//            {
//                sp::real t = spectrPeriods[k];
//                signal[index] += a*sp::sin((x-xTarget)*sp::g_2pi/t);
//            }


//            //cout << x<<","<<signal[index]<< endl;

//        }


        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
//        SignalConvolver c;
//        c.setup(POW, echoPeriods, sampleStep, splp, cpo, SignalApproxType::poly6p5o32x);

//        std::cerr<<"push signal"<<std::endl;
//        c.pushSignal(&signal[0], signal.size());

//        std::cerr<<"convolve signal"<<std::endl;
//        response = c.convolve();




        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        for(size_t i(0); i<echoPeriods.size(); ++i)
        {
            //std::cerr<<"mk echo #"<<i<<std::endl;

            sp::conv::SpectrPoint s_one;
            s_one(0) = 1;
            s_one(1) = 0;

            std::size_t k = spectrPeriods.size()/2;
            //for(std::size_t k(2); k<spectrPeriods.size(); k+=2)
            {
                //std::cerr<<(echoPeriods[i]/echoPeriods[k])<<std::endl;
                echo[i] += kt.eval(
                            echoPeriods[i],
                            (spectrPeriods[k] + spectrPeriods[k+1])/2,
                            s_one);
            }
        }
        kt.flush();

        for(size_t i(0); i<echoPeriods.size(); ++i)
        {
            conv::EchoPoint ep = echo[i];
            std::cout<<(echoPeriods[i]/spectrPeriods[spectrPeriods.size()/2])<<", "<<ep(0)<<", "<<ep(1);//<<", "<<ep(2)<<", "<<ep(3)<<", "<<ep(4)<<", "<<ep(5);
            std::cout<<std::endl;
        }
        exit(0);

        std::cerr<<"deconvolve"<<std::endl;
        //for(int iters0(1); iters0<200; iters0++)
        {
            TVSpectrPoint spectr(spectrPeriods.size());

            kt.deconvolve(
                    echo.size(), &echoPeriods[0], &echo[0],
                    spectr.size(), &spectrPeriods[0], &spectr[0]);

            cerr<<"deconvolved"<<endl;
            for(size_t i(0); i<spectr.size(); ++i)
            {
                conv::SpectrPoint s = spectr[i];
                std::cout<<s(0)<<", "<<s(1)<<std::endl;
            }
        }
        kt.flush();
    }

    return 0;
}

#endif
