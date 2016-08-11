#include "stdafx.h"
#include "hel/levmar.h"

#include "hel/types.h"

namespace hel
{

	real g_levmarOptsAutoMu[LM_OPTS_SZ] = 
	{
		1E-40, //LM_INIT_MU,//mu
		1E-60, //LM_STOP_THRESH,	//stopping thresholds for ||J^T e||_inf, 
		1E-60, //LM_STOP_THRESH,	//||Dp||_2 and 
		1E-60, //LM_STOP_THRESH,	//||e||_2. Set to NULL for defaults to be used.
	};

	int dlevmar_der_autoMu(
		void (*func)(double *p, double *hx, int m, int n, void *adata),
		void (*jacf)(double *p, double *j, int m, int n, void *adata),
		double *p, const double *x, int m, int n, TVReal &work, void *adata, int maxIters)
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

		int res = 0;
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
}

