#include "stdafx.h"
#include "hel/levmar.h"

#include "hel/types.h"

namespace hel
{

	//////////////////////////////////////////////////////////////////////////
	real g_levmarOptsAutoMu[LM_OPTS_SZ] = 
	{
		1E-60, //LM_INIT_MU,//mu
		1E-80, //LM_STOP_THRESH,	//stopping thresholds for ||J^T e||_inf, 
		1E-80, //LM_STOP_THRESH,	//||Dp||_2 and 
		1E-80, //LM_STOP_THRESH,	//||e||_2. Set to NULL for defaults to be used.

// 		LM_INIT_MU,//mu
// 		LM_STOP_THRESH,	//stopping thresholds for ||J^T e||_inf, 
// 		LM_STOP_THRESH,	//||Dp||_2 and 
// 		LM_STOP_THRESH,	//||e||_2. Set to NULL for defaults to be used.
	};

	//////////////////////////////////////////////////////////////////////////
	int dlevmar_der_autoMu(
		void (*func)(double *p, double *hx, int m, int n, void *adata),
		void (*jacf)(double *p, double *j, int m, int n, void *adata),
		double *p, const double *x, size_t m, size_t n, TVReal &work, void *adata, size_t maxIters)
	{
		size_t wSize = jacf?LM_DER_WORKSZ(m, n):LM_DIF_WORKSZ(m, n);
		if(work.size() < wSize)
		{
			work.resize(wSize);
		}

		real info[LM_INFO_SZ];
		real levmarOpts[LM_OPTS_SZ];
		memcpy(levmarOpts, g_levmarOptsAutoMu, sizeof(g_levmarOptsAutoMu));







// 		if(jacf)
// 		{
// 			TVReal err(n);
// 			dlevmar_chkjac(
// 				func, 
// 				jacf, 
// 				p, 
// 				m, 
// 				n,
// 				adata, 
// 				&err.front());
// 			std::cout<<"dlevmar_chkjac: "<<std::accumulate(err.begin(), err.end(), 0.0)/err.size()<<std::endl;
// 		}

		size_t res = 0;
		bool exist = false;
		for(size_t iter=0; iter < 100 && res < maxIters; iter++)
		{
			/* O: information regarding the minimization. Set to NULL if don't care
			* info[0]= ||e||_2 at initial p.
			* info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
			* info[5]= # iterations,
			* info[6]=reason for terminating: 1 - stopped by small gradient J^T e
			*                                 2 - stopped by small Dp
			*                                 3 - stopped by itmax
			*                                 4 - singular matrix. Restart from current p with increased mu 
			*                                 5 - no further error reduction is possible. Restart with increased mu
			*                                 6 - stopped by small ||e||_2
			*                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
			* info[7]= # function evaluations
			* info[8]= # Jacobian evaluations
			*/
			int dmdRes;

			int iters = (int)(50*(iter+1));
			if(res + iters > maxIters)
			{
				iters = maxIters-res;
			}
				
			if(jacf)
			{
				dmdRes = dlevmar_der(
					func, 
					jacf, 
					p, 
					const_cast<double *>(x), 
					m, 
					n, 
					iters, 
					levmarOpts, 
					info, 
					&work.front(), 
					NULL, 
					adata);
			}
			else
			{
				dmdRes = dlevmar_dif(
					func, 
					p, 
					const_cast<double *>(x), 
					m, 
					n, 
					iters, 
					levmarOpts, 
					info, 
					&work.front(), 
					NULL, 
					adata);
			}
			res += dmdRes;

			if(dmdRes < 0)//LM_ERROR
			{
				break;
			}

			switch(int(info[6]+0.1))
			{
			case 1://1 - stopped by small gradient J^T e
				iter = -3;
				break;
			case 2://2 - stopped by small Dp
				iter = -3;
				break;
			case 3://3 - stopped by itmax
				break;
			case 4://4 - singular matrix. Restart from current p with increased mu 
				levmarOpts[0] *= 10;
				if(levmarOpts[0]<0.1)
				{
					iter = 0;
				}
				else
				{
					iter = -3;
				}
				break;
			case 5://5 - no further error reduction is possible. Restart with increased mu
				levmarOpts[0] *= 10;
				if(levmarOpts[0]<0.1)
				{
					iter = 0;
				}
				else
				{
					iter = -3;
				}
				break;
			case 6://6 - stopped by small ||e||_2
				iter = -3;
				break;
			case 7://7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
				assert(0);
				iter = -3;
				break;
			}
		}
		return res;
	}



	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	real normXPoly(real x, real a, real b)
	{
		return 2.0*(x - a)/(b - a) - 1.0;
	}
	real evalPoly(real x, const real *args, size_t maxPow, real a, real b)
	{
		double polyVal;
		double b1;
		double b2;
		int i;

		x = normXPoly(x, a, b);
		b1 = 0;
		b2 = 0;
		i = maxPow;
		do
		{
			polyVal = 2*x*b1-b2+args[i];
			b2 = b1;
			b1 = polyVal;
			i = i-1;
		}
		while(i>=0);
		polyVal = polyVal-x*b2;

		return polyVal;
	}

	real diffPoly(real x, const real *args, size_t maxPow, real a, real b)
	{
		x = normXPoly(x, a, b);

		if(0 == maxPow)
		{
			return 0;
		}
		if(1 == maxPow)
		{
			return args[1];
		}

		real Tnm1=0;
		real Tn=1;
		real Tnp1=x;

		real DTnm1=0;
		real DTn=0;
		real DTnp1=1;


		real dsum = DTnp1*args[1];
		for(size_t np1=2; np1<=maxPow; np1++)
		{
			Tnm1 = Tn;
			Tn = Tnp1;
			DTnm1 = DTn;
			DTn = DTnp1;

			DTnp1 = 2*x*DTn + 2*Tn - DTnm1;
			Tnp1 = 2*x*Tn - Tnm1;


			dsum += DTnp1*args[np1];
		}

		return dsum/(b-a)*2;
	}

	void gradPoly(real x, real *d, size_t maxPow, real a, real b)
	{
		x = normXPoly(x, a, b);

		if(maxPow == 0)
		{
			d[0] = 1;
		}
		else if(maxPow == 1)
		{
			d[0] = 1;
			d[1] = x;
		}
		else
		{
			d[0] = 1;
			d[1] = x;

			for(size_t i=2; i<=maxPow; i++)
			{
				d[i] = 2.0*x*d[i-1] - d[i-2];
			}
		}
	}



	//////////////////////////////////////////////////////////////////////////
	void PolyDlevmarFunc(double *p, double *hx, int m, int n, void *adata)
	{
		PolyDlevmarParams *params = (PolyDlevmarParams *)adata;

		real a = params->_a;
		real b = params->_b;

		for(int i(0); i<n; i++)
		{
			real x = params->_xdata[i];
			hx[i] = evalPoly(x, p, params->_polyPow, a, b);
		}
	}

	void PolyDlevmarJaco(double *p, double *jx, int m, int n, void *adata)
	{
		PolyDlevmarParams *params = (PolyDlevmarParams *)adata;

		real a = params->_a;
		real b = params->_b;

		for(int i(0); i<n; i++)
		{
			real x = params->_xdata[i];
			gradPoly(x, jx+i*m, params->_polyPow, a, b);
		}
	}

}

