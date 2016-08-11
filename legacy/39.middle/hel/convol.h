#ifndef _hel_convol_h_
#define _hel_convol_h_

#include "hel/types.h"
#include "hel/complex.h"
#include "hel/math.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	class Convol
	{
		real _sop;
		Complex	_sum;

		//size_t cnt;

		real _prevY_re[3];
		real _prevY_im[3];

		static void iterNull(real prevY[3], real y)
		{
			prevY[2] = prevY[1];
			prevY[1] = prevY[0];
			prevY[0] = y;
		}
		static real iterLeft(real prevY[3], real y)
		{
			real dyp1 = (y - prevY[1])/4;
			real res = integrate2pLeft(prevY[1], prevY[0], dyp1);

			iterNull(prevY, y);

			return res;
		}
		static real iterMiddle(real prevY[3], real y)
		{
			real dym1 = (prevY[0] - prevY[2])/4;
			real dyp1 = (y - prevY[1])/4;
			real res = integrate2p(prevY[1], prevY[0], dym1, dyp1);

			iterNull(prevY, y);

			return res;
		}
		static real iterRight(real prevY[3])
		{
			real dym1 = (prevY[0] - prevY[2])/4;
			real res = integrate2pRight(prevY[1], prevY[0], dym1);

			return res;
		}
	public:
		void reset(real sop)
		{
			//cnt = 0;
			//return;
			_sop = sop;
			_sum.set(0, 0);

			_prevY_re[0] = _prevY_re[1] = _prevY_re[2] = 0;
			_prevY_im[0] = _prevY_im[1] = _prevY_im[2] = 0;
		}

		void push(real v, size_t idx)
		{
			real x = c_2Pi * (idx) / _sop;
			real vre = cos(x) * v;
			real vim = -sin(x) * v;

			if(idx>2)
			{
				_sum.re() += iterMiddle(_prevY_re, vre);
				_sum.im() += iterMiddle(_prevY_im, vim);
				//cnt++;
			}
			else if(idx>1)
			{
				_sum.re() += iterLeft(_prevY_re, vre);
				_sum.im() += iterLeft(_prevY_im, vim);
				//cnt++;
			}
			else
			{
				iterNull(_prevY_re, vre);
				iterNull(_prevY_im, vim);
			}
		}

		Complex get(size_t amount)
		{
			Complex last = Complex(iterRight(_prevY_re), iterRight(_prevY_im));
			return (_sum+last)/amount/2;
		}

	};
}

#endif
