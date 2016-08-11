#include "stdafx.h"
#include "hel/level.h"
#include "hel/math.h"

namespace hel
{

	//////////////////////////////////////////////////////////////////////////
	void Level::pushOne(real value, real time, real amount)
	{
		if(!amount)
		{
			return;
		}
		value *= amount;
		size_t size = this->size();
		size_t leveledIdx = size-1;
		static const real eps = 1E-99;
		while(leveledIdx < size)
		{
			LeveledValue &lv = operator[](leveledIdx);
			if(fabs(lv.getTime() - time) < eps)
			{
				//found
				lv._sum += value;
				lv._amount += amount;
				lv.updateValue();
				_firstPushedIndex = std::min(_firstPushedIndex, leveledIdx);
				return;
			}
			else if(lv.getTime() < time)
			{
				//insert after this element
				insert(begin() + leveledIdx+1, LeveledValue(time, value, amount));
				_firstPushedIndex = std::min(_firstPushedIndex, leveledIdx+1);
				return;
			}
			leveledIdx--;
		}

		//not found, insert into front
		assert(leveledIdx > size);

		insert(begin(), LeveledValue(time, value, amount));
		_firstPushedIndex = 0;
		return;
	}


	//////////////////////////////////////////////////////////////////////////
	real Level::getValueAtTime(real time) const
	{
		if(empty())
		{
			return 0;
		}

		LeveledValue key(time);
		const_iterator iter = std::lower_bound(begin(), end(), key);
		if(end() == iter)
		{
			return back()._value;
		}
		if(begin() == iter)
		{
			return front()._value;
		}
		const_iterator iter2 = iter; iter--;

		real time1 = iter->_time;
		real value1 = iter->_value;
		real time2 = iter2->_time;
		real value2 = iter2->_value;

		real value = value1 + (value2-value1)/(time2-time1)*(time-time1);
		return value;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t Level::getBeginIndex(real time) const
	{
		if(empty())
		{
			return 0;
		}

		LeveledValue key(time);
		const_iterator iter = std::lower_bound(begin(), end(), key);
		if(end() == iter)
		{
			return size() - 1;
		}
		if(begin() == iter)
		{
			return 0;
		}
		return iter - begin() - 1;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Level::getEndIndex(real time) const
	{
		if(empty())
		{
			return 0;
		}

		LeveledValue key(time);
		const_iterator iter = std::lower_bound(begin(), end(), key);
		if(end() == iter)
		{
			return size() - 1;
		}
		return iter - begin() + 1;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void Level::makeAligned(size_t size, real x0, TVReal *values, TVReal *times) const
	{
		const Level &level = *this;
		size_t levelSize = level.size();
		if(levelSize < size)
		{
			return;
		}

		TVReal &v = *values;
		v.resize(size);

		if(times)
		{
			times->resize(size);
		}

		real levelPeriod = level.getPeriod();

		size_t stop = level.size();

		while(x0<level[stop-1].getTime())
		{
			stop--;
		}
		size_t start = stop - size;
		if(start-1 > stop)
		{
			assert(!"too small level size");
			throw 220;
		}


		real offset = x0 - level[stop-1].getTime();

		real wFirst = 1-offset/levelPeriod;
		real wSecond = 1-wFirst;

		for(size_t i(start); i<stop; i++)
		{
			v[i-start] = level[i-1].getValue()*wFirst + level[i].getValue()*wSecond;
			if(times)
			{
				times->operator[](i-start) = level[i-1].getTime()+offset;
			}
		}

// 		real delta = x0 - (level[stop-1].getTime()+offset);
// //		if(fabs(delta) > levelPeriod/10000)
// 		{
// 			std::cout<<"-------------------- "<<delta<<std::endl;
// 		}
// 
	}

	//////////////////////////////////////////////////////////////////////////
	void Level::makeAligned(size_t size, real x0, Level &l) const
	{
		TVReal v,t;
		makeAligned(size, x0, &v, &t);
		l.resize(v.size());
		for(size_t i(0); i<v.size(); i++)
		{
			l[i]._value = v[i];
			l[i]._time = t[i];
		}
	}


}

