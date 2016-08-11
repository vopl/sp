#ifndef _hel_convolCore_meandr_h_
#define _hel_convolCore_meandr_h_

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct ConvolCore_meandr
	{
		real _period;

		void setup(real period)
		{
			_period = period;
		}
		Complex evalSegment(const real xc0, const TimedValue &tv0, const TimedValue &tv1, real xStart, real xStop)
		{
			real re(0),im(0);

			real k = (tv1._value - tv0._value) / (tv1._time - tv0._time);
			real b = tv1._value - tv1._time*k;

			//////////////////////////////////////////////////////////////////////////
			//re
			real xStart_ = xStart;
			real xStop_ = xStop;
			real pdcStart = (xStart - xc0)/(_period/2) + 0.5;
			real pdcStop = (xStop - xc0)/(_period/2) + 0.5;

			while(pdcStop != pdcStart)
			{
				real pdcStop_i = floor(pdcStop);
				real pdcStart_i = floor(pdcStart);

				if(pdcStart_i == pdcStop_i)
				{
					real integral = (k*xStop_*xStop_+2*b*xStop_)/2-(k*xStart_*xStart_+2*b*xStart_)/2;
					if(int(-pdcStop_i+0.5) & 1)
					{
						//negative
						re += -integral;
					}
					else
					{
						//positive
						re += integral;
					}
					pdcStart = pdcStop;
					xStart_ = xStop_;
				}
				else
				{
					real pdcStart2_i = pdcStart_i+1;
					real xStart2 = (pdcStart2_i-0.5)*(_period/2) + xc0;

					real integral = (k*xStart_*xStart_+2*b*xStart_)/2-(k*xStart2*xStart2+2*b*xStart2)/2;
					if(int(-pdcStart2_i+0.5) & 1)
					{
						//negative
						re += -integral;
					}
					else
					{
						//positive
						re += +integral;
					}

					xStart_ = xStart2;
					pdcStart = pdcStart2_i;
				}
			}


			//////////////////////////////////////////////////////////////////////////
			//im
			xStart_ = xStart;
			xStop_ = xStop;
			pdcStart = (xStart - xc0)/(_period/2);
			pdcStop = (xStop - xc0)/(_period/2);

			while(pdcStop != pdcStart)
			{
				real pdcStop_i = floor(pdcStop);
				real pdcStart_i = floor(pdcStart);

				if(pdcStart_i == pdcStop_i)
				{
					real integral = (k*xStop_*xStop_+2*b*xStop_)/2-(k*xStart_*xStart_+2*b*xStart_)/2;
					if(int(-pdcStop_i+0.5) & 1)
					{
						//negative
						im += -integral;
					}
					else
					{
						//positive
						im += integral;
					}
					pdcStart = pdcStop;
					xStart_ = xStop_;
				}
				else
				{
					real pdcStart2_i = pdcStart_i+1;
					real xStart2 = (pdcStart2_i)*(_period/2) + xc0;

					real integral = (k*xStart_*xStart_+2*b*xStart_)/2-(k*xStart2*xStart2+2*b*xStart2)/2;
					if(int(-pdcStart2_i+0.5) & 1)
					{
						//negative
						im += -integral;
					}
					else
					{
						//positive
						im += +integral;
					}

					xStart_ = xStart2;
					pdcStart = pdcStart2_i;
				}
			}


			return Complex(re, im);
		}

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _period;
		}
	};

}
#endif
