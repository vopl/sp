#ifndef _hel_approxResult_h_
#define _hel_approxResult_h_

#include "hel/types.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct ApproxResult
	{
		size_t	_levelIndex;
		real	_timeStart;
		real	_timeStop;
		real	_period;
		real	_phase;
		real	_amplitude;

		bool operator <(const ApproxResult &a) const
		{
			return _period < a._period;
		}

		bool isSame(const ApproxResult &ar, real pBound, real aBound)const
		{
			if(fabs(_amplitude-ar._amplitude)/std::max(_amplitude, ar._amplitude) > aBound)
			{
				return false;
			}

			real PP1 = _timeStart*c_2Pi/_period + _phase;
			real PP2 = _timeStart*c_2Pi/ar._period + ar._phase;

			if(fabs(alignPhase(PP1-PP2)) > pBound)
			{
				return false;
			}

			PP1 = _timeStop*c_2Pi/_period+_phase - PP1;
			PP2 = _timeStop*c_2Pi/ar._period+ar._phase - PP2;


			if(fabs(PP1-PP2) > pBound)
			{
				return false;
			}

			return true;
		}

	};

	typedef std::deque<ApproxResult> TVApproxResult;
}

BOOST_CLASS_IMPLEMENTATION(hel::ApproxResult, boost::serialization::primitive_type);

#endif

