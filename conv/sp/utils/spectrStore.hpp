#pragma once
#include <cstdint>
#include <fstream>
#include "sp/math.hpp"

namespace sp { namespace utils
{
    class SpectrStore
    {
    public:
        struct Header
        {
            std::uint64_t   _realBittness = 0;
            real            _samplesPerSecond = 0;
            TVReal          _periods;
            std::uint64_t   _samplesAmount = 0;
        };

        using Sample = real *;

    public:
        SpectrStore();
        SpectrStore(const char *fName, bool app);//open
        SpectrStore(const char *fName, const Header &header);//create
        ~SpectrStore();

        bool good() const;
        operator bool() const;

        bool open(const char *fName, bool app);
        bool create(const char *fName, const Header &header);

        const Header &header() const;

        bool seek(std::size_t pos);
        std::size_t tell() const;

        bool read(Sample sample);
        bool write(const Sample sample);

//    public:
//        template <class real>
//        bool readRect(
//                complex_tmpl<real> *rect,
//                std::size_t frame0, std::size_t framesAmount, std::size_t framesStride,
//                std::size_t period0, std::size_t periodsAmount, std::size_t periodsStride);

//        template <class real>
//        bool fetchRectColumn(
//                complex_tmpl<real> *col,
//                std::size_t frame,
//                std::size_t period0, std::size_t periodsAmount, std::size_t periodsStride);



    private:
        bool readReals(real *data, std::size_t size);
        bool writeReals(const real *data, std::size_t size);

    private:
        Header _header;
        std::fstream _file;
        std::size_t _offset0 = 0;
        std::size_t _pos = 0;
        std::size_t _sampleBytes = 0;
        bool _good = false;
    };



//    template <class real>
//    bool SpectrStore::readRect(
//            complex_tmpl<real> *rect,
//            std::size_t frame0, std::size_t framesAmount, std::size_t framesStride,
//            std::size_t period0, std::size_t periodsAmount, std::size_t periodsStride)
//    {
//        for(std::size_t frameIndex(0); frameIndex<framesAmount; ++frameIndex)
//        {
//            if(!fetchRectColumn(&rect[frameIndex*periodsAmount],
//                                frame0 + frameIndex*framesStride,
//                                period0, periodsAmount, periodsStride))
//            {
//                return false;
//            }
//        }

//        return true;
//    }



//    template <class real>
//    bool SpectrStore::fetchRectColumn(
//            complex_tmpl<real> *col,
//            std::size_t frame,
//            std::size_t period0, std::size_t periodsAmount, std::size_t periodsStride)
//    {

//        TVComplex prefetchedFrame(_header._periods.size());
//        seek(frame);
//        if(!read(&prefetchedFrame, 1))
//        {
//            return false;
//        }

//        if(prefetchedFrame.size() < period0 + periodsAmount)
//        {
//            return false;
//        }

//        for(std::size_t periodIndex(0); periodIndex<periodsAmount; ++periodIndex)
//        {
//            if(period0 + periodIndex*periodsStride >= prefetchedFrame.size())
//            {
//                return false;
//            }

//            const auto &c1 = prefetchedFrame[period0 + periodIndex*periodsStride];
//            auto &c2 = col[periodIndex];
//            c2.re() = c1.re();
//            c2.im() = c1.im();
//        }

//        return true;
//    }


}}
