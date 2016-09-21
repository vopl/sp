#include "sp/cls/spectrFetcher.hpp"

#include "sp/cls/shape.hpp"
#include "sp/cls/patternExtractor.hpp"

#include <memory>
#include <sys/resource.h>

using namespace sp;
using namespace sp::cls;


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

    using Shape = sp::cls::Shape<6, 6, float>;

    SpectrFetcher<Shape::real> sf1("/home/vopl/work/tmp/sp_run/med_100_4/out");
    SpectrFetcher<Shape::real> sf2("/home/vopl/work/tmp/sp_run/pod_100_4/out");


    PatternExtractor<Shape> pe1(20*20);
    PatternExtractor<Shape> pe2(50*50, false);

    pe1.load("pe1");
    pe2.load("pe2");

    pe1.save("pe1");
    pe2.save("pe2");


    std::size_t periodsAmount = sf1.periodGrid().size();

    if(periodsAmount != sf2.periodGrid().size())
    {
        std::cout<<"bad input"<<std::endl;
    }

    std::size_t frame1 = 0;
    std::size_t frame2 = 0;

    for(;;)
    {
        std::size_t pushed1 = 0;

        for(std::size_t periodOffset(0); periodOffset<periodsAmount-Shape::rows; ++periodOffset)
        {
            Shape::Ptr shape1(new Shape);

            if(!sf1.fetchRect(shape1->data(), frame1, Shape::cols, periodOffset, Shape::rows))
            {
                frame1 = 0;
                sf1.fetchRect(shape1->data(), frame1, Shape::cols, periodOffset, Shape::rows);
            }
            pe1.push4Learn(std::move(shape1));

            Shape::Ptr shape2(new Shape);

            if(!sf2.fetchRect(shape2->data(), frame2, Shape::cols, periodOffset, Shape::rows))
            {
                frame2 = 0;
                sf2.fetchRect(shape2->data(), frame2, Shape::cols, periodOffset, Shape::rows);
            }
            pushed1 = pe1.push4Learn(std::move(shape2));

            if(pushed1 > 10000)
            {
                pushed1 = 0;

                pe1.fixLearn(10.0);
                pe1.save("pe1");

                std::vector<Shape> learnedShapes1(10000);
                pe1.export_(learnedShapes1);
                learnedShapes1.resize(learnedShapes1.size()/10+1);

                std::size_t pushed2 = 0;
                for(Shape &shape : learnedShapes1)
                {
                    pushed2 = pe2.push4Learn(Shape::Ptr(new Shape(shape)));

                    //shape.centrate();
                    //pushed2 = pe2.push4Learn(Shape::Ptr(new Shape(shape)));
                }

                if(pushed2 > 10000)
                {
                    pushed2 = 0;
                    pe2.fixLearn(0.001);
                    pe2.mergeSames(0.2);

                    pe2.save("pe2");
                }

                std::cout<<frame1<<", "<<frame2<<", pe1: "<<pushed1<<", pe2: "<<pushed2<<std::endl;
            }

        }

        frame1++;
        frame2++;
    }

    return 0;
}
