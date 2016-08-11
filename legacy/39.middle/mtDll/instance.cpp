#include "stdafx.h"
#include "instance.h"









//////////////////////////////////////////////////////////////////////////
Instance::Instance()
	: _helCore(NULL)
{
#define PARAM_DEF(c,n,t,v) _params[#c "._" #n] = v;
#include "params.defs"
}

//////////////////////////////////////////////////////////////////////////
Instance::~Instance()
{
	destroy();
}

//////////////////////////////////////////////////////////////////////////
bool Instance::create(const char *path)
{
	destroy();
	if(!path || !path[0])
	{
		return false;
	}

	_path = path;
	if(!boost::filesystem::is_directory(_path))
	{
		boost::filesystem::create_directories(_path);
	}
	if(!boost::filesystem::is_directory(_path))
	{
		return false;
	}

	_log.close();
	_log.clear();
	_log.open((_path/"log.txt").string().c_str());
	if(!_log)
	{
		return false;
	}
	DOLOG("create");

	_progress.close();
	_progress.clear();
	_progress.open((_path/"progress.txt").string().c_str());
	if(!_progress)
	{
		_log.close();
		_log.clear();
		DOLOG("failed open progress file");
		return false;
	}

	_helCore = hel::createICore();
	if(!_helCore)
	{
		DOLOG("failed create hel core");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Instance::destroy()
{
	DOLOG("destroy");
	if(_helCore)
	{
		delete _helCore;
		_helCore = NULL;
	}

	assert(0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Instance::setParam(const char *name, double value)
{
	DOLOG("set param "<<name<<"="<<value);
#define PARAM_DEF(c,n,t,v) if(!strcmp(#c "._" #n, name)){_params[name] = value; return true;}
#include "params.defs"
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Instance::setupScaledData()
{
	DOLOG("setup scaled data");
	if(!_helCore)
	{
		DOLOG("failed, no hel core");
		return false;
	}

	hel::SettingsScaledData ssd;
#define PARAM_DEF_ssd(n,t,v) ssd._##n = t(_params["ssd._" #n]);
#define PARAM_DEF_so(n,t,v)
#define PARAM_DEF_ss(n,t,v)
#define PARAM_DEF_g(n,t,v)
#define PARAM_DEF(c,n,t,v) PARAM_DEF_##c (n,t,v)
#include "params.defs"
#undef PARAM_DEF_ssd
#undef PARAM_DEF_so
#undef PARAM_DEF_ss
#undef PARAM_DEF_g

	return _helCore->setupData(ssd);
}

//////////////////////////////////////////////////////////////////////////
bool Instance::addOptimizer()
{
	DOLOG("add optimizer");
	if(!_helCore)
	{
		DOLOG("failed, no hel core");
		return false;
	}

	hel::SettingsOptimizer so;
#define PARAM_DEF_ssd(n,t,v)
#define PARAM_DEF_so(n,t,v) so._##n = t(_params["so._" #n]);
#define PARAM_DEF_ss(n,t,v)
#define PARAM_DEF_g(n,t,v)
#define PARAM_DEF(c,n,t,v) PARAM_DEF_##c (n,t,v)
#include "params.defs"
#undef PARAM_DEF_ssd
#undef PARAM_DEF_so
#undef PARAM_DEF_ss
#undef PARAM_DEF_g

	return _helCore->addOptimizer(so);
}

//////////////////////////////////////////////////////////////////////////
bool Instance::setupSignaller()
{
	DOLOG("setup signaller");
	if(!_helCore)
	{
		DOLOG("failed, no hel core");
		return false;
	}

	hel::SettingsSignaller ss;
#define PARAM_DEF_ssd(n,t,v)
#define PARAM_DEF_so(n,t,v)
#define PARAM_DEF_ss(n,t,v) ss._##n = t(_params["ss._" #n]);
#define PARAM_DEF_g(n,t,v)
#define PARAM_DEF(c,n,t,v) PARAM_DEF_##c (n,t,v)
#include "params.defs"
#undef PARAM_DEF_ssd
#undef PARAM_DEF_so
#undef PARAM_DEF_ss
#undef PARAM_DEF_g

	return _helCore->setupSignaller(ss);
}

//////////////////////////////////////////////////////////////////////////
bool Instance::pushData(const double *timestamps, const double *values, int size)
{
	DOLOG("push data "<<size);
	if(!_helCore)
	{
		DOLOG("failed, no hel core");
		return false;
	}

	assert(0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Instance::startUpdate()
{
	assert(0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Instance::waitUpdate(double sec)
{
	assert(0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Instance::getSignals(
				double *periods,
				double *statisticLengthInPeriods,
				int *statisticLengthInSamples,
				double *tans,
				double *disps,
				double *tansDeriv,
				double *signals,
				int size)
{
	assert(0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
std::string Instance::time4log()
{
	time_t t = time(NULL);
	tm stm;
	localtime_s(&stm, &t);

	char tmp[1024];
	sprintf_s(tmp, "%04d-%02d-%02d %02d:%02d:%02d", 
		stm.tm_year+1900, 
		stm.tm_mon+1,
		stm.tm_mday,
		stm.tm_hour,
		stm.tm_min,
		stm.tm_sec);

	return tmp;
}
