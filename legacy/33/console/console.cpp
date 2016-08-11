// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
#include "hel/icore.h"
#include "hel/math.h"
using namespace hel;

#include <Windows.h>


#define TIMEZERO "0006-06-01 00:00:00"
#define TIMEBEG "2008-01-15 10:00:00"
#define TIMEEND "2008-12-31 00:00:00"

#define TIMESTEP 60
//#define TIMESTEP 0

#define EXTRAPSTEPS		60*24*30
//#define EXTRAPSTEPS		1



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
void dumpExtrap(hel::ICore *core, const string &dir, const TVTimedValue &data, size_t idx)
{
	//////////////////////////////////////////////////////////////////////////
	//текущее время
	{
		std::ofstream out((dir+"\\out_x0").c_str(), std::ios::app);
		out.precision(20);
		out.setf(std::ios::scientific);
		out<<core->getLastDataTime()<<std::endl;
	}



	//////////////////////////////////////////////////////////////////////////
	//спектр
	{
		TVReal period(2000);
		TVComplex value(period.size());
		size_t s = core->fillSpectr(0, &period.front(), &value.front(), period.size());

// 		if(!boost::filesystem::exists(dir + "\\out_spectr"))
// 		{
// 			std::ofstream out((dir + "\\out_spectr").c_str(), std::ios::app);
// 			out<<"time\tperiod\treal\timag\n";
// 		}

		std::ofstream out((dir+"\\out_spectr").c_str(), std::ios::app);
		out.precision(20);
		out.setf(std::ios::scientific);

		real x0 = core->getLastDataTime();

		for(size_t i(0); i<s; i++)
		{
			out<<x0<<"\t";
			out<<period[i]<<"\t";
			out<<value[i].re()<<"\t";
			out<<value[i].im()<<"\n";
		}
	}

	return;



	//////////////////////////////////////////////////////////////////////////
	//время для экстрапов
	TVReal time(EXTRAPSTEPS);
	TVReal original(EXTRAPSTEPS);
	{
		//отступить назад на половину EXTRAPSTEPS от текущей точки
		real now = core->getLastDataTime();

		size_t offset(0);
		size_t offsetMean(0);
		while(offsetMean<EXTRAPSTEPS/2)
		{
			if(!core->isExternalDropable(data[idx-offset]._time))
			{
				offsetMean++;
			}
			offset++;

			if(idx < offset)
			{
				std::cerr<<"too early start, unable rewind EXTRAPSTEPS/2"<<std::endl;
				exit(0);
			}
		}

		//заполнить время
		for(size_t i(0), j(0); i<EXTRAPSTEPS; i++, j++)
		{
			while(core->isExternalDropable(data[idx-offset+j]._time))
			{
				j++;
				if(idx-offset+j >= data.size())
				{
					std::cerr<<"too late stop, unable wind EXTRAPSTEPS/2"<<std::endl;
					exit(0);
				}
			}
			time[i] = data[idx-offset+j]._time;
			original[i] = data[idx-offset+j]._value;
		}

		TVReal value(time.size());
		core->fillExtrap(0, &time.front(), &value.front(), time.size());

// 		//записать
// 		if(!boost::filesystem::exists(dir + "\\out_extrap"))
// 		{
// 			std::ofstream out((dir + "\\out_extrap").c_str(), std::ios::app);
// 			out<<"time\toe\tx0\tkey\n";
// 		}
// 		std::ofstream out((dir + "\\out_extrap").c_str(), std::ios::app);
// 		out.precision(20);
// 		out.setf(std::ios::scientific);
// 
// 		for(size_t i(0); i<time.size(); i++)
// 		{
// 			out<<time[i]<<"\t";
// 			out<<original[i]<<"\t";
// 			out<<now<<"\t";
// 			out<<1<<"\n";
// 
// 			out<<time[i]<<"\t";
// 			out<<value[i]<<"\t";
// 			out<<now<<"\t";
// 			out<<2<<"\n";
// 		}

		//записать
		{
			std::ofstream out((dir+"\\out_time").c_str(), std::ios::app);
			out.precision(20);
			out.setf(std::ios::scientific);
			std::ofstream outo((dir + "\\out_original").c_str(), std::ios::app);
			outo.precision(20);
			outo.setf(std::ios::scientific);

			for(size_t i(0); i<time.size(); i++)
			{
				out<<time[i]<<"\t";
				outo<<original[i]<<"\t";
			}

			out<<std::endl;
			outo<<std::endl;
		}


		{
			std::ofstream out((dir+"\\out_extrap").c_str(), std::ios::app);
			out.precision(20);
			out.setf(std::ios::scientific);

			for(size_t i(0); i<time.size(); i++)
			{
				out<<value[i]<<"\t";
			}

			out<<std::endl;
		}

	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int processOne(const char *inFileName, int pow)
{
	char dir[256];
	sprintf_s(dir, "out_small_%02d", int(pow));
	_mkdir(dir);

	hel::ICore *core;

	string stateFileName = dir;
	stateFileName += "\\state.nhel";


	core = hel::createICore();
	if(!core->load(stateFileName.c_str()))
	{
		hel::SettingsScaledData ssd={};

		ssd._linearFillWhenPush = (false);
		ssd._periodMult = (0.99);
		ssd._zeroPeriod = (60);
		ssd._dropWeekends = (true);
		ssd._weekendBegin = (5*24*60*60 - 1*60*60);

		ssd._timeMostPast = 0;
		ssd._timeZero = 946684800;//2000-01-01 00:00:00

		core->setupData(ssd);


		hel::SettingsOptimizer so={};

		so._periodsOnWin = (pow);
		so._samplesOnPeriod = (100);
		so._analyzeBestTime = (0.5);
		so._polyPow = 0;
		so._kaizerBeta = 0;

		const size_t stepsOnPeriod = 1000;
		so._calibratorMinPeriod = 1.0/stepsOnPeriod;
		so._calibratorMaxPeriod = 5+1.0/stepsOnPeriod;
		so._calibratorStepsPeriod = size_t(stepsOnPeriod*(so._calibratorMaxPeriod - so._calibratorMinPeriod)+0.5);
		so._calibratorMultPeriodLevel2 = 0.1;
		so._calibratorStepsPhase = 20;

		so._extrapConstPeriods = 1;
		so._extrapPeriods = 2;

		core->addOptimizer(so);
	}




// 
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	core->startUpdate();
// 
// 	real prevCp=-1;
// 	for(bool done = false; !done;)
// 	{
// 		done = core->joinUpdate(1);
// 
// 		real cp, speed;
// 		core->getUpdateStateOptimizers(&cp, &speed, NULL, NULL);
// 		if(cp != prevCp)
// 		{
// 			cout<<pow<<"	eval opt "<<cp<<"\t"<<speed<<endl;
// 			prevCp = cp;
// 		}
// 	}
// 
// 	core->separateSpectrum(0);
// 	exit(0);










	//////////////////////////////////////////////////////////////////////////
	bool calibrateWas = false;
	if(core->getProcessCalibratorsState() != 1)
	{
		calibrateWas = true;
		cout<<pow<<" calibrate "<<core->getProcessCalibratorsState()<<endl;
		while(!core->processCalibrators(100))
		{
			cout<<pow<<" calibrate "<<core->getProcessCalibratorsState()<<endl;
			core->save(stateFileName.c_str());
		}
		cout<<pow<<" calibrate "<<core->getProcessCalibratorsState()<<endl;
		core->save(stateFileName.c_str());
	}

	if(calibrateWas)
	{
		cout<<pow<<" calibrated. Run again"<<std::endl;
		exit(0);
	}

	bool coreStartedEmpty = !core->getLastDataTime();


	//////////////////////////////////////////////////////////////////////////
	TVTimedValue data;
	loadData(inFileName, data);
	if(data.empty())
	{
		std::cout<<pow<<" data absent\n";
		return -1;
	}

	real timeBeg = s2time(TIMEBEG);

	if(timeBeg < core->getLastDataTime())
	{
		timeBeg = core->getLastDataTime();
	}

	unsigned long tick = GetTickCount();

	//////////////////////////////////////////////////////////////////////////
	size_t prevIdx=0;
	for(size_t idx=1; idx<data.size(); idx++)
	{
		if(data[idx]._time < timeBeg)
		{
			continue;
		}

		if(data[idx]._time < data[prevIdx]._time+TIMESTEP)
		{
			continue;
		}

		if(data[idx]._time <= core->getLastDataTime())
		{
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		cout<<dir<<" push "<<idx-prevIdx<<", "<<idx<<endl;

		if(!prevIdx && coreStartedEmpty)
		{

			core->pushData(&data[prevIdx], idx-prevIdx);
			core->save(stateFileName.c_str());
			cout<<pow<<" initial data pushed. Run again"<<std::endl;
			exit(0);
		}
		else
		{
			core->pushData(&data[prevIdx], idx-prevIdx);
		}

		//////////////////////////////////////////////////////////////////////////
		core->startUpdate();

		real prevCp=-1;
		for(bool done = false; !done;)
		{
			done = core->joinUpdate(1);

			real cp, speed;
			core->getUpdateStateOptimizers(&cp, &speed, NULL, NULL);
			if(cp != prevCp)
			{
				cout<<pow<<"	eval opt "<<cp<<"\t"<<speed<<endl;
				prevCp = cp;
			}
		}

		core->separateSpectrum(0);

		prevIdx = idx;
		dumpExtrap(core, dir, data, idx);


		//////////////////////////////////////////////////////////////////////////
//		if(tick + 10*60*1000 < GetTickCount())
// 		{
//  			cout<<dir<<" DO SAVE-------------------------------------------------"<<endl;
//  			core->save(stateFileName.c_str());
//  			tick = GetTickCount();
// 		}
		//////////////////////////////////////////////////////////////////////////

	}



	delete core;
	return 0;
}






















//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	std::cout.precision(20);
	vector<boost::thread *> threads;

	for(int k=10; k<=10; k+=1)
	{
		threads.push_back(new boost::thread(boost::bind(&processOne, "P:\\finance\\quotes\\crosses\\ocs\\EURXAG", k)));
		//SetThreadPriority(threads.back()->native_handle(), THREAD_PRIORITY_IDLE);
		SetThreadPriority(threads.back()->native_handle(), THREAD_PRIORITY_LOWEST);
	}

	for(size_t k(0); k<threads.size(); k++)
	{
		threads[k]->join();
	}
	return 0;
}