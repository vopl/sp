#include "sp/utils/spectrDumper.hpp"
#include <iostream>
#include <limits>
#include <cassert>

namespace sp { namespace utils
{
    SpectrDumper::SpectrDumper(const std::string &fName, const TVReal &periodGrid)
        : _periodGrid(periodGrid)
    {
        //store period grid
        {
            std::ofstream out(fName+".period");

            if(!out)
            {
                std::cerr<<"unable to open file: "<<fName+".period"<<std::endl;
            }
            else
            {
                out.precision(std::numeric_limits<real>::digits10+1);
                out.setf(std::ios::scientific);
                for(const auto &t : periodGrid)
                {
                    out<<t<<std::endl;
                }
            }
        }

        //open streams, count lines (each line is a frame)
        auto opener = [](std::fstream &fs, const std::string fName)->std::size_t
        {
            std::size_t lines = 0;
            fs.open(fName, std::ios_base::in);
            if(fs)
            {
                std::string unused;
                while(std::getline(fs, unused))
                {
                   ++lines;
                }
            }
            fs.close();

            fs.open(fName, std::ios_base::out|std::ios_base::app);
            if(!fs)
            {
                std::cerr<<"unable to open file: "<<fName<<std::endl;
                return std::size_t(-1);
            }

            fs.precision(std::numeric_limits<real>::digits10+1);
            fs.setf(std::ios::scientific);

            return lines;
        };


        std::size_t linesRe = opener(_re, fName+".re");
        std::size_t linesIm = opener(_im, fName+".im");

        if(linesRe != linesIm || linesRe==size_t(-1))
        {
            std::cerr<<"re/im files corrupted: "<<fName<<std::endl;
            _re.close();
            _im.close();
            _framesPushed = size_t(-1);
            return;
        }

        _framesPushed = linesRe;
    }

    SpectrDumper::~SpectrDumper()
    {
        _re.close();
        _im.close();
    }

    std::size_t SpectrDumper::framesPushed() const
    {
        return (_re && _im) ? _framesPushed : std::size_t(-1);
    }

    bool SpectrDumper::pushFrames(const TVComplex &frame)
    {
        assert(frame.size() == _periodGrid.size());


        if(frame.size() != _periodGrid.size() || !_re || !_im)
        {
            return false;
        }

        for(const complex &v : frame)
        {
            _re<<v.re()<<" ";
            _im<<v.im()<<" ";
        }

        _re<<std::endl;
        _im<<std::endl;

        return _re && _im;
    }
}}
