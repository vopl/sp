#pragma once
#include "sp/math.hpp"

#include <fstream>

namespace sp { namespace utils
{
    class SpectrDumper
    {
    public:
        SpectrDumper(const std::string &fName, const TVReal &periodGrid);
        ~SpectrDumper();

        std::size_t framesPushed() const;
        bool pushFrames(const TVComplex &frame);

    private:
        TVReal _periodGrid;
        std::fstream _re;
        std::fstream _im;
        std::size_t _framesPushed = 0;
    };
}}
