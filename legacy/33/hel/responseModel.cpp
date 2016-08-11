#include "stdafx.h"
#include "hel/responseModel.h"
#include "hel/math.h"
#include "hel/level.h"
#include "hel/optimizer.h"
#include "hel/calibrator_laws.cpp"
#include "hel/levmar.h"

#define DBG_OUT 1

namespace hel
{

	//////////////////////////////////////////////////////////////////////////
	ResponseModel::ResponseModel()
	{

	}
	ResponseModel::~ResponseModel()
	{

	}
	bool ResponseModel::setup(Optimizer *optimizer, real minPeriod, real maxPeriod, size_t stepsPeriod, size_t stepsPhase)
	{
		_minPeriod = minPeriod;
		_maxPeriod = maxPeriod;
		_calibratorMultPeriodLevel2 = optimizer->getSettings()._calibratorMultPeriodLevel2;
		_stepsPhase = stepsPhase;
		_stepsPeriod = stepsPeriod;

		_T0 = 1;
		_A0 = 1;
// 		_minPeriod *= _T0;
// 		_maxPeriod *= _T0;

		_periodsOnWin = optimizer->getSettings()._periodsOnWin;
		_samplesOnLevel = optimizer->getNeedDataSamples();
		_samplesOnWin = optimizer->getSettings()._samplesOnPeriod * _periodsOnWin;
		_x0 = _maxPeriod * optimizer->getSettings()._periodsOnWin;
		_x0 *= 2;

		_realLevel1.resize(_stepsPeriod);
		_imagLevel1.resize(_stepsPeriod);

		_ti = 0;
		_matrixReady = false;
		_approxReady = false;

		return true;
	}

	void corrTest(ResponseModel *rm)
	{

	}

	real atanh(real x)
	{
		return 0.5*log((1+x)/(1-x));
	}

	//////////////////////////////////////////////////////////////////////////
	real ResponseModel::getCompletePercent()
	{

		return real(_ti+(_approxReady?1:0)) / (_stepsPeriod+1);
	}




	//////////////////////////////////////////////////////////////////////////
	bool ResponseModel::build(Optimizer *optimizer, size_t tiAmount)
	{
		if(!_matrixReady)
		{
			if(buildMatrix(optimizer, tiAmount))
			{
				_matrixReady = true;
			}
			return false;
		}
		//return _matrixReady;

		if(!_approxReady)
		{
			buildApprox();
			_approxReady = true;
		}
		return _approxReady;
	}



	//////////////////////////////////////////////////////////////////////////
	real ResponseModel::getMinPeriod()
	{
		return _minPeriod;
	}
	
	//////////////////////////////////////////////////////////////////////////
	real ResponseModel::getMaxPeriod()
	{
		return _maxPeriod;
	}

	//////////////////////////////////////////////////////////////////////////
	real ResponseModel::getMinSignificantPeriod(real T0)
	{
		return T0 * _minSignificantPeriod/_T0;
	}

	//////////////////////////////////////////////////////////////////////////
	real ResponseModel::getMaxSignificantPeriod(real T0)
	{
		return T0 * _maxSignificantPeriod/_T0;
	}

	//////////////////////////////////////////////////////////////////////////
	real ResponseModel::getMinSignificantPeriodMult()
	{
		return _minSignificantPeriod/_T0;
	}

	//////////////////////////////////////////////////////////////////////////
	real ResponseModel::getMaxSignificantPeriodMult()
	{
		return _maxSignificantPeriod/_T0;
	}




