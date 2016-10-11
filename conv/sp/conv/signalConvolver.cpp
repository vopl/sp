#include "sp/conv/signalConvolver.hpp"
#include "sp/conv/signalConvolverLevel.hpp"
#include "sp/math.hpp"
#include <cassert>
#include <numeric>
#include <boost/math/special_functions.hpp>

#include <iostream>

namespace sp { namespace conv
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
        void halfLowPassFir(
            real bndT,
            std::size_t n,
            std::vector<real> &A)
        {
            real w = g_2pi/bndT;
            real q = 2/bndT;

            n+=2;
            n |= 1;
            std::size_t mn = n/2-1;
            A.resize(mn+1);

//            real kaizerBeta = 15;
//            real kaizerDenominator = kaizerDenom(kaizerBeta);

            A[mn] = q;
            //Summator<real> sum = A[mn]/2;
            for(std::size_t k(1); k<n/2; k++)
            {

                real x = k*w;
                //real v = q * sin(x)/(x);// * kaizer(kaizerBeta, mn+k, n-2, kaizerDenominator);
                real v = q * boost::math::sinc_pi(x);// * kaizer(kaizerBeta, int(mn+k), int(n-2), kaizerDenominator);

                A[mn-k] = v;
                //sum += v;
            }

//            for(auto &v : A)
//            {
//                v /= sum*2;
//            }
        }

        void halfHighPassFir(
            real bndT,
            std::size_t n,
            std::vector<real> &A)
        {
            halfLowPassFir(bndT, n, A);

            for(std::size_t i = 0; i < (n-1)/2; i++)
            {
                A[i] = -A[i];
            }

            A[(n-1)/2] = -A[(n-1)/2] + 1.0;
        }

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setup(real ppw, std::size_t samplesPerPeriod)
    {
        _ppw = ppw;
        _samplesPerPeriod = samplesPerPeriod;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setupSignal(real maxPeriod, real sampleStep, SignalApproxType sat)
    {
        _signalSampleStep = sampleStep;
        _signalSamplesPushed = 0;

        //_signal.clear();
        _signal.resize(std::size_t(maxPeriod*_ppw*2/_signalSampleStep+0.5) + 7);
        std::fill(_signal.begin(), _signal.end(), real(0));

        _sat = sat;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void SignalConvolver::setup(real ppw, const TVReal &periods, real sampleStep, std::size_t samplesPerPeriod, SignalApproxType sat)
    {
        setup(ppw, samplesPerPeriod);
        setupSignal(periods.back(), sampleStep, sat);

        _levels.resize(periods.size());

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            LevelPtr &l = _levels[i];

            l.reset(new SignalConvolverLevel(ppw, periods[i], _signalSampleStep, _samplesPerPeriod));
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
        else if(amount)
        {
            //std::move(_signal.begin(), _signal.end()-amount, _signal.begin()+amount);
            std::move_backward(_signal.begin(), _signal.end()-amount, _signal.end());

            std::copy(samples, samples+amount, _signal.begin());
            std::reverse(_signal.begin(), _signal.begin()+amount);
        }

        _signalSamplesPushed += amount;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real SignalConvolver::getTime() const
    {
        return _signalSampleStep * _signalSamplesPushed;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    TVEchoPoint SignalConvolver::convolve()
    {
        const real *signal = &_signal[0];
        std::size_t signalSize = _signal.size();

        TVEchoPoint res(_levels.size());
        TVReal valuesWorkArray(std::size_t(_samplesPerPeriod*_ppw + 0.5));

        for(std::size_t i(0); i<_levels.size(); ++i)
        {
            //std::cerr<<"convolve level "<<i<<std::endl;
            res[i] = _levels[i]->convolve(signal, signalSize, _sat, valuesWorkArray);
        }

        return res;
    }

    EchoPoint SignalConvolver::convolve(real period)
    {
        const real *signal = &_signal[0];
        std::size_t signalSize = _signal.size();

        TVReal valuesWorkArray(std::size_t(_samplesPerPeriod*_ppw + 0.5));

        SignalConvolverLevel level(_ppw, period, _signalSampleStep, _samplesPerPeriod);
        return level.convolve(signal, signalSize, _sat, valuesWorkArray);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    EchoPoint SignalConvolver::convolveIdentity(real period, real phase)
    {
        SignalConvolverLevel level(_ppw, period, _signalSampleStep, _samplesPerPeriod);
        return level.convolveIdentity(period, phase);
    }
}}
