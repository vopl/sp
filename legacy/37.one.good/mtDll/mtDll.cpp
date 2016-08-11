// mtDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "instance.h"

//////////////////////////////////////////////////////////////////////////
typedef std::map<int, InstancePtr> TMInstances;
TMInstances g_instances;
InstancePtr getInstance(int key)
{
	TMInstances::iterator iter = g_instances.find(key);
	if(g_instances.end() == iter)
	{
		return InstancePtr();
	}

	return iter->second;
}

class Mutex
{
	CRITICAL_SECTION _cs;
public:
	Mutex()
	{
		InitializeCriticalSection(&_cs);
	}
	~Mutex()
	{
		DeleteCriticalSection(&_cs);
	}

	void lock()
	{
		EnterCriticalSection(&_cs);
	}
	void unlock()
	{
		LeaveCriticalSection(&_cs);
	}
};
Mutex gm;

class ScopeLocker
{
public:
	ScopeLocker()
	{
		gm.lock();
	}
	~ScopeLocker()
	{
		gm.unlock();
	}

};


//////////////////////////////////////////////////////////////////////////
#define EXPORT __declspec(dllexport)
#define CALLCNVT __stdcall


//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_createInstance(const char *path)
{
	ScopeLocker l;
	static int nextKey = 3;

	InstancePtr i(new Instance());
	if(!i->create(path))
	{
		return 0;
	}
	g_instances[++nextKey] = i;
	return nextKey;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_destroyInstance(int inst)
{
	ScopeLocker l;
	InstancePtr i = getInstance(inst);
	if(!i) return 0;
	if(!i->destroy()) return 0;
	g_instances.erase(inst);
	return 1;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_setParam(int inst, const char *name, double value)
{
	ScopeLocker l;
	InstancePtr i = getInstance(inst);
	if(!i) return -1;
	if(!i->setParam(name, value)) return 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_setupScaledData(int inst)
{
	ScopeLocker l;
	InstancePtr i = getInstance(inst);
	if(!i) return 0;
	if(!i->setupScaledData()) return 0;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_addOptimizer(int inst)
{
	ScopeLocker l;
	InstancePtr i = getInstance(inst);
	if(!i) return 0;
	if(!i->addOptimizer()) return 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_setupSignaller(int inst)
{
	ScopeLocker l;
	InstancePtr i = getInstance(inst);
	if(!i) return 0;
	if(!i->setupSignaller()) return 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_pushData(int inst, const double *timestamps, const double *values, int size)
{
	ScopeLocker l;
	InstancePtr i = getInstance(inst);
	if(!i) return 0;
	if(!i->pushData(timestamps, values, size)) return 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_startUpdate(int inst)
{
	ScopeLocker l;
	InstancePtr i = getInstance(inst);
	if(!i) return 0;
	if(!i->startUpdate()) return 0;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_waitUpdate(int inst, double sec)
{
	InstancePtr i;
	{
		ScopeLocker l;
		i = getInstance(inst);
		if(!i) return 0;
	}

	bool res = i->waitUpdate(sec);

	{
		ScopeLocker l;
		i.reset();
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////
EXPORT int CALLCNVT hel_getSignals(
	int inst,
	double *periods,
	double *statisticLengthInPeriods,
	int *statisticLengthInSamples,
	double *tans,
	double *disps,
	double *tansDeriv,
	double *signals,
	int size)
{
	ScopeLocker l;
	InstancePtr i = getInstance(inst);
	if(!i) return 0;

	size_t ssize = size;
	size = i->getSignals(
			periods, 
			statisticLengthInPeriods, 
			statisticLengthInSamples,
			tans,
			disps,
			tansDeriv,
			signals,
			size);

	return size;
}

