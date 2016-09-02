#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

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





namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char *argv[])
{
    ////////////////////////////////////////////////////////////////////////////////
    po::options_description desc("conv");
    desc.add_options()
            ("help", "produce help message")

            ("pow", po::value<sp::real>()->default_value(10), "periods on analyze window")

            ("spls", po::value<std::size_t>()->default_value(10000), "samples per level sample")
            ("splp", po::value<std::size_t>()->default_value(400), "samples per level period")

            ("efmin", po::value<sp::real>()->default_value(5), "echo frequency grid minimum")
            ("efmax", po::value<sp::real>()->default_value(20000), "echo frequency grid maximum")
            ("efcount", po::value<std::size_t>()->default_value(1000), "echo frequency grid size")
            //("eftype", po::value<std::string>()->default_value("flog"), "echo frequency grid type (plin|plog|flin|flog)")

            ("sfminmult", po::value<sp::real>()->default_value(2.5), "spectr frequency minimum value part")
            ("sfcountmult", po::value<std::size_t>()->default_value(2), "spectr frequency count mult")

            ("fps", po::value<sp::real>()->default_value(100), "frames per second")

            ("in-file", po::value<std::string>()->default_value("in.wav"), "input wav file name")
            ("calibrate", po::value<std::size_t>()->default_value(0), "simulate calibration harminics, N-step in spectr grid")

            ("out-dir", po::value<std::string>()->default_value("out"), "output directory")
            ;

    ////////////////////////////////////////////////////////////////////////////////
    po::variables_map vars;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vars);
    }
    catch(std::exception &e)
    {
        std::cerr<<e.what();
        return EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr<<"unknown exception";
        return EXIT_FAILURE;
    }
    po::notify(vars);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    if(vars.count("help"))
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PeriodGrid echoPeriodsGrid(
        sp::real(1)/vars["efmax"].as<sp::real>(),
        sp::real(1)/vars["efmin"].as<sp::real>(),
        vars["efcount"].as<std::size_t>(),
        PeriodGridType::frequencyLog);

    TVReal echoPeriods = echoPeriodsGrid.grid();

    cout<<"efmin: "<<(sp::real(1)/echoPeriods.back())<<", efmax: "<<(sp::real(1)/echoPeriods.front())<<", efcount: "<<echoPeriods.size()<<endl;

    TVReal spectrPeriods;
    {
        sp::real sfMinMult = vars["sfminmult"].as<sp::real>();
        if(sfMinMult < 1)
        {
            cerr<<"bad sfminmult: "<<sfMinMult<<std::endl;
            return EXIT_FAILURE;
        }

        std::size_t sfCountMult = vars["sfcountmult"].as<std::size_t>();
        if(sfCountMult < 1)
        {
            cerr<<"bad sfcountmult: "<<sfCountMult<<std::endl;
            return EXIT_FAILURE;
        }

        auto iter = std::find_if(echoPeriods.rbegin(), echoPeriods.rend(), [&](sp::real t){return t<=echoPeriods.back()/sfMinMult;});
        if(iter==echoPeriods.rend())
        {
            cerr<<"bad sfminmult: "<<sfMinMult<<std::endl;
            return EXIT_FAILURE;
        }

        std::size_t echoPeriodsSize4Spectr = std::size_t(echoPeriods.rend() - iter);
        std::size_t spectrPeriodsSize = std::size_t(sp::real(echoPeriodsSize4Spectr)/sfCountMult+0.5);

        if(spectrPeriodsSize < 2 || spectrPeriodsSize>echoPeriods.size())
        {
            cerr<<"bad sfminmult sfcountmult combination, spectrSize: "<<spectrPeriodsSize<<std::endl;
            return EXIT_FAILURE;
        }

        spectrPeriods.resize(spectrPeriodsSize);
        for(std::size_t i(0); i<spectrPeriodsSize; i++)
        {
            spectrPeriods[i] = echoPeriods[std::size_t(i*sfCountMult)];
        }
    }

    cout<<"sfmin: "<<(sp::real(1)/spectrPeriods.back())<<", sfmax: "<<(sp::real(1)/spectrPeriods.front())<<", sfcount: "<<spectrPeriods.size()<<endl;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVReal samples;
    uint32_t sps=0;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    if(vars.count("in-file"))
    {
        if(!loadWav(vars["in-file"].as<std::string>(), samples, sps))
        {
            cerr<<"unable to load wav file"<<endl;
            return -1;
        }

        cout<<"wav loaded: "<<samples.size()<<" samples at "<<sps<<"Hz ("<<sp::real(samples.size())/sps<<" sec)"<<endl;
    }

    {
        std::size_t calibrate = vars["calibrate"].as<std::size_t>();
        if(calibrate)
        {
            cout<<"generate calibration signal...";
            cout.flush();

            sp::real sampleStep = sp::real(1)/sps;
            for(size_t index(0); index<samples.size(); ++index)
            {
                sp::real x = index * sampleStep;

                samples[index] = 0;

                for(std::size_t k(0); k<spectrPeriods.size(); k+=calibrate)
                {
                    sp::real t = spectrPeriods[k];
                    samples[index] += sp::sin(x*sp::g_2pi/t);
                }
            }

            cout<<"done"<<std::endl;
        }
    }

    boost::system::error_code ec;
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    fs::path outDir(vars["out-dir"].as<std::string>());
    if(!fs::is_directory(outDir, ec) && !fs::create_directories(outDir, ec))
    {
        cerr<<"unable to create output directory: "<<outDir<<endl;
        return EXIT_FAILURE;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SpectrStore echoStore((outDir/"echo").string(), echoPeriods);
    SpectrStore spectrStore((outDir/"spectr").string(), spectrPeriods);

    if(
       echoStore.framesPushed() == std::size_t(-1) ||
       echoStore.framesPushed() != spectrStore.framesPushed())
    {
        cerr<<"spectr stores are inconsistent: "<<echoStore.framesPushed()<<" vs "<<spectrStore.framesPushed()<<endl;
        return EXIT_FAILURE;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sp::real framesPerSecond = vars["fps"].as<sp::real>();
    cout<<"fps: "<<framesPerSecond<<endl;

    const sp::real samplesPerFrame = sps/framesPerSecond;
    cout<<"samplesPerFrame: "<<samplesPerFrame<<endl;

    static const std::size_t extraSamples4Push = 2;//2 extra samples for poly signal approximator

    const size_t framesAmount = samples.size() > 3 ?
                                    std::size_t((samples.size()-1-extraSamples4Push)/samplesPerFrame) :
                                    0;
    cout<<"framesAmount: "<<framesAmount<<endl;

    size_t frameIndex = echoStore.framesPushed();

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolver convolver;
    convolver.setup(vars["pow"].as<sp::real>(), echoPeriods, sp::real(1)/sps, vars["splp"].as<std::size_t>(), SignalApproxType::poly6p5o32x);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    KernelTabled k(vars["pow"].as<sp::real>(), vars["spls"].as<std::size_t>(), vars["splp"].as<std::size_t>());


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVComplex echo;
    TVComplex spectr(spectrPeriods.size());

    size_t sampleIndex = 0;

    std::vector<double> kwork;
    for(; frameIndex<framesAmount; ++frameIndex)
    {
        std::size_t needSampleIndex = std::size_t(samplesPerFrame*(frameIndex+1))+extraSamples4Push;
        convolver.pushSignal(&samples[sampleIndex], needSampleIndex - sampleIndex);
        sampleIndex = needSampleIndex;

        cout<<"frame "<<frameIndex<<"/"<<framesAmount<<" ("<<sp::real(frameIndex*100)/framesAmount<<"%, "<<sp::real(sampleIndex-extraSamples4Push)/sps<<" sec) ";
        cout.flush();

        cout<<"c..";
        cout.flush();

        echo = convolver.convolve();

        cout<<"d..";
        cout.flush();

        std::fill(spectr.begin(), spectr.end(), sp::real(0));

        std::size_t iters = 5;
        sp::real error = 0;
        k.deconvolve(
            echo.size(), &echoPeriods[0], &echo[0],
            spectr.size(), &spectrPeriods[0], &spectr[0],
            iters,
            error,
            kwork);
        cout<<"ok, iters: "<<iters<<", error: "<<error<<endl;

        echoStore.pushFrames(echo);
        spectrStore.pushFrames(spectr);
    }

    return EXIT_SUCCESS;
}

























int main_old(int argc, char *argv[])
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
    PeriodGrid echoPeriods(1.0L/20000, 1.0L/2, 1000, PeriodGridType::frequencyLog);
    cout<<"efmin: "<<(1.0/echoPeriods.grid().back())<<", efmax: "<<(1.0/echoPeriods.grid().front())<<", efcount: "<<echoPeriods.grid().size()<<endl;

    PeriodGrid spectrPeriods(echoPeriods.grid()[0], echoPeriods.grid()[699], 700, PeriodGridType::frequencyLog);
    cout<<"sfmin: "<<1.0/spectrPeriods.grid().back()<<", sfmax: "<<1.0/spectrPeriods.grid().front()<<", sfcount: "<<spectrPeriods.grid().size()<<endl;


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SpectrStore echoStore(wavFName+".echo", echoPeriods.grid());
    SpectrStore spectrStore(wavFName+".spectr", spectrPeriods.grid());

    if(
       echoStore.framesPushed() == std::size_t(-1) ||
       echoStore.framesPushed() != spectrStore.framesPushed())
    {
        cerr<<"spectr stores are inconsistent: "<<echoStore.framesPushed()<<" vs "<<spectrStore.framesPushed()<<endl;
        return -1;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sp::real framesPerSecond = 200;
    const size_t samplesPerFrame = size_t(sps/framesPerSecond);
    cout<<"samplesPerFrame: "<<samplesPerFrame<<endl;

    framesPerSecond = sp::real(sps)/samplesPerFrame;
    cout<<"framesPerSecond: "<<framesPerSecond<<endl;

    const size_t framesAmount = (samples.size()-2)/samplesPerFrame;//2 extra samples for poly signal approximator
    cout<<"framesAmount: "<<framesAmount<<endl;

    size_t frameIndex = echoStore.framesPushed() + 95;


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolver convolver;
    convolver.setup(10, echoPeriods.grid(), sp::real(1)/sps, 400, SignalApproxType::poly6p5o32x);

    convolver.pushSignal(&samples[0], 2);//2 extra samples for poly signal approximator
    size_t sampleIndex = 2;

    if(frameIndex)
    {
        convolver.pushSignal(&samples[sampleIndex], samplesPerFrame*frameIndex);
        sampleIndex += samplesPerFrame*frameIndex;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    KernelTabled k(10, 10000, 400);


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVComplex echo;
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

        echo = convolver.convolve();

        cout<<"d";
        cout.flush();

        std::fill(spectr.begin(), spectr.end(), sp::real(0));

        std::size_t iters = 5;
        sp::real error = 0;
        k.deconvolve(
            echo.size(), &echoPeriods.grid()[0], &echo[0],
            spectr.size(), &spectrPeriods.grid()[0], &spectr[0],
            iters,
            error,
            kwork);
        cout<<" iters: "<<iters<<", error: "<<error<<endl;

        echoStore.pushFrames(echo);
        spectrStore.pushFrames(spectr);
    }


    return 0;
}
