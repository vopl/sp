#ifndef _hel_convolCore_trig2_h_
#define _hel_convolCore_trig2_h_

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct ConvolCore_trig2
	{
		real	_period;
		real	_phase;

		void setup(real period)
		{
			_period = period;
			_phase = 0;
		}
		void setPhase(real p)
		{
			_phase = p;
		}
		Complex evalSegment(const real xc0, const TimedValue &tv0, const TimedValue &tv1, real xStart, real xStop)
		{
			real x = c_2Pi * (tv1._time - xc0) / _period + _phase;
			return Complex(cos(x), -sin(x))* tv1._value;
		}

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _period;
		}
	};

}
#endif
