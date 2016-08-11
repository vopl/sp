#ifndef _hel_icore_h_
#define _hel_icore_h_

#include "hel/settings.h"
#include "hel/complex.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	class ICore
	{
	public:
		virtual ~ICore()=0{};
		virtual bool setupData(const SettingsScaledData &)=0;
		virtual bool addOptimizer(const SettingsOptimizer&)=0;
		virtual size_t getOptimizersAmount()=0;

		virtual bool processCalibrators(size_t steps)=0;
		virtual real getProcessCalibratorsState()=0;

		virtual bool load(CSZ fName)=0;
		virtual bool save(CSZ fName)=0;

		virtual void pushData(const TimedValue *data, size_t size)=0;
		virtual void pushHstData(CSZ fileName)=0;
		virtual bool readHstData(
			CSZ fileName, 
			TimedValue *data, 
			size_t &size, 
			bool sizeFromEnd, 
			real *zeroPeriod_)const=0;

		virtual real getLastDataTime()=0;

		virtual size_t startUpdate()=0;
		virtual bool joinUpdate(real timeoutSec)=0;
		virtual void stopUpdate()=0;


		virtual bool getUpdateStateOptimizer(size_t, real *completePercent, real *speed, real *elapsedTime, real *estimateTime)=0;
		virtual bool getUpdateStateOptimizers(real *completePercent, real *speed, real *elapsedTime, real *estimateTime)=0;

		virtual bool separateSpectrum(
			size_t optimizerIndex)=0;

		virtual size_t fillExtrap(
			size_t optimizerIndex,
			real *time,
			real *value,
			size_t size)=0;

		virtual size_t fillSpectr(
			size_t optimizerIndex,
			real *period,
			Complex *value,
			size_t size)=0;


		virtual bool isExternalDropable(real) const = 0;
	};

	//////////////////////////////////////////////////////////////////////////
#ifdef CORE_EXPORTS
	__declspec(dllexport)
#else
	__declspec(dllimport)
#endif
	ICore *createICore();
}

#endif
