#ifndef _test_furie_h_
#define _test_furie_h_

#include "hel/fft.h"
namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	real gaussian(real x, real mu, real sigma)
	{
		real tmp = (x-mu)/sigma;
		return exp(-0.5*tmp*tmp);///sigma/sqrt(c_2Pi);
	}


	//////////////////////////////////////////////////////////////////////////
	void test_furie(ResponseModel *rm)
	{
		TVReal vt = rm->getPeriod();

// 		vt.erase(vt.begin(), vt.begin()+30);
// 		vt.erase(vt.end()-30, vt.end()+30);

		//////////////////////////////////////////////////////////////////////////
		//построить обратный фильтр
		TVReal vk;
		for(size_t i(0); i<vt.size(); i++)
		{
			Complex R, I;
			rm->evalRI(vt[i], 1, R, I);
			vk.push_back(R.re());
		}
		if(vk.size()%2)
		{
			vk.pop_back();
		}

		TVComplex vfk;
		f_fwd(vk, vfk);

// 		for(size_t i(0); i<vfk.size(); i++)
// 		{
// 			vfk[i] = 1.0 / vfk[i];
// 		}
// 
// 		TVReal vkm1;
// 		f_bwd(vfk, vkm1);

		//////////////////////////////////////////////////////////////////////////
		TVReal vt_test = rm->getPeriod();

// 		real step = vt[0] / vt[1];
// 		real minT = vt_test.front();
// 		real maxT = vt_test.back();
// 		for(size_t i(1); i<31; i++)
// 		{
// 			vt_test.insert(vt_test.begin(), minT*::pow(step, int(i)));
// 			vt_test.push_back(maxT/::pow(step, int(i)));
// 		}

		TVReal vk_test;
		for(size_t i(0); i<vt_test.size(); i++)
		{
			real res=0;
			Complex R, I;

			size_t amount=0;
			for(real k(1); k<200; k/=0.999453456)
			{
				rm->evalRI(vt_test[i], k, R, I);
				res += R.re();

				if(++amount>=1000)
				{
					break;
				}
			}
			vk_test.push_back(res);
		}

		TVReal res(vk_test.size());
		TVReal amount(vk_test.size());
		for(size_t i(0); i<=vk_test.size()-vk.size(); i++)
		{
			TVReal k2(vk_test.begin()+i, vk_test.begin()+i+vk.size());
			TVComplex fk2;
			f_fwd(k2, fk2);

			for(size_t j(0); j<fk2.size(); j++)
			{
				fk2[j] /= vfk[j];
				//fk2[j] *= vfg[j];
			}

			f_bwd(fk2, k2);

			for(size_t j(0); j<k2.size(); j++)
			{
				res[i+j] += k2[j];
				amount[i+j] ++;
			}
			break;

// 			res[i] = 0;
// 			for(size_t j(0); j<vkm1.size(); j++)
// 			{
// 				res[i] += k2[j] * vkm1[j];
// 			}
		}

		for(size_t j(0); j<res.size(); j++)
		{
			if(amount[j])
			{
				res[j] /= amount[j];
			}
		}


	}
}

#endif
