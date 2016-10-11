#include "sp/utils/spectrStore.hpp"

namespace sp { namespace utils
{
    SpectrStore::SpectrStore()
    {
    }

    SpectrStore::SpectrStore(const char *fName, bool app)
    {
        open(fName, app);
    }

    SpectrStore::SpectrStore(const char *fName, const Header &header)
    {
        create(fName, header);
    }

    SpectrStore::~SpectrStore()
    {
    }

    bool SpectrStore::good() const
    {
        return _good;
    }

    SpectrStore::operator bool() const
    {
        return good();
    }

    bool SpectrStore::open(const char *fName, bool app)
    {
        _good = false;
        _pos = 0;

        _file.close();
        _file.clear();
        _file.open(fName, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
        if(!_file)
        {
            //std::cerr<<"unable to open spectr file: "<<fName<<std::endl;
            return false;
        }

        char magic[4];
        if(!_file.read(magic, sizeof(magic)))
        {
            std::cerr<<"bad spectr file (header): "<<fName<<std::endl;
            return false;
        }

        if(magic[0] != 'Q' && magic[1] != 'L' && magic[2] != 'S' && magic[3] != 'O')
        {
            std::cerr<<"bad spectr file (header, magic): "<<fName<<std::endl;
            return false;
        }

        if(!_file.read(reinterpret_cast<char *>(&_header._realBittness), sizeof(_header._realBittness)))
        {
            std::cerr<<"bad spectr file (header): "<<fName<<std::endl;
            return false;
        }

        if(!readReals(&_header._samplesPerSecond, 1))
        {
            std::cerr<<"bad spectr file (header): "<<fName<<std::endl;
            return false;
        }

        if(_header._realBittness != sizeof(float)*8 && _header._realBittness != sizeof(double)*8 && _header._realBittness != sizeof(long double)*8)
        {
            std::cerr<<"bad spectr file (header, realBittness): "<<fName<<std::endl;
            return false;
        }

        std::uint64_t periodsAmount;
        if(!_file.read(reinterpret_cast<char *>(&periodsAmount), sizeof(periodsAmount)))
        {
            std::cerr<<"bad spectr file (header): "<<fName<<std::endl;
            return false;
        }

        if(!periodsAmount || periodsAmount > 1024*1024)
        {
            std::cerr<<"bad spectr file (header, periodsAmount): "<<fName<<std::endl;
            return false;
        }

        _header._periods.resize(periodsAmount);
        if(!readReals(&_header._periods[0], periodsAmount))
        {
            std::cerr<<"bad spectr file (header, periods): "<<fName<<std::endl;
            return false;
        }

        if(!_file.read(magic, sizeof(magic)))
        {
            std::cerr<<"bad spectr file (header): "<<fName<<std::endl;
            return false;
        }

        if(magic[0] != 'O' && magic[1] != 'S' && magic[2] != 'L' && magic[3] != 'Q')
        {
            std::cerr<<"bad spectr file (header, magic2): "<<fName<<std::endl;
            return false;
        }

        _sampleBytes = _header._realBittness/8*_header._periods.size()*2;

        _offset0 = _file.tellg();

        _file.seekg(0, std::ios::end);
        _header._samplesAmount = (std::size_t(_file.tellg()) - _offset0) / _sampleBytes;

        if(app)
        {
            _file.seekg(0, std::ios::end);
            _file.seekp(0, std::ios::end);
        }
        else
        {
            _file.seekg(_offset0, std::ios::beg);
            _file.seekp(_offset0, std::ios::beg);
        }
        _pos = (std::size_t(_file.tellg()) - _offset0) / _sampleBytes;

        _good = true;
        return good();
    }

    bool SpectrStore::create(const char *fName, const Header &header)
    {
        _good = false;
        _pos = 0;

        _file.close();
        _file.clear();
        _file.open(fName, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        if(!_file)
        {
            std::cerr<<"unable to create spectr file: "<<fName<<std::endl;
            return false;
        }

        char magic[4] = {'Q','L','S','O'};
        if(!_file.write(magic, sizeof(magic)))
        {
            std::cerr<<"unable t write spectr file (header): "<<fName<<std::endl;
            return false;
        }


        _header = header;

        if(!_file.write(reinterpret_cast<char *>(&_header._realBittness), sizeof(_header._realBittness)))
        {
            std::cerr<<"unable to write spectr file (header): "<<fName<<std::endl;
            return false;
        }

        if(!writeReals(&_header._samplesPerSecond, 1))
        {
            std::cerr<<"unable to write spectr file (header): "<<fName<<std::endl;
            return false;
        }

        std::uint64_t periodsAmount = _header._periods.size();
        if(!_file.write(reinterpret_cast<char *>(&periodsAmount), sizeof(periodsAmount)))
        {
            std::cerr<<"unable to write spectr file (header): "<<fName<<std::endl;
            return false;
        }

        if(!writeReals(&_header._periods[0], periodsAmount))
        {
            std::cerr<<"unable to write spectr file (header, periods): "<<fName<<std::endl;
            return false;
        }

        char magic2[4] = {'O','S','L','Q'};
        if(!_file.write(magic2, sizeof(magic2)))
        {
            std::cerr<<"unable t write spectr file (header): "<<fName<<std::endl;
            return false;
        }

        _file.flush();

        _sampleBytes = _header._realBittness/8*_header._periods.size()*2;

        _file.seekg(_file.tellp());
        _offset0 = _file.tellp();
        _good = true;

        return true;
    }

    const SpectrStore::Header &SpectrStore::header() const
    {
        return _header;
    }

    bool SpectrStore::seek(std::size_t pos)
    {
        if(!_good)
        {
            return false;
        }

        if(_pos == pos)
        {
            return true;
        }

        _pos = pos;
        _file.seekg(_offset0 + _pos * _sampleBytes);
        _file.seekp(_offset0 + _pos * _sampleBytes);

        return true;
    }

    std::size_t SpectrStore::tell() const
    {
        return _pos;
    }


    bool SpectrStore::read(Sample sample)
    {
        if(!_good)
        {
            return false;
        }

        if(!readReals(sample, _sampleBytes/(_header._realBittness/8)))
        {
            return false;
        }
        seek(_pos+1);

        return true;
    }

    bool SpectrStore::write(const Sample sample)
    {
        if(!_good)
        {
            return false;
        }

        if(!writeReals(sample, _sampleBytes/(_header._realBittness/8)))
        {
            return false;
        }
        _file.flush();
        seek(_pos+1);

        return true;
    }

    bool SpectrStore::readReals(real *data, std::size_t size)
    {
        char buf[256];

        while(size--)
        {
            if(!_file.read(buf, _header._realBittness/8))
            {
                std::cerr<<"unexpected spectr file end"<<std::endl;
                return false;
            }

            switch(_header._realBittness)
            {
            case sizeof(float)*8:
                *data = sp::real(*reinterpret_cast<float *>(buf));
                break;
            case sizeof(double)*8:
                *data = sp::real(*reinterpret_cast<double *>(buf));
                break;
            case sizeof(long double)*8:
                *data = sp::real(*reinterpret_cast<long double *>(buf));
                break;
            default:
                std::cerr<<"unknown sizeof real in spectr file: "<<_header._realBittness<<std::endl;
                assert(0);
                abort();
            }

            ++data;
        }

        return true;
    }

    bool SpectrStore::writeReals(const real *data, std::size_t size)
    {
        if(!_file.write(reinterpret_cast<const char *>(data), size*_header._realBittness/8))
        {
            std::cerr<<"unable to write spectrStore reals"<<std::endl;
            return false;
        }

        return true;
    }

}}
