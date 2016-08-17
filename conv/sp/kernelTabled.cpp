#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"
#include "levmar.h"

#include <iostream>

namespace sp
{
    KernelTabled::KernelTabled()
    {

    }

    KernelTabled::~KernelTabled()
    {

    }

    namespace
    {
        complex approxCos(TVReal &ys)
        {
            real levmarInfo[LM_INFO_SZ];
            TVReal work;
            if(work.size() < LM_DIF_WORKSZ(2, ys.size()*2))
            {
                work.resize(LM_DIF_WORKSZ(2, ys.size()*2));
            }

            static real levmarOpts[LM_OPTS_SZ] =
            {
                1e-3,  //LM_INIT_MU,        //mu
                1e-40,  //LM_STOP_THRESH,    //stopping thresholds for ||J^T e||_inf,
                1e-40,  //LM_STOP_THRESH,    //||Dp||_2 and
                1e-40,  //LM_STOP_THRESH,    //||e||_2. Set to NULL for defaults to be used.
            };

            real p[2]={1,0};

            int levmarResult = dlevmar_der(
                        [](double *p, double *hx, int m, int n, void *_levmarParams)->void{
                            for(int i(0); i<n; i++)
                            {
                                hx[i] = p[0]*cos(g_2pi*i/n + p[1]);
                            }
                        },
                        [](double *p, double *jx, int m, int n, void *_levmarParams){
                            for(int i(0); i<n; i++)
                            {
                                jx[i*2+0] = cos(g_2pi*i/n + p[1]);
                                jx[i*2+1] = -p[0]*sin(g_2pi*i/n + p[1]);
                            }
                        },
                        &p[0],
                        &ys[0],
                        2,
                        ys.size(),
                        50,
                        levmarOpts,
                        levmarInfo,
                        &work[0],
                        NULL,
                        NULL);

//            std::cerr<<"result: "<<levmarResult<<std::endl;
//            std::cerr<<"||e||_2 at initial p.:"<<levmarInfo[0]<<std::endl;
//            std::cerr<<"||e||_2:"<<levmarInfo[1]<<std::endl;
//            std::cerr<<"||J^T e||_inf:"<<levmarInfo[2]<<std::endl;
//            std::cerr<<"||Dp||_2:"<<levmarInfo[3]<<std::endl;
//            std::cerr<<"\\mu/max[J^T J]_ii:"<<levmarInfo[4]<<std::endl;
//            std::cerr<<"# iterations:"<<levmarInfo[5]<<std::endl;
//            std::cerr<<"reason for terminating:";
//            switch(int(levmarInfo[6]+0.5))
//            {
//            case 1: std::cerr<<" - stopped by small gradient J^T e"<<std::endl;break;
//            case 2: std::cerr<<" - stopped by small Dp"<<std::endl;break;
//            case 3: std::cerr<<" - stopped by itmax"<<std::endl;break;
//            case 4: std::cerr<<" - singular matrix. Restart from current p with increased \\mu"<<std::endl;break;
//            case 5: std::cerr<<" - no further error reduction is possible. Restart with increased mu"<<std::endl;break;
//            case 6: std::cerr<<" - stopped by small ||e||_2"<<std::endl;break;
//            case 7: std::cerr<<" - stopped by invalid (i.e. NaN or Inf) \"func\" values; a user error"<<std::endl;break;
//            }
//            std::cerr<<"# function evaluations:"<<levmarInfo[7]<<std::endl;
//            std::cerr<<"# Jacobian evaluations:"<<levmarInfo[8]<<std::endl;
//            std::cerr<<"# linear systems solved:"<<levmarInfo[9]<<std::endl;
//            exit(1);

            return complex(p[0],p[1],CAP);
        }
    }

    void KernelTabled::setup(real pow, real periodSmallMult, real periodBigMult, std::size_t periodSteps)
    {
//        if(!load())
//        {
//            build();
//            save();
//        }

        Convolver c(pow, periodSmallMult, periodBigMult, periodSteps);

        real targetX = periodBigMult*pow;
        const real sampleStep = periodSmallMult/10;//1000 сэмплов на минимальный период
        TVReal signal(targetX/sampleStep+1000);

        const std::size_t phasesAmount = 30;

        std::vector<TVComplex> echos(phasesAmount);
        for(std::size_t phaseIndex(0); phaseIndex<phasesAmount; ++phaseIndex)
        {
            //fill signal
            //std::cout<<phaseIndex<<" signal"<<std::endl;
            for(std::size_t sindex(0); sindex<signal.size(); sindex++)
            {
                signal[sindex] = cos(g_2pi*sampleStep*sindex + phaseIndex*g_2pi/phasesAmount);
            }

            //std::cout<<phaseIndex<<" echo"<<std::endl;
            //fill echo
            c.execute(0, sampleStep, signal, targetX, echos[phaseIndex]);

            //std::cout<<echos[phaseIndex][10].re()<<", "<<echos[phaseIndex][10].im()<<std::endl;
        }

//        //проверить отклонение
//        for(std::size_t i(0); i<periodSteps; ++i)
//        {
//            real re=0, im=0;
//            for(std::size_t phaseIndex(0); phaseIndex<phasesAmount; ++phaseIndex)
//            {
//                re += echos[phaseIndex][i].re();
//                im += echos[phaseIndex][i].im();
//            }
//            std::cout<<re<<", "<<im<<std::endl;
//        }


        _kre.resize(periodSteps);
        _kim.resize(periodSteps);

        TVReal hre(phasesAmount), him(phasesAmount);
        for(std::size_t periodIndex(0); periodIndex<periodSteps; ++periodIndex)
        {
            for(std::size_t phaseIndex(0); phaseIndex<phasesAmount; ++phaseIndex)
            {
                hre[phaseIndex] = echos[phaseIndex][periodIndex].re();
                him[phaseIndex] = echos[phaseIndex][periodIndex].im();
            }

            _kre[periodIndex] = approxCos(hre);
            _kim[periodIndex] = approxCos(him);
        }

        _kdre.resize(periodSteps);
        _kdim.resize(periodSteps);

        _kdre[0] = (_kre[1] - complex())/2;
        _kdim[0] = (_kim[1] - complex())/2;

        for(std::size_t periodIndex(1); periodIndex<periodSteps-1; ++periodIndex)
        {
            _kdre[periodIndex] = (_kre[periodIndex+1] - _kre[periodIndex-1])/2;
            _kdim[periodIndex] = (_kim[periodIndex+1] - _kim[periodIndex-1])/2;
        }

        _kdre[periodSteps-1] = (complex() - _kre[periodSteps-2])/2;
        _kdim[periodSteps-1] = (complex() - _kim[periodSteps-2])/2;

        assert(0);
    }
}
