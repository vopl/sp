#include "stdafx.h"
#include "hel/core.h"
#include <Windows.h>
#undef min
#undef max

#include "hel/mtTypes.h"

BOOST_CLASS_IMPLEMENTATION(hel::SettingsScaledData, boost::serialization::primitive_type);
BOOST_CLASS_IMPLEMENTATION(hel::SettingsOptimizer, boost::serialization::primitive_type);
BOOST_CLASS_IMPLEMENTATION(hel::TimedValue, boost::serialization::primitive_type);

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	bool Core::isWorkersExists()
	{
		return !_workerThreadsOptimizer.empty();
	}

	//////////////////////////////////////////////////////////////////////////
	void Core::threadWorkerOptimizer()
	{
		ThreadPtr thisThread;
		size_t optimizerIndex;

		{
			boost::lock_guard<boost::mutex> lock(_mtx);
			TMThreadsOptimizer::iterator iter = _workerThreadsOptimizer.begin();
			TMThreadsOptimizer::iterator end = _workerThreadsOptimizer.end();

			for(; iter!=end; iter++)
			{
				if(iter->first->get_id() == boost::this_thread::get_id())
				{
					thisThread = iter->first;
					optimizerIndex = iter->second._optimizerIndex;
					break;
				}
			}
		}

		if(!thisThread)
		{
			assert(0);
			return;
		}

		Optimizer &optimizer = _optimizers[optimizerIndex];
		ProgressState &optimizerState = _optimizerStates[optimizerIndex];

		//////////////////////////////////////////////////////////////////////////
		{
			optimizer.prepareWork(_scaledData);
			boost::lock_guard<boost::mutex> lock(_mtx);
			updateOptimizerState(optimizerState, optimizer);
		}
		//////////////////////////////////////////////////////////////////////////
		{
			while(optimizer.workPart(_scaledData))
			{
				boost::lock_guard<boost::mutex> lock(_mtx);

				updateOptimizerState(optimizerState, optimizer);

				if(_stopWorkers)
				{
					break;
				}

				updateThreadPriorites();
			}
		}
		optimizer.terminateWork(_scaledData);

		//////////////////////////////////////////////////////////////////////////
		{
			boost::lock_guard<boost::mutex> lock(_mtx);
			updateOptimizerState(optimizerState, optimizer);

			//cleanup _workerThreads
			_workerThreadsOptimizer.erase(thisThread);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void Core::updateOptimizerState(ProgressState &state, const Optimizer &optimizer)
	{
		state._completePercent = optimizer.getCompletePercent();
		state._elapsedTime = optimizer.getElapsedTime();
		state._estimateTime = optimizer.getEstimateTime();
		state._speed = optimizer.getSpeed();
	}

	//////////////////////////////////////////////////////////////////////////
	void Core::updateThreadPriorites()
	{
		return;
		std::multimap<real, ThreadStateOptimizer *> mm;

		TMThreadsOptimizer::iterator iter = _workerThreadsOptimizer.begin();
		TMThreadsOptimizer::iterator end = _workerThreadsOptimizer.end();
		for(; iter!=end; iter++)
		{
			real estimateTime = _optimizerStates[iter->second._optimizerIndex]._estimateTime;
			mm.insert(std::make_pair(estimateTime, &iter->second));
		}

		unsigned nHighPriorityThreads = boost::thread::hardware_concurrency();
		unsigned index=0;

		std::multimap<real, ThreadStateOptimizer *>::reverse_iterator mmIter = mm.rbegin();
		std::multimap<real, ThreadStateOptimizer *>::reverse_iterator mmEnd = mm.rend();

		for(; mmIter!=mmEnd; mmIter++, index++)
		{
			ThreadStateOptimizer &threadStateOptimizer = *mmIter->second;
			if(index < nHighPriorityThreads || !mmIter->first)
			{
				if(!threadStateOptimizer._highPriority)
				{
					//enable high prio
					SetThreadPriority(threadStateOptimizer._thread->native_handle(), THREAD_PRIORITY_NORMAL);
					threadStateOptimizer._highPriority = true;
				}
			}
			else
			{
				if(threadStateOptimizer._highPriority)
				{
					//disable high prio
					SetThreadPriority(threadStateOptimizer._thread->native_handle(), THREAD_PRIORITY_IDLE);
					threadStateOptimizer._highPriority = false;
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	Core::~Core()
	{
		stopUpdate();
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::setupData(const SettingsScaledData &settings)
	{
		boost::lock_guard<boost::mutex> lock(_mtx);

		if(_dataSettedUp || _optimizersAdded || _signallerSettedUp)
		{
			assert(0);
			return false;
		}

		if(isWorkersExists())
		{
			assert(0);
			return false;
		}
		_optimizers.clear();
		_settingsScaledData = settings;
		_scaledData.setup(_settingsScaledData);

		_dataSettedUp = true;
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::addOptimizer(const SettingsOptimizer &settings)
	{
		boost::lock_guard<boost::mutex> lock(_mtx);

		if(!_dataSettedUp || _signallerSettedUp)
		{
			assert(0);
			return false;
		}

		if(isWorkersExists())
		{
			assert(0);
			return false;
		}
		_optimizers.push_back(Optimizer());
		_optimizers.back().setup(_scaledData, settings);
		_optimizerStates.push_back(ProgressState());
		updateOptimizerState(_optimizerStates.back(), _optimizers.back());

		_optimizersAdded++;
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t Core::getOptimizersAmount()
	{
		boost::lock_guard<boost::mutex> lock(_mtx);
		return _optimizers.size();
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::processCalibrators(size_t steps)
	{
		bool result = true;
		for(size_t i(0); i<_optimizers.size(); i++)
		{
			result &= _optimizers[i].processCalibrator(steps);
		}
		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	real Core::getProcessCalibratorsState()
	{
		real result = 0;
		for(size_t i(0); i<_optimizers.size(); i++)
		{
			result += _optimizers[i].getProcessCalibratorState();
		}
		return result / _optimizers.size();
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::load(CSZ fName)
	{
		boost::lock_guard<boost::mutex> lock(_mtx);

		if(isWorkersExists())
		{
			return false;
		}

		try
		{
			{
				std::ifstream ifs(fName, std::ios::binary);
				boost::archive::binary_iarchive ia(ifs);

				ia >> _dataSettedUp;
				ia >> _optimizersAdded;
				ia >> _signallerSettedUp;
				ia >> _signallerSettedUpInternal;

				ia >> _settingsScaledData;
				ia >> _scaledData;
				ia >> _optimizers;
				ia >> _optimizerStates;
			}
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::save(CSZ fName)
	{
		boost::lock_guard<boost::mutex> lock(_mtx);

		if(isWorkersExists())
		{
			return false;
		}

		try
		{
			{
				std::string tmp = fName;
				tmp += ".tmp";
				std::ofstream ofs(tmp.c_str(), std::ios::binary);
				boost::archive::binary_oarchive oa(ofs);

				oa << _dataSettedUp;
				oa << _optimizersAdded;
				oa << _signallerSettedUp;
				oa << _signallerSettedUpInternal;

				oa << _settingsScaledData;
				oa << _scaledData;
				oa << _optimizers;
				oa << _optimizerStates;

				ofs.close();

 				if(boost::filesystem::is_regular_file(fName))
 				{
 					boost::filesystem::remove(fName);
 				}
				boost::filesystem::rename(tmp, fName);
			}
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	void Core::pushData(const TimedValue *data, size_t size)
	{
		if(!size)
		{
			return;
		}
		boost::lock_guard<boost::mutex> lock(_mtx);
		if(isWorkersExists())
		{
			assert(0);
			return;
		}

		size_t keepSamples = 0;
		for(size_t i(0); i<_optimizers.size(); i++)
		{
			keepSamples = std::max(keepSamples, _optimizers[i].getNeedDataSamples());
		}
		_scaledData.pushData(data, size, keepSamples);
	}

	//////////////////////////////////////////////////////////////////////////
	void Core::pushHstData(CSZ fileName)
	{
		if(!boost::filesystem::is_regular_file(fileName))
		{
			return;
		}

		boost::lock_guard<boost::mutex> lock(_mtx);
		if(isWorkersExists())
		{
			assert(0);
			return;
		}

		size_t keepSamples = 0;
		for(size_t i(0); i<_optimizers.size(); i++)
		{
			keepSamples = std::max(keepSamples, _optimizers[i].getNeedDataSamples());
		}

		size_t size = size_t(boost::filesystem::file_size(fileName));
		size = (size - sizeof(HistoryHeader))/sizeof(RateInfo);
		TVTimedValue data(size);

		if(!readHstData(fileName, &data.front(), size, false, &_settingsScaledData._zeroPeriod))
		{
			assert(0);
			return;
		}
		_scaledData.pushData(&data.front(), size, keepSamples);
	}
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	bool Core::readHstData(
		CSZ fileName, 
		TimedValue *data, 
		size_t &size, 
		bool sizeFromEnd, 
		real *zeroPeriod_)const
	{
		real zeroPeriod = zeroPeriod_?*zeroPeriod_:0;
		std::ifstream ifstr(fileName, std::ios::binary);
		if(!ifstr)
		{
			assert(0);
			return false;
		}
		ifstr.seekg(0, std::ios::end);
		size_t fileSize = ifstr.tellg();
		ifstr.seekg(0, std::ios::beg);

		if(fileSize<sizeof(HistoryHeader))
		{
			return false;
		}
		if((fileSize-sizeof(HistoryHeader))%sizeof(RateInfo))
		{
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		HistoryHeader hh;
		ifstr.read((char*)&hh, sizeof(hh));

		size_t recordsAmount = (fileSize-sizeof(HistoryHeader))/sizeof(RateInfo);
		size_t recordIndex = 0;
		size_t recordEnd = recordsAmount;

		if(recordEnd > size)
		{
			if(sizeFromEnd)
			{
				recordIndex = recordEnd - size;
			}
			else
			{
				recordEnd = size;
			}
			recordsAmount = recordEnd - recordIndex;
			ifstr.seekg(sizeof(HistoryHeader) + recordIndex*sizeof(RateInfo));
			recordEnd -= recordIndex;
			recordIndex = 0;
		}

		size = recordsAmount;

		std::vector<RateInfo> ris(recordsAmount);
		ifstr.read((char *)&ris.front(), recordsAmount*sizeof(RateInfo));
		ifstr.close();

		//////////////////////////////////////////////////////////////////////////
		//*
		real period = zeroPeriod;
		if(!period)
		{
			period = 60.0 * hh.period;
		}
		else
		{
			//period = period;
		}

		for(size_t i(0); recordIndex<recordEnd; recordIndex++,i++)
		{
			RateInfo &ri = ris[recordIndex];
			data[i]._time = ris[recordIndex].ctm;// + period/2;
			data[i]._value = (ris[recordIndex].open + ris[recordIndex].close + ris[recordIndex].high + ris[recordIndex].low)/4;
		}

		if(zeroPeriod_)
		{
			*zeroPeriod_ = zeroPeriod;
		}

		return true;
	}


	//////////////////////////////////////////////////////////////////////////
	real Core::getLastDataTime()
	{
		boost::lock_guard<boost::mutex> lock(_mtx);
		if(isWorkersExists())
		{
			assert(0);
			return nullReal;
		}
		return _scaledData.getLastTimeExternal();
	}

	//////////////////////////////////////////////////////////////////////////
	size_t Core::startUpdate()
	{
		boost::lock_guard<boost::mutex> lock(_mtx);

		if(!_optimizersAdded)
		{
			return false;
		}

		if(isWorkersExists())
		{
			assert(0);
			return false;
		}
		_stopWorkers = false;

		size_t nThreads = _optimizers.size();
		while(_workerThreadsOptimizer.size() < nThreads)
		{
			ThreadStateOptimizer threadState;
			threadState._optimizerIndex = _workerThreadsOptimizer.size();
			threadState._highPriority = false;
			threadState._thread = ThreadPtr(new boost::thread(boost::bind(&Core::threadWorkerOptimizer, this)));

			_workerThreadsOptimizer[threadState._thread] = threadState;
#ifdef WIN32
			SetThreadPriority(threadState._thread->native_handle(), THREAD_PRIORITY_IDLE);
#else
#	pragma message("ATTENTION!!! write code for low-priority worker threads here")
#endif
		}

		return nThreads;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::joinUpdate(real timeoutSec)
	{
		unsigned long amount;
		HANDLE staticBuf[1024];
		HANDLE *handles = staticBuf;

		TMThreadsOptimizer	workerThreads;
		{
			boost::lock_guard<boost::mutex> lock(_mtx);
			amount = _workerThreadsOptimizer.size();
			if(!amount)
			{
				return true;
			}

			if(amount > 1024)
			{
				handles = new HANDLE[amount];
			}

			TMThreadsOptimizer::iterator iterO = _workerThreadsOptimizer.begin();
			TMThreadsOptimizer::iterator endO = _workerThreadsOptimizer.end();

			size_t i(0);
			for(; iterO!=endO; i++, iterO++)
			{
				handles[i] = iterO->first->native_handle();
			}
		}

		WaitForMultipleObjects(amount, handles, TRUE, unsigned long(timeoutSec*1000));
		if(staticBuf != handles)
		{
			delete [] handles;
		}

		boost::lock_guard<boost::mutex> lock(_mtx);
		return !isWorkersExists();
	}

	//////////////////////////////////////////////////////////////////////////
	void Core::stopUpdate()
	{
		std::vector<ThreadPtr> workerThreads;
		{
			boost::lock_guard<boost::mutex> lock(_mtx);
			_stopWorkers = true;

			TMThreadsOptimizer::iterator iterO = _workerThreadsOptimizer.begin();
			TMThreadsOptimizer::iterator endO = _workerThreadsOptimizer.end();
			for(; iterO!=endO; iterO++)
			{
				workerThreads.push_back(iterO->first);
			}
		}

		for(size_t i(0); i<workerThreads.size(); i++)
		{
			workerThreads[i]->join();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::getUpdateStateOptimizer(
		size_t index, 
		real *completePercent, 
		real *speed, 
		real *elapsedTime, 
		real *estimateTime)
	{
		boost::lock_guard<boost::mutex> lock(_mtx);
		if(_optimizerStates.size() > index)
		{
			if(completePercent)
			{
				*completePercent = _optimizerStates[index]._completePercent;
			}
			if(speed)
			{
				*speed = _optimizerStates[index]._speed;
			}
			if(elapsedTime)
			{
				*elapsedTime = _optimizerStates[index]._elapsedTime;
			}
			if(estimateTime)
			{
				*estimateTime = _optimizerStates[index]._estimateTime;
			}

			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::getUpdateStateOptimizers(real *completePercent_, real *speed_, real *elapsedTime_, real *estimateTime_)
	{
		boost::lock_guard<boost::mutex> lock(_mtx);

		real completePercent=0;
		real speed=0;
		real elapsedTime=0;
		real estimateTime=0;

		for(size_t i(0); i<_optimizerStates.size(); i++)
		{
			elapsedTime += _optimizerStates[i]._elapsedTime;
			estimateTime += _optimizerStates[i]._estimateTime;

			real totalTime = _optimizerStates[i]._elapsedTime + _optimizerStates[i]._estimateTime;

			completePercent += _optimizerStates[i]._completePercent * totalTime;
			speed += _optimizerStates[i]._speed * totalTime;
		}

		real totalTime = elapsedTime+estimateTime;
		if(totalTime)
		{
			completePercent /= totalTime;
			speed /= totalTime;
		}
		else
		{
			completePercent = 0;
			speed = 0;
		}

		if(completePercent_)
		{
			*completePercent_ = completePercent;
		}
		if(speed_)
		{
			*speed_ = speed;
		}
		if(elapsedTime_)
		{
			*elapsedTime_ = elapsedTime;
		}
		if(estimateTime_)
		{
			*estimateTime_ = estimateTime;
		}


		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::separateSpectrum(
		size_t optimizerIndex)
	{
		boost::lock_guard<boost::mutex> lock(_mtx);
		if(isWorkersExists())
		{
			return false;
		}

		if(_optimizers.size() <= optimizerIndex)
		{
			return false;
		}

		_optimizers[optimizerIndex].separateSpectrum(_scaledData);
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t Core::fillExtrap(
		size_t optimizerIndex,
		real *time,
		real *value,
		size_t size)
	{
 		boost::lock_guard<boost::mutex> lock(_mtx);
 		if(isWorkersExists())
 		{
 			return 0;
 		}

 		if(_optimizers.size() <= optimizerIndex)
 		{
 			return 0;
 		}
 
 		const TimeMapper &tm = _scaledData.getTimeMapper();

		for(size_t i(0); i<size; i++)
		{
			time[i] = tm.external2Internal(time[i]);
		}

		size_t res = _optimizers[optimizerIndex].fillExtrap(_scaledData, time, value, size);

		for(size_t i(0); i<size; i++)
		{
			time[i] = tm.internal2External(time[i]);
		}

		return  res;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t Core::fillSpectr(
		size_t optimizerIndex,
		real *period,
		Complex *value,
		size_t size)
	{
		boost::lock_guard<boost::mutex> lock(_mtx);
		if(isWorkersExists())
		{
			return 0;
		}

		if(_optimizers.size() <= optimizerIndex)
		{
			return 0;
		}

		TVReal &p = _optimizers[optimizerIndex].getSpectrPeriod();
		TVComplex &v = _optimizers[optimizerIndex].getSpectrValue();

		size_t s = std::min(size, p.size());

		const TimeMapper &tm = _scaledData.getTimeMapper();
		for(size_t i(0); i<s; i++)
		{
			period[i] = tm.internal2ExternalOffset(p[i]);
			value[i] = v[i];
		}

		for(size_t i(s); i<size; i++)
		{
			period[i] = 0;
			value[i].set(0,0);
		}

		return s;
	}

	//////////////////////////////////////////////////////////////////////////
	Core::Core()
		: _dataSettedUp(false)
		, _optimizersAdded(0)
		, _signallerSettedUp(false)
		, _signallerSettedUpInternal(false)
	{
	}


}
