
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
}

