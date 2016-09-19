#pragma once
#include "sp/math.hpp"

#include <fstream>

namespace sp
{
    class SpectrStore
    {
    public:
        SpectrStore(const std::string &fName, const TVReal &periodGrid);
        ~SpectrStore();

        std::size_t framesPushed() const;
        bool pushFrames(const TVComplex &frame);

    private:
        TVReal _periodGrid;
        std::fstream _re;
        std::fstream _im;
        std::size_t _framesPushed = 0;
    };
}
