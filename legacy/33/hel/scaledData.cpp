#include "stdafx.h"
#include "hel/scaledData.h"
#include "hel/math.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	LevelPtr ScaledData::_getLevel(size_t levelIdx)
	{
		while(_levels.size() <= levelIdx)
		{
			real levelPeriod;
			if(_settings._periodMult)
			{
				levelPeriod = _timeMapper.external2InternalOffset(_settings._zeroPeriod) / pow(_settings._periodMult, (int)levelIdx);
			}
			else
			{
				levelPeriod = 0;
			}
			LevelPtr level(new Level(levelPeriod, levelIdx));
			_levels.push_back(level);
		}

		return _levels[levelIdx];
	}


	//////////////////////////////////////////////////////////////////////////
	ScaledData::ScaledData()
		: _pushCounter(0)
		, _lastTimeExternal(0)
		, _lastValue(0)
		, _lastDerivative(0)
		, _lastDerivative2(0)
		, _lastITime(0)
		, _lastIValue(0)

	{
	}

	//////////////////////////////////////////////////////////////////////////
	void ScaledData::setup(const SettingsScaledData &settings)
	{
		_settings = settings;
		_timeMapper.setup(
			_settings._timeMostPast, _settings._timeZero, 
			-1, 0, 
			_settings._dropWeekends, 
			_settings._weekendBegin);
	}

	//////////////////////////////////////////////////////////////////////////
	const SettingsScaledData &ScaledData::getSettings()const
	{
		return _settings;
	}

	//////////////////////////////////////////////////////////////////////////
	void ScaledData::pushDataIntoLevels(const TimedValue *data, size_t size, size_t keepSamples, real internalZeroPeriod)
	{
		//сгладить сплайнами
		TVTimedValue splinedData;
		splinedData.reserve(size_t((data[size-1]._time-data[0]._time)/internalZeroPeriod + 100));
		if(!_lastITime)
		{
			_lastITime = data[0]._time-internalZeroPeriod;
			_lastIValue = data[0]._value;
		}
		for(size_t i(0); i<size; i++)
		{
			real newDerivative = (data[i]._value - _lastIValue)/2;
			real x1(_lastITime),x2(data[i]._time);
			real args[5];
			bcs4(_lastIValue, 
				data[i]._value, 
				args,
				_lastDerivative, 
				newDerivative,
				_lastDerivative2,
				_lastDerivative2);

			for(real x(_lastITime+internalZeroPeriod); x<data[i]._time+internalZeroPeriod/2; x+=internalZeroPeriod)
			{
				real y = ecs4(x1, x2, x, args);
				TimedValue tv;
				tv._time = x;
				tv._value = y;
				splinedData.push_back(tv);
			}

			_lastITime = data[i]._time;
			_lastIValue = data[i]._value;
			_lastDerivative = newDerivative;
			_lastDerivative2 = _lastDerivative2;
		}
		data = &splinedData.front();
		size = splinedData.size();

//////////////////////////////////////////////////////////////////////////
// 		{
// 			std::ofstream out("c:\\temp\\delMe1");
// 			out.precision(20);
// 			for(int i(0); i<51; i++)
// 			{
// 				out<<data[i]._time<<"\t"<<data[i]._value<<std::endl;
// 			}
// 		}
// 		{
// 			std::ofstream out("c:\\temp\\delMe2");
// 			out.precision(20);
// 			for(int i(0); i<splinedData.size(); i++)
// 			{
// 				out<<splinedData[i]._time<<"\t"<<splinedData[i]._value<<std::endl;
// 			}
// 		}
//////////////////////////////////////////////////////////////////////////


		size_t levelIdx(0);
		LevelPtr lp;

		do
		{
			lp = _getLevel(levelIdx);

			if(lp->empty() && _originalData.size())
			{
				lp->push(&_originalData.front(), _originalData.size(), keepSamples, _originalData.getPeriod());
			}
			lp->push(data, size, keepSamples, internalZeroPeriod);

			levelIdx++;
		}
		//while(lp->size() > 1);
		while(lp->size() >= keepSamples);

		//доложить оригинал
		_originalData.push(data, size, 0, internalZeroPeriod);
		_lastPushedTime = data[size-1]._time;

	}

	//////////////////////////////////////////////////////////////////////////
	void ScaledData::pushData(const TimedValue *data, size_t size, size_t keepSamples)
	{
		//TRATATA
// 		_settings._periodMult = 0.98;
// 		_settings._dropWeekends = true;
// 		_settings._zeroPeriod = 6;
// 		_timeMapper._dropWekends = true;

// 		_settings._timeMostPast = 946684800;
// 		_settings._timeZero = 0;
// 		_timeMapper._mult = 1;
// 		_timeMapper._offset = 0;
//////////////////////////////////////////////////////////////////////////
// 		{
// 			std::ofstream out("c:\\temp\\delMe", std::ios::app);
// 			for(int i(0); i<size; i++)
// 			{
// 				char tmp[256];
// 				sprintf_s(tmp, "%.16g\t%f\n", 
// 					data[i]._time, 
// 					data[i]._value);
// 
// 				out<<tmp;
// 			}
// 		}
//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//setup time mapper
		if(_originalData.empty())
		{
			if(size < 2)
			{
				//first pushed block must be more of 2 size
				return;
			}
		}

		if(!size)
		{
			return;
		}

//////////////////////////////////////////////////////////////////////////
// 		{
// 			std::ofstream out("c:\\temp\\delMe", std::ios::app);
// 			for(int i(0); i<size; i++)
// 			{
// 				time_t t = data[i]._time+0.5;
// 				int isDrop = _timeMapper.isExternalDropable(t);
// 				tm *ptm = gmtime(&t);
// 
// 				char tmp[256];
// 				sprintf_s(tmp, "(%d)\t%04d:%02d:%02d %02d:%02d:%02d\t%d\n", 
// 					ptm->tm_wday, 
// 
// 					ptm->tm_year+1900, 
// 					ptm->tm_mon+1, 
// 					ptm->tm_mday, 
// 
// 					ptm->tm_hour, 
// 					ptm->tm_min, 
// 					ptm->tm_sec,
// 					isDrop);
// 
// 				out<<tmp;
// 			}
// 			out<<"------------------------\n";
// 		}
//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//drop values before already pushed
		real internalZeroPeriod = _timeMapper.external2InternalOffset(_settings._zeroPeriod);

		if(_originalData.empty())
		{
			_originalData.setPeriod(internalZeroPeriod);
		}

		if(_lastTimeExternal)
		{
			while(size && (_lastTimeExternal >= data->_time || _timeMapper.isExternalDropable(data->_time)))
			{
				data++;
				size--;
			}
			if(!size)
			{
				return;
			}
		}
		else
		{
			while(size && _timeMapper.isExternalDropable(data->_time))
			{
				data++;
				size--;
			}
			if(!size)
			{
				return;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//prepare internal data
		TVTimedValue internalData;
//#define MAXINTERNALDATASIZE (800000000/sizeof(TimedValue))
#define MAXINTERNALDATASIZE (800000/sizeof(TimedValue))

		if(_settings._linearFillWhenPush)
		{
			size_t approxNeedDataSize = size_t((data[size-1]._time - data[0]._time)/_settings._zeroPeriod+1.5);

			approxNeedDataSize = std::min(approxNeedDataSize, size_t(MAXINTERNALDATASIZE));
			internalData.reserve(approxNeedDataSize);

			real lastTime = _lastTimeExternal?_lastTimeExternal:data[0]._time - _settings._zeroPeriod;
			lastTime = _timeMapper.external2Internal(lastTime);
			real lastValue = _lastTimeExternal?_lastValue:data[0]._value;

			real boundPeriod = internalZeroPeriod*3/2;
			real smallPartOfPeriod = internalZeroPeriod/100;
			for(size_t i(0); i<size; i++)
			{
				if(_timeMapper.isExternalDropable(data[i]._time))
				{
					continue;
				}
				real time = _timeMapper.external2Internal(data[i]._time);
				if(time - lastTime > boundPeriod)
				{
					for(real timeInner = lastTime+internalZeroPeriod; timeInner < time+smallPartOfPeriod; timeInner+=internalZeroPeriod)
					{
						TimedValue tv;
						tv._time = timeInner;
						tv._value = lastValue + (data[i]._value-lastValue)/(time-lastTime)*(timeInner-lastTime);

						internalData.push_back(tv);

						if(internalData.size() >= MAXINTERNALDATASIZE)
						{
							pushDataIntoLevels(&internalData.front(), internalData.size(), keepSamples, internalZeroPeriod);
							internalData.clear();
						}
					}
				}
				else
				{
					TimedValue tv;
					tv._value = data[i]._value;
					tv._time = time;
					internalData.push_back(tv);
					if(internalData.size() >= MAXINTERNALDATASIZE)
					{
						pushDataIntoLevels(&internalData.front(), internalData.size(), keepSamples, internalZeroPeriod);
						internalData.clear();
					}
				}
				lastTime = time;
				lastValue = data[i]._value;
				lastValue = data[i]._value;
			}
		}
		else
		{
			//no linear fill
			size_t approxNeedDataSize = std::min(size, size_t(MAXINTERNALDATASIZE));
			internalData.reserve(approxNeedDataSize);

			for(size_t i(0); i<size; i++)
			{
				if(_timeMapper.isExternalDropable(data[i]._time))
				{
					continue;
				}
				TimedValue tv;
				tv._value = data[i]._value;
				tv._time = _timeMapper.external2Internal(data[i]._time);
				internalData.push_back(tv);
				if(internalData.size() >= MAXINTERNALDATASIZE)
				{
					pushDataIntoLevels(&internalData.front(), internalData.size(), keepSamples, internalZeroPeriod);
					internalData.clear();
				}
			}
		}

		if(internalData.size())
		{
			pushDataIntoLevels(&internalData.front(), internalData.size(), keepSamples, internalZeroPeriod);
			internalData.clear();
		}

		_pushCounter++;

		_lastTimeExternal = data[size-1]._time;
		_lastValue = data[size-1]._value;

//////////////////////////////////////////////////////////////////////////
// 		{
// 			for(size_t k=0; k<10; k++)
// 			{
// 				char fname[256];
// 				sprintf_s(fname, "_%02d.level", k);
// 				std::ofstream out(fname);
// 
// 				const Level &level = (*_levels[k]);//k?(*_levels[k-1]):_originalData;
// 				for(size_t i(0); i<level.size(); i++)
// 				{
// 					char line[1024];
// 					sprintf_s(line, "%.16g\t%.16g\n", level[i].getTime(), level[i].getValue());
// 					out<<line;
// 				}
// 			}
// 
// 		}
//////////////////////////////////////////////////////////////////////////
	}


	//////////////////////////////////////////////////////////////////////////
	real ScaledData::getLastTime() const
	{
// 		if(_originalData.empty())
// 		{
// 			return 0;
// 		}
// 		return _originalData.back().getTime();
		return _lastPushedTime;
	}

	//////////////////////////////////////////////////////////////////////////
	real ScaledData::getLastTimeExternal() const
	{
		return _lastTimeExternal;
	}

	//////////////////////////////////////////////////////////////////////////
	real ScaledData::getLastValue() const
	{
		return _lastValue;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t ScaledData::getPushCounter() const
	{
		return _pushCounter;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t ScaledData::getLevelsAmount() const
	{
		return _levels.size();
	}

	//////////////////////////////////////////////////////////////////////////
	const Level &ScaledData::getLevel(size_t index) const
	{
		if(index >= _levels.size())
		{
			static Level stub;
			return stub;
		}

		return *_levels[index];
	}

	//////////////////////////////////////////////////////////////////////////
	const Level &ScaledData::getOriginal() const
	{
		return _originalData;
	}

	//////////////////////////////////////////////////////////////////////////
	const TimeMapper &ScaledData::getTimeMapper() const
	{
		return _timeMapper;
	}

}

