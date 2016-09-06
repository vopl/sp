#include <iostream>
#include <fstream>
#include <chrono>
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
     * ppw
     * splp - samples per leveled period = 800
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

void prony();
void test();

int main(int argc, char *argv[])
{
//    prony();
//    test();
//    return 0;


//    {
//        std::cerr.precision(20);
//        std::cerr.setf(std::ios::scientific);

//        std::cout.precision(20);
//        std::cout.setf(std::ios::scientific);

//        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
//        KernelTabled k(10, 100, 100, 11);

//        k.eval(1.1, 1, sp::complex(1.0));


//        exit(0);
//    }








    ////////////////////////////////////////////////////////////////////////////////
    po::options_description desc("conv");
    desc.add_options()
            ("help", "produce help message")

            ("ppw", po::value<sp::real>()->default_value(10), "periods per analyze window")

            ("splp", po::value<std::size_t>()->default_value(200), "samples per level period")

            ("cpo", po::value<std::size_t>()->default_value(11), "convolver polynome order")

            ("efmin", po::value<sp::real>()->default_value(5), "echo frequency grid minimum")
            ("efmax", po::value<sp::real>()->default_value(20000), "echo frequency grid maximum")
            ("efcount", po::value<std::size_t>()->default_value(1000), "echo frequency grid size")
            ("eftype", po::value<std::string>()->default_value("flog"), "echo frequency grid type (plin|plog|flin|flog)")

            ("sfminmult", po::value<sp::real>()->default_value(7.5), "spectr frequency minimum value part")
            ("sfmaxmult", po::value<sp::real>()->default_value(0.99), "spectr frequency maximum value part")
            ("sfcountmult", po::value<std::size_t>()->default_value(1), "spectr frequency count mult")

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

    PeriodGridType pgt = PeriodGridType::frequencyLog;

    if("plin" == vars["eftype"].as<std::string>())
    {
        pgt = PeriodGridType::periodLin;
    }
    else if("plog" == vars["eftype"].as<std::string>())
    {
        pgt = PeriodGridType::periodLog;
    }
    else if("flin" == vars["eftype"].as<std::string>())
    {
        pgt = PeriodGridType::frequencyLin;
    }
    else if("flog" == vars["eftype"].as<std::string>())
    {
        pgt = PeriodGridType::frequencyLog;
    }
    else
    {
        cerr<<"bad eftype: "<<vars["eftype"].as<std::string>()<<std::endl;
        return EXIT_FAILURE;
    }

    cout<<"pgt: "<<vars["eftype"].as<std::string>()<<endl;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PeriodGrid echoPeriodsGrid(
        sp::real(1)/vars["efmax"].as<sp::real>(),
        sp::real(1)/vars["efmin"].as<sp::real>(),
        vars["efcount"].as<std::size_t>(),
        pgt);

    TVReal echoPeriods = echoPeriodsGrid.grid();

    cout
        <<"efmin: "<<(sp::real(1)/echoPeriods.back())
        <<", efmax: "<<(sp::real(1)/echoPeriods.front())
        <<", efcount: "<<echoPeriods.size()
        <<", efstep: "<<echoPeriods[1]/echoPeriods[0]
        <<endl;

    TVReal spectrPeriods;
    {

        TVReal::const_iterator sfBegin = std::lower_bound(echoPeriods.begin(), echoPeriods.end(), echoPeriods.front()/vars["sfmaxmult"].as<sp::real>());
        TVReal::const_iterator sfEnd = std::lower_bound(echoPeriods.begin(), echoPeriods.end(), echoPeriods.back()/vars["sfminmult"].as<sp::real>());


        std::size_t sfCountMult = vars["sfcountmult"].as<std::size_t>();
        if(sfCountMult < 1)
        {
            cerr<<"bad sfcountmult: "<<sfCountMult<<std::endl;
            return EXIT_FAILURE;
        }


        spectrPeriods.clear();
        spectrPeriods.resize((sfEnd-sfBegin)/sfCountMult);
        auto sfIter = sfBegin;
        for(std::size_t idx(0); idx<spectrPeriods.size(); ++idx)
        {
            spectrPeriods[idx] = *sfIter;
            sfIter += std::ptrdiff_t(sfCountMult);
        }

    }

    cout
        <<"sfmin: "<<(sp::real(1)/spectrPeriods.back())
        <<", sfmax: "<<(sp::real(1)/spectrPeriods.front())
        <<", sfcount: "<<spectrPeriods.size()
        <<", sfstep: "<<spectrPeriods[1]/spectrPeriods[0]
        <<endl;

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

                for(std::size_t k(calibrate); k<=spectrPeriods.size()-calibrate; k+=calibrate)
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
    convolver.setup(
            vars["ppw"].as<sp::real>(),
            echoPeriods,
            sp::real(1)/sps,
            vars["splp"].as<std::size_t>(),
            vars["cpo"].as<std::size_t>(),
            SignalApproxType::poly6p5o32x);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    KernelTabled k(
            vars["ppw"].as<sp::real>(),
            vars["splp"].as<std::size_t>(),
            vars["cpo"].as<std::size_t>());


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVComplex echo;
    TVComplex spectr(spectrPeriods.size());

    size_t sampleIndex = 0;

    auto moment = std::chrono::high_resolution_clock::now();

    std::vector<double> kwork;
    for(; frameIndex<framesAmount; ++frameIndex)
    {
        //rotate spectr to new position
        {
            sp::real dx = 1/framesPerSecond;
            for(std::size_t i(0); i<spectr.size(); ++i)
            {
                sp::real t = spectrPeriods[i];
                sp::real dp = dx*sp::g_2pi/t;
                spectr[i] = spectr[i].rotate(dp);
            }
        }



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

        //std::fill(spectr.begin(), spectr.end(), sp::complex(0));


        std::size_t iters = 15;
        sp::real error0 = 0;
        sp::real error1 = 0;
        k.deconvolve(
            echo.size(), &echoPeriods[0], &echo[0],
            spectr.size(), &spectrPeriods[0], &spectr[0],
            iters,
            1e-40,
            error0,
            error1,
            kwork);

        auto moment1 = std::chrono::high_resolution_clock::now();

        sp::real dur = std::chrono::duration<sp::real>(moment1 - moment).count();
        cout<<"ok, iters: "<<iters<<", error: "<<error1<<"/"<<error0<<"="<<(error1/error0)<<", dur: "<<dur<< std::endl;
        moment = moment1;

        if(0)
        {
            iters = 15;
            error0 = 0;
            error1 = 0;
            k.deconvolve(
                echo.size(), &echoPeriods[0], &echo[0],
                spectr.size(), &spectrPeriods[0], &spectr[0],
                iters,
                1e-1,
                error0,
                error1,
                kwork);

            moment1 = std::chrono::high_resolution_clock::now();

            dur = std::chrono::duration<sp::real>(moment1 - moment).count();
            cout<<"ok2, iters: "<<iters<<", error: "<<error1<<"/"<<error0<<"="<<(error1/error0)<<", dur: "<<dur<< std::endl;
            moment = moment1;
        }

        echoStore.pushFrames(echo);
        spectrStore.pushFrames(spectr);
    }

    return EXIT_SUCCESS;
}





