#ifndef _hel_goertzel_h_
#define _hel_goertzel_h_

#include "hel/types.h"
#include "hel/complex.h"
#include "hel/math.h"

namespace hel
{
//#define GOERTZEL_USE_COS

	class Goertzel
	{
#ifdef GOERTZEL_USE_COS
		real _sop;
		Complex	_sum;
#else
		real _cosine;
		real _sine;
		real _coeff;
		real _Q0, _Q1, _Q2;
#endif

	public:
#ifdef GOERTZEL_USE_COS
		void reset(real sop)
		{
			_sop = sop;
			_sum.set(0, 0);
		}
		void push(real v, size_t idx)
		{
			real x = c_2Pi * (idx++) / _sop;
			_sum.re() += cos(x) * v;
			_sum.im() -= sin(x) * v;
		}

		Complex get(size_t amount)
		{
			return _sum / amount;
		}

#else

		void reset(real sop)
		{
			if(sop)
			{
				real w = c_2Pi/sop;
				_cosine = cos(w);
				_sine = sin(w);
			}
			else
			{
				_cosine = 0;
				_sine = 0;
			}
			_coeff = 2 * _cosine;

			_Q0=0;
			_Q1=0;
			_Q2=0;
		}

		void reset2(real pos)
		{
			real w = c_2Pi*pos;
			_cosine = cos(w);
			_sine = sin(w);

			_coeff = 2 * _cosine;

			_Q0=0;
			_Q1=0;
			_Q2=0;
		}

		void push(real v, size_t idx)
		{
			_Q0 = _coeff * _Q1 - _Q2 + v;
			_Q2 = _Q1;
			_Q1 = _Q0;
		}

		Complex get(size_t amount)
		{
			return Complex(
				_Q1 - _Q2 * _cosine, 
				_Q2 * _sine)/amount;
		}
#endif
	};
}

#endif
