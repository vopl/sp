#include "stdafx.h"
#include "dataIterator.h"

namespace hel
{

	//////////////////////////////////////////////////////////////////////////
	DataIterator::DataIterator()
		: _stepPeriodLog(1)
		, _periodsOnWin(10)
		, _lsq_qq(0)
		, _lsq_pq(0)
		, _lsq_b1(0)
		, _lsq_b2(0)
		, _timeStep(1)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void DataIterator::setup(
		real timeStep, 
		real minPeriod, 
		real maxPeriod, 
		real stepPeriodLog,
		real periodsOnWin)
	{
		_timeStep = timeStep;
		_minPeriodLog = log(minPeriod);
		_maxPeriodLog = log(maxPeriod);
		_stepPeriodLog = stepPeriodLog;
		_periodsOnWin = periodsOnWin;

		_periodGrid.clear();
		_period.clear();
		_value.clear();

		_periodGrid.push_back(exp(_minPeriodLog));

		lsq_init();
		_evaluators.clear();
	}

	//////////////////////////////////////////////////////////////////////////
	void DataIterator::preIterate0()
	{
		lsq_init();
		_evaluators.clear();
		_period.clear();
		_value.clear();
	}

	//////////////////////////////////////////////////////////////////////////
	template <>
	bool DataIterator::iterate<0>(const real &v, size_t idx)
	{
		real x = _timeStep*idx;
		lsq_push(x, v);

		while(_periodGrid.size() <= _period.size())
		{
			_periodGrid.push_back(exp(_minPeriodLog + _stepPeriodLog*_periodGrid.size()));
		}

		real period = x/_periodsOnWin;
		//period += period/(idx+1);
		real m = real(idx+1)/(idx+2);
		while(period >= _periodGrid[_period.size()]*m)
		{
			real period0 = _periodGrid[_period.size()];
			_period.push_back(period0);
			_evaluators.push_back(Evaluator());
			_evaluators.back()._size = idx;

			real step0 = 1.0/_periodsOnWin;
			_evaluators.back()._g0.reset(period0 / _timeStep);
			_evaluators.back()._gm1.reset(period0 /(1.0 - step0) / _timeStep);
			_evaluators.back()._gp1.reset(period0 /(1.0 + step0) / _timeStep);

			lsq_eval(_evaluators.back()._size, _evaluators.back()._k, _evaluators.back()._b);

			if(_periodGrid.size() <= _period.size())
			{
				_periodGrid.push_back(exp(_minPeriodLog + _stepPeriodLog*_periodGrid.size()));
			}
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	void DataIterator::preIterate1()
	{
		if(_evaluators.empty())
		{
			return;
		}

		Evaluator e = _evaluators.back();
		for(;;)
		{
			real periodLog = _minPeriodLog + _stepPeriodLog*_period.size();
			if(periodLog > _maxPeriodLog)
			{
				break;
			}

			real period = exp(periodLog);
			_period.push_back(period);
			_evaluators.push_back(e);
			_evaluators.back()._size = size_t(_periodsOnWin * period / _timeStep + 0.5);

			real step0 = 1.0/_periodsOnWin;
			_evaluators.back()._g0.reset(period / _timeStep);
			_evaluators.back()._gm1.reset(period/(1.0 - step0) / _timeStep);
			_evaluators.back()._gp1.reset(period/(1.0 + step0) / _timeStep);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	template <>
	bool DataIterator::iterate<1>(const real &v, size_t idx)
	{
		real x = _timeStep*idx;
		size_t es = _evaluators.size();
		size_t i(es-1);
		for(; i<es; i--)
		{
			Evaluator &e = _evaluators[i];
			if(e._size < idx)
			{
				break;
			}

			real v1 = v - (e._k*x + e._b);

			e._g0.push(v1, idx);
			e._gp1.push(v1, idx);
			e._gm1.push(v1, idx);
		}
		return i != es-1;
	}

	//////////////////////////////////////////////////////////////////////////
	void DataIterator::postIterate()
	{
		_value.resize(_period.size());
		real step0 = 1.0/_periodsOnWin;
		size_t es = _evaluators.size();
		for(size_t i(0); i<es; i++)
		{
			Evaluator &e = _evaluators[i];

 			_value[i] = 
 				(e._g0.get(e._size)-(e._gm1.get(e._size)*(1-step0)+e._gp1.get(e._size)*(1+step0))/2)*2;
		}
	}



	//////////////////////////////////////////////////////////////////////////
	const TVReal &DataIterator::accessVPeriod() const
	{
		return _period;
	}

	//////////////////////////////////////////////////////////////////////////
	const TVComplex &DataIterator::accessVValue() const
	{
		return _value;
	}

	//////////////////////////////////////////////////////////////////////////
	void DataIterator::lsq_init()
	{
		_lsq_qq = 0;
		_lsq_pq = 0;
		_lsq_b1 = 0;
		_lsq_b2 = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	void DataIterator::lsq_push(real x, real y)
	{
		_lsq_pq += x;
		_lsq_qq += x*x;
		_lsq_b1 += y;
		_lsq_b2 += x*y;
	}
	
	//////////////////////////////////////////////////////////////////////////
	void DataIterator::lsq_eval(size_t size, real &k, real &b)
	{
		real phi = atan2(2*_lsq_pq, _lsq_qq-size)/2;
		real c = cos(phi);
		real s = sin(phi);
		real c2 = c*c;
		real s2 = s*s;

		real d1 = c2*size + s2*_lsq_qq - 2*s*c*_lsq_pq;
		real d2 = s2*size + c2*_lsq_qq + 2*s*c*_lsq_pq;

		real t1 = c*_lsq_b1 - s*_lsq_b2;
		real t2 = s*_lsq_b1 + c*_lsq_b2;

		t1 = t1/d1;
		t2 = t2/d2;

		b = c*t1+s*t2;
		k = c*t2-s*t1;
	}

}
