#ifndef _hel_core_h_
#define _hel_core_h_

#include "hel/icore.h"
#include "hel/scaledData.h"
#include "hel/optimizer.h"



namespace hel
{
	class Core
		: public ICore
	{
		bool	_dataSettedUp;
		size_t	_optimizersAdded;
		bool	_signallerSettedUp;
		bool	_signallerSettedUpInternal;

		SettingsScaledData					_settingsScaledData;

		ScaledData							_scaledData;

		//////////////////////////////////////////////////////////////////////////
		std::vector<Optimizer>				_optimizers;
		struct ProgressState
		{
			real _completePercent;
			real _speed;
			real _elapsedTime;
			real _estimateTime;
			ProgressState()
				: _completePercent(0)
				, _speed(0)
				, _elapsedTime(0)
				, _estimateTime(0)
			{
			}
		};
		std::vector<ProgressState>			_optimizerStates;

		//////////////////////////////////////////////////////////////////////////
		typedef boost::shared_ptr<boost::thread> ThreadPtr;
		struct ThreadStateOptimizer
		{
			ThreadPtr	_thread;
			size_t		_optimizerIndex;
			bool		_highPriority;
		};
		typedef std::map<ThreadPtr, ThreadStateOptimizer>	TMThreadsOptimizer;
		TMThreadsOptimizer					_workerThreadsOptimizer;

		bool								_stopWorkers;
		boost::mutex						_mtx;


	private:
		bool isWorkersExists();
		void threadWorkerOptimizer();
		void updateOptimizerState(ProgressState &state, const Optimizer &optimizer);
		void updateThreadPriorites();


	private:
		virtual ~Core();
		virtual bool setupData(const SettingsScaledData &);

		virtual bool addOptimizer(const SettingsOptimizer&);
		virtual size_t getOptimizersAmount();
		virtual bool processCalibrators(size_t steps);
		virtual real getProcessCalibratorsState();

		virtual bool load(CSZ fName);
		virtual bool save(CSZ fName);

		virtual void pushData(const TimedValue *data, size_t size);
		virtual void pushHstData(CSZ fileName);
		virtual bool readHstData(
			CSZ fileName, 
			TimedValue *data, 
			size_t &size, 
			bool sizeFromEnd, 
			real *zeroPeriod_)const;

		virtual real getLastDataTime();

		virtual size_t startUpdate();
		virtual bool joinUpdate(real timeoutSec);
		virtual void stopUpdate();


		virtual bool getUpdateStateOptimizer(size_t, real *completePercent, real *speed, real *elapsedTime, real *estimateTime);
		virtual bool getUpdateStateOptimizers(real *completePercent, real *speed, real *elapsedTime, real *estimateTime);

		virtual bool separateSpectrum(
			size_t optimizerIndex);

		virtual size_t fillExtrap(
			size_t optimizerIndex,
			real *time,
			real *value,
			size_t size);

		virtual size_t fillSpectr(
			size_t optimizerIndex,
			real *period,
			Complex *value,
			size_t size);

		virtual bool isExternalDropable(real t) const
		{
			return _scaledData.getTimeMapper().isExternalDropable(t);
		}

	public:
		Core();

	};
}

BOOST_CLASS_IMPLEMENTATION(hel::Core::ProgressState, boost::serialization::primitive_type);

#endif
