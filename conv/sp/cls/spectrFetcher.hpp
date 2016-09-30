#pragma once
#include "sp/complex.hpp"

#include <fstream>
#include <vector>

namespace sp { namespace cls
{

    template <class real>
    class SpectrFetcher
    {
    public:
        SpectrFetcher(const std::string &inDir);
        ~SpectrFetcher();

        const std::vector<real> &periodGrid();

        bool fetchRect(complex_tmpl<real> *rect,
                       std::size_t frame0, std::size_t framesAmount, std::size_t framesStride,
                       std::size_t period0, std::size_t periodsAmount, std::size_t periodsStride);

    private:
        bool fetchRectColumn(complex_tmpl<real> *col, std::size_t frame, std::size_t period0, std::size_t periodsAmount, std::size_t periodsStride);

        bool prefetch(std::size_t frame);

        bool loadNextLine();

        void pushFrame(const char *linebufRe, std::size_t linebufReSize, const char *linebufIm, std::size_t linebufImSize, std::size_t frame);

    private:
        std::vector<real> _periodGrid;
        std::vector<std::pair<std::size_t, std::size_t>> _lineIndexRe;
        std::vector<std::pair<std::size_t, std::size_t>> _lineIndexIm;

        std::size_t _wantPrefetchedFrames = 200;
        std::size_t _prefetchedFrame0 = 0;
        std::vector<std::vector<complex_tmpl<real>>> _prefetchedFrames;

        std::ifstream _re;
        std::ifstream _im;
    };
}}
