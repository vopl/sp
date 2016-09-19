#pragma once
#include "sp/math.hpp"

#include <fstream>

namespace sp { namespace cls
{

    class SpectrFetcher
    {
    public:
        SpectrFetcher(const std::string &inDir);
        ~SpectrFetcher();

        const TVReal &periodGrid();

        bool fetchRect(TVComplex &rect,
                       std::size_t frame0, std::size_t framesAmount,
                       std::size_t period0, std::size_t periodsAmount);

    private:
        bool fetchRectColumn(complex *col, std::size_t frame, std::size_t period0, std::size_t periodsAmount);
        bool prefetch(std::size_t frame);
        bool loadNextLine();
        void pushFrame(const char *linebufRe, std::size_t linebufReSize, const char *linebufIm, std::size_t linebufImSize, std::size_t frame);

    private:
        TVReal _periodGrid;
        std::vector<std::pair<std::size_t, std::size_t>> _lineIndexRe;
        std::vector<std::pair<std::size_t, std::size_t>> _lineIndexIm;

        std::size_t _wantPrefetchedFrames = 200;
        std::size_t _prefetchedFrame0 = 0;
        std::vector<TVComplex> _prefetchedFrames;

        std::ifstream _re;
        std::ifstream _im;
    };
}}
