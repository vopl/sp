#include "sp/cls/spectrFetcher.hpp"
#include <sstream>

namespace sp { namespace cls
{

    SpectrFetcher::SpectrFetcher(const std::string &inDir)
    {
        std::ifstream in(inDir+"/spectr.period");
        if(!in)
        {
            std::cerr<<"unable to open "<<inDir+"/spectr.period"<<std::endl;
            return;
        }

        real r;

        while(in)
        {
            in >> r;
            if(in)
            {
                _periodGrid.push_back(r);
            }
        }

        _re.open(inDir+"/spectr.re");
        if(!_re)
        {
            std::cerr<<"unable to open "<<inDir+"/spectr.re"<<std::endl;
            return;
        }

        _im.open(inDir+"/spectr.im");
        if(!_im)
        {
            std::cerr<<"unable to open "<<inDir+"/spectr.im"<<std::endl;
            return;
        }
    }

    SpectrFetcher::~SpectrFetcher()
    {

    }

    const TVReal &SpectrFetcher::periodGrid()
    {
        return _periodGrid;
    }

    bool SpectrFetcher::fetchRect(TVComplex &rect,
                   std::size_t frame0, std::size_t framesAmount,
                   std::size_t period0, std::size_t periodsAmount)
    {
        rect.resize(framesAmount*periodsAmount);

        for(std::size_t frame(0); frame<framesAmount; ++frame)
        {
            if(!fetchRectColumn(&rect[frame*periodsAmount], frame + frame0, period0, periodsAmount))
            {
                return false;
            }
        }

        return true;
    }

    bool SpectrFetcher::fetchRectColumn(complex *col, std::size_t frame, std::size_t period0, std::size_t periodsAmount)
    {
        if(!prefetch(frame))
        {
            return false;
        }

        std::size_t index = frame - _prefetchedFrame0;
        assert(index < _prefetchedFrames.size());

        const TVComplex &prefetchedFrame = _prefetchedFrames[index];

        if(prefetchedFrame.size() < period0 + periodsAmount)
        {
            return false;
        }

        std::copy(prefetchedFrame.begin()+period0, prefetchedFrame.begin()+period0+periodsAmount, col);
        return true;
    }

    bool SpectrFetcher::prefetch(std::size_t frame)
    {
        bool loaded = false;
        while(_lineIndexRe.size()<=frame)
        {
            if(!loadNextLine())
            {
                return false;
            }

            loaded = true;
        }

        if(!loaded)
        {
            if(frame >= _prefetchedFrame0 && frame < _prefetchedFrame0+_prefetchedFrames.size())
            {
                loaded = true;
            }
        }

        if(!loaded)
        {
            std::pair<std::size_t, std::size_t> indexRe = _lineIndexRe[frame];
            std::pair<std::size_t, std::size_t> indexIm = _lineIndexIm[frame];

            _re.clear();
            _im.clear();

            _re.seekg(indexRe.first);
            _im.seekg(indexIm.first);

            static char linebufRe[1024*1024];
            static char linebufIm[1024*1024];

            if(!_re.read(linebufRe, indexRe.second - indexRe.first))
            {
                return false;
            }

            if(!_im.read(linebufIm, indexIm.second - indexIm.first))
            {
                return false;
            }

            pushFrame(
                        linebufRe, indexRe.second - indexRe.first,
                        linebufIm, indexIm.second - indexIm.first,
                        frame);
        }

        return true;
    }

    bool SpectrFetcher::loadNextLine()
    {
        std::pair<std::size_t, std::size_t> indexRe;
        std::pair<std::size_t, std::size_t> indexIm;
        if(_lineIndexRe.empty())
        {
            indexRe.first = 0;
            indexIm.first = 0;
        }
        else
        {
            indexRe.first = _lineIndexRe.back().second;
            indexIm.first = _lineIndexIm.back().second;
        }

        _re.clear();
        _im.clear();

        _re.seekg(indexRe.first);
        _im.seekg(indexIm.first);

        static char linebufRe[1024*1024];
        static char linebufIm[1024*1024];

        if(!_re.getline(linebufRe, sizeof(linebufRe)))
        {
            return false;
        }

        if(!_im.getline(linebufIm, sizeof(linebufIm)))
        {
            return false;
        }

        indexRe.second = _re.tellg();
        indexIm.second = _im.tellg();

        _lineIndexRe.push_back(indexRe);
        _lineIndexIm.push_back(indexIm);

        pushFrame(
                    linebufRe, indexRe.second - indexRe.first,
                    linebufIm, indexIm.second - indexIm.first,
                    _lineIndexRe.size()-1);

        return true;
    }

    void SpectrFetcher::pushFrame(const char *linebufRe, std::size_t linebufReSize, const char *linebufIm, std::size_t linebufImSize, std::size_t frame)
    {
        if(_prefetchedFrames.empty())
        {
            _prefetchedFrame0 = frame;
        }
        else
        {
            if(frame < _prefetchedFrame0 || frame>_prefetchedFrame0+_prefetchedFrames.size())
            {
                _prefetchedFrames.clear();

                _prefetchedFrame0 = frame;
            }
        }

        std::size_t index = frame - _prefetchedFrame0;
        assert(index < _prefetchedFrames.size()+1);

        while(index >= _prefetchedFrames.size())
        {
            _prefetchedFrames.push_back(TVComplex());
        }

        TVComplex &prefetchedFrame = _prefetchedFrames[index];

        prefetchedFrame.clear();

         std::istringstream inRe(linebufRe);
         std::istringstream inIm(linebufIm);

         for(;;)
         {
             complex v;
             inRe >> v.re();
             inIm >> v.im();

             if(!inRe || !inIm)
             {
                 break;
             }

             prefetchedFrame.push_back(v);
         }
    }
}}
