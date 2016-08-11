#ifndef _instance_h_
#define _instance_h_

#include "hel/icore.h"

/*
	состав директории экземпляра

	/log.txt
	/progress.txt
	/backups
		/backups/core.YYYY-MM-DD_hh-mm-ss.ser
		
*/
class Instance
{
public:
	Instance();
	virtual ~Instance();
	bool create(const char *path);
	bool destroy();
	bool setParam(const char *name, double value);
	bool setupScaledData();
	bool addOptimizer();
	bool setupSignaller();
	bool pushData(const double *timestamps, const double *values, int size);
	bool startUpdate();
	bool waitUpdate(double sec);
	bool getSignals(
		double *periods,
		double *statisticLengthInPeriods,
		int *statisticLengthInSamples,
		double *tans,
		double *disps,
		double *tansDeriv,
		double *signals,
		int size);

private:
	typedef std::map<std::string, double> TMParams;
	TMParams					_params;
	boost::filesystem::path		_path;

	std::ofstream	_log;
	std::ofstream	_progress;

	hel::ICore		*_helCore;

private:
	static std::string time4log();
#define DOLOG(...) if(_log){_log<<time4log()<<" "<< __VA_ARGS__ << std::endl;}

};

typedef boost::shared_ptr<Instance> InstancePtr;
#endif
