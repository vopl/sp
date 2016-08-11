#include "stdafx.h"
#include "hel/timeMapper.h"

namespace hel
{

	//////////////////////////////////////////////////////////////////////////
	void TimeMapper::setup(real externalStart, real externalStop, real internalStart, real internalStop, bool dropWekends, real weekendBegin)
	{
		_dropWekends = dropWekends;
		_weekendBegin = weekendBegin;

		if(_dropWekends)
		{
			_mult = 1.0;
			_offset = 0.0;

			externalStart = external2Internal(externalStart);
			externalStop = external2Internal(externalStop);

		}
		_mult = (externalStop - externalStart) / (internalStop - internalStart);
		_offset = externalStop - internalStop / _mult;
	}

	//////////////////////////////////////////////////////////////////////////
	TimeMapper::TimeMapper()
		: _mult(1)
		, _offset(0)
		, _dropWekends(false)
		, _weekendBegin(5*24*60*60)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	bool TimeMapper::isExternalDropable(real ext) const
	{
		enum {SECINDAY = 24*60*60};

		if(_dropWekends)
		{
			real weektime = fmod(ext - 4*SECINDAY, 7*SECINDAY);
			return weektime >= _weekendBegin && weektime < _weekendBegin + 2*SECINDAY;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	real TimeMapper::external2Internal(real ext) const
	{
		if(_dropWekends)
		{
			//1970-01-01 00:00:00 четверг

			enum {SECINDAY = 24*60*60};
			size_t weekendsAmount = size_t((ext-4*SECINDAY) / (7*SECINDAY));

			real weektime = fmod(ext - 4*SECINDAY, 7*SECINDAY);
			if(weektime < _weekendBegin)
			{
				ext -= weekendsAmount * (2*SECINDAY);
			}
			else if(weektime >= _weekendBegin+2*SECINDAY)
			{
				ext -= (weekendsAmount+1) * (2*SECINDAY);
			}
			else
			{
				ext -= (weekendsAmount) * (2*SECINDAY);
			}
		}
		return (ext - _offset) / _mult;
	}

	//////////////////////////////////////////////////////////////////////////
	real TimeMapper::internal2External(real inte) const
	{
		inte = inte*_mult + _offset;

		if(_dropWekends)
		{
			enum {SECINDAY = 24*60*60};
			size_t weekendsAmount = size_t((inte-4*SECINDAY) / (5*SECINDAY));

			real weektime = fmod(inte - 4*SECINDAY, 5*SECINDAY);
			if(weektime < _weekendBegin)
			{
				inte += weekendsAmount * (2*SECINDAY);
			}
			else
			{
				inte += (weekendsAmount+1) * (2*SECINDAY);
			}
		}

		return inte;
	}

	//////////////////////////////////////////////////////////////////////////
	real TimeMapper::external2InternalOffset(real ext) const
	{
		return ext / _mult;
	}

	//////////////////////////////////////////////////////////////////////////
	real TimeMapper::internal2ExternalOffset(real internal) const
	{
		return internal * _mult;
	}


}
