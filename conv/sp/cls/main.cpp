#include "sp/utils/spectrStore.hpp"

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

    using Shape = sp::cls::Shape<5, 5, long double>;

    std::vector<utils::SpectrStore*> sStores;
    std::vector<std::size_t> frameCounters;

    sStores.push_back(new utils::SpectrStore("/home/vopl/projects/sp/conv/bin/out/spectr", false));

    frameCounters.resize(sStores.size());

    //load("frameCounters", frameCounters);



    PatternExtractor<Shape> pe1(0, false);
//    PatternExtractor<Shape> pe2(150*150, false);

    //pe1.load("pe1");
    //pe2.load("pe2");

//    {
//        pe2.load("pe2.10x10");
//        pe2.pca2();
//        pe2.save("pe2.pca");
//        return 0;
//    }

    //pe1.save("pe1");
    //pe2.save("pe2");


    std::size_t periodsAmount = sStores[0]->header()._periods.size();

    for(;;)
    {
        std::size_t pushed1 = 0;

        for(std::size_t idx(0); idx<sStores.size(); ++idx)
        {
            std::size_t &frameCounter = frameCounters[idx];
            auto &sStore = *sStores[idx];

            for(std::size_t periodOffset(0); periodOffset<periodsAmount-Shape::rows; ++periodOffset)
            {
                Shape shape;

                if(!sStore.readRect(shape.data(), frameCounter, Shape::cols, 1, periodOffset, Shape::rows, 1))
                {
                    frameCounter = 0;
                    break;
                }

                //std::cerr<<frameCounter<<", "<<periodOffset<<std::endl;

                //shape1->smooth();
                pushed1 = pe1.pushBasis(shape);
            }
        }


        std::cout<<frameCounters[0]<<", pe1: "<<pushed1<<std::endl;

        if(pushed1 > 1000*1000)
        {
            pe1.pca2();
            //pe1.fixLearn(1e10);
            //pe1.mergeSames(0.005);
            exit(0);
            pe1.save("pe1");
            save("frameCounters", frameCounters);

//            std::vector<Shape> learnedShapes1(2000);
//            pe1.export_(learnedShapes1);
//            //learnedShapes1.resize(learnedShapes1.size()/2);

//            std::size_t pushed2 = 0;
//            for(Shape &shape : learnedShapes1)
//            {
//                pushed2 = pe2.push4Learn(Shape::Ptr(new Shape(shape)));

//                //shape.centrate();
//                //pushed2 = pe2.push4Learn(Shape::Ptr(new Shape(shape)));
//            }

            pushed1 = 0;

//            if(pushed2 > 1000*1000)
//            {
//                pushed2 = 0;
//                pe2.fixLearn(1e6);
//                //pe2.mergeSames(0.1);
//                pe2.replaceNulls();
//                //pe2.test();
//                pe2.save("pe2");
//                //pe1.save("pe1");

//                save("frameCounters", frameCounters);
//            }
        }

        for(std::size_t idx(0); idx<sStores.size(); ++idx)
        {
            frameCounters[idx] += 20;
        }
    }

    return 0;
}
