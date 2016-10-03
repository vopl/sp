#include "wavStore.hpp"

namespace sp { namespace utils
{
    namespace
    {
#pragma pack(push, 1)
        struct  WAV_HEADER
        {
            /* RIFF Chunk Descriptor */
            uint8_t         RIFF[4];        // RIFF Header Magic header
            uint32_t        ChunkSize;      // RIFF Chunk Size
            uint8_t         WAVE[4];        // WAVE Header
            /* "fmt" sub-chunk */
            uint8_t         fmt[4];         // FMT header
            uint32_t        Subchunk1Size;  // Size of the fmt chunk
            uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
            uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
            uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
            uint32_t        bytesPerSec;    // bytes per second
            uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
            uint16_t        bitsPerSample;  // Number of bits per sample
            /* "data" sub-chunk */
            uint8_t         Subchunk2ID[4]; // "data"  string
            uint32_t        Subchunk2Size;  // Sampled data length
        };
#pragma pack(pop)
    }



    WavStore::WavStore()
    {
    }

    WavStore::WavStore(const char *fname)
    {
        open(fname);
    }

    WavStore::WavStore(const char *fname, const Header &header)
    {
        create(fname, header);
    }

    WavStore::~WavStore()
    {
    }

    bool WavStore::good()
    {
        return _good;
    }

    WavStore::operator bool()
    {
        return good();
    }

    bool WavStore::open(const char *fName)
    {
        _good = false;
        _read = true;
        _pos = 0;

        _file.close();
        _file.clear();
        _file.open(fName, std::ios_base::binary | std::ios_base::in);
        if(!_file)
        {
            std::cerr<<"bad wav file: "<<fName<<std::endl;
            return false;
        }

        WAV_HEADER hdr;
        if(!_file.read(reinterpret_cast<char *>(&hdr), sizeof(hdr)))
        {
            std::cerr<<"bad wav file (header): "<<fName<<std::endl;
            return false;
        }

        if(memcmp(hdr.RIFF, "RIFF", 4))
        {
            std::cerr<<"bad wav file (RIFF header): "<<fName<<std::endl;
            return false;
        }

        if(memcmp(hdr.WAVE, "WAVE", 4))
        {
            std::cerr<<"bad wav file (WAVE header): "<<fName<<std::endl;
            return false;
        }

        if(memcmp(hdr.fmt, "fmt ", 4))
        {
            std::cerr<<"bad wav file (fmt header): "<<fName<<std::endl;
            return false;
        }

        if(1 != hdr.AudioFormat)
        {
            std::cerr<<"non-PCM wav file: "<<fName<<std::endl;
            return false;
        }

        if(1 != hdr.NumOfChan)
        {
            std::cerr<<"non-mono wav file: "<<fName<<std::endl;
            return false;
        }

        _header._frequency = hdr.SamplesPerSec;

        if(memcmp(hdr.Subchunk2ID, "data", 4))
        {
            std::cerr<<"bad wav file (data header): "<<fName<<std::endl;
            return false;
        }

        if(4 < hdr.blockAlign)
        {
            std::cerr<<"bad wav file (block align): "<<fName<<std::endl;
            return false;
        }

        _header._bitness = hdr.bitsPerSample;
        _header._align = hdr.blockAlign;
        _header._samples = hdr.Subchunk2Size/hdr.blockAlign;

        _good = true;
        return good();
    }

    bool WavStore::create(const char *fName, const Header &header)
    {
        assert(0);
    }

    WavStore::Header &WavStore::header()
    {
        return _header;
    }

    bool WavStore::seek(std::size_t pos)
    {
        if(pos == _pos)
        {
            return good();
        }

        if(_read)
        {
            _pos = pos;
            if(!_file.seekg(pos * _header._align))
            {
                _pos = _file.tellg()/_header._align;
                return false;
            }

            return good();
        }

        //if(_write)
        assert(0);
    }

    std::size_t WavStore::tell()
    {
        return _pos;
    }

    bool WavStore::read(Sample *data, std::size_t size)
    {
        assert(_read);
        if(!_read)
        {
            return false;
        }

        if(_pos + size > _header._samples)
        {
            std::cerr<<"too small wav file for read such size: "<<size<<std::endl;
            return false;
        }

        _pos += size;

        char sampleBuf[4];

        while(size--)
        {
            if(!_file.read(sampleBuf, _header._align))
            {
                std::cerr<<"unexpected wav file end"<<std::endl;
                return false;
            }

            switch(_header._bitness)
            {
            case 8:
                *data = sp::real(*reinterpret_cast<int8_t *>(sampleBuf))/(1<<7);
                break;
            case 16:
                *data = sp::real(*reinterpret_cast<int16_t *>(sampleBuf))/(1<<15);
                break;
            case 32:
                *data = sp::real(*reinterpret_cast<int32_t *>(sampleBuf))/(1L<<31);
                break;
            }

            ++data;
        }

        return good();
    }

    bool WavStore::write(const Sample *data, std::size_t size)
    {
        assert(0);
    }
}}
