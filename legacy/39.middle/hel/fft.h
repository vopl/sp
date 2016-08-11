#ifndef _hel_fft_h_
#define _hel_fft_h_

#include "hel/complex.h"
#include <fftw3.h>

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	inline void f_fwd(const TVReal &in, TVComplex &out)
	{
		out.resize(in.size()/2+1);
		fftw_plan p = fftw_plan_dft_r2c_1d(
			in.size(), 
			const_cast<real *>(&in[0]), 
			(fftw_complex *)&out[0],
			FFTW_MEASURE|FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
		fftw_execute(p);
		fftw_destroy_plan(p);
	}

	//////////////////////////////////////////////////////////////////////////
	inline void f_bwd(const TVComplex &in, TVReal &out)
	{
		size_t n = 2*(in.size()-1);
		out.resize(n);
		fftw_plan p = fftw_plan_dft_c2r_1d(
			n, 
			(fftw_complex *)&in[0], 
			&out[0],
			FFTW_MEASURE|FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
		fftw_execute(p);
		fftw_destroy_plan(p);

		for(size_t i(0); i<n; i++)
		{
			out[i] /= n;
		}
	}

}
#endif
