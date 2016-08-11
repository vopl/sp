#ifndef _hel_approx_h_
#define _hel_approx_h_

#include "hel/types.h"
#include "hel/approxResult.h"
#include "levmar-2.3/lm.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	template <class Law>
	class Approx
		: public ApproxResult
	{
		struct SEvalFuncParams
		{
			TVLeveledValue::const_iterator _valuesIter;
			real					_a;
			real					_b;
		};
	protected:
		static void evalFunc(double *p, double *hx, int m, int n, void *adata);
		static void jacoFunc(double *p, double *jx, int m, int n, void *adata);

	protected:
		static real _levmarOpts[LM_OPTS_SZ];
	public:
		bool probe(
			TVLeveledValue::const_iterator valuesIter, 
			size_t size, 
			const LawParams &lawParams, 
			TVReal &threadStorage);
	};



	//////////////////////////////////////////////////////////////////////////
	template <class Law>
	real Approx<Law>::_levmarOpts[LM_OPTS_SZ] = 
	{
		1E-40, //LM_INIT_MU,//mu
		1E-60, //LM_STOP_THRESH,	//stopping thresholds for ||J^T e||_inf, 
		1E-60, //LM_STOP_THRESH,	//||Dp||_2 and 
		1E-60, //LM_STOP_THRESH,	//||e||_2. Set to NULL for defaults to be used.






// 		LM_INIT_MU,//mu
// 		LM_STOP_THRESH,	//stopping thresholds for ||J^T e||_inf, 
// 		LM_STOP_THRESH,	//||Dp||_2 and 
// 		LM_STOP_THRESH,	//||e||_2. Set to NULL for defaults to be used.
// 		LM_DIFF_DELTA
	};


	//////////////////////////////////////////////////////////////////////////
	template <class Law>
	bool Approx<Law>::probe(
		TVLeveledValue::const_iterator valuesIter, 
		size_t size, 
		const LawParams &lawParams, 
		TVReal &threadStorage)
	{
		assert(size > 1);
		if(size < 2)
		{
			//_error = std::numeric_limits<real>::max();
			//return _error;
			return false;
		}



		real	args[Law::argumentsAmount];
		Law::prepareArgs(valuesIter, size, lawParams, args);

		SEvalFuncParams efp;
		efp._valuesIter = valuesIter;
		efp._a = args[Law::argumentsAmount-2];
		efp._b = args[Law::argumentsAmount-1];

		real multStep = pow(lawParams._periodMult, lawParams._periodOnNearLevels);
		real fMin = args[0]*multStep;
		real fMax = args[0]/multStep;

		threadStorage.resize(LM_DER_WORKSZ(Law::argumentsAmount, size));

		real info[LM_INFO_SZ];
		real levmarOpts[LM_OPTS_SZ];
		memcpy(levmarOpts, _levmarOpts, sizeof(_levmarOpts));

		//////////////////////////////////////////////////////////////////////////
		bool exist = false;
		for(size_t iter=0; iter < 100; iter++)
		{
			/* O: information regarding the minimization. Set to NULL if don't care
			* info[0]= ||e||_2 at initial p.
			* info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
			* info[5]= # iterations,
			* info[6]=reason for terminating: 1 - stopped by small gradient J^T e
			*                                 2 - stopped by small Dp
			*                                 3 - stopped by itmax
			*                                 4 - singular matrix. Restart from current p with increased mu 
			*                                 5 - no further error reduction is possible. Restart with increased mu
			*                                 6 - stopped by small ||e||_2
			*                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
			* info[7]= # function evaluations
			* info[8]= # Jacobian evaluations
			*/
			int dmdRes = dlevmar_der(
				evalFunc, 
				jacoFunc, 
				args, 
				NULL, 
				Law::argumentsAmountOpt, 
				(int)size, 
				(int)(5*(iter+1)), 
				levmarOpts, 
				info, 
				&threadStorage.front(), 
				NULL, 
				(void *)&efp);

			if(dmdRes < 0)//LM_ERROR
			{
				break;
			}

			if(args[0] > fMax || args[0] < fMin)
			{
				exist = false;
				break;
			}
			Law::correctArgs(valuesIter, size, args);

			switch(int(info[6]+0.1))
			{
			case 1://1 - stopped by small gradient J^T e
				exist = true;
				iter = -3;
				break;
			case 2://2 - stopped by small Dp
				exist = true;
				iter = -3;
				break;
			case 3://3 - stopped by itmax
				break;
			case 4://4 - singular matrix. Restart from current p with increased mu 
				levmarOpts[0] *= 10;
				if(levmarOpts[0]<0.1)
				{
					iter = 0;
				}
				else
				{
					iter = -3;
				}
				break;
			case 5://5 - no further error reduction is possible. Restart with increased mu
				levmarOpts[0] *= 10;
				if(levmarOpts[0]<0.1)
				{
					iter = 0;
				}
				else
				{
					iter = -3;
				}
				break;
			case 6://6 - stopped by small ||e||_2
				exist = true;
				iter = -3;
				break;
			case 7://7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
				assert(0);
				iter = -3;
				break;
			}
		}

		if(exist)
		{
			if(
				_isnan(args[0]) || !_finite(args[0]) ||
				_isnan(args[1]) || !_finite(args[1]) ||
				_isnan(args[2]) || !_finite(args[2]) ||
				0)
			{
				return false;
			}

			_period = args[0];
			_phase = args[1];
			_amplitude = args[2];

			_timeStart = valuesIter->getTime();
			_timeStop = (valuesIter+(size-1))->getTime();

			return true;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	template <class Law>
	void Approx<Law>::evalFunc(double *p, double *hx, int m, int n, void *adata)
	{
		assert(Law::argumentsAmountOpt == m);
		SEvalFuncParams *efp = (SEvalFuncParams *)adata;
		TVLeveledValue::const_iterator valuesIter = efp->_valuesIter;
		for(int i=0; i<n; i++, valuesIter++)
		{
			hx[i] = Law::eval(valuesIter->getTime(), p, efp->_a, efp->_b) - valuesIter->getValue();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	template <class Law>
	void Approx<Law>::jacoFunc(double *p, double *jx, int m, int n, void *adata)
	{
		assert(Law::argumentsAmountOpt == m);
		SEvalFuncParams *efp = (SEvalFuncParams *)adata;
		TVLeveledValue::const_iterator valuesIter = efp->_valuesIter;
		for(int i=0; i<n; i++, valuesIter++)
		{
			Law::grad(valuesIter->getTime(), p, jx+i*m, efp->_a, efp->_b);
		}
	}
}
#endif

