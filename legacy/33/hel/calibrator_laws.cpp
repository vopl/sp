
namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	real evalLogPoly(real x, const real *args, size_t maxPow)
	{
		real logVal = log(x);
		real val = args[0];
		for(int j(1); j<=int(maxPow); j++)
		{
			val += args[j]/pow(logVal, j);
		}

		return val;
	}

	//////////////////////////////////////////////////////////////////////////
	real diffLogPoly(real x, const real *args, size_t maxPow)
	{
		real logVal = log(x);
		real val = 0;
		for(int j(1); j<=int(maxPow); j++)
		{
			val -= j*args[j]/pow(logVal, j+1)/x;
		}

		return val;
	}

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


}

