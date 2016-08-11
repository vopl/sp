#ifndef _hel_level_h_
#define _hel_level_h_

#include "hel/types.h"
namespace hel
{
	struct LeveledValue
	{
		real            _time;
		real            _value;
		real            _sum;
		real            _amount;

		LeveledValue(real time=0)
			: _time(time)
			, _sum(0)
			, _amount(0)
			, _value(0)
		{
		}
		LeveledValue(real time, real sum, real amount)
			: _time(time)
			, _sum(sum)
			, _amount(amount)
			, _value(amount?sum/amount:0)
		{
		}
		LeveledValue(const LeveledValue &from)
			: _time(from._time)
			, _sum(from._sum)
			, _amount(from._amount)
			, _value(from._value)
		{
		}

		real getValue() const
		{
			return _value;
		}
		real getTime() const
		{
			return _time;
		}

		void updateValue()
		{
			if(_amount)
			{
				_value = _sum/_amount;
			}
			else
			{
				_value = 0;
			}
		}

		bool operator<(const LeveledValue& with) const
		{
			return _time < with._time;
		}
		bool operator<(real time) const
		{
			return _time < time;
		}
	};

	inline bool operator<(real time, const LeveledValue& with)
	{
		return time < time;
	}
	typedef std::vector<LeveledValue> TVLeveledValue;

	//////////////////////////////////////////////////////////////////////////
	class Level
		: public TVLeveledValue
	{
		const Level &operator=(const Level &);
	protected:
		real    _period;
		size_t	_levelIndex;
		size_t	_firstPushedIndex;
		size_t	_droppedAmount;
		void pushOne(real value, real time, real amount);

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _period;
			ar & _levelIndex;
			ar & _firstPushedIndex;
			ar & _droppedAmount;
			ar & (TVLeveledValue&)*this;
		}

	public:
		Level(real period, size_t index);
		Level();
		virtual ~Level(){};

		template <class TimedValue>
		void push(
			const TimedValue *values, 
			size_t amount, 
			size_t keepSamples, 
			real valuesPeriod,
			const real *weights=NULL);

		template <class TimedValue>
		void pushSingle(
			const TimedValue &value, 
			size_t keepSamples, 
			real valuesPeriod,
			const real weight=1);

		size_t getLevelIndex() const;
		size_t getFirstPushedIndex() const;
		size_t getDroppedAmount() const;
		real getPeriod() const;
		void setPeriod(real period);

		real getValueAtTime(real time) const;
		size_t getBeginIndex(real time) const;
		size_t getEndIndex(real time) const;

		//формирует новый уровень со временем, выровненым на x0 по levelPeriod
		//размер на 1 меньше исходного
		void makeAligned(size_t size, real x0, TVReal *values, TVReal *times=NULL) const;
		void makeAligned(size_t size, real x0, Level &l) const;

	};
	typedef boost::shared_ptr<Level> LevelPtr;



	//////////////////////////////////////////////////////////////////////////
	inline
		Level::Level(real period, size_t index)
		: _period(period)
		, _levelIndex(index)
		, _firstPushedIndex(0)
		, _droppedAmount(0)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	inline
		Level::Level()
		: _period(0)
		, _levelIndex(0)
		, _firstPushedIndex(0)
		, _droppedAmount(0)
	{
	}


	//////////////////////////////////////////////////////////////////////////
	inline
		size_t Level::getLevelIndex() const
	{
		return _levelIndex;
	}

	//////////////////////////////////////////////////////////////////////////
	inline
		size_t Level::getFirstPushedIndex() const
	{
		return _firstPushedIndex;
	}

	//////////////////////////////////////////////////////////////////////////
	inline 
		size_t Level::getDroppedAmount() const
	{
		return _droppedAmount;
	}

	//////////////////////////////////////////////////////////////////////////
	inline
		real Level::getPeriod() const
	{
		return _period;
	}

	//////////////////////////////////////////////////////////////////////////
	inline
		void Level::setPeriod(real period)
	{
		_period = period;
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	template <class TimedValue>
	void Level::push(
		const TimedValue *values, 
		size_t amount, 
		size_t keepSamples, 
		real valuesPeriod,
		const real *weights)
	{
		_firstPushedIndex = size();

		if(!_period || !amount)
		{
			return;
		}

		if(keepSamples)
		{
			size_t keepAlien = size_t(keepSamples * _period / valuesPeriod+1);

			if(keepAlien < amount)
			{
				values += amount - keepAlien;
				if(weights)
				{
					weights += amount - keepAlien;
				}
				amount = keepAlien;
			}
		}
		//reserve(valuesPeriod/_period*amount+1+size());

		const real valuesPeriodDiv2 = valuesPeriod/2;
		const real periodDiv2 = _period/2;
		const real smallPeriodPart = valuesPeriod<_period?valuesPeriod/100:_period/100;

		size_t startValuesIndex = 0;

// 		size_t keepValuesSamples = (size_t)(keepSamples?floor(real(keepSamples) * _period / valuesPeriod /* + 1*/):-1);
// 
// 		if(keepValuesSamples < amount)
// 		{
// 			startValuesIndex = amount - keepValuesSamples;
// 		}

		for(size_t inIdx(startValuesIndex); inIdx < amount; inIdx++)
		{
			//начало диапазона входящего значения
			real weight = weights?weights[inIdx]:1;
			real valueTimeBeg = values[inIdx]._time - valuesPeriodDiv2;
			//конец
			real valueTimeEnd = values[inIdx]._time + valuesPeriodDiv2;

			while(valueTimeBeg < valueTimeEnd)
			{
				real leveledTime = valueTimeBeg - fmod(valueTimeBeg, _period) + periodDiv2;
				real last = std::min(leveledTime + periodDiv2, valueTimeEnd);
				real amount = last - valueTimeBeg;

				if(amount <= 0)
				{
					valueTimeBeg += smallPeriodPart;
					continue;
				}

				pushOne(values[inIdx]._value, leveledTime, amount*weight);

				valueTimeBeg += amount;
			}
		}

		if(keepSamples)
		{
			if(size() > keepSamples)
			{
				if(_firstPushedIndex > size()-keepSamples)
				{
					_firstPushedIndex -= size()-keepSamples;
				}
				else
				{
					_firstPushedIndex = 0;
				}
				_droppedAmount += size() - keepSamples;
				erase(begin(), end()-keepSamples);
			}

// 			if(capacity() > keepSamples*2)
// 			{
// 				TVLeveledValue tmp(*this);
// 				tmp.swap(*this);
// 			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	template <class TimedValue>
	void Level::pushSingle(
		const TimedValue &value, 
		size_t keepSamples, 
		real valuesPeriod,
		const real weight)
	{
		push(&value, 1, keepSamples, valuesPeriod, &weight);
	}

}

BOOST_CLASS_IMPLEMENTATION(hel::LeveledValue, boost::serialization::primitive_type);

#endif


