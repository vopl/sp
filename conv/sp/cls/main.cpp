#include "sp/cls/spectrFetcher.hpp"
#include "sp/cls/som.hpp"

#include <memory>

using namespace sp;
using namespace sp::cls;


int main(int argc, char *argv[])
{
    SpectrFetcher sf("/home/vopl/work/tmp/sp_run/med_100_4/out");

    SOM som;

    std::size_t shapeCols = 20;
    std::size_t shapeRows = 10;

    som.init(shapeCols, shapeRows, 100);

    std::size_t periodsAmount = sf.periodGrid().size();

    if(periodsAmount > sf.periodGrid().size())
    {
        std::cout<<"bad input"<<std::endl;
    }

    for(std::size_t iter(0); iter<10; ++iter)
    {

        std::size_t frame = 0;

        bool frameOk = true;
        while(frameOk)
        {
            for(std::size_t periodOffset(0); periodOffset<periodsAmount-shapeRows; ++periodOffset)
            {
                ShapePtr shape = std::make_shared<Shape>();

                if(!sf.fetchRect(shape->_values, frame, shapeCols, periodOffset, shapeRows))
                {
                    frameOk = false;
                    break;
                }

                //выровнять амплитуду на 1 и фазу на 0
                complex middle;
                for(const complex &v : shape->_values)
                {
                    middle += v;
                }
                middle /= shape->_values.size();

                if(middle.a() > 0)
                {
                    for(complex &v : shape->_values)
                    {
                        v /= middle;
                    }

                    som.push4Learn(shape);
                }
            }

            frame++;

            if(!(frame%30))
            {
                std::cout<<frame<<std::endl;
                som.fixLearn(0.0001);
            }
        }
    }

    return 0;
}
