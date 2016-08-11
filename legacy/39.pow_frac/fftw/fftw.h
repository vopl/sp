#ifndef _ge_fftw_h_
#define _ge_fftw_h_

#include "hel/types.h"
#include <fftw3.h>

namespace ge
{
	class Fftw
	{
		struct PlanDescr
		{
			size_t	_size;
			size_t	_amount;
			bool	_forw;
			bool	_inPlace;
			bool operator<(const PlanDescr &with)const
			{
				if(_size<with._size) return true;
				if(_size>with._size) return false;
				if(_amount<with._amount) return true;
				if(_amount>with._amount) return false;
				if(_forw<with._forw) return true;
				if(_forw>with._forw) return false;
				if(_inPlace<with._inPlace) return true;
				if(_inPlace>with._inPlace) return false;
				return false;
			}
		};
		typedef std::map<PlanDescr, fftw_plan> TMPlans;
		TMPlans _plans;
		fftw_plan mkPlan(size_t oneSize, bool forw, bool optimize, bool inPlace);
		fftw_plan mkPlanMulti(
			size_t amount, 
			size_t oneSize, 
			bool forw, 
			bool optimize,
			bool inPlace);
	public:
		Fftw();
		~Fftw();

		void initThreads();
		void deinitThreads();

		void eval(hel::TVReal &in, hel::TVReal &out, bool forw, bool optimize = true);

// 		void eval(real* in, real *out, size_t size, bool forw, bool optimize = true);
// 		void eval(TVRealAligned &inOut, bool forw, bool optimize = true);
// 		void eval(real* inOut, size_t size, bool forw, bool optimize = true);

		void evalMulti(hel::TVReal &in, hel::TVReal &out, size_t oneSize, bool forw, bool optimize = true);
		void flushWisdom();
		void destroyPlans();
	};

	extern Fftw fftw;
}
#endif
