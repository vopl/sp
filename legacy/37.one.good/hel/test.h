
#ifndef _hel_test_h_
#define _hel_test_h_

namespace hel
{

	//////////////////////////////////////////////////////////////////////////
	struct Const
	{
		real _v;
		Const(real v)
			: _v(v)
		{
		}

		real operator()(real x) const
		{
			return _v;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct K
	{
		ResponseModel *_rm;
		K(ResponseModel *rm)
			: _rm(rm)
		{
		}

		real operator()(real x) const
		{
			Complex R,I;
			_rm->evalRI(x,1,R,I);
			return R.re();
		}
	};

	//////////////////////////////////////////////////////////////////////////
	inline void mellinFwd(
		const real *x,		//аргумент оригинала, должен быть >= 0
		const real *f,		//значение оригинала
		size_t srcAmount,	//количество отсчетов оригинала
		const real sImMax,	//мнимая часть аргумента изображения
		Complex *m,			//значение изображения
		size_t dstAmount)	//количество отсчетов изображения
	{
		//интегрирование делать со второго по предпоследний
		srcAmount--;
		real xDist = (x[srcAmount-1] + x[srcAmount]) - (x[0] + x[1]);

		real sImStep = sImMax / dstAmount;
		Complex c;

		for(size_t i(0); i<dstAmount; i++)
		{
			c.set(0, 0);
			real sIm = sImStep*i;
			real prevXBnd = (x[0] + x[1]);

			for(size_t j(1); j<srcAmount; j++)
			{
				real xBnd = (x[j] + x[j+1]);
				real part = xBnd - prevXBnd;
				prevXBnd = xBnd;

				real a = f[j] * part / x[j];
				real p = sIm * ::log(x[j]);

				c.re() += a*cos(p);
				c.im() += a*sin(p);
			}

			m[i] = c / xDist;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	inline void mellinBwd(
		real sImMax,		//мнимая часть аргумента изображения
		const Complex *m,	//значение изображения
		size_t srcAmount,	//количество отсчетов изображения
		const real *x,		//аргумент оригинала, должен быть > 0
		real *f,			//значение оригинала
		size_t dstAmount)	//количество отсчетов оригинала
	{
		real sImStep = sImMax / srcAmount;
		for(size_t i(0); i<dstAmount; i++)
		{
			real sum = 0;
			real logx = ::log(x[i]);
			real pdx = sImStep * logx;

			for(size_t j(0); j<srcAmount; j++)
			{
				real p = pdx * j;

				real rem = cos(p);
				real imm = sin(p);
				sum += (m[j].re()*rem + m[j].im()*imm);
			}

			f[i] = sum / srcAmount;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void dump(const char *fname, const TVReal &x, const TVComplex &v)
	{
		std::ofstream out(fname);
		out.precision(16);
		out.setf(std::ios::scientific);

		for(size_t i(0); i<x.size(); i++)
		{
			out<<x[i]<<"\t"<<v[i].re()<<"\t"<<v[i].im()<<std::endl;
		}
		std::cout<<"dump "<<fname<<std::endl;
	}
	//////////////////////////////////////////////////////////////////////////
	void dump(const char *fname, const TVReal &x, const TVReal &vre, const TVReal &vim)
	{
		std::ofstream out(fname);
		out.precision(16);
		out.setf(std::ios::scientific);

		for(size_t i(0); i<x.size(); i++)
		{
			out<<x[i]<<"\t"<<vre[i]<<"\t"<<vim[i]<<std::endl;
		}
		std::cout<<"dump "<<fname<<std::endl;
	}
	//////////////////////////////////////////////////////////////////////////
	void dump(const char *fname, const TVReal &x, const TVReal &vre)
	{
		std::ofstream out(fname);
		out.precision(16);
		out.setf(std::ios::scientific);

		for(size_t i(0); i<x.size(); i++)
		{
			out<<x[i]<<"\t"<<vre[i]<<"\t"<<0<<std::endl;
		}

		std::cout<<"dump "<<fname<<std::endl;
	}

	//////////////////////////////////////////////////////////////////////////
	void test(
		ResponseModel *rm,
		const TVReal &x2,
		const TVReal &ere,
		const TVReal &eim,
		TVReal &sre,
		TVReal &sim)
	{
		K ke(rm);
		TVReal x(rm->getPeriod());
#define MIN_SIM 0
#define MAX_SIM 200
#define AMOUNT_SIM 2000000
		real sRe = 0;
		TVReal sIm(AMOUNT_SIM);
		for(size_t i(0); i<sIm.size(); i++)
		{
			sIm[i] = real(MIN_SIM) + i*(real(MAX_SIM)-real(MIN_SIM))/sIm.size();
		}

		//////////////////////////////////////////////////////////////////////////
		TVReal rre(x.size());
		TVReal rim(x.size());
		TVReal ire(x.size());
		TVReal iim(x.size());

		for(size_t i(0); i<x.size(); i++)
		{
			Complex R, I;
			rm->evalRI(x[i], 1, R, I);
			rre[i] = R.re();
			rim[i] = R.im();
			ire[i] = I.re();
			iim[i] = I.im();
		}
		dump("rre", x, rre);
		dump("rim", x, rim);
		dump("ire", x, ire);
		dump("iim", x, iim);

		TVComplex mrre(AMOUNT_SIM);
		TVComplex mrim(AMOUNT_SIM);
		TVComplex mire(AMOUNT_SIM);
		TVComplex miim(AMOUNT_SIM);

		mellinFwd(
			&x[0], &rre[0], x.size(),
			MAX_SIM, &mrre[0], AMOUNT_SIM);
		mellinFwd(
			&x[0], &rim[0], x.size(),
			MAX_SIM, &mrim[0], AMOUNT_SIM);
		mellinFwd(
			&x[0], &ire[0], x.size(),
			MAX_SIM, &mire[0], AMOUNT_SIM);
		mellinFwd(
			&x[0], &iim[0], x.size(),
			MAX_SIM, &miim[0], AMOUNT_SIM);

		dump("mrre", sIm, mrre);
		dump("mrim", sIm, mrim);
		dump("mire", sIm, mire);
		dump("miim", sIm, miim);

		//////////////////////////////////////////////////////////////////////////
		dump("ere", x2, ere);
		dump("eim", x2, eim);

		TVComplex mere(AMOUNT_SIM);
		TVComplex meim(AMOUNT_SIM);
		mellinFwd(
			&x2[0], &ere[0], x2.size(),
			MAX_SIM, &mere[0], AMOUNT_SIM);
		mellinFwd(
			&x2[0], &eim[0], x2.size(),
			MAX_SIM, &meim[0], AMOUNT_SIM);

		dump("mere", sIm, mere);
		dump("meim", sIm, meim);

		//////////////////////////////////////////////////////////////////////////
		TVComplex msre(AMOUNT_SIM);
		TVComplex msim(AMOUNT_SIM);

		for(size_t i(0); i<AMOUNT_SIM; i++)
		{
			msim[i] = 
				(mere[i]/mrre[i] - meim[i]/mire[i]) / 
				(miim[i]/mire[i] - mrim[i]/mrre[i]);

			msre[i] = mere[i]/mrre[i] + mrim[i]/mrre[i]*msim[i];
		}
		dump("msre", sIm, msre);
		dump("msim", sIm, msim);

		sre.resize(x2.size());
		sim.resize(x2.size());

		mellinBwd(
			MAX_SIM, &msre[0], AMOUNT_SIM, 
			&x2[0], &sre[0], sre.size());
		mellinBwd(
			MAX_SIM, &msim[0], AMOUNT_SIM, 
			&x2[0], &sim[0], sim.size());

		dump("s", x2, sre, sim);

		return;

		
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		TVReal k(x.size());
		for(size_t i(0); i<x.size(); i++)
		{
			k[i] = ke(x[i]);
		}
		TVComplex mk(sIm.size());
		mellinFwd(
			&x[0], &k[0], x.size(),
			MAX_SIM, &mk[0], mk.size());

		std::ofstream outmk("outmk");
		outmk.precision(16);
		outmk.setf(std::ios::scientific);

		for(size_t i(0); i<sIm.size(); i++)
		{
			outmk<<sIm[i]<<"\t"<<mk[i].re()<<"\t"<<mk[i].im()<<std::endl;
		}


		//////////////////////////////////////////////////////////////////////////
		TVReal f(x.size());
		for(size_t i(0); i<x.size(); i++)
		{
			f[i]  = 1*ke(x[i]/1.0);
 			f[i] += 1*ke(x[i]/1.5);
			f[i] += 1*ke(x[i]/2.0);
			f[i] += 1*ke(x[i]/2.5);
			f[i] += 1*ke(x[i]/3.0);
			f[i] += 1*ke(x[i]/3.5);
			f[i] += 1*ke(x[i]/4.0);
		}
		TVComplex mf(sIm.size());
		mellinFwd(
			&x[0], &f[0], x.size(),
			MAX_SIM, &mf[0], mf.size());


		std::ofstream outmf("outmf");
		outmf.precision(16);
		outmf.setf(std::ios::scientific);

		for(size_t i(0); i<sIm.size(); i++)
		{
			outmf<<sIm[i]<<"\t"<<mf[i].re()<<"\t"<<mf[i].im()<<std::endl;
		}

		//////////////////////////////////////////////////////////////////////////
		TVComplex my(sIm.size(), Complex(0,0));
		for(size_t i(0); i<sIm.size(); i++)
		{
			my[i] = mf[i]/mk[i];
		}

		std::ofstream outmy("outmy");
		outmy.precision(16);
		outmy.setf(std::ios::scientific);

		for(size_t i(0); i<sIm.size(); i++)
		{
			outmy<<sIm[i]<<"\t"<<my[i].re()<<"\t"<<my[i].im()<<std::endl;
		}

		//////////////////////////////////////////////////////////////////////////
		x.clear();
		for(real t(0.0001); t<5; t+=0.0001)
		{
			x.push_back(t);
		}

		TVReal y(x.size());

		mellinBwd(
			MAX_SIM, &my[0], my.size(), 
			&x[0], &y[0], y.size());

		std::ofstream out("out");
		out.precision(16);
		out.setf(std::ios::scientific);

		for(size_t i(0); i<y.size(); i++)
		{
			out<<x[i]<<"\t"<<y[i]<<"\t"<<0<<std::endl;
		}

		exit(0);
	}
}

#endif
