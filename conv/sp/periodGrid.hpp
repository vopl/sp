#pragma once

#include "types.hpp"

namespace sp
{
    enum class PeriodGridType
    {
        periodLin,
        frequencyLin,
        periodLog,
        frequencyLog,
    };

    class PeriodGrid
    {
    public:
        PeriodGrid(real periodMin, real periodMax, std::size_t steps, PeriodGridType type);
        ~PeriodGrid();

        const TVReal &grid();

    private:
        TVReal _grid;
    };
}
