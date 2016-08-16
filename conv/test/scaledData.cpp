#include "scaledData.h"
#include "math.h"

#include <algorithm>
#include <boost/math/special_functions.hpp>
#include <iostream>

namespace hel
{

    //////////////////////////////////////////////////////////////////////////
    ScaledData::ScaledData()
        : _pushCounter(0)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    void ScaledData::setup(const SettingsScaledData &settings)
    {
        _settings = settings;
    }

    //////////////////////////////////////////////////////////////////////////
    const SettingsScaledData &ScaledData::getSettings()const
    {
        return _settings;
    }

    //////////////////////////////////////////////////////////////////////////
    void ScaledData::pushAndFill(const TimedValue *data, size_t size, TDTimedValue &dst)
    {
        if(dst.size())
        {
            while(size && data[0]._time <= dst.back()._time)
            {
                data++;
                size--;
            }
        }
        if(!size)
        {
            return;
        }
        if(dst.empty())
        {
            dst.push_back(data[0]);
            data++;
            size--;
        }

        while(size)
        {
            const TimedValue &tv0 = dst.back();
            const TimedValue &tv1 = data[0];

            size_t insAmount = size_t((tv1._time - tv0._time)/_settings._zeroPeriod + 0.5);

            for(size_t i(0); i<insAmount; i++)
            {
                TimedValue tv;
                tv._time = tv0._time + (tv1._time - tv0._time)*(i+1)/insAmount;
                tv._value = tv0._value + (tv1._value - tv0._value)*(i+1)/insAmount;

                dst.push_back(tv);
            }

            data++;
            size--;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void ScaledData::pushData(const TimedValue *data_, size_t size)
    {
        _pushCounter += size;

        if(_settings._smoothLength<3)
        {//без сглаживания

            pushAndFill(data_, size, _originalSmoothed);
        }
        else
        {//со сглаживанием

            pushAndFill(data_, size, _original);

            _medianBuf.resize(_settings._smoothLength);
            size_t sizeLlsq = _settings._smoothLength*48/100;
            while(sizeLlsq<2)
            {
                sizeLlsq++;
            }
            size_t beginLlsq = (_settings._smoothLength - sizeLlsq)/2;

            while(_original.size() >= _settings._smoothLength)
            {
                //сглаживать по медиане
                for(size_t i(0); i<_settings._smoothLength; i++)
                {
                    _medianBuf[i] = _original[i]._value;
                }

                std::sort(_medianBuf.begin(), _medianBuf.end());

                real k, b;
                llsq(NULL, &_medianBuf[beginLlsq], sizeLlsq, k, b);

                TimedValue tv;
                tv._time = _original[_settings._smoothLength/2]._time;
                tv._value = k*(sizeLlsq-1)/2 + b;

                if(0)//TRATATA
                {
                    real peakX = 867995730.00000000;//  -60*60*4;

                    tv._value = 0;
//                     static bool peakWas = false;
//                     //if(!peakWas)
//                     {
//                         if(peakX <= tv._time)
//                         {
//                             tv._value = 1;
//                             peakWas = true;
//                         }
//                     }

                    real t = 1*60*60;
                    //if(peakX - tv._time < t*5)
                    if(tv._time > peakX)
                    {
                        tv._value += cos(c_2Pi*(tv._time-peakX)/t);//  *  (cos(c_pi*(tv._time-peakX)/(t*10))/2+0.5);
                    }

//                    tv._value = real(rand())/RAND_MAX;
                }

                _originalSmoothed.push_back(tv);
                _original.pop_front();
            }
        }


        if(_originalSmoothed.size() > _settings._maxLength)
        {
            _originalSmoothed.erase(_originalSmoothed.begin(), _originalSmoothed.begin() + (_originalSmoothed.size() - _settings._maxLength));
        }
    }

    //////////////////////////////////////////////////////////////////////////
    real ScaledData::getLastTime() const
    {
        if(_originalSmoothed.empty())
        {
            return 0;
        }
        return _originalSmoothed.back()._time;
    }

    //////////////////////////////////////////////////////////////////////////
    real ScaledData::getNearTime(real t) const
    {
        size_t idx = std::lower_bound(_originalSmoothed.begin(), _originalSmoothed.end(), t) - _originalSmoothed.begin();
        if(idx==_originalSmoothed.size())
        {
            return _originalSmoothed.back()._time;
        }

        if(idx==_originalSmoothed.size()-1)
        {
            return _originalSmoothed.back()._time;
        }
        if(!idx)
        {
            return _originalSmoothed.front()._time;
        }

        if(_originalSmoothed[idx]._time - t < t-_originalSmoothed[idx-1]._time)
        {
            return _originalSmoothed[idx]._time;
        }

        return _originalSmoothed[idx-1]._time;
    }

    //////////////////////////////////////////////////////////////////////////
    real ScaledData::getLastTimeOriginal() const
    {
        if(_original.empty())
        {
            return 0;
        }
        return _original.back()._time;
    }

    //////////////////////////////////////////////////////////////////////////
    real ScaledData::getLastValue() const
    {
        if(_originalSmoothed.empty())
        {
            return 0;
        }
        return _originalSmoothed.back()._value;
    }


    //////////////////////////////////////////////////////////////////////////
    size_t ScaledData::getSamplesAmount() const
    {
        return _originalSmoothed.size() + _original.size();
    }


    //////////////////////////////////////////////////////////////////////////
    size_t ScaledData::fillScaled(
        real samplePeriod,
        size_t amount,
        real *values,
        real extraMult) const
    {










        if(_originalSmoothed.empty())
        {
            return 0;
        }

//        real mult = _settings._zeroPeriod / samplePeriod;
//         size_t approxSourceAmount = size_t(amount / mult + 1);
//         if(_originalSmoothed.size() - approxSourceAmount > _originalSmoothed.size())
//         {
//             //не хватает данных
//             return 0;
//         }

        real startTimeNoExtra = getLastTime() - samplePeriod * amount / extraMult;
        if(startTimeNoExtra < _originalSmoothed[0]._time)
        {
            return 0;
        }

        real startTime = getLastTime() - samplePeriod * amount;
        startTime -= _settings._zeroPeriod*1e-5;//чтобы не выйти за конец оригинала из за неточности вещественного числа


        size_t startIdx = std::lower_bound(_originalSmoothed.begin(), _originalSmoothed.end(), startTime) - _originalSmoothed.begin() - 1;
        if(startIdx > _originalSmoothed.size())
        {
            startIdx = 0;
        }

        //формирование будет производится суммированием, такчто почистить нулями
        //std::fill_n(values, amount, 0.0);

        //////////////////////////////////////////////////////////////////////////
        size_t sourceIdx = startIdx;
        real time = startTime;
        for(size_t vi(0); vi<amount; vi++)
        {
            real nextTime = startTime + samplePeriod*(vi+1);
            size_t sourceEnd = std::lower_bound(_originalSmoothed.begin() + sourceIdx, _originalSmoothed.end(), nextTime) - _originalSmoothed.begin();

            if(sourceEnd<2)
            {
                values[vi] = _originalSmoothed[0]._value;
                continue;
            }

            values[vi] = 0;
            real a=0;
            for(size_t i(sourceIdx); i<sourceEnd; i++)
            {
                values[vi] += _originalSmoothed[i]._value;
                a++;
            }

            //вычесть часть начала и часть конца
            real tmp = (time - _originalSmoothed[sourceIdx]._time) / _settings._zeroPeriod;
            a -= tmp;
            values[vi] -= _originalSmoothed[sourceIdx]._value * tmp;

            tmp = 1.0-(nextTime - _originalSmoothed[sourceEnd-1]._time) / _settings._zeroPeriod;
            a -= tmp;
            values[vi] -= _originalSmoothed[sourceEnd-1]._value * tmp;

            //нормировать объем
            values[vi] /= a;

            sourceIdx = sourceEnd-1;
            time = nextTime;
        }

        return amount;
    }


    real kaizer(real beta, int n, int N)
    {
        real nom = boost::math::cyl_bessel_i(0, beta*sqrt(1.0-sqr(real(2*n-N+1)/(N-1))));
        real denom = boost::math::cyl_bessel_i(0, beta);

        return nom / denom;
    }

    void lowPassFir(
        real Fs,
        real Fd,
        size_t n,
        TVReal &A)
    {
        real f = Fs/Fd;
        real w = c_2Pi*f;
        real q = 2*f;

        n+=2;
        n |= 1;
        A.resize(n-2);

        size_t mn = n/2-1;
        A[mn] = q;
        for(size_t k(1); k<n/2; k++)
        {
            //A[mn+k] = A[mn-k] = q*(sin(k*w)/(k*w));
            A[mn+k] = A[mn-k] = q* boost::math::sinc_pi(k*w);
        }

        real kaizerBeta = 5;
        TVReal _wnd(A.size());
        n = _wnd.size();
        mn = n/2;
        _wnd[mn+0] = kaizer(kaizerBeta, n/2, n);
        for(size_t k(1); k<=n/2; k++)
        {
            if(mn+k < _wnd.size())
            {
                _wnd[mn+k] = kaizer(kaizerBeta, n/2+k, n);
            }
            _wnd[mn-k] = kaizer(kaizerBeta, n/2-k, n);
        }

        for(size_t i(0); i<A.size(); i++)
        {
            A[i] *= _wnd[i];
        }

    }

    //////////////////////////////////////////////////////////////////////////
    void ScaledData::fillSmoothedDischarged(
        real xLast,
        TDTimedValue &data,
        real samplesOnPeriod,
        real periodsOnWin,
        bool addLongStart) const
    {

        {
            size_t lastIdx = _originalSmoothed.size()-1;
            xLast += _settings._zeroPeriod*1.5 + _settings._zeroPeriod * periodsOnWin*10/2;
            while(_originalSmoothed[lastIdx]._time > xLast)
            {
                lastIdx--;
            }


            data.clear();
            for(size_t len(periodsOnWin*10); len < lastIdx-2; len+=2)
            {
                if(len == lastIdx-3)
                {
                    int k = 220;
                }
                real bndT = len/(periodsOnWin);
                TVReal fir;
                lowPassFir(1+1.0/periodsOnWin, bndT/2, len, fir);

                TimedValue tv;
                tv._value=0;
                for(size_t i(0); i<fir.size(); i++)
                {
                    tv._value += _originalSmoothed[lastIdx-1-i]._value * fir[i];
                }
                //tv._value /= fir.size();
                if(len & 1)
                {
                    tv._time =
                        (_originalSmoothed[lastIdx-1-len/2]._time+
                        _originalSmoothed[lastIdx-1-len/2-1]._time)/2;
                }
                else
                {
                    tv._time =
                        _originalSmoothed[lastIdx-1-len/2]._time;
                }
                data.push_front(tv);
            }

            //удлинить с начала на максимум
            if(data.size() && addLongStart)
            {
                TimedValue tv = data.front();
                tv._time = -1e200;
                data.push_front(tv);
            }

            return;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        data.clear();

        real sum(0);

        size_t lastIdx = _originalSmoothed.size()-1;
        xLast += _settings._zeroPeriod/10;
        while(_originalSmoothed[lastIdx]._time > xLast)
        {
            lastIdx--;
        }
        real x0(_originalSmoothed[lastIdx]._time);
        real x1(_originalSmoothed[lastIdx]._time);

        for(size_t i(lastIdx-1); i<_originalSmoothed.size(); i--)
        {
            real period = (_originalSmoothed.back()._time - _originalSmoothed[i]._time) / periodsOnWin;
            real sampleLength = period / samplesOnPeriod;

            sum += 0.5*(_originalSmoothed[i]._value+_originalSmoothed[i+1]._value)*(_originalSmoothed[i+1]._time - _originalSmoothed[i]._time);
            x0 = _originalSmoothed[i]._time;

            real amount = x1-x0;

            if(amount >= sampleLength)
            {
                TimedValue tv;
                tv._time = (x1+x0)/2;
                tv._value = sum/(x1-x0);
                data.push_front(tv);

                sum = 0;
                x0 = _originalSmoothed[i]._time;
                x1 = _originalSmoothed[i]._time;
            }

        }

        //удлинить с начала на максимум
        if(data.size() && addLongStart)
        {
            TimedValue tv = data.front();
            tv._time = -1e200;
            data.push_front(tv);
        }
    }

}

namespace
{
    using namespace hel;
    int f289062()
    {
        ScaledData sd;

        SettingsScaledData settings;

        settings._zeroPeriod = 1.0/1000;
        settings._smoothLength = 0;
        settings._maxLength = 1000000;

        sd.setup(settings);

        for(real x(0); x<6.0; x+= settings._zeroPeriod)
        {
            TimedValue tv;
            tv._time = x;
            tv._value = cos(c_2Pi*(tv._time-5.0)*10);

            sd.pushData(&tv, 1);
        }

        TDTimedValue res;
        sd.fillSmoothedDischarged(5.0, res, 100, 5, false);

        for(const auto tv : res)
        {
            std::cout<<tv._time<<", "<<tv._value<<std::endl;
        }

        exit(0);

        return rand();
    }

    //static int ksero283 = f289062();
}
