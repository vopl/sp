#ifndef _hel_dataIterator_h_
#define _hel_dataIterator_h_

#include "hel/types.h"
#include "hel/complex.h"
#include "hel/scaledData.h"
#include "hel/goertzel.h"
#include "hel/convol.h"

namespace hel
{
	//typedef Convol EvaluatorCore;
	typedef Goertzel EvaluatorCore;

	//////////////////////////////////////////////////////////////////////////
	class DataIterator
	{
	public:
		DataIterator();
		void setup(
			real timeStep, 
			real minPeriod, 
			real maxPeriod, 
			real stepPeriodLog,
			real periodsOnWin);

		void preIterate0();
		void preIterate1();
		template <size_t stage>
		bool iterate(const real &v, size_t idx);
		void postIterate();

		const TVReal &accessVPeriod() const;
		const TVComplex &accessVValue() const;

		//////////////////////////////////////////////////////////////////////////
		template <size_t stage>
		class Iterator
		{
			DataIterator &_di;
		public:
			Iterator(DataIterator &di)
				: _di(di)
			{
			}
			Iterator(Iterator &i)
				: _di(i._di)
			{
			}
			bool operator()(const real &v, size_t idx)
			{
				return _di.iterate<stage>(v, idx);
			}
		};

		Iterator<0> mkF0()
		{
			return Iterator<0>(*this);
		}
		Iterator<1> mkF1()
		{
			return Iterator<1>(*this);
		}


	private:
		real		_timeStep;
		real		_minPeriodLog;
		real		_maxPeriodLog;
		real		_stepPeriodLog;
		real		_periodsOnWin;
		TVReal		_periodGrid;

		//////////////////////////////////////////////////////////////////////////
		real _lsq_qq;
		real _lsq_pq;
		real _lsq_b1;
		real _lsq_b2;

		//////////////////////////////////////////////////////////////////////////
		struct Evaluator
		{
			size_t	_size;
			real	_k;
			real	_b;

			EvaluatorCore	_g0;
			EvaluatorCore	_gp1;
			EvaluatorCore	_gm1;
		};
		typedef std::deque<Evaluator> TDEvaluator;
		TDEvaluator _evaluators;

		TVReal		_period;
		TVComplex	_value;

	private:
		void lsq_init();
		void lsq_push(real x, real y);
		void lsq_eval(size_t size, real &k, real &b);

	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _timeStep;
			ar & _minPeriodLog;
			ar & _maxPeriodLog;
			ar & _stepPeriodLog;
			ar & _periodsOnWin;
			ar & _periodGrid;

// 			ar & _lsq_qq;
// 			ar & _lsq_pq;
// 			ar & _lsq_b1;
// 			ar & _lsq_b2;
// 
// 			ar & _evaluators;

			ar & _period;
			ar & _value;
		}

	};
}
#endif
