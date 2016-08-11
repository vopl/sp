#ifndef _hel_levmar_h_
#define _hel_levmar_h_

#include "levmar-2.3/lm.h"
#include "hel/types.h"

namespace hel
{
	int dlevmar_der_autoMu(
		void (*func)(double *p, double *hx, int m, int n, void *adata),
		void (*jacf)(double *p, double *j, int m, int n, void *adata),
		double *p, const double *x, int m, int n, TVReal &work, void *adata, int maxIters=INT_MAX);
}

#endif
