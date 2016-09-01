#include "sp/signalConvolver.hpp"
#include "sp/signalConvolverLevel.hpp"
#include "sp/math.hpp"
#include <cassert>
#include <numeric>
#include <boost/math/special_functions.hpp>

#include <iostream>

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
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        real kaizerDenom(real beta)
        {
            return boost::math::cyl_bessel_i(0, beta);
        }

        real kaizer(real beta, int n, int N, real denom)
        {
            real nom = boost::math::cyl_bessel_i(0, beta*sqrt(1.0L-sqr(real(2*n-N+1)/(N-1))));

            return nom / denom;
        }

        real kaizer(real beta, int n, int N)
        {
            real nom = boost::math::cyl_bessel_i(0, beta*sqrt(1.0L-sqr(real(2*n-N+1)/(N-1))));

            return nom / kaizerDenom(beta);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        void lowPassHalfFir(
            real bndT,
            std::size_t n,
            std::vector<real> &A)
        {
            real w = g_2pi/bndT;
            real q = 1;//2/bndT;

            n+=2;
            n |= 1;
            std::size_t mn = n/2-1;
            A.resize(mn+1);

//            real kaizerBeta = 5;
//            real kaizerDenominator = kaizerDenom(kaizerBeta);

            A[mn] = q;
            Summator<real> sum = A[mn]/2;
            for(std::size_t k(1); k<n/2; k++)
            {

                real x = k*w;
                //real v = q * sin(x)/(x);// * kaizer(kaizerBeta, mn+k, n-2, kaizerDenominator);
                real v = q * boost::math::sinc_pi(x);// * kaizer(kaizerBeta, int(mn+k), int(n-2), kaizerDenominator);

                A[mn-k] = v;
                sum += v;
            }

            for(auto &v : A)
            {
                v /= sum*2;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setupFirs(real pow, std::size_t samplesPerPeriod)
    {
        _pow = pow;
        _samplesPerPeriod = samplesPerPeriod;

        _halfFirs.resize(std::size_t(_samplesPerPeriod*_pow + 0.5));

        for(std::size_t firIdx(0); firIdx<_halfFirs.size(); ++firIdx)
        {
            std::size_t firLen = (firIdx*2)+3;
            real bndT = (real(firLen-1))/(_pow)/2;
            lowPassHalfFir(bndT, firLen, _halfFirs[firIdx]);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setupSignal(real maxPeriod, real sampleStep, SignalApproxType sat)
    {
        _signalSampleStep = sampleStep;
        _signalSamplesPushed = 0;

        //_signal.clear();
        _signal.resize(std::size_t(maxPeriod*_pow*2/_signalSampleStep+0.5) + 7);
        std::fill(_signal.begin(), _signal.end(), real(0));

        _sat = sat;

        _dirty = true;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setup(real pow, const PeriodGrid &periodGrid, real sampleStep, std::size_t samplesPerPeriod, SignalApproxType sat)
    {
        setupFirs(pow, samplesPerPeriod);
        setupSignal(periodGrid.grid().back(), sampleStep, sat);

        _levels.resize(periodGrid.grid().size());

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            LevelPtr &l = _levels[i];

            l.reset(new SignalConvolverLevel(pow, periodGrid.grid()[i], _signalSampleStep, _samplesPerPeriod));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::pushSignal(const real *samples, std::size_t amount)
    {
        if(amount >= _signal.size())
        {
            std::copy(samples+amount-_signal.size(), samples+amount, _signal.begin());
            std::reverse(_signal.begin(), _signal.end());
        }
        else
        {
            std::move(_signal.begin(), _signal.end()-amount, _signal.begin()+amount);
            std::copy(samples, samples+amount, _signal.begin());
            std::reverse(_signal.begin(), _signal.begin()+amount);
        }

        _signalSamplesPushed += amount;
        _dirty = true;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real SignalConvolver::getTime() const
    {
        return _signalSampleStep * _signalSamplesPushed;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVComplex /*echo*/ SignalConvolver::convolve()
    {
        /*
        что дальше

            обрезка по частоте, попробовать через сигмоид переключать сигнал-результат фильтра
        */

        prepareValues();

        TVComplex res(_levels.size());

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            //std::cerr<<"convolve level "<<i<<std::endl;
            res[i] = _levels[i]->convolve();
        }

        return res;
    }

    complex /*echo*/ SignalConvolver::convolve(real period)
    {
        const real *signal = &_signal[0];
        std::size_t signalSize = _signal.size();

        SignalConvolverLevel level(_pow, period, _signalSampleStep, _samplesPerPeriod);
        level.update(signal, signalSize, _sat);
        level.filtrate(_halfFirs);

        return level.convolve();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::prepareValues()
    {
        if(!_dirty)
        {
            return;
        }

        _dirty = false;

        {
            const real *signal = &_signal[0];
            std::size_t signalSize = _signal.size();

            for(std::size_t i(0); i<_levels.size(); ++i)
            {
                //std::cerr<<"update level "<<i<<std::endl;
                _levels[i]->update(signal, signalSize, _sat);
            }
        }

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            //std::cerr<<"filtrate level "<<i<<std::endl;
            _levels[i]->filtrate(_halfFirs);
        }

    }
}
