#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include "sp/math.hpp"
#include "sp/kernelTabled.hpp"
#include "sp/periodGrid.hpp"
#include "sp/signalConvolver.hpp"

#include "sp/loadWav.hpp"
#include "sp/spectrStore.hpp"

using namespace std;
using namespace sp;





//hann, hann2



    /*
     * параметры ядра
     * pow
     * splp - samples per leveled period = 800
     * sspls - signal samples per leveled sample = 800
     *
     * сетка периода отклика
     * efmin, efmax, efcount, efscaletype
     *
     * сетка периода спектра
     * sfmin, sfmax, sfcount, sfscaletype
     *
     *
     * входной файл (wav моно, отфильтрованый, в высоком разрешении)
     * in
     *
     * выходной файл с откликом (поток вещественных блоков)
     * eout
     *
     * выходной файл со спектром (поток вещественных блоков)
     * sout
     */







#define POW 10.0



int main(int argc, char *argv[])
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string wavFName = "/home/vopl/tmp/13/start.wav";
    TVReal samples;
    uint32_t sps;

    if(!loadWav(wavFName, samples, sps))
    {
        cerr<<"unable to load wav file"<<endl;
        return -1;
    }

    cout<<"wav loaded: "<<samples.size()<<" samples at "<<sps<<"Hz ("<<sp::real(samples.size())/sps<<" sec)"<<endl;


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PeriodGrid echoPeriods(1.0/20000, 1.0/10, 1000, PeriodGridType::frequencyLog);
    cout<<"efmin: "<<(1.0/echoPeriods.grid().back())<<", efmax: "<<(1.0/echoPeriods.grid().front())<<", efcount: "<<echoPeriods.grid().size()<<endl;

    PeriodGrid spectrPeriods(echoPeriods.grid()[0], echoPeriods.grid()[798], 400, PeriodGridType::frequencyLog);
    cout<<"sfmin: "<<1.0/spectrPeriods.grid().back()<<", sfmax: "<<1.0/spectrPeriods.grid().front()<<", sfcount: "<<spectrPeriods.grid().size()<<endl;


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SpectrStore echoStore1(wavFName+".echo1", echoPeriods.grid());
    SpectrStore spectrStore(wavFName+".spectr", spectrPeriods.grid());

    if(
       echoStore1.framesPushed() == std::size_t(-1) ||
       echoStore1.framesPushed() != spectrStore.framesPushed())
    {
        cerr<<"spectr stores are inconsistent: "<<echoStore1.framesPushed()<<" vs "<<spectrStore.framesPushed()<<endl;
        return -1;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sp::real framesPerSecond = 100;
    const size_t samplesPerFrame = size_t(sps/framesPerSecond);
    cout<<"samplesPerFrame: "<<samplesPerFrame<<endl;

    framesPerSecond = sp::real(samplesPerFrame)/sps;
    cout<<"framesPerSecond: "<<framesPerSecond<<endl;

    const size_t framesAmount = (samples.size()-2)/samplesPerFrame;//2 extra samples for poly signal approximator
    cout<<"framesAmount: "<<framesAmount<<endl;

    size_t frameIndex = echoStore1.framesPushed();


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolver convolver;
    convolver.setup(10, echoPeriods, sp::real(1)/sps, 200, SignalApproxType::poly6p5o32x);

    convolver.pushSignal(&samples[0], 2);//2 extra samples for poly signal approximator
    size_t sampleIndex = 2;

    if(frameIndex)
    {
        convolver.pushSignal(&samples[sampleIndex], samplesPerFrame*frameIndex);
        sampleIndex += samplesPerFrame*frameIndex;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    KernelTabled k(10);


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVComplex echo1, echo2;
    TVComplex spectr(spectrPeriods.grid().size());

    std::vector<double> kwork;
    for(; frameIndex<framesAmount; ++frameIndex)
    {
        cout<<"frame "<<frameIndex<<"/"<<framesAmount<<" ("<<sp::real(sampleIndex-2)/sps<<" sec, "<<sp::real(frameIndex*100)/framesAmount<<"%) ";
        cout.flush();

        convolver.pushSignal(&samples[sampleIndex], samplesPerFrame);
        sampleIndex += samplesPerFrame;

        cout<<"c";
        cout.flush();

        echo1 = convolver.convolve();

        cout<<"d";
        cout.flush();

        std::fill(spectr.begin(), spectr.end(), sp::real(0));

        std::size_t iters = 6;
        sp::real error = 0;
        k.deconvolve(
            echo1.size(), &echoPeriods.grid()[0], &echo1[0],
            spectr.size(), &spectrPeriods.grid()[0], &spectr[0],
            iters,
            error,
            kwork);
        cout<<" iters: "<<iters<<", error: "<<error<<endl;

        echoStore1.pushFrames(echo1);
        spectrStore.pushFrames(spectr);
    }


    return 0;
}
