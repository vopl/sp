#include "signalConvolver.hpp"
#include "signalConvolverLevel.hpp"
#include <cassert>

namespace sp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolver::SignalConvolver()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    SignalConvolver::~SignalConvolver()
    {
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setup(real pow, const PeriodGrid &periodGrid, real sampleStep, std::size_t samplesPerPeriod)
    {
        _sampleStep = sampleStep;
        _samplesPushed = 0;
        _signal.clear();
        _signal.resize(std::size_t(periodGrid.grid().back()*pow*2/_sampleStep+0.5) + 100);
        _dirty = true;

        _levels.resize(periodGrid.grid().size());

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            LevelPtr &l = _levels[i];

            l.reset(new SignalConvolverLevel(pow, periodGrid.grid()[i], sampleStep, samplesPerPeriod));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::pushSignal(const real *samples, std::size_t amount)
    {
        _signal.insert(_signal.end(), samples, samples+amount);
        _samplesPushed += amount;
        _dirty = true;

        const real *first = _signal.array_one().first;
        std::size_t firstSize = _signal.array_one().second;

        const real *second = _signal.array_two().first;
        std::size_t secondSize = _signal.array_two().second;

        /*
        что дальше


            подготовка фнч
            обрезка по частоте
            свертка
        */


        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            _levels[i]->update(first, firstSize, second, secondSize);
        }
        exit(0);

    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real SignalConvolver::getTime() const
    {
        return _sampleStep * _samplesPushed;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVComplex /*echo*/ SignalConvolver::convolve() const
    {
        assert(0);
    }

}
