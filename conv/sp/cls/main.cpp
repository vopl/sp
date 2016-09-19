#include "sp/cls/spectrFetcher.hpp"
#include "sp/cls/som.hpp"

#include <memory>

using namespace sp;
using namespace sp::cls;


int main(int argc, char *argv[])
{
    SpectrFetcher sf("/home/vopl/work/tmp/sp_run/med_100_4/out");

    SOM som;

    std::size_t shapeCols = 8;
    std::size_t shapeRows = 12;

    som.init(shapeCols, shapeRows, 24);

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

                som.push4Learn(shape);
            }

            frame++;

            if(!(frame%10))
            {
                som.fixLearn(0.001);
            }
        }
    }

    return 0;
}
