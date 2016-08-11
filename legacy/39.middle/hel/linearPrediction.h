#ifndef _hel_linearPrediction_h_
#define _hel_linearPrediction_h_

#include "hel/math.h"
#include "hel/complex.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	inline
		real memcof(const TVReal &data, TVReal &d)
	{


		signed_t n = data.size();
		signed_t m = d.size();

		signed_t k,j,i;
		real p=0.0;
		TVReal wk1(n);
		TVReal wk2(n);
		TVReal wkm(m);

		for (j=0;j<n;j++)
		{
			p += SQR(data[j]);
		}

		real xms = p/n;
		wk1[0]=data[0];
		wk2[n-2]=data[n-1];

		for (j=1;j<n-1;j++)
		{
			wk1[j]=data[j];
			wk2[j-1]=data[j];
		}

		for (k=0;k<m;k++)
		{
			real num=0.0,denom=0.0;
			for (j=0;j<(n-k-1);j++)
			{
				num += wk1[j]*wk2[j];
				denom += SQR(wk1[j])+SQR(wk2[j]);
			}
			if(num)
			{
				d[k]=2.0*num/denom;
			}
			else
			{
				d[k]=0;
			}

			xms *= (1.0-SQR(d[k]));
			for (i=0;i<(k);i++)
			{
				d[i]=wkm[i]-d[k]*wkm[k-i-1];
			}
			if ((k+1) == m)
			{
				return xms;
			}

			for (i=0;i<k+1;i++)
			{
				wkm[i] = d[i];
			}

			for (j=0;j<(n-k-2);j++)
			{
				wk1[j] -= wkm[k]*wk2[j];
				wk2[j] = wk2[j+1]-wkm[k]*wk1[j+1];
			}
		}
		throw("never get here in memcof.");
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	inline
		Complex memcofc(const TVComplex &data, TVComplex &d)
	{


		signed_t n = data.size();
		signed_t m = d.size();

		signed_t k,j,i;
		Complex p=0.0;
		TVComplex wk1(n);
		TVComplex wk2(n);
		TVComplex wkm(m);

		for (j=0;j<n;j++)
		{
			p += data[j].sqr();
		}

		real xms = p/n;
		wk1[0]=data[0];
		wk2[n-2]=data[n-1];

		for (j=1;j<n-1;j++)
		{
			wk1[j]=data[j];
			wk2[j-1]=data[j];
		}

		for (k=0;k<m;k++)
		{
			real num=0.0,denom=0.0;
			for (j=0;j<(n-k-1);j++)
			{
				num += wk1[j]*wk2[j];
				denom += wk1[j].sqr()+wk2[j].sqr();
			}
			if(num)
			{
				d[k]=2.0*num/denom;
			}
			else
			{
				d[k]=0;
			}

			xms *= (Complex(1,1)-d[k].sqr());
			for (i=0;i<(k);i++)
			{
				d[i]=wkm[i]-d[k]*wkm[k-i-1];
			}
			if ((k+1) == m)
			{
				return xms;
			}

			for (i=0;i<k+1;i++)
			{
				wkm[i] = d[i];
			}

			for (j=0;j<(n-k-2);j++)
			{
				wk1[j] -= wkm[k]*wk2[j];
				wk2[j] = wk2[j+1]-wkm[k]*wk1[j+1];
			}
		}
		throw("never get here in memcof.");
	}





	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	inline
		void predict(const TVReal &data, const TVReal &d, TVReal &future)
	{
		signed_t ndata = data.size();
		signed_t m = d.size();
		signed_t nfut = future.size();

		signed_t k,j;
		real sum,discrp;
		TVReal reg(m+1);
		for (j=0;j<m;j++)
		{
			reg[j]=data[ndata-1-j];
		}

		for (j=0;j<nfut;j++)
		{
			discrp=0.0;
			sum=discrp;
			for (k=0;k<m;k++)
			{
				sum += d[k]*reg[k];
			}

			for (k=m-1;k>=1;k--)
			{
				reg[k]=reg[k-1];
			}

			future[j]=reg[0]=sum;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	inline
		void predictc(const TVComplex &data, const TVComplex &d, TVComplex &future)
	{
		signed_t ndata = data.size();
		signed_t m = d.size();
		signed_t nfut = future.size();

		signed_t k,j;
		Complex sum,discrp;
		TVComplex reg(m+1);
		for (j=0;j<m;j++)
		{
			reg[j]=data[ndata-1-j];
		}

		for (j=0;j<nfut;j++)
		{
			discrp=0.0;
			sum=discrp;
			for (k=0;k<m;k++)
			{
				sum += d[k]*reg[k];
			}

			for (k=m-1;k>=1;k--)
			{
				reg[k]=reg[k-1];
			}

			future[j]=reg[0]=sum;
		}
	}
}

#endif
