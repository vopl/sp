#ifndef _hel_math_h_
#define _hel_math_h_

#include "hel/types.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	const real c_piDiv2	= (1.57079632679489661923132169163975144209858469968755291048747229615390820314310449931401741267105853);
	const real c_pi		= (3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214);
	const real c_2Pi	= (6.28318530717958647692528676655900576839433879875021164194988918461563281257241799725606965068423413);
	const real c_2PiPow2= (39.4784176043574319692197605036199092864990234375);

	//////////////////////////////////////////////////////////////////////////
	template <class T>
	T sqr(T x)
	{
		return x*x;
	}

	template <class T>
	T sqrtGtZero(T x)
	{
		if(x<0) return 0;
		return sqrt(x);
	}

	template <class T>
	int sign(T x)
	{
		return x<0?-1:1;
	}


	//////////////////////////////////////////////////////////////////////////
	template <class T>
	T halfWnd01(T x01)
	{
		return 0.5 + 0.5 * cos(c_pi+c_pi*x01);
	}

	//////////////////////////////////////////////////////////////////////////
	inline
	real tinyfmod(real x, real y)
	{
		return x - int(x/y)*y;
	}

	//////////////////////////////////////////////////////////////////////////
	inline
		real alignPhase(real p)
	{
		p = fmod(p + c_pi, c_2Pi) - c_pi;
		if(p < -c_pi)
		{
			p += c_2Pi;
		}

		return p;
	}

	//////////////////////////////////////////////////////////////////////////
	inline
		real alignPhase(real p, real pTo)
	{
		return alignPhase(p - pTo) + pTo;
	}

	//////////////////////////////////////////////////////////////////////////
	inline
		real middlePhase(const real *angles, const size_t amount)
	{
		real sumx=0;
		real sumy=0;
		for(size_t i(0); i<amount; i++)
		{
			real x = cos(angles[i]);
			real y = sin(angles[i]);

			sumx += x;
			sumy += y;
		}
		if(!sumy && !sumx)
		{
			return 0;
		}

		real middle2d = atan2(sumy,sumx);

		real sumdelta=0;
		for(size_t i(0); i<amount; i++)
		{
			real angle = alignPhase(angles[i]);
			real delta = middle2d - angle;

			delta = alignPhase(delta);

			sumdelta+=delta;
		}

		real middle = middle2d + sumdelta / amount;

		middle = alignPhase(middle);

		return middle;
	}

	//////////////////////////////////////////////////////////////////////////
	inline 
		real SQR(real x)
	{
		return x*x;
	}


	//////////////////////////////////////////////////////////////////////////
	inline void llsq(const real *x, const real *y, size_t size, real &k, real &b)
	{
		real qq = 0;
		real pq = 0;
		real b1 = 0;
		real b2 = 0;
		for(size_t i = 0; i < size; i++)
		{
			const real rx = x?x[i]:real(i);
			const real ry = y?y[i]:real(i);

			pq = pq+rx;
			qq = qq+sqr(rx);
			b1 = b1+ry;
			b2 = b2+rx*ry;
		}
		real phi = atan2(2*pq, qq-size)/2;
		real c = cos(phi);
		real s = sin(phi);
		real d1 = sqr(c)*size+sqr(s)*qq-2*s*c*pq;
		real d2 = sqr(s)*size+sqr(c)*qq+2*s*c*pq;

		real t1 = c*b1-s*b2;
		real t2 = s*b1+c*b2;

		t1 = t1/d1;
		t2 = t2/d2;

		b = c*t1+s*t2;
		k = c*t2-s*t1;
	}

	//////////////////////////////////////////////////////////////////////////
	//���� ��� ��������� y = ax^2 + bx + c �������� ���������� 3-� ��������� ����� ��� ������� (x1;y1), (x2;y2), (x3;y3), �� ��� ������������ ����� ���� ������� ���:
	inline void parabolicArgs(real x1, real y1, real x2, real y2, real x3, real y3, real &a, real &b, real &c)
	{
		a = (y3- (x3*(y2-y1) + x2*y1 - x1*y2)/(x2-x1)) / (x3*(x3-x1-x2)+x1*x2);
		b = (y2-y1)/(x2-x1) - a*(x1+x2);
		c = (x2*y1-x1*y2)/(x2-x1) + a*x1*x2;
	}

	//////////////////////////////////////////////////////////////////////////
	//���� ��������� �������� �� 3 ������
	inline void parabolicExtr(real x1, real y1, real x2, real y2, real x3, real y3, real &xRes, real &yRes)
	{
		real a,b,c;
		parabolicArgs(x1, y1, x2, y2, x3, y3, a, b, c);

		xRes = -b/2/a;
		yRes = a*xRes*xRes + b*xRes + c;
	}

	//////////////////////////////////////////////////////////////////////////
	//���� ����� ����������� ������ 2 ����� � �����
	inline void linearZero(real x1, real y1, real x2, real y2, real &xRes)
	{
		xRes = (x1*y2 - x2*y1)/(y2-y1);
	}

	//////////////////////////////////////////////////////////////////////////
	//������� ������ ��������� �������� 1 ���� 
	inline void bcs4(real y1, real y2, real args[5], real dy1, real dy2, real ddy1, real &ddy2)
	{
// 		args[0] = (+4*y2+4*y1-dy2+dy1)/8;
// 		args[1] = (+9*y2-9*y1-dy2-dy1)/16;
// 		args[2] = (-dy1+dy2)/8;
// 		args[3] = (-y2+y1+dy2+dy1)/16;

		args[0] = (110*y2+146*y1-20*dy2+56*dy1+12*ddy1)/256;
		args[1] = (36*y2-36*y1-4*dy2-4*dy1)/64;
		args[2] = (6*y2-6*y1+4*dy2-16*dy1-4*ddy1)/64;
		args[3] = (-4*y2+4*y1+4*dy2+4*dy1)/64;
		args[4] = (-6*y2+6*y1+4*dy2+8*dy1+4*ddy1)/256;

		ddy2 = 80*args[4]+24*args[3]+4*args[2];
	}
	inline real ecs4(real x1, real x2, real x, real args[5])
	{
		x = 2.0*(x - x1)/(x2 - x1) - 1.0;
//		return args[0]*1 + args[1]*x + args[2]*(2*x*x-1) + args[3]*(4*x*x*x-3*x);
		return 
			args[0]*1 + 
			args[1]*x + 
			args[2]*(2*sqr(x)-1) + 
			args[3]*(4*sqr(x)*x-3*x) + 
			args[4]*(8*sqr(x)*sqr(x) - 8*sqr(x) + 1);
	}







	//////////////////////////////////////////////////////////////////////////
	//���������� ��������� �� �������� �������� 3 �������, ��������� ����� 
	//�� ������� [-1,1]
	inline real integrate2p(
		real ym1,  real yp1,
		real dym1, real dyp1)
	{
		return yp1+ym1+(dym1-dyp1)/3;
	}
	inline real integrate2pLeft(
		real ym1,  real yp1,
		real dyp1)
	{
		return (4*yp1+2*ym1-2*dyp1)/3;
	}

	inline real integrate2pRight(
		real ym1,  real yp1,
		real dym1)
	{
		return (2*yp1+4*ym1+2*dym1)/3;
	}

	//////////////////////////////////////////////////////////////////////////
	inline real gaussian1(real x, real mu, real sigma)
	{
		real tmp = (x-mu)/sigma;
		real res = exp(-0.5*tmp*tmp);///sigma/sqrt(c_2Pi);

		if(res<1e-300)
		{
			return 0;
		}

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	inline real gaussian(real x, real mu, real sigma)
	{
		real tmp = (x-mu)/sigma;
		real res = exp(-0.5*tmp*tmp)/sigma/sqrt(c_2Pi);

		if(res<1e-300)
		{
			return 0;
		}

		return res;
	}

}

#endif
