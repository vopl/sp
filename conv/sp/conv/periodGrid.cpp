#include "periodGrid.hpp"
#include <cassert>

namespace sp { namespace conv
{
    PeriodGrid::PeriodGrid(real periodMin, real periodMax, std::size_t steps, PeriodGridType type)
    {
        _grid.resize(steps);

        switch(type)
        {
        case PeriodGridType::periodLin:
            {
                real min = periodMin;
                real max = periodMax;
                real step = (max - min) / (steps);

                for(std::size_t k(0); k<steps; k++)
                {
                    _grid[k] = min + k*step;
                }
            }
            break;
        case PeriodGridType::frequencyLin:
            {
                real min = 1.0/periodMax;
                real max = 1.0/periodMin;
                real step = (max - min) / (steps);

                for(std::size_t k(0); k<steps; k++)
                {
                    _grid[steps-1-k] = 1.0/(min + k*step);
                }
            }
            break;
        case PeriodGridType::periodLog:
            {
                real min = log(periodMin);
                real max = log(periodMax);
                real step = (max - min) / (steps);

                for(std::size_t k(0); k<steps; k++)
                {
                    _grid[k] = exp(min + k*step);
                }
            }
            break;
        case PeriodGridType::frequencyLog:
            {
                real min = -log(periodMax);
                real max = -log(periodMin);
                real step = (max - min) / (steps);

                for(std::size_t k(0); k<steps; k++)
                {
                    _grid[steps-1-k] = exp(-(min + k*step));
                }
            }
            break;
        }
    }

    PeriodGrid::~PeriodGrid()
    {

    }

    const TVReal &PeriodGrid::grid() const
    {
        return _grid;
    }

}}
