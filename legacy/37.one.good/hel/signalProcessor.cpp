#include "stdafx.h"
#include "hel/signalProcessor.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	OnePeriodProcessor::OnePeriodProcessor()
		: _period(1)
		, _winLen(1)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void OnePeriodProcessor::setup(real period, real periodsOnWin)
	{
		_period = period;
		_winLen = _period * periodsOnWin;

		_step0 = 1.0 / periodsOnWin;
		_part_p1.setup(period /(1.0+_step0/2));
		_part_m1.setup(period /(1.0-_step0/2));
	}

	//////////////////////////////////////////////////////////////////////////
	Complex OnePeriodProcessor::eval(const TDTimedValue &signal)
	{
		real xStop = signal.back()._time;
		real xStart = xStop - _winLen;
		if(xStart < signal.front()._time)
		{
			//assert(0);//окно всегда должно помещатся в сигнал
			xStart = signal.front()._time;
		}

		assert(xStart <= xStop);
		assert(xStart >= signal.front()._time);
		assert(xStop <= signal.back()._time);

		TDTimedValue::const_iterator iter1 = std::lower_bound(signal.begin(), signal.end(), xStart);
		TDTimedValue::const_iterator iter0;
		if(iter1->_time > xStart)
		{
			iter0 = iter1-1;
		}
		else
		{
			iter0 = iter1;
			iter1++;
		}
		TDTimedValue::const_iterator end = signal.end();

		Complex res_m1;
		Complex res_p1;


		while(iter1 != end)
		{
			const TimedValue &tv0 = *iter0;
			const TimedValue &tv1 = *iter1;

			if(xStart <= tv0._time && xStop >= tv1._time)
			{
				res_m1 += _part_m1.evalSegment(xStop, tv0, tv1, tv0._time, tv1._time);
				res_p1 += _part_p1.evalSegment(xStop, tv0, tv1, tv0._time, tv1._time);
			}
			else
			{
				real start = std::max(xStart, tv0._time);
				real stop = std::min(xStop, tv1._time);
				res_m1 += _part_m1.evalSegment(xStop, tv0, tv1, start, stop);
				res_p1 += _part_p1.evalSegment(xStop, tv0, tv1, start, stop);
			}

			if(xStop<=tv1._time)
			{
				break;
			}
			iter0 = iter1++;
		}

		return (res_p1*(1.0+_step0/2) - res_m1*(1.0-_step0/2)) / _winLen;
	}










	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	SignalProcessor::SignalProcessor()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void SignalProcessor::setup(
		real minPeriodE, 
		real maxPeriodE, 
		size_t stepsPeriodE, 
		size_t periodsOnWin)

	{

		_periodsOnWin = periodsOnWin;

		real minTLog = log(minPeriodE);
		real maxTLog = log(maxPeriodE);
		real logtStep = (maxTLog - minTLog) / (stepsPeriodE);
		_periodLogE.resize(stepsPeriodE);
		for(size_t k(0); k<stepsPeriodE; k++)
		{
			_periodLogE[k] = minTLog+k*logtStep;
		}
		_valueE.resize(_periodLogE.size());

		_processors.clear();
		_processors.resize(_periodLogE.size());

		for(size_t i(0); i<_processors.size(); i++)
		{
			_processors[i].setup(exp(_periodLogE[i]), _periodsOnWin);
		}

	}

	//////////////////////////////////////////////////////////////////////////
	void SignalProcessor::update(const TDTimedValue &signal)
	{
		for(size_t i(0); i<_processors.size(); i++)
		{
			_valueE[i] = _processors[i].eval(signal);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	const TVReal &SignalProcessor::getPeriodLogE() const
	{
		return _periodLogE;
	}

	//////////////////////////////////////////////////////////////////////////
	const TVComplex &SignalProcessor::getValueE() const
	{
		return _valueE;
	}
}