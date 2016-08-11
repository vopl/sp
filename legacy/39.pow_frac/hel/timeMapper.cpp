#include "stdafx.h"
#include "hel/timeMapper.h"

namespace hel
{

	//////////////////////////////////////////////////////////////////////////
	void TimeMapper::setup(const SettingsTimeMapper &settings)
	{
		(SettingsTimeMapper &)*this = settings;
	}

	//////////////////////////////////////////////////////////////////////////
	TimeMapper::TimeMapper()
	{
		_dropWeekends = (true);
		_weekendBegin = (5*24*60*60);
	}

	//////////////////////////////////////////////////////////////////////////
	bool TimeMapper::isExternalDropable(real ext) const
	{
		enum {SECINDAY = 24*60*60};

		if(_dropWeekends)
		{
			real weektime = fmod(ext - 4*SECINDAY, 7*SECINDAY);
			return weektime >= _weekendBegin && weektime < _weekendBegin + 2*SECINDAY;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	real TimeMapper::external2Internal(real ext) const
	{
		if(_dropWeekends)
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
		return ext;
	}

	//////////////////////////////////////////////////////////////////////////
	real TimeMapper::internal2External(real inte) const
	{
		if(_dropWeekends)
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
}
