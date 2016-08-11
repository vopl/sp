#ifndef _hel_scaledData_h_
#define _hel_scaledData_h_

#include "hel/settings.h"

namespace hel
{
	class ScaledData
	{
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _settings;
			ar & _original;
			ar & _originalSmoothed;
			ar & _pushCounter;
		}
	private:
		SettingsScaledData	_settings;
		TDTimedValue	_original;
		TDTimedValue	_originalSmoothed;

		TVReal _medianBuf;

		size_t			_pushCounter;

	public:
		ScaledData();
		void setup(const SettingsScaledData &settings);
		const SettingsScaledData &getSettings()const;
		void pushData(const TimedValue *data, size_t size);
		real getLastTime() const;
		real getNearTime(real t) const;
		real getLastTimeOriginal() const;
		real getLastValue() const;
		size_t getSamplesAmount() const;

		size_t getPushCounter() const
		{
			return _pushCounter;
		}

		size_t fillScaled(
			real samplePeriod,
			size_t amount,
			real *values,
			real extraMult = 1) const;

		template <class Functor>
		size_t mapScaled(
			real samplePeriod,
			size_t amount,
			Functor &f,
			real extraMult = 1) const;

		//функтор принимает отсчеты от x0 назад во времени поштучно и если возвращает false то перебор прекращается
		template <class Functor>
		size_t iterate(Functor &f) const;

		const TDTimedValue &getSmoothed() const
		{
			return _originalSmoothed;
		}

		void fillSmoothedDischarged(
			real xLast,
			TDTimedValue &data,
			real samplesOnPeriod,
			real periodsOnWin,
			bool addLongStart) const;
	private:
		void pushAndFill(const TimedValue *data, size_t size, TDTimedValue &dst);



	};




	//////////////////////////////////////////////////////////////////////////
	template <class Functor>
	size_t ScaledData::mapScaled(
		real samplePeriod,
		size_t amount,
		Functor &f,
		real extraMult) const
	{

		if(_originalSmoothed.empty())
		{
			return 0;
		}

		//		real mult = _settings._zeroPeriod / samplePeriod;
		// 		size_t approxSourceAmount = size_t(amount / mult + 1);
		// 		if(_originalSmoothed.size() - approxSourceAmount > _originalSmoothed.size())
		// 		{
		// 			//не хватает данных
		// 			return 0;
		// 		}

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
				//values[vi] = _originalSmoothed[0]._value;
				f(i, _originalSmoothed[0]._value);
				continue;
			}

			//values[vi] = 0;
			real value = 0;
			real a=0;
			for(size_t i(sourceIdx); i<sourceEnd; i++)
			{
				//values[vi] += _originalSmoothed[i]._value;
				value += _originalSmoothed[i]._value;
				a++;
			}

			//вычесть часть начала и часть конца
			real tmp = (time - _originalSmoothed[sourceIdx]._time) / _settings._zeroPeriod;
			a -= tmp;
			//values[vi] -= _originalSmoothed[sourceIdx]._value * tmp;
			value -= _originalSmoothed[sourceIdx]._value * tmp;

			tmp = 1.0-(nextTime - _originalSmoothed[sourceEnd-1]._time) / _settings._zeroPeriod;
			a -= tmp;
			//values[vi] -= _originalSmoothed[sourceEnd-1]._value * tmp;
			value -= _originalSmoothed[sourceEnd-1]._value * tmp;

			//нормировать объем
			//values[vi] /= a;
			value /= a;
			f(i, value);

			sourceIdx = sourceEnd-1;
			time = nextTime;
		}

		return amount;
	}

	//////////////////////////////////////////////////////////////////////////
	template <class Functor>
	size_t ScaledData::iterate(Functor &f) const
	{
		if(_originalSmoothed.empty())
		{
			return 0;
		}

		size_t res(0);

		TDTimedValue::const_reverse_iterator riter = _originalSmoothed.rbegin();
		TDTimedValue::const_reverse_iterator rend = _originalSmoothed.rend();

		for(; riter!=rend; riter++)
		{
			res++;
			if(!f(riter->_value, res))
			{
				break;
			}
		}
		return res;
	}


}
BOOST_CLASS_VERSION(hel::ScaledData, 0);

#endif
