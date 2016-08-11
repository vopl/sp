#ifndef _hel_core_h_
#define _hel_core_h_

#include "hel/icore.h"
#include "hel/scaledData.h"
#include "hel/optimizer.h"
#include "hel/timeMapper.h"



namespace hel
{
	class Core
		: public ICore
	{
		TimeMapper					_timeMapper;
		ScaledData					_scaledData;
		real						_lastUpdateX0;
		std::vector<OptimizerPtr>	_optimizers;
		TVSize_t					_optimizerPows;
		TVReal						_optimizerWeights;


	private:
		virtual ~Core();
		virtual bool setup(const SettingsTimeMapper &stm, const SettingsScaledData &ssd);

		virtual bool addOptimizer(const SettingsOptimizer &s);
		virtual size_t getOptimizersAmount();

		virtual bool load(CSZ fName);
		virtual bool save(CSZ fName);

		virtual void pushData(const TimedValue *data, size_t size);

		virtual real getLastUpdateDataTime();

		virtual bool needUpdate();
		virtual bool update(real step, bool response=true, bool spectr=true);

		virtual size_t fillResponse(
			size_t optimizerIndex,
			real *period,
			Complex *value,
			size_t size);


		virtual size_t fillSpectr(
			size_t optimizerIndex,
			real *period,
			Complex *value,
			size_t size);

		virtual real getSeparatorError(size_t optimizerIndex);

		virtual bool isExternalDropable(real t) const
		{
			return _timeMapper.isExternalDropable(t);
		}

		virtual real external2Internal(real t) const
		{
			return _timeMapper.external2Internal(t);
		}
		
		virtual real internal2External(real t) const
		{
			return _timeMapper.internal2External(t);
		}

	public:
		Core();

	private:

	};
}

#endif
