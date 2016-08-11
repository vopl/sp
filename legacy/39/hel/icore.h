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
		virtual bool setup(
			const SettingsTimeMapper &, 
			const SettingsScaledData &)=0;
		virtual bool addOptimizer(const SettingsOptimizer &s)=0;
		virtual size_t getOptimizersAmount()=0;

		virtual bool load(CSZ fName)=0;
		virtual bool save(CSZ fName)=0;

		virtual void pushData(const TimedValue *data, size_t size)=0;

		virtual real getLastUpdateDataTime()=0;
		
		virtual bool needUpdate()=0;
		virtual bool update(real step, bool response=true, bool spectr=true)=0;

		virtual size_t fillResponse(
			size_t optimizerIndex,
			real *period,
			Complex *value,
			size_t size)=0;


		virtual size_t fillSpectr(
			size_t optimizerIndex,
			real *period,
			Complex *value,
			size_t size)=0;

		virtual real getSeparatorError(size_t optimizerIndex) = 0;

		virtual bool isExternalDropable(real) const = 0;
		virtual real external2Internal(real) const = 0;
		virtual real internal2External(real) const = 0;

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
