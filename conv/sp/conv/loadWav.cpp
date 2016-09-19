#include "sp/conv/loadWav.hpp"
#include <iostream>
#include <fstream>

namespace sp { namespace conv
{
    using namespace std;
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

    bool loadWav(const string &fName, TVReal &samples, uint32_t &sps)
    {
        samples.clear();
        sps = 0;

        using namespace std;
        ifstream in(fName, ifstream::binary);
        if(!in)
        {
            cerr<<"bad wav file: "<<fName<<endl;
            return false;
        }

        WAV_HEADER hdr;
        if(!in.read(reinterpret_cast<char *>(&hdr), sizeof(hdr)))
        {
            cerr<<"bad wav file (header): "<<fName<<endl;
            return false;
        }

        if(memcmp(hdr.RIFF, "RIFF", 4))
        {
            cerr<<"bad wav file (RIFF header): "<<fName<<endl;
            return false;
        }

        if(memcmp(hdr.WAVE, "WAVE", 4))
        {
            cerr<<"bad wav file (WAVE header): "<<fName<<endl;
            return false;
        }

        if(memcmp(hdr.fmt, "fmt ", 4))
        {
            cerr<<"bad wav file (fmt header): "<<fName<<endl;
            return false;
        }

        if(1 != hdr.AudioFormat)
        {
            cerr<<"non-PCM wav file: "<<fName<<endl;
            return false;
        }

        if(1 != hdr.NumOfChan)
        {
            cerr<<"non-mono wav file: "<<fName<<endl;
            return false;
        }

        sps = hdr.SamplesPerSec;

        if(memcmp(hdr.Subchunk2ID, "data", 4))
        {
            cerr<<"bad wav file (data header): "<<fName<<endl;
            return false;
        }

        if(4 < hdr.blockAlign)
        {
            cerr<<"bad wav file (block align): "<<fName<<endl;
            return false;
        }

        samples.resize(hdr.Subchunk2Size/hdr.blockAlign);

        char sampleBuf[4];

        for(sp::real &s : samples)
        {
            if(!in.read(sampleBuf, hdr.blockAlign))
            {
                cerr<<"bad wav file (unexpected end): "<<fName<<endl;
                return false;
            }

            switch(hdr.bitsPerSample)
            {
            case 8:
                s = sp::real(*reinterpret_cast<int8_t *>(sampleBuf))/(1<<7);
                break;
            case 16:
                s = sp::real(*reinterpret_cast<int16_t *>(sampleBuf))/(1<<15);
                break;
            case 32:
                s = sp::real(*reinterpret_cast<int32_t *>(sampleBuf))/(1L<<31);
                break;
            }
        }

        return true;
    }
}}
