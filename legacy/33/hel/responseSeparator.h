#ifndef _hel_responseSeparator_h_
#define _hel_responseSeparator_h_

#include "hel/types.h"
#include "hel/responseModel.h"

namespace hel
{
	struct TAP
	{
		real _t;
		Complex _ap;

		bool operator<(const TAP &with) const
		{
			return _t < with._t;
		}

		//относительное расстояние между краями по периодам
		real hDist(const TAP &with)
		{

			real middlet = (_t+with._t)/2;
  			return fabs(_t-with._t)/middlet;

			real t0 = _t;
			real t1 = with._t;

			//прямая
			real p0 = _ap.p();
			real x0 = 0;
			real y0 = x0 + t0;

			real p1 = alignPhase(with._ap.p(), p0);
			real x1 = (p0-p1)*t1/c_2Pi;
			real y1 = x1 + t1;

			real hDistForw = (fabs(x0-x1) + fabs(y0-y1))/middlet;

			//обратная
			//p0 = _ap.p();
			//x0 = -p0/c_2Pi;
			//y0 = x0 + t0;

			p1 = alignPhase(with._ap.p()+c_pi, p0);

			x1 = (p0-p1)*t1/c_2Pi;
			y1 = x1 + t1;
			real hDistBack = (fabs(x0-x1) + fabs(y0-y1))/middlet;

			if(hDistForw < hDistBack)
			{
				return hDistForw;
			}

			return -hDistBack;
		}
	};
	inline bool operator<(const TAP &tap, const real &t)
	{
		return tap._t < t;
	}
	inline bool operator<(const real &t, const TAP &tap)
	{
		return t<tap._t;
	}

	typedef std::vector<TAP> TVTap;

	//////////////////////////////////////////////////////////////////////////
	class ResponseSeparator
	{
	public:
		ResponseSeparator(
			const TVReal &tVectorIn, 
			const TVComplex &vectorIn,
			ResponseModelPtr responseModel,
			TVReal &tVectorOut, 
			TVComplex &vectorOut);


		virtual ~ResponseSeparator();

		real separate(TVReal &tVectorOutSmoothed, 
			TVComplex &vectorOutSmoothed);


	private:
		TVTap _input;
		TVTap _inputLevel;
		TVTap _output;

		ResponseModelPtr	_responseModel;

		TVReal &_tVectorOut; 
		TVComplex &_vectorOut;

		real _normaK;
		real _normaB;

		real getNorma(real t)
		{
			real res = t*_normaK + _normaB;
			return res;
		}

	private:
		TAP findMaxMatch(const TVTap &vTap, size_t begin, size_t end, size_t &idx);
		real makeInputTail(const TVTap &input, const TVTap &output, TVTap &inputTail);
		bool optimizeOut(
			const TVTap &input, 
			TVTap &output, 
			real minPeriod, 
			real maxPeriod, 
			size_t maxIters=20,
			bool optPeriod = true);
// 		void mutateOut(TVTap &output, real noiseVolume=0.01);
 		real collapseOut_(TVTap &output, real minDist, real minValue);

	};
}


#endif
