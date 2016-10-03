#pragma once
#include <cstdint>
#include <fstream>
#include "sp/math.hpp"

namespace sp { namespace utils
{
    class WavStore
    {
    public:
        struct Header
        {
            std::size_t _frequency = 0;
            std::size_t _align = 0;
            std::size_t _bitness = 0;
            std::size_t _samplesAmount = 0;
        };

        using Sample = real;

    public:
        WavStore();
        WavStore(const char *fname);//open
        WavStore(const char *fname, const Header &header);//create
        ~WavStore();

        bool good() const;
        operator bool() const;

        bool open(const char *fName);
        bool create(const char *fName, const Header &header);

        const Header &header() const;

        bool seek(std::size_t pos);
        std::size_t tell() const;

        bool read(Sample *data, std::size_t size);
        bool write(const Sample *data, std::size_t size);

    private:
        Header _header;
        std::fstream _file;
        std::size_t _pos = 0;
        bool _read = true;//write
        bool _good = false;
    };

}}
