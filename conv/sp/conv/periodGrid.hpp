#pragma once

#include "sp/math.hpp"

namespace sp { namespace conv
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
        PeriodGrid(real periodMin, real periodMax, std::size_t steps, PeriodGridType type, bool exactMax = false);
        ~PeriodGrid();

        const TVReal &grid() const;

    private:
        TVReal _grid;
    };
}}
