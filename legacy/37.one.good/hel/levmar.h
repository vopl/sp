#ifndef _hel_levmar_h_
#define _hel_levmar_h_

#include "levmar-2.5/levmar.h"
#include "hel/types.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	int dlevmar_der_autoMu(
		void (*func)(double *p, double *hx, int m, int n, void *adata),
		void (*jacf)(double *p, double *j, int m, int n, void *adata),
		double *p, const double *x, size_t m, size_t n, TVReal &work, void *adata, size_t maxIters=INT_MAX);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	real normXPoly(real x, real a, real b);
	real evalPoly(real x, const real *args, size_t maxPow, real a, real b);
	real diffPoly(real x, const real *args, size_t maxPow, real a, real b);
	void gradPoly(real x, real *d, size_t maxPow, real a, real b);

	//////////////////////////////////////////////////////////////////////////
	struct PolyDlevmarParams
	{
		const real *_xdata;
		const real *_ydata;
		size_t _polyPow;
		real _a;
		real _b;
	};
	void PolyDlevmarFunc(double *p, double *hx, int m, int n, void *adata);
	void PolyDlevmarJaco(double *p, double *jx, int m, int n, void *adata);

}

#endif