	//////////////////////////////////////////////////////////////////////////
	bool ResponseModel::buildMatrix(Optimizer *optimizer, size_t tiAmount)
	{
		size_t endTi = _ti+tiAmount;
		if(endTi > _stepsPeriod)
		{
			endTi = _stepsPeriod;
		}

		std::vector<Complex> apLine(_stepsPhase);

		TVReal alignedValues;

		if(!_ti)
		{
			_storeAPLineArgs.clear();
		}

		for(; _ti < endTi; _ti++)
		{
			real period = _minPeriod + (_maxPeriod - _minPeriod)*_ti/_stepsPeriod;

			real winLen = _periodsOnWin * period;
			real levelPeriod = winLen / _samplesOnLevel;
			_valuesPeriod = levelPeriod/500;

			Level level(levelPeriod, 0);

			real tBegin = _x0-winLen*_samplesOnLevel/_samplesOnWin-levelPeriod*2;
			real tEnd = _x0 + _valuesPeriod/2;
			real t(tBegin);

			for(size_t i(0); t<tEnd; i++)
			{
				TimedValue tv;
				tv._time = t;
				tv._value = 0;
				tv._value += _A0 * cos(c_2Pi*(t-_x0)/_T0);
				//tv._value += _A0 * cos(c_2Pi*(t-_x0)/1.15);
				level.pushSingle(tv, _samplesOnLevel, _valuesPeriod);
				t = tBegin + _valuesPeriod*i;
			}
			tBegin = t;

			//тут готов уровень с фазой 0
			size_t mi = _ti*_stepsPhase;
 			level.makeAligned(_samplesOnWin, _x0, &alignedValues);

 			apLine[0] =  optimizer->probeOne(&alignedValues.front());


			for(size_t pi(1); pi<_stepsPhase; pi++)
			{
				real tBegin2 = _x0 + _T0*pi/_stepsPhase - winLen - levelPeriod*2;
				if(tBegin2 > tEnd)
				{
					tBegin = tBegin2;
					t = tBegin;
				}

				tEnd = _x0 + _T0*pi/_stepsPhase + _valuesPeriod/2;

				for(size_t i(1); t<tEnd; i++)
				{
					TimedValue tv;
					tv._time = t;
					tv._value = 0;
					tv._value += _A0 * cos(c_2Pi*(t-_x0)/_T0);
					//tv._value += _A0 * cos(c_2Pi*(t-_x0)/1.15);
					level.pushSingle(tv, _samplesOnLevel, _valuesPeriod);
					t = tBegin + _valuesPeriod*i;
				}
				tBegin = t;

 				level.makeAligned(_samplesOnWin, _x0 + _T0*pi/_stepsPhase, &alignedValues);
 				apLine[pi] = optimizer->probeOne(&alignedValues.front());
			}

 			storeAPLine(apLine);

		}

		return _ti >= _stepsPeriod;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	struct Cos1TParams
	{
		const std::vector<Complex> &apLine;
	};
	void evalFuncCos1T(double *p, double *hx, int m, int n, void *adata)
	{
		assert(m == 4);
		Cos1TParams *params = (Cos1TParams *)adata;
		assert(n == params->apLine.size()*2);

		size_t n2 = n/2;
		real *rex = hx;
		real *imx = hx + n2;

		for(size_t idx=0; idx<n2; idx++)
		{
			real t = real(idx)/n2;
			real hArgRe = t * c_2Pi / 1 + p[0];
			real hArgIm = t * c_2Pi / 1 + p[2];
			rex[idx] = cos(hArgRe) * p[1];
			imx[idx] = cos(hArgIm) * p[3];

			rex[idx] -= params->apLine[idx].re();
			imx[idx] -= params->apLine[idx].im();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void jacoFuncCos1T(double *p, double *jx, int m, int n, void *adata)
	{
		assert(m == 4);
		Cos1TParams *params = (Cos1TParams *)adata;
		assert(n == params->apLine.size()*2);

		size_t n2 = n/2;
		real *rex = jx;
		real *imx = jx + m*n2;

		for(size_t idx=0; idx<n2; idx++)
		{
			real t = real(idx)/n2;
			real hArgRe = t * c_2Pi / 1 + p[0];
			real hArgIm = t * c_2Pi / 1 + p[2];

			rex[idx*m+0] = -sin(hArgRe) * p[1];
			rex[idx*m+1] = cos(hArgRe);
			rex[idx*m+2] = 0;
			rex[idx*m+3] = 0;

			imx[idx*m+0] = 0;
			imx[idx*m+1] = 0;
			imx[idx*m+2] = -sin(hArgIm) * p[3];
			imx[idx*m+3] = cos(hArgIm);
		}
	}



	void ResponseModel::storeAPLine(const std::vector<Complex> &apLine)
	{
		{
			static std::ofstream mt("mt");
			static std::ofstream ma("ma");
			static std::ofstream mp("mp");

			mt.precision(16);
			mt.setf(std::ios::scientific);
			ma.precision(16);
			ma.setf(std::ios::scientific);
			mp.precision(16);
			mp.setf(std::ios::scientific);

			real period = _minPeriod + (_maxPeriod - _minPeriod)*_ti/_stepsPeriod;
			mt<<period<<std::endl;

			for(size_t i(0); i<_stepsPhase; i++)
			{
				ma<<apLine[i].a()<<"\t";
				mp<<apLine[i].p()<<"\t";
			}
			ma<<std::endl;
			mp<<std::endl;
		}

// 		return;

		_storeAPLineArgs.resize(4, 0.1);
		Cos1TParams params = {apLine};

		dlevmar_der_autoMu(
			evalFuncCos1T,
			jacoFuncCos1T,
			&_storeAPLineArgs[0],
			NULL,
			_storeAPLineArgs.size(),
			apLine.size()*2,
			_threadStorage,
			&params);

		_realLevel1[_ti].setAP(_storeAPLineArgs[1], _storeAPLineArgs[0]);
		_imagLevel1[_ti].setAP(_storeAPLineArgs[3], _storeAPLineArgs[2]);

		TVReal l2(apLine.size()*2);
		evalFuncCos1T(&_storeAPLineArgs[0], &l2[0], _storeAPLineArgs.size(), l2.size(), &params);

		real sum = 0;
		for(size_t i(0); i<l2.size(); i++)
		{
			sum += fabs(l2[i]);
		}
		sum /= l2.size();
	}




	//////////////////////////////////////////////////////////////////////////
	void ResponseModel::buildApprox()
	{
		//построить среднюю линию для реальной и для мнимой
		TVReal period(_stepsPeriod);

		TVReal rem(_stepsPeriod);
		TVReal imm(_stepsPeriod);
		TVReal red(_stepsPeriod);
		TVReal imd(_stepsPeriod);

		{
			TVReal am(_stepsPeriod);
			TVReal ad(_stepsPeriod);
			TVReal pm(_stepsPeriod);
			TVReal pd(_stepsPeriod);

			for(size_t i(0); i<_stepsPeriod; i++)
			{
				period[i] = _minPeriod + real(i)/_stepsPeriod*(_maxPeriod - _minPeriod);

				Complex m(
					(_realLevel1[i].re() + _imagLevel1[i].re())/2, 
					(_realLevel1[i].im() + _imagLevel1[i].im())/2);

				Complex d(
					(_realLevel1[i].re() - _imagLevel1[i].re()), 
					(_realLevel1[i].im() - _imagLevel1[i].im()));

				am[i] = m.a();
				ad[i] = d.a();
				pm[i] = m.p();
				pd[i] = d.p();
			}
			_centerIdx = std::lower_bound(period.begin(), period.end(), _T0) - period.begin();

			//выровнять фазу на 0 в центре
			//на скачок 2пи и 
			//знак амплитуды на скачок пи
			alignMDPhase(pm, am, _centerIdx);
			alignMDPhase(pd, ad, _centerIdx);


			//найти границы линейной зоны фазы как первый минимум амплитуды от центра (сначала отступить на пол-серидины)
			real mina = (fabs(am[_centerIdx]) + fabs(ad[_centerIdx]))/2;
			for(_beginSignificantZone = _centerIdx; _beginSignificantZone >0; _beginSignificantZone--)
			{
				real a = fabs(am[_beginSignificantZone]) + fabs(ad[_beginSignificantZone]);
				if(mina >= a)
				{
					break;
				}
			}
			for(; _beginSignificantZone >0; _beginSignificantZone--)
			{
				real a = fabs(am[_beginSignificantZone]) + fabs(ad[_beginSignificantZone]);
				if(mina > a)
				{
					mina = a;
				}

				if(a > mina)
				{
					break;
				}
			}
			mina = (fabs(am[_centerIdx]) + fabs(ad[_centerIdx]))/2;
			for(_endSignificantZone = _centerIdx; _endSignificantZone <_stepsPeriod; _endSignificantZone++)
			{
				real a = fabs(am[_endSignificantZone]) + fabs(ad[_endSignificantZone]);
				if(mina >= a)
				{
					break;
				}
			}
			for(; _endSignificantZone <_stepsPeriod; _endSignificantZone++)
			{
				real a = fabs(am[_endSignificantZone]) + fabs(ad[_endSignificantZone]);
				if(mina > a)
				{
					mina = a;
				}

				if(a > mina)
				{
					break;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			for(size_t i(0); i<_stepsPeriod; i++)
			{
				rem[i] = am[i]*cos(pm[i]);
				imm[i] = am[i]*sin(pm[i]);
				red[i] = ad[i]*cos(pd[i]);
				imd[i] = ad[i]*sin(pd[i]);
			}
		}


		_minSignificantPeriod = period[_beginSignificantZone];
		_maxSignificantPeriod = period[_endSignificantZone-1];


		//заготовка второго уровня
		_stepsPeriodLevel2 = size_t(real(_stepsPeriod)/_calibratorMultPeriodLevel2 + 0.5);
		_periodLevel2.resize(_stepsPeriodLevel2);

		for(size_t i(0); i<_stepsPeriodLevel2; i++)
		{
			_periodLevel2[i] = _minPeriod + (_maxPeriod - _minPeriod)*i/_stepsPeriodLevel2;
		}
		_periodLevel2Step = (_periodLevel2.back() - _periodLevel2.front()) / _stepsPeriodLevel2;

		//////////////////////////////////////////////////////////////////////////
		//
		period.insert(period.begin(), 0);
		rem.insert(rem.begin(), 0);
		imm.insert(imm.begin(), 0);
		red.insert(red.begin(), 0);
		imd.insert(imd.begin(), 0);


		//////////////////////////////////////////////////////////////////////////
		//амплитуды сгладить полиномами
		TVReal remLevel2;
		TVReal immLevel2;
		TVReal redLevel2;
		TVReal imdLevel2;

		smoothPolynomial(
			period, 
			0,
			_stepsPeriod+1,
			_periodLevel2, 
			rem,
			1.0/_periodsOnWin,
			remLevel2);

		smoothPolynomial(
			period, 
			0,
			_stepsPeriod+1,
			_periodLevel2, 
			imm,
			1.0/_periodsOnWin,
			immLevel2);

		//////////////////////////////////////////////////////////////////////////
		//фазы сгладить полиномами
		smoothPolynomial(
			period, 
			0,
			_stepsPeriod+1,
			_periodLevel2, 
			red,
			1.0/_periodsOnWin,
			redLevel2);

		smoothPolynomial(
			period, 
			0,
			_stepsPeriod+1,
			_periodLevel2, 
			imd,
			1.0/_periodsOnWin,
			imdLevel2);


		//слить обратно на второй уровень
		_realLevel2.resize(_stepsPeriodLevel2);
		_imagLevel2.resize(_stepsPeriodLevel2);
		_realLevel2Diff.resize(_stepsPeriodLevel2);
		_imagLevel2Diff.resize(_stepsPeriodLevel2);

		for(size_t i(0); i<_stepsPeriodLevel2; i++)
		{
			real t = _periodLevel2[i];

			Complex m(remLevel2[i], immLevel2[i]);
			Complex d(redLevel2[i], imdLevel2[i]);
			d /= 2;

			_realLevel2[i] = m + d;
			_imagLevel2[i] = m - d;
		}

		//вычислить норму в T0
		Complex normaT0 = evalAP(_T0, _T0, Complex(1, 0, CAP));
		_level2AmpNorma4rating = normaT0.a();

		//производные
		for(size_t i(1); i<_stepsPeriodLevel2-1; i++)
		{
			real rra,rrb,rrc;
			real ria,rib,ric;
			real ira,irb,irc;
			real iia,iib,iic;

			parabolicArgs(
				_periodLevel2[i-1], _realLevel2[i-1].re(),
				_periodLevel2[i], _realLevel2[i].re(),
				_periodLevel2[i+1], _realLevel2[i+1].re(),
				rra, rrb, rrc);

			parabolicArgs(
				_periodLevel2[i-1], _realLevel2[i-1].im(),
				_periodLevel2[i], _realLevel2[i].im(),
				_periodLevel2[i+1], _realLevel2[i+1].im(),
				ria, rib, ric);
			
			parabolicArgs(
				_periodLevel2[i-1], _imagLevel2[i-1].re(),
				_periodLevel2[i], _imagLevel2[i].re(),
				_periodLevel2[i+1], _imagLevel2[i+1].re(),
				ira, irb, irc);

			parabolicArgs(
				_periodLevel2[i-1], _imagLevel2[i-1].im(),
				_periodLevel2[i], _imagLevel2[i].im(),
				_periodLevel2[i+1], _imagLevel2[i+1].im(),
				iia, iib, iic);

			_realLevel2Diff[i].set(2*rra*_periodLevel2[i] + rrb, 2*ria*_periodLevel2[i] + rib);
			_imagLevel2Diff[i].set(2*ira*_periodLevel2[i] + irb, 2*iia*_periodLevel2[i] + iib);
		}
		_realLevel2Diff.front() = 0;
		_imagLevel2Diff.front() = 0;
		_realLevel2Diff.back() = 0;
		_imagLevel2Diff.back() = 0;
	}


	//////////////////////////////////////////////////////////////////////////
	void ResponseModel::alignMDPhase(TVReal &p, TVReal &a, size_t centerIdx)
	{
		p[centerIdx] = alignPhase(p[centerIdx]);
		if(p[centerIdx] > c_piDiv2)
		{
			p[centerIdx] -= c_pi;
			a[centerIdx] = -a[centerIdx];
		}
		if(p[centerIdx] < -c_piDiv2)
		{
			p[centerIdx] += c_pi;
		}

		for(size_t i(centerIdx+1); i<p.size(); i++)
		{
			p[i] = alignPhase(p[i], p[i-1]);

			if(p[i] - p[i-1] > c_piDiv2)
			{
				p[i] -= c_pi;
				a[i] = -a[i];
			}
			if(p[i] - p[i-1] < -c_piDiv2)
			{
				p[i] += c_pi;
				a[i] = -a[i];
			}
		}

		for(size_t i(centerIdx-1); i<p.size(); i--)
		{
			p[i] = alignPhase(p[i], p[i+1]);

			if(p[i] - p[i+1] > c_piDiv2)
			{
				p[i] -= c_pi;
				a[i] = -a[i];
			}
			if(p[i] - p[i+1] < -c_piDiv2)
			{
				p[i] += c_pi;
				a[i] = -a[i];
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	struct PolyDlevmarParams
	{
		const real *_xdata;
		const real *_ydata;
		size_t _polyPow;
		real _a;
		real _b;
	};

	static void PolyDlevmarFunc(double *p, double *hx, int m, int n, void *adata)
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

	static void PolyDlevmarJaco(double *p, double *jx, int m, int n, void *adata)
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

	//////////////////////////////////////////////////////////////////////////
	void ResponseModel::smoothPolynomial(
		const TVReal &period, 
		size_t beginSignificantZone,
		size_t endSignificantZone,
		const TVReal &periodLevel2, 
		TVReal &am,
		real winLen,
		TVReal &ams)
	{
		TVReal amsAmount(periodLevel2.size(), 0);
		ams.clear();
		ams.resize(periodLevel2.size(), 0);

		//длина периода для полинома - 1 период фазы
		real polyLen = winLen;
		size_t polySamples = size_t (polyLen / ((_maxPeriod-_minPeriod)/_stepsPeriod) + 0.5);
		if(polySamples > 100)
		{
			polySamples = 100;
		}
		size_t polyPow = polySamples/5;

		size_t begin = beginSignificantZone - polySamples;
		size_t end = endSignificantZone + polySamples+1;

		if(begin >= _stepsPeriod)
		{
			begin = 0;
		}
		if(end > _stepsPeriod-polySamples)
		{
			end = _stepsPeriod-polySamples;
		}

		TVReal poly(polyPow+1);
		for(size_t i(begin); i<end; i++)
		{
			PolyDlevmarParams params;
			params._a = period[i];
			params._b = period[i+polySamples-1];
			params._polyPow = polyPow;
			params._xdata = &period[i];
			params._ydata = &am[i];

			real emissionError=0;
			for(;;)//фильтрация выбросов
			{
				int res = dlevmar_der_autoMu(
					PolyDlevmarFunc, 
					PolyDlevmarJaco, 
					&poly[0], 
					&am[i],
					poly.size(), 
					polySamples,
					_threadStorage,
					&params);

				//break;

				real norma = 0;
				for(size_t j(0); j<polySamples; j++)
				{
					norma += fabs(am[i+j]);
				}
				norma /= polySamples;
				if(!norma)
				{
					break;
				}

				emissionError = 0;
				size_t maxErrIdx = 0;
				for(size_t j(0); j<polySamples; j++)
				{
					real t = period[i+j];
					real w = (t-params._a)/(params._b-params._a);
					w = cos(w*c_2Pi - c_pi)/2+0.5;

					real p = hel::evalPoly(t, &poly[0], polyPow, params._a, params._b);
					real v = am[i+j];

					real e = fabs((p-v)/norma)*w;
					if(emissionError < e)
					{
						emissionError = e;
						maxErrIdx = i+j;
					}
				}

				if(emissionError > 0.01)
				{
					//std::cout<<"correct "<<am[maxErrIdx]<<" -> "<<hel::evalPoly(period[maxErrIdx], &poly[0], polyPow, params._a, params._b)<<std::endl;
					am[maxErrIdx] = hel::evalPoly(period[maxErrIdx], &poly[0], polyPow, params._a, params._b);
				}
				else
				{
					break;
				}
			}

			size_t level2Begin = std::upper_bound(periodLevel2.begin(), periodLevel2.end(), period[i]) - periodLevel2.begin();
			size_t level2End = std::upper_bound(periodLevel2.begin(), periodLevel2.end(), period[i+polySamples]) - periodLevel2.begin();

			for(size_t j(level2Begin); j<level2End; j++)
			{
				real t = periodLevel2[j];
				real v = hel::evalPoly(t, &poly[0], polyPow, params._a, params._b);
				real w = (t-params._a)/(params._b-params._a);
				w = cos(w*c_2Pi - c_pi)/2+0.5;

				ams[j] += v*w;
				amsAmount[j] += w;
			}
		}

		for(size_t i(0); i<ams.size(); i++)
		{
			if(amsAmount[i])
			{
				ams[i] /= amsAmount[i];
			}
		}
	}



	//////////////////////////////////////////////////////////////////////////
	Complex ResponseModel::evalAP(real T, real T0, const Complex &AP0)
	{
		real t = T *_T0 / T0;

		if(t <= _periodLevel2.front() || t >= _periodLevel2.back())
		{
			return Complex(0, 0);
		}
//  		if(t <= _minSignificantPeriod || t >= _maxSignificantPeriod)
//  		{
//  			return Complex(0, 0);
//  		}

		size_t idx = size_t((t - _periodLevel2[0]) / _periodLevel2Step + 0.5);
		while(idx < _stepsPeriodLevel2 && _periodLevel2[idx] < t)
		{
			idx++;
		}
		while(idx && _periodLevel2[idx-1] > t)
		{
			idx--;
		}

		real t1 = _periodLevel2[idx-1];
		real t2 = _periodLevel2[idx];

		real w = (t-t1)/(t2-t1);
		Complex re = (_realLevel2[idx-1]*(1-w) + _realLevel2[idx]*w)*AP0;
		Complex im = (_imagLevel2[idx-1]*(1-w) + _imagLevel2[idx]*w)*AP0;

		return Complex(re.re(), im.re());
	}


	//////////////////////////////////////////////////////////////////////////
	/*
	rr
	ri
	ir
	ii

	t(T, T0) := T *_T0 / T0;
	cmulRe(re1, im1, re2, im2) := re1*re2 - im1*im2;
	cmulIm(re1, im1, re2, im2) := re1*im2 + im1*re2;

	FRe(T, T0, AP0Re, AP0Im) := cmulRe(rr(t(T, T0)),ri(t(T, T0)),AP0Re,AP0Im);
	FIm(T, T0, AP0Re, AP0Im) := cmulRe(ir(t(T, T0)),ii(t(T, T0)),AP0Re,AP0Im);

	diff(FRe(T, T0, AP0Re, AP0Im), T0);
	diff(FRe(T, T0, AP0Re, AP0Im), AP0Re);
	diff(FRe(T, T0, AP0Re, AP0Im), AP0Im);

	diff(FIm(T, T0, AP0Re, AP0Im), T0);
	diff(FIm(T, T0, AP0Re, AP0Im), AP0Re);
	diff(FIm(T, T0, AP0Re, AP0Im), AP0Im);


	diff(t(T, T0), T0);
	*/
	void ResponseModel::evalAP_grad(real T, real T0, const Complex &AP0,
		Complex &dt0,
		Complex &dre0,
		Complex &dim0)
	{
		real t = T *_T0 / T0;

		if(t <= _periodLevel2.front() || t >= _periodLevel2.back())
		{
			dt0.set(0,0);
			dre0.set(0,0);
			dim0.set(0,0);
			return;
		}
//  		if(t <= _minSignificantPeriod || t >= _maxSignificantPeriod)
//  		{
//  			dt0.set(0,0);
//  			dre0.set(0,0);
//  			dim0.set(0,0);
//  			return;
//  		}

		size_t idx = size_t((t - _periodLevel2[0]) / _periodLevel2Step + 0.5);
		while(idx < _stepsPeriodLevel2 && _periodLevel2[idx] < t)
		{
			idx++;
		}
		while(idx && _periodLevel2[idx-1] > t)
		{
			idx--;
		}

		real t1 = _periodLevel2[idx-1];
		real t2 = _periodLevel2[idx];

		real w = (t-t1)/(t2-t1);
		Complex re = (_realLevel2[idx-1]*(1-w) + _realLevel2[idx]*w);
		Complex im = (_imagLevel2[idx-1]*(1-w) + _imagLevel2[idx]*w);

		Complex dre = (_realLevel2Diff[idx-1]*(1-w) + _realLevel2Diff[idx]*w);
		Complex dim = (_imagLevel2Diff[idx-1]*(1-w) + _imagLevel2Diff[idx]*w);

		real rr = re.re();
		real ri = re.im();
		real ir = im.re();
		real ii = im.im();

		real drrdt = dre.re();
		real dridt = dre.im();
		real dirdt = dim.re();
		real diidt = dim.im();

		real dtdT0 = -(_T0*T)/sqr(T0);

		real drrdT0 = drrdt*dtdT0;
		real dridT0 = dridt*dtdT0;
		real dirdT0 = dirdt*dtdT0;
		real diidT0 = diidt*dtdT0;

		//////////////////////////////////////////////////////////////////////////
		dt0.set(
			AP0.re()*drrdT0 - AP0.im()*dridT0,
			AP0.re()*dirdT0 - AP0.im()*diidT0);

		dre0.set(
			rr,
			ir);

		dim0.set(
			-ri,
			-ii);
	}

	//////////////////////////////////////////////////////////////////////////
	//solve([FRe(T, T0, AP0Re, AP0Im)=re, FIm(T, T0, AP0Re, AP0Im)=im], [AP0Re, AP0Im]);
	Complex ResponseModel::evalAP0(real T, real T0, const Complex &AP)
	{
		real t = T *_T0 / T0;

		if(t <= _periodLevel2.front() || t >= _periodLevel2.back())
		{
			return Complex(0, 0);
		}
//  		if(t <= _minSignificantPeriod || t >= _maxSignificantPeriod)
//  		{
//  			return Complex(0, 0);
//  		}

		size_t idx = size_t((t - _periodLevel2[0]) / _periodLevel2Step + 0.5);
		while(idx < _stepsPeriodLevel2 && _periodLevel2[idx] < t)
		{
			idx++;
		}
		while(idx && _periodLevel2[idx-1] > t)
		{
			idx--;
		}

		real t1 = _periodLevel2[idx-1];
		real t2 = _periodLevel2[idx];

		real w = (t-t1)/(t2-t1);
		Complex re_ = (_realLevel2[idx-1]*(1-w) + _realLevel2[idx]*w);
		Complex im_ = (_imagLevel2[idx-1]*(1-w) + _imagLevel2[idx]*w);


		real rr = re_.re();
		real ri = re_.im();
		real ir = im_.re();
		real ii = im_.im();

		real re = AP.re();
		real im = AP.im();

		return Complex(
			-(im*ri-re*ii)/(ii*rr-ir*ri),
			-(im*rr-re*ir)/(ii*rr-ir*ri));
	}


	//////////////////////////////////////////////////////////////////////////
	Complex ResponseModel::evalAP_rating(real T, real T0, const Complex &AP)
	{
		real width = 0.01;

		real dist = T/T0;
		if(dist > 1.0+width || dist <1.0-width)
		{
			return Complex(0,0);
		}

		real wnd = cos(c_pi*dist/width)/2+0.5;
		return AP*wnd;
	}

}

