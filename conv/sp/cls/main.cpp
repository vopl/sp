#include "sp/cls/spectrFetcher.hpp"
#include "sp/cls/som.hpp"

#include "sp/cls/shape.hpp"
#include "sp/cls/patternExtractor.hpp"

#include <memory>

using namespace sp;
using namespace sp::cls;


int main(int argc, char *argv[])
{
    using Shape = sp::cls::Shape<8, 12, float>;

    SpectrFetcher<Shape::real> sf1("/home/vopl/work/tmp/sp_run/med_100_4/out");
    SpectrFetcher<Shape::real> sf2("/home/vopl/work/tmp/sp_run/pod_100_4/out");


    PatternExtractor<Shape> pe(10*10);

    std::size_t periodsAmount = sf1.periodGrid().size();

    if(periodsAmount != sf2.periodGrid().size())
    {
        std::cout<<"bad input"<<std::endl;
    }

    std::size_t frame1 = 10;
    std::size_t frame2 = 10;

    for(;;)
    {

        for(std::size_t periodOffset(0); periodOffset<periodsAmount-Shape::rows; ++periodOffset)
        {
            Shape::Ptr shape1(new Shape);

            if(!sf1.fetchRect(shape1->data(), frame1, Shape::cols, periodOffset, Shape::rows))
            {
                frame1 = 0;
                sf1.fetchRect(shape1->data(), frame1, Shape::cols, periodOffset, Shape::rows);
            }
            pe.push4Learn(std::move(shape1));

            Shape::Ptr shape2(new Shape);

            if(!sf2.fetchRect(shape2->data(), frame2, Shape::cols, periodOffset, Shape::rows))
            {
                frame2 = 0;
                sf2.fetchRect(shape2->data(), frame2, Shape::cols, periodOffset, Shape::rows);
            }
            pe.push4Learn(std::move(shape2));

        }

        frame1++;
        frame2++;

        if(!(frame1%30))
        {
            std::cout<<frame1<<", "<<frame2<<std::endl;
            pe.fixLearn(0.0001);
        }
    }

    return 0;
}

/*
int main_old(int argc, char *argv[])
{
    SpectrFetcher sf1("/home/vopl/work/tmp/sp_run/med_100_4/out");
    SpectrFetcher sf2("/home/vopl/work/tmp/sp_run/pod_100_4/out");

    SOM som;

    std::size_t shapeCols = 50;
    std::size_t shapeRows = 50;

    som.init(shapeCols, shapeRows, 10*10);

    std::size_t periodsAmount = sf1.periodGrid().size();

    if(periodsAmount > sf1.periodGrid().size())
    {
        std::cout<<"bad input"<<std::endl;
    }

    std::size_t frame1 = 0;
    std::size_t frame2 = 0;

    for(;;)
    {

        for(std::size_t periodOffset(0); periodOffset<periodsAmount-shapeRows; ++periodOffset)
        {
            ShapeOldPtr shape1 = std::make_shared<ShapeOld>();

            if(!sf1.fetchRect(shape1->_values, frame1, shapeCols, periodOffset, shapeRows))
            {
                frame1 = 0;
                sf1.fetchRect(shape1->_values, frame1, shapeCols, periodOffset, shapeRows);
            }
            som.push4Learn(shape1);

            ShapeOldPtr shape2 = std::make_shared<ShapeOld>();

            if(!sf2.fetchRect(shape2->_values, frame2, shapeCols, periodOffset, shapeRows))
            {
                frame2 = 0;
                sf2.fetchRect(shape2->_values, frame2, shapeCols, periodOffset, shapeRows);
            }
            som.push4Learn(shape2);

        }

        frame1++;
        frame2++;

        if(!(frame1%30))
        {
            std::cout<<frame1<<", "<<frame2<<std::endl;
            som.fixLearn(0.0001);
        }
    }

    return 0;
}
*/
