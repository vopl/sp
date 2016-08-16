#ifndef _hel_convolCore_triangle_h_
#define _hel_convolCore_triangle_h_

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct ConvolCore_triangle
	{
		real _period;

		void setup(real period)
		{
			_period = period;
		}
		Complex evalSegment(const real xc0, const TimedValue &tv0, const TimedValue &tv1, real xStart, real xStop)
		{
			real re(0),im(0);

			real ps = (xc0 - tv1._time)/(_period/2);

			if(int(ps) & 1)
			{
				re = fmod(ps, 1)*2-1;
			}
			else
			{
				re = 1-fmod(ps, 1)*2;
			}

			ps += 0.5;
			if(int(ps) & 1)
			{
				im = fmod(ps, 1)*2-1;
			}
			else
			{
				im = 1-fmod(ps, 1)*2;
			}

			return Complex(re, im)*tv1._value;
		}

	};

}
#endif
