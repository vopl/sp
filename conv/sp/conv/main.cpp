#include <iostream>
#include <fstream>
#include <chrono>
#include <csignal>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "sp/math.hpp"
#include "sp/conv/kernelTabled.hpp"
#include "sp/conv/periodGrid.hpp"
#include "sp/conv/signalConvolver.hpp"

//#include "sp/conv/loadWav.hpp"
#include "sp/utils/wavStore.hpp"
#include "sp/utils/spectrStore.hpp"
#include "sp/utils/spectrDumper.hpp"

//#include "sp/conv/spectrStore.hpp"

using namespace std;
using namespace sp;
using namespace sp::conv;









namespace po = boost::program_options;
namespace fs = boost::filesystem;


volatile std::size_t g_stop = 0;
volatile bool g_stopBlocked = false;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
void signal_handler(int)
{
    std::cerr<<"stop request "<<std::endl;
    std::cerr.flush();

    if(!g_stopBlocked)
    {
        exit(-4);
    }

    g_stop++;

    if(g_stop > 20)
    {
        std::cerr<<"force termination"<<std::endl;
        std::cerr.flush();
        exit(-5);
    }
}

void prony();
void test();
void inv();

int main(int argc, char *argv[])
{
//    prony();return 0;
//    test();return 0;
//    inv();return 0;



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



    std::signal(SIGTERM, signal_handler);
    std::signal(SIGQUIT, signal_handler);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGSTOP, signal_handler);





    ////////////////////////////////////////////////////////////////////////////////
    po::options_description desc("conv");
    desc.add_options()
            ("help", "produce help message")

            ("ppw", po::value<sp::real>()->default_value(0.125), "periods per analyze window")

            ("splp", po::value<std::size_t>()->default_value(2000), "samples per level period")

            ("efmin", po::value<sp::real>()->default_value(0.01), "echo frequency grid minimum")
            ("efmax", po::value<sp::real>()->default_value(20000), "echo frequency grid maximum")
            ("efcount", po::value<std::size_t>()->default_value(40000), "echo frequency grid size")
            ("eftype", po::value<std::string>()->default_value("flog"), "echo frequency grid type (plin|plog|flin|flog)")

            ("sfminmult", po::value<sp::real>()->default_value(2000), "spectr frequency minimum value part")
            ("sfmaxmult", po::value<sp::real>()->default_value(1), "spectr frequency maximum value part")
            ("sfcountmult", po::value<std::size_t>()->default_value(16), "spectr frequency count mult")

            ("fps", po::value<sp::real>()->default_value(1000), "frames per second")

            ("in-file", po::value<std::string>()->default_value("in.wav"), "input wav file name")

            ("out-file", po::value<std::string>()->default_value("out.spectr"), "output spectr file name")
            ("dump-prefix", po::value<std::string>()/*->default_value("dump/spectr")*/, "spectr text dump files prefix")

            ("inititersmax", po::value<std::size_t>()->default_value(15), "maximum initial iterations for lsq")
            ("itersmax", po::value<std::size_t>()->default_value(1), "maximum iterations for lsq")
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
#ifndef GIT_COMMIT
#   define GIT_COMMIT unknown
#endif
#define STR(x) STR2(x)
#define STR2(x) #x
        std::cout << "git: "<<STR(GIT_COMMIT) << std::endl;
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
    utils::WavStore wavStore;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    if(vars.count("in-file"))
    {
        if(!wavStore.open(vars["in-file"].as<std::string>().c_str()))
        {
            cerr<<"unable to read wav file"<<endl;
            return -1;
        }

        cout<<"input wav: "
           <<wavStore.header()._samplesAmount<<" samples at "
           <<wavStore.header()._frequency<<"Hz ("
          <<sp::real(wavStore.header()._samplesAmount)/wavStore.header()._frequency<<" sec)"<<endl;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sp::real framesPerSecond = vars["fps"].as<sp::real>();
    cout<<"fps: "<<framesPerSecond<<endl;

    const sp::real samplesPerFrame = wavStore.header()._frequency/framesPerSecond;
    cout<<"samplesPerFrame: "<<samplesPerFrame<<endl;

    static const std::size_t extraSamples4Push = 2;//2 extra samples for poly signal approximator

    const size_t framesAmount = wavStore.header()._samplesAmount > 1+extraSamples4Push ?
                                    std::size_t((wavStore.header()._samplesAmount-1-extraSamples4Push)/samplesPerFrame) :
                                    0;
    cout<<"framesAmount: "<<framesAmount<<endl;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string outFile = vars["out-file"].as<std::string>();
    utils::SpectrStore spectrStore(outFile.c_str(), true);
    if(spectrStore)
    {
        if(spectrStore.header()._periods != spectrPeriods)
        {
            cerr<<"spectrStore period grid mismatch (total): "<<outFile<<endl;
            //return EXIT_FAILURE;
        }
        if(spectrStore.header()._periods.size() != spectrPeriods.size())
        {
            cerr<<"spectrStore period grid mismatch (size): "<<outFile<<endl;
            return EXIT_FAILURE;
        }
        if(spectrStore.header()._samplesPerSecond != framesPerSecond)
        {
            cerr<<"spectrStore fps mismatch: "<<outFile<<endl;
            return EXIT_FAILURE;
        }
    }

    if(!spectrStore)
    {
        utils::SpectrStore::Header header;

        header._realBittness = sizeof(sp::real)*8;
        header._samplesPerSecond = framesPerSecond;
        header._periods = spectrPeriods;
        header._samplesAmount = 0;

        spectrStore.create(outFile.c_str(), header);

        if(!spectrStore)
        {
            cerr<<"unable to create spectrStore: "<<outFile<<endl;
            return EXIT_FAILURE;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::unique_ptr<utils::SpectrDumper> spectrDumper;
    if(vars.count("dump-prefix"))
    {
        spectrDumper.reset(new utils::SpectrDumper(vars["dump-prefix"].as<std::string>(), spectrPeriods));
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    size_t frameIndex = spectrStore.header()._samplesAmount;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolver convolver;
    convolver.setup(
            vars["ppw"].as<sp::real>(),
            echoPeriods,
            sp::real(1)/wavStore.header()._frequency,
            vars["splp"].as<std::size_t>(),
            SignalApproxType::linear);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    KernelTabled k(
            vars["ppw"].as<sp::real>(),
            vars["splp"].as<std::size_t>());


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVEchoPoint echo;
    TVSpectrPoint spectr(spectrPeriods.size());

    size_t sampleIndex = 0;

    auto moment = std::chrono::high_resolution_clock::now();

    bool first = true;
    for(; frameIndex<framesAmount; ++frameIndex)
    {
        std::size_t needSampleIndex = std::size_t(samplesPerFrame*(frameIndex+1))+extraSamples4Push;

        TVReal samples(needSampleIndex - sampleIndex);
        if(!wavStore.read(&samples[0], samples.size()))
        {
            std::cerr<<"unable to read wav file"<<std::endl;
            abort();
        }

        convolver.pushSignal(&samples[0], samples.size());
        sampleIndex = needSampleIndex;

        cout
                <<"frame "<<frameIndex<<"/"<<framesAmount
                <<" ("<<sp::real(frameIndex*100)/framesAmount<<"%, "
                <<sp::real(sampleIndex-extraSamples4Push)/wavStore.header()._frequency<<" sec) ";
        cout.flush();

        cout<<"c..";
        cout.flush();

        echo = convolver.convolve();

        cout<<"d..";
        cout.flush();



        std::size_t iters = vars["itersmax"].as<std::size_t>();
        if(first)
        {
            iters = vars["inititersmax"].as<std::size_t>();
            first = false;
        }

        sp::real error0 = 0;
        sp::real error1 = 0;
        k.deconvolve2(
            echo.size(), &echoPeriods[0], &echo[0],
            spectr.size(), &spectrPeriods[0], &spectr[0],
            iters,
            0,//1e-80,
            error0,
            error1);

        auto moment1 = std::chrono::high_resolution_clock::now();

        sp::real dur = std::chrono::duration<double>(moment1 - moment).count();
        //cout<<"ok, iters: "<<iters<<", error: "<<error1<<"/"<<error0<<"="<<(error1/error0)<<", dur: "<<dur<< std::endl;
        cout<<"ok: "<<dur<< std::endl;
        moment = moment1;

        g_stopBlocked = true;

        k.flush();

        if(!spectrStore.write(spectr.front().data()))
        {
            std::cerr<<"unable to write spectrStore"<<std::endl;
            break;
        }

        if(spectrDumper)
        {
            spectrDumper->pushFrames(spectr.front().data(), spectr.size()*SpectrPoint::SizeAtCompileTime);
        }

        g_stopBlocked = false;

        if(g_stop)
        {
            break;
        }
    }

    return EXIT_SUCCESS;
}





