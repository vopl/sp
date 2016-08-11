// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
#include "hel/icore.h"
#include "hel/math.h"
using namespace hel;

#include <Windows.h>

#define MPFR_PREC 512

#define TIMEZERO "0000-00-01 00:00:00"
#define TIMEBEG "2000-01-01 04:40:00"
#define TIMEEND "2000-01-01 06:05:00"

#define TIMESTEP (1)
#define SPECTR_SAVE_CNT 24*5


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
		tv._value = (value);

// 		if(!stubTime)
// 		{
// 			stubTime = tv._time;
// 		}
// 		else
// 		{
// 			stubTime += 60;
// 			tv._time = stubTime;
// 		}
// 
// 		tv._value = 0;
// // 		for(real t=15; t<=15; t++)
// // 		{
// // 			int seed = int(t*10000);
// // 			srand(seed);
// // 			tv._value += sqrt(t)*cos(tv._time*c_2Pi/(60*60*24*t)+rand());
// // 		}
// 		tv._value += 1*cos(tv._time*c_2Pi/(60*60*24* 1 ) + 0);
// //		tv._value += 1*cos(tv._time*c_2Pi/(60*60*24* 1.2 ) + 0);

		sdata.push_back(tv);
	}

	data.reserve(sdata.size());
	data.resize(sdata.size());

	copy(sdata.begin(), sdata.end(), data.begin());

	return true;
}

