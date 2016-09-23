#include "sp/cls/spectrFetcher.hpp"

#include "sp/cls/shape.hpp"
#include "sp/cls/patternExtractor.hpp"

#include <memory>
#include <sys/resource.h>

using namespace sp;
using namespace sp::cls;

namespace
{
    template <class Value>
    void save(const std::string &fname, const Value &v)
    {
        std::ofstream ofs(fname+".tmp");
        if(!ofs)
        {
            std::cerr<<"unable to create file "<<fname+".tmp";
            return;
        }

        boost::archive::text_oarchive oa(ofs);

        oa << SER_NVP(v, v);

        ofs.close();

        rename((fname+".tmp").c_str(), (fname).c_str());

        std::cout<<"saved "<<fname<<std::endl;
    }

    template <class Value>
    void load(const std::string &fname, Value &v)
    {
        std::ifstream ifs(fname);
        if(!ifs)
        {
            std::cerr<<"unable to open file "<<fname<<std::endl;
            return;
        }

        try
        {
            boost::archive::text_iarchive ia(ifs);
            ia >> SER_NVP(v, v);
        }
        catch(std::exception &e)
        {
            std::cerr<<"load failed: "<<e.what()<<std::endl;
            return;
        }

        std::cout<<"loaded "<<fname<<std::endl;
    }

}


int main(int argc, char *argv[])
{
    {
        const rlim_t kStackSize = 16 * 1024 * 1024;   // min stack size = 16 MB
            struct rlimit rl;
            int result;

            result = getrlimit(RLIMIT_STACK, &rl);
            if (result == 0)
            {
                if (rl.rlim_cur < kStackSize)
                {
                    rl.rlim_cur = kStackSize;
                    result = setrlimit(RLIMIT_STACK, &rl);
                    if (result != 0)
                    {
                        fprintf(stderr, "setrlimit returned result = %d\n", result);
                    }
                }
            }
    }

    using Shape = sp::cls::Shape<5, 5, float>;

    std::vector<SpectrFetcher<Shape::real>*> sfs;
    std::vector<std::size_t> frameCounters;

    sfs.push_back(new SpectrFetcher<Shape::real>("/home/vopl/work/tmp/sp_run/med_100_4/out"));
    sfs.push_back(new SpectrFetcher<Shape::real>("/home/vopl/work/tmp/sp_run/med_100_6/out"));

    sfs.push_back(new SpectrFetcher<Shape::real>("/home/vopl/work/tmp/sp_run/pod_100_4/out"));
    sfs.push_back(new SpectrFetcher<Shape::real>("/home/vopl/work/tmp/sp_run/pod_100_8/out"));
    sfs.push_back(new SpectrFetcher<Shape::real>("/home/vopl/work/tmp/sp_run/pod_100_12/out"));

    frameCounters.resize(sfs.size());

    load("frameCounters", frameCounters);



    PatternExtractor<Shape> pe1(10*10, false);
    PatternExtractor<Shape> pe2(100*100, false);

    pe1.load("pe1");
    pe2.load("pe2");

    pe1.save("pe1");
    pe2.save("pe2");


    std::size_t periodsAmount = sfs[0]->periodGrid().size();

    for(;;)
    {
        std::size_t pushed1 = 0;

        for(std::size_t periodOffset(0); periodOffset<periodsAmount-Shape::rows; ++periodOffset)
        {
            for(std::size_t idx(0); idx<sfs.size(); ++idx)
            {
                std::size_t &frameCounter = frameCounters[idx];
                auto &sf = *sfs[idx];
                Shape::Ptr shape(new Shape);

                if(!sf.fetchRect(shape->data(), frameCounter, Shape::cols, periodOffset, Shape::rows))
                {
                    frameCounter = 0;
                    sf.fetchRect(shape->data(), frameCounter, Shape::cols, periodOffset, Shape::rows);
                }

                //shape1->smooth();
                pushed1 = pe1.push4Learn(std::move(shape));
            }
        }

        if(pushed1 > 10000)
        {
            pe1.fixLearn(1000000.0);
            //pe1.mergeSames(0.005);
            //pe1.save("pe1");

            std::vector<Shape> learnedShapes1(2000);
            pe1.export_(learnedShapes1);
            //learnedShapes1.resize(learnedShapes1.size()/2);

            std::size_t pushed2 = 0;
            for(Shape &shape : learnedShapes1)
            {
                pushed2 = pe2.push4Learn(Shape::Ptr(new Shape(shape)));

                //shape.centrate();
                //pushed2 = pe2.push4Learn(Shape::Ptr(new Shape(shape)));
            }

            std::cout<<frameCounters[0]<<", pe1: "<<pushed1<<", pe2: "<<pushed2<<std::endl;
            pushed1 = 0;

            if(pushed2 > 1000*1000)
            {
                pushed2 = 0;
                pe2.fixLearn(0.1);
                pe2.mergeSames(0.1);
                pe2.replaceNulls();
                pe2.save("pe2");
                //pe1.save("pe1");

                save("frameCounters", frameCounters);
            }
        }

        for(std::size_t idx(0); idx<sfs.size(); ++idx)
        {
            frameCounters[idx]++;
        }
    }

    return 0;
}
