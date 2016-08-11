#ifndef _hel_convolCore_trig_h_
#define _hel_convolCore_trig_h_

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct ConvolCore_trig
	{
		real _2pi_div_period;
		real _period_pow_2;
		real _period_mul_2pi;

		void setup(real period)
		{
			_2pi_div_period = c_2Pi/period;
			_period_pow_2 = period*period;
			_period_mul_2pi = period*c_2Pi;
		}
		Complex evalSegment(const real xc0, const TimedValue &tv0, const TimedValue &tv1, real xStart, real xStop)
		{
			real xStart_mul_2pi_div_period = (xStart-xc0)*_2pi_div_period;
			real xStop_mul_2pi_div_period = (xStop-xc0)*_2pi_div_period;

			real sinStop = sin(xStop_mul_2pi_div_period);
			real cosStop = cos(xStop_mul_2pi_div_period);

			real sinStart = sin(xStart_mul_2pi_div_period);
			real cosStart = cos(xStart_mul_2pi_div_period);

			real x0 = tv0._time;
			real x1 = tv1._time;
			real y0 = tv0._value;
			real y1 = tv1._value;

			real x1_mul_y0_minus_x0_mul_y1 = (x1*y0-x0*y1);

			real y1_minus_y0 = y1-y0;
			real x1_minus_x0 = x1-x0;

			//TRATATA тут можно еще закэшировать
			real re = 
				(
				+y1_minus_y0*_period_pow_2*cosStop
				+(y1_minus_y0*xStop+x1_mul_y0_minus_x0_mul_y1)*_period_mul_2pi*sinStop

				-y1_minus_y0*_period_pow_2*cosStart
				-(y1_minus_y0*xStart+x1_mul_y0_minus_x0_mul_y1)*_period_mul_2pi*sinStart
				)/
				(x1_minus_x0*c_2PiPow2);

			real im = 
				-(

				+y1_minus_y0*_period_pow_2*sinStop
				-(y1_minus_y0*xStop+x1_mul_y0_minus_x0_mul_y1)*_period_mul_2pi*cosStop

				-y1_minus_y0*_period_pow_2*sinStart
				+(y1_minus_y0*xStart+x1_mul_y0_minus_x0_mul_y1)*_period_mul_2pi*cosStart
				)/
				(x1_minus_x0*c_2PiPow2);

			return Complex(re, im);
		}

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _2pi_div_period;
			ar & _period_pow_2;
			ar & _period_mul_2pi;
		}
	};

}
#endif