//////////////////////////////////////////////////////////////////////////
void dumpExtrap(
				hel::ICore *core, 
				const string &spDir, 
				const string &sp1Dir, 
				const string &respDir, 
				const TVTimedValue &data, 
				size_t idx, 
				std::ofstream &outSpectr, 
				std::ofstream &outSpectr1, 
				std::ofstream &outResponse)
{
	real x0 = core->getLastUpdateDataTime();
	x0 = core->external2Internal(x0);

	//////////////////////////////////////////////////////////////////////////
	//отклик
	{
		TVReal period(10000);
		TVComplex value(period.size());
		size_t s = core->fillResponse(0,&period.front(), &value.front(), period.size());

		if(!outResponse.is_open() || !outResponse)
		{
			char tmp[1024];
			sprintf_s(tmp, "%s\\%012Iu", respDir.data(), size_t(x0+0.5));
			//std::cout<<tmp<<std::endl;
			outResponse.clear();
			outResponse.open(tmp);
			outResponse.precision(20);
			outResponse.setf(std::ios::scientific);
		}


		for(size_t i(0); i<s; i++)
		{
			outResponse<<x0<<"\t";
			outResponse<<period[i]<<"\t";
			outResponse<<value[i].re()<<"\t";
			outResponse<<value[i].im()<<"\n";
		}
		outResponse.flush();
	}

	//////////////////////////////////////////////////////////////////////////
	//спектр
	{
		TVReal period(10000);
		TVComplex value(period.size());
		size_t s = core->fillSpectr(0,&period.front(), &value.front(), period.size());
		//TVComplex value2(period.size());
		//s = core->fillSpectr(1,&period.front(), &value2.front(), period.size());

		if(!outSpectr.is_open() || !outSpectr)
		{
			char tmp[1024];
			sprintf_s(tmp, "%s\\%012Iu", spDir.data(), size_t(x0+0.5));
			//std::cout<<tmp<<std::endl;
			outSpectr.clear();
			outSpectr.open(tmp);
			outSpectr.precision(20);
			outSpectr.setf(std::ios::scientific);
		}


//		real x0 = core->getLastDataTime();

		for(size_t i(0); i<s; i++)
		{
			Complex v = value[i];// + -2*value2[i];
			outSpectr<<x0<<"\t";
			outSpectr<<period[i]<<"\t";
			outSpectr<<v.re()<<"\t";
			outSpectr<<v.im()<<"\n";
		}
		outSpectr.flush();
	}

	//////////////////////////////////////////////////////////////////////////
	//спектр свернутый с гаусианом
	{
		TVReal period(10000);
		TVComplex value(period.size());
		size_t s = core->fillSpectr(0,&period.front(), &value.front(), period.size());

		if(!outSpectr1.is_open() || !outSpectr1)
		{
			char tmp[1024];
			sprintf_s(tmp, "%s\\%012Iu", sp1Dir.data(), size_t(x0+0.5));
			//std::cout<<tmp<<std::endl;
			outSpectr1.clear();
			outSpectr1.open(tmp);
			outSpectr1.precision(20);
			outSpectr1.setf(std::ios::scientific);
		}


//		real x0 = core->getLastDataTime();

		for(size_t i(0); i<s; i++)
		{
			outSpectr1<<x0<<"\t";
			outSpectr1<<period[i]<<"\t";

			real re(0);
			real im(0);

			for(size_t j(0); j<s; j++)
			{
				re += value[j].re() * gaussian(j, i, 2);
				im += value[j].im() * gaussian(j, i, 2);
			}
			outSpectr1<<re<<"\t";
			outSpectr1<<im<<"\n";
		}
		outSpectr1.flush();
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
std::map<boost::thread *, real> g_threads;
boost::mutex g_mtx;
//////////////////////////////////////////////////////////////////////////
int processOne(real pow)
{
	//////////////////////////////////////////////////////////////////////////
	std::cout.precision(16);
	std::cout.setf(std::ios::scientific);

	char dir[256];
	sprintf_s(dir, "out_small_%02.4f", pow);
	_mkdir(dir);

	char spDir[256];
	sprintf_s(spDir, "%s\\spectr", dir);
	_mkdir(spDir);

	char sp1Dir[256];
	sprintf_s(sp1Dir, "%s\\spectr1", dir);
	_mkdir(sp1Dir);

	char respDir[256];
	sprintf_s(respDir, "%s\\response", dir);
	_mkdir(respDir);

	hel::ICore *core;

	string stateFileName = dir;
	stateFileName += "\\state.nhel";

	std::ofstream outSpectr;
	std::ofstream outSpectr1;
	std::ofstream outResponse;

	core = hel::createICore();
	if(!core->load(stateFileName.c_str()))
	{
		hel::SettingsTimeMapper stm={};
		stm._dropWeekends = (true);
		stm._dropWeekends = (false);
		stm._weekendBegin = (5*24*60*60 - 1*60*60);

		hel::SettingsScaledData ssd={};
		ssd._zeroPeriod = (1);//60 сек
		ssd._smoothLength = 0;	//N по ssd._zeroPeriod
		


		hel::SettingsOptimizer so={};
		real mn=1;
		real mx=1000.0;
		//////////////////////////////////////////////////////////////////////////
		so._minPeriodE = mn;
		so._maxPeriodE = mx;
		real mult = 0.99;
		size_t stepsPeriod = size_t(log(so._minPeriodE/so._maxPeriodE) / log(mult) + 0.5);
		so._stepsPeriodE = stepsPeriod;


		//////////////////////////////////////////////////////////////////////////
		so._minPeriodS = so._minPeriodE;
		so._maxPeriodS = so._maxPeriodE;
		so._stepsPeriodS = so._stepsPeriodE;

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		ssd._maxLength = size_t(mx * pow / ssd._zeroPeriod);

		core->setup(stm, ssd);

		so._periodsOnWin = (pow);
 		core->addOptimizer(so);

// 		so._periodsOnWin = 9;
// 		so._minPeriodE=so._minPeriodE*10/9;
// 		so._maxPeriodE=so._maxPeriodE*10/9;
// 		so._minPeriodS=so._minPeriodS*10/9;
// 		so._maxPeriodS=so._maxPeriodS*10/9;
// 		core->addOptimizer(so);

		core->save(stateFileName.c_str());
		core->save((stateFileName+".init").c_str());
	}



	bool coreStartedEmpty = nullReal == core->getLastUpdateDataTime();


	//////////////////////////////////////////////////////////////////////////
	real timeBeg = s2time(TIMEBEG);

	if(timeBeg < core->getLastUpdateDataTime())
	{
		timeBeg = core->getLastUpdateDataTime();
	}

	unsigned long tick = GetTickCount();


// 	if(nullReal != core->getLastUpdateDataTime())
// 	{
// 		while(!core->update(TIMESTEP));
// 		exit(0);
// 	}
	if(g_data.empty())
	{
		loadData("P:\\finance\\quotes\\crosses\\ocs\\EURXAG", g_data);
		if(g_data.empty())
		{
			std::cout<<" data absent\n";
			return -1;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	size_t cnt=0;
	size_t prevIdx=0;
	for(size_t idx=1; idx<g_data.size(); idx++)
	{
		if(g_data[idx]._time < timeBeg)
		{
			continue;
		}

		if(g_data[prevIdx]._time + TIMESTEP > g_data[idx]._time)
		{
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//cout<<dir<<" push "<<idx-prevIdx<<", "<<idx<<endl;

		if(!prevIdx && coreStartedEmpty)
		{

			core->pushData(&g_data[prevIdx], idx-prevIdx+1);
			core->save(stateFileName.c_str());
			core->save((stateFileName+".data0").c_str());
			cout<<pow<<" initial data pushed. Run again"<<std::endl;
			//exit(0);
		}
		else
		{
			core->pushData(&g_data[prevIdx], idx-prevIdx+1);
		}
		prevIdx = idx;


		//////////////////////////////////////////////////////////////////////////
		while(core->needUpdate())
		{
			core->update(TIMESTEP, true, true);

			dumpExtrap(core, spDir, sp1Dir, respDir, g_data, idx, outSpectr, outSpectr1, outResponse);

			size_t ssize = core->fillResponse(0,0,0,0);
			real error = core->getSeparatorError(0) + core->getSeparatorError(1);

			char time_[256];
			tm stm = {};
			time_t t(time_t(core->getLastUpdateDataTime()+0.5));
			gmtime_s(&stm, &t);
			sprintf_s(time_, "%04d-%02d-%02d %02d:%02d:%02d", stm.tm_year+1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
			std::cout<<pow<<"\t"<<time_<<"\t"<<error<<"\t"<<ssize<<std::endl;

			cnt++;
			if(cnt>=SPECTR_SAVE_CNT)
			{
				outSpectr.close();
				outSpectr1.close();
				outResponse.close();
				cnt = 0;

				core->save(stateFileName.c_str());
				std::cout<<pow<<"\tsaved "<<stateFileName<<std::endl;
			}
		}




	}
	core->save(stateFileName.c_str());
	std::cout<<pow<<"\tsaved "<<stateFileName<<std::endl;



	delete core;
	return 0;
}





















//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	//loadData("P:\\finance\\quotes\\crosses\\ocs\\EURXAG", g_data);
	loadData("P:\\finance\\waves\\sector.out", g_data);
	if(g_data.empty())
	{
		std::cout<<" data absent\n";
		return -1;
	}

	std::cout.precision(20);
	vector<boost::thread> threads;

	real step = 20;
	for(real pow=10.0; pow<10.0+step/2; pow+=step)
	{
		boost::thread *t = new boost::thread(boost::bind(&processOne, pow));
		SetThreadPriority(t->native_handle(), THREAD_PRIORITY_IDLE);
		//SetThreadPriority(threads.back()->native_handle(), THREAD_PRIORITY_LOWEST);

		boost::lock_guard<boost::mutex> lock(g_mtx);
		g_threads[t] = 0;
	}

	for(std::map<boost::thread *, real>::iterator iter(g_threads.begin()); iter!=g_threads.end(); iter++)
	{
		iter->first->join();
	}
	return 0;
}