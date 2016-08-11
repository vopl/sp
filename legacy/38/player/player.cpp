// player.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
#include "hel/icore.h"
#include "hel/math.h"
#include "hel/timeMapper.h"
using namespace hel;

#define TIMEZERO "0003-00-01 00:00:00"
#define TIMEBEG "2002-01-20 00:00:00"
#define TIMEEND "2009-10-31 00:00:00"

TVTimedValue g_data;

real s2time(const char *csz)
{
	tm stm = {};
	if(6 != sscanf_s(csz, "%d-%d-%d %d:%d:%d", &stm.tm_year, &stm.tm_mon, &stm.tm_mday, &stm.tm_hour, &stm.tm_min, &stm.tm_sec))
	{
		return 0;
	}
	stm.tm_year -= 1900;
	stm.tm_mon -= 1;
	return real(_mkgmtime(&stm));
}

//////////////////////////////////////////////////////////////////////////
hel::TimeMapper tmap;
bool loadData(const char *fName, TVTimedValue &data)
{
	ifstream in(fName);
	TVTimedValue sdata;

	if(!in)
	{
		return false;
	}

	real stubTime = 0;
	while(in)
	{
		char line[1024];
		in.getline(line, 1024);

		if(string(TIMEZERO) > line)
		{
			continue;
		}

		if(string(TIMEEND) < line)
		{
			break;
		}

		tm stm = {};
		real value;

		if(7 != sscanf_s(line, "%d-%d-%d %d:%d:%d\t%lf", &stm.tm_year, &stm.tm_mon, &stm.tm_mday, &stm.tm_hour, &stm.tm_min, &stm.tm_sec, &value))
		{
			continue;
		}
		if(stm.tm_year < 1970)
		{
			continue;
		}
		stm.tm_year -= 1900;
		stm.tm_mon -= 1;


		TimedValue tv;
		tv._time = real(_mkgmtime(&stm));
		tv._time = tmap.external2Internal(tv._time);
		tv._value = (value);

		sdata.push_back(tv);
	}

	data.reserve(sdata.size());
	data.resize(sdata.size());

	copy(sdata.begin(), sdata.end(), data.begin());

	return true;
}

void process(TVTimedValue &serie, std::string dir);

int _tmain(int argc, _TCHAR* argv[])
{
	hel::SettingsTimeMapper stm;
	stm._dropWeekends = true;
	stm._weekendBegin = (5*24*60*60 - 1*60*60);
	tmap.setup(stm);
	loadData("P:\\finance\\quotes\\crosses\\ocs\\EURXAG", g_data);
	if(g_data.empty())
	{
		std::cout<<" data absent\n";
		return -1;
	}

	//process(g_data, "..\\console\\out_small_10_2002-2009\\response\\");
	process(g_data, "..\\console\\out_small_15\\response\\");

	return 0;
}

