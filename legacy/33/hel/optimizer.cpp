#include "stdafx.h"
#include "hel/optimizer.h"
#include "hel/workTimeUtils.h"
#include "hel/levmar.h"
#include "hel/responseSeparator.h"
#include "dsp/DspFilter.h"


namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	Optimizer::Optimizer()
		: _levelPosition2Process(0)
		, _levelPositionsTotal(0)
		, _x0Spectrum(0)
	{
	}

	void Optimizer::lowPassFir(
		real Fs,
		real Fd,
		size_t n,
		TVReal &A)
	{
		real f = Fs/Fd;
		real w = c_2Pi*f;
		real q = 2*f;

		n+=2;
		n |= 1;
		A.resize(n-2);

		size_t mn = n/2-1;
		A[mn] = q;
		for(size_t k(1); k<n/2; k++)
		{
			//A[mn+k] = A[mn-k] = q*(sin(k*w)/(k*w));
			A[mn+k] = A[mn-k] = q* boost::math::sinc_pi(k*w);
		}
	}


	real Optimizer::kaizer(real beta, int n, int N)
	{
		real nom = boost::math::cyl_bessel_i(0, beta*sqrt(1.0-sqr(real(2*n-N+1)/(N-1))));
		real denom = boost::math::cyl_bessel_i(0, beta);

		return nom / denom;
	}

	//////////////////////////////////////////////////////////////////////////
	void Optimizer::setup(ScaledData &scaledData, const SettingsOptimizer &settingsOptimizer)
	{
		_settingsScaledData = scaledData.getSettings();
		_settingsOptimizer = settingsOptimizer;

		if(!_responseModel)
		{
			_responseModel.reset(new ResponseModel);
			_responseModel->setup(
				this,
				_settingsOptimizer._calibratorMinPeriod, 
				_settingsOptimizer._calibratorMaxPeriod, 
				_settingsOptimizer._calibratorStepsPeriod,
				_settingsOptimizer._calibratorStepsPhase);
		}

		size_t levelSize = _settingsOptimizer._samplesOnPeriod * _settingsOptimizer._periodsOnWin;
		//build fir impulse
		_wnd.resize(levelSize);
		size_t n = _wnd.size();
		size_t mn = n/2;
		_wnd[mn+0] = kaizer(_settingsOptimizer._kaizerBeta, n/2, n);
		for(size_t k(1); k<=n/2; k++)
		{
			if(mn+k < _wnd.size())
			{
				_wnd[mn+k] = kaizer(_settingsOptimizer._kaizerBeta, n/2+k, n);
			}
			_wnd[mn-k] = kaizer(_settingsOptimizer._kaizerBeta, n/2-k, n);
		}

		real gain=0;
		for(size_t k(1); k<_wnd.size(); k++)
		{
			gain += (_wnd[k-1] + _wnd[k]);
		}
		gain = gain/2.0/_wnd.size();
		gain /= 2;
		for(size_t k(0); k<_wnd.size(); k++)
		{
			_wnd[k] /= gain;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool Optimizer::processCalibrator(size_t steps)
	{
		return _responseModel->build(this, steps);
	}

	//////////////////////////////////////////////////////////////////////////
	real Optimizer::getProcessCalibratorState()
	{
		return _responseModel->getCompletePercent();
	}


	//////////////////////////////////////////////////////////////////////////
	const SettingsOptimizer &Optimizer::getSettings()const
	{
		return _settingsOptimizer;
	}

	//////////////////////////////////////////////////////////////////////////
	const SettingsScaledData &Optimizer::getSettingsScaledData()const
	{
		return _settingsScaledData;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t Optimizer::getNeedDataSamples() const
	{
		//+1 для того чтобы после дропа одного сэмпла в alignLevel осталось нужное количество
		//дропать больше одного
		return size_t(_settingsOptimizer._samplesOnPeriod * _settingsOptimizer._periodsOnWin) + 5;
	}








	//////////////////////////////////////////////////////////////////////////
	struct SEvalFuncParams
	{
		const real *_times;
		const real *_values;
		real		_T;
		real		_a;
		real		_b;
		real		_x0;
		bool		_minusLevel;
	};

	//////////////////////////////////////////////////////////////////////////
	void evalFuncCosPoly(double *p, double *hx, int m, int n, void *adata)
	{
		int maxPow = m-2-1;
		SEvalFuncParams *efp = (SEvalFuncParams *)adata;
		const real * times = efp->_times;
		real a = efp->_a;
		real b = efp->_b;
		real x0 = efp->_x0;
		real T = efp->_T;
		for(int idx=0; idx<n; idx++)
		{
			real t = times?times[idx]:idx;
			real cosVal = T?(cos((t-x0) * c_2Pi / T + p[0]) * p[1]):0;

			t = 2.0*(t - a)/(b - a) - 1.0;

			double polyVal;
			double b1;
			double b2;
			int i;

			b1 = 0;
			b2 = 0;
			i = maxPow;
			do
			{
				polyVal = 2*t*b1-b2+p[2+i];
				b2 = b1;
				b1 = polyVal;
				i = i-1;
			}
			while(i>=0);
			polyVal = polyVal-t*b2;

			hx[idx] = cosVal + polyVal;
		}

		if(efp->_minusLevel)
		{
			const real *values = efp->_values;
			for(int idx=0; idx<n; idx++, values++)
			{
				hx[idx] =  hx[idx] - *values;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void jacoFuncCosPoly(double *p, double *jx, int m, int n, void *adata)
	{
		int maxPow = m-2-1;
		SEvalFuncParams *efp = (SEvalFuncParams *)adata;
		const real * times = efp->_times;
		real a = efp->_a;
		real b = efp->_b;
		real x0 = efp->_x0;
		real T = efp->_T;

		for(int idx=0; idx<n; idx++)
		{
			real *d = jx+idx*m;

			real t = times?times[idx]:idx;

			if(T)
			{
				real hArg = c_2Pi / T * (t-x0) + p[0];
				d[0] = -sin(hArg) * p[1];
				d[1] = cos(hArg);
			}
			else
			{
				d[0] = 0;
				d[1] = 0;
			}

			t = 2.0*(t - a)/(b - a) - 1.0;
			if(maxPow == 0)
			{
				d[2+0] = 1;
			}
			else if(maxPow == 1)
			{
				d[2+0] = 1;
				d[2+1] = t;
			}
			else
			{
				d[2+0] = 1;
				d[2+1] = t;

				for(int i=2; i<=maxPow; i++)
				{
					d[2+i] = 2.0*t*d[2+i-1] - d[2+i-2];
				}
			}
		}
	}


	Complex goertzel(const real *samples, size_t size, size_t sop)
	{
		real w = c_2Pi/sop;
		real cosine = cos(w);
		real sine = sin(w);
		real coeff = 2 * cosine;

		real Q0=0, Q1=0, Q2=0;

		for(size_t i(0); i<size; i++)
		{
			Q0 = coeff * Q1 - Q2 + samples[i];
			Q2 = Q1;
			Q1 = Q0;
		}

		return Complex((Q1 - Q2 * cosine)/size, (Q2 * sine)/size);
	}

	Complex goertzel2(const real *samples, size_t size, real sop)
	{
		Complex res;

		for(size_t i(0); i<size; i++)
		{
			real x = c_2Pi * i / sop;
			res += Complex(cos(x), -sin(x)) * samples[i];
		}

		return res/size;
	}

	void Optimizer::applyWnd(TVReal &s, real beta)
	{
		size_t n = s.size();
		size_t mn = n/2;
		s[mn+0] *= kaizer(beta, n/2, n);
		for(size_t k(1); k<=n/2; k++)
		{
			if(mn+k < s.size())
			{
				s[mn+k] *= kaizer(beta, n/2+k, n);
			}
			s[mn-k] *= kaizer(beta, n/2-k, n);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	Complex Optimizer::probeOne(real *values)
	{
		//////////////////////////////////////////////////////////////////////////
		real mean = 0;
		real amount = 0;
		for(size_t i(0); i<_wnd.size(); i++)
		{
			mean += values[i]*_wnd[i];
			amount += _wnd[i];
		}
		mean /= amount;
		for(size_t i(0); i<_wnd.size(); i++)
		{
			values[i] = (values[i] - mean) * _wnd[i];
		}

		Complex m0 = goertzel2(values, _wnd.size(), _settingsOptimizer._samplesOnPeriod);

		real step0 = 1.0/_settingsOptimizer._periodsOnWin;
		Complex mm1 = goertzel2(values, _wnd.size(), 1.0/(1.0 - 1*step0)*_settingsOptimizer._samplesOnPeriod);
		Complex mp1 = goertzel2(values, _wnd.size(), 1.0/(1.0 + 1*step0)*_settingsOptimizer._samplesOnPeriod);

		return m0-(mm1*(1-step0)+mp1*(1+step0))/2;
	}


	//////////////////////////////////////////////////////////////////////////
	void Optimizer::prepareWork(const ScaledData &scaledData)
	{
		_levelPositionsTotal = scaledData.getLevelsAmount();
		if(_prevScaledDataPushCounter == scaledData.getPushCounter() && _TVector.size() >= _levelPositionsTotal)
		{
			return;
		}
		_levelPosition2Process = 0;

		size_t samplesOnWin = size_t(_settingsOptimizer._samplesOnPeriod * _settingsOptimizer._periodsOnWin);
		while(_levelPositionsTotal > 0 && scaledData.getLevel(_levelPositionsTotal-1).size() < getNeedDataSamples())
		{
			_levelPositionsTotal--;
		}

		while(_TVector.size() < _levelPositionsTotal)
		{
			const Level &level = scaledData.getLevel(_TVector.size());
			real range = level.getPeriod()*samplesOnWin;
			_TVector.push_back(range/_settingsOptimizer._periodsOnWin);
		}
 		_APVector.resize(_TVector.size());
		_x0 = scaledData.getLastTime();
	}

	//////////////////////////////////////////////////////////////////////////
	void Optimizer::terminateWork(const ScaledData &scaledData)
	{
		_prevScaledDataPushCounter = scaledData.getPushCounter();
	}

	//////////////////////////////////////////////////////////////////////////
	void Optimizer::separateSpectrum(const ScaledData &scaledData)
	{
		const TimeMapper &tm = scaledData.getTimeMapper();

		//_spectrumPeriod.clear();
		//_spectrumValue.clear();
		if(_spectrumPeriod.size())
		{
			real dx = _x0 - _x0Spectrum;
			for(size_t i(0); i<_spectrumPeriod.size(); i++)
			{
				_spectrumValue[i] = _spectrumValue[i].rotate(c_2Pi*dx/_spectrumPeriod[i]);
			}
		}

		ResponseSeparator rs(
			_TVector, 
			_APVector, 
			_responseModel,
			_spectrumPeriod,
			_spectrumValue);

		real separatorError = rs.separate(_spectrumPeriod2, _spectrumValue2);
		_x0Spectrum = _x0;

		//_spectrStore.push(_x0Spectrum, separatorError, _spectrumPeriod, _spectrumValue);
	}

	//////////////////////////////////////////////////////////////////////////
	Complex Optimizer::evalConvolution(const TVReal &TVector, const TVComplex &APVector, size_t idx, real phase)
	{

		size_t start = std::lower_bound(TVector.begin(), TVector.end(), _responseModel->getMinSignificantPeriod(TVector[idx])) - TVector.begin();
		size_t stop = std::lower_bound(TVector.begin(), TVector.end(), _responseModel->getMaxSignificantPeriod(TVector[idx])) - TVector.begin();
		if(stop < TVector.size())
		{
			stop++;
		}

		Complex AP0(1, phase, CAP);
		real T0 = TVector[idx];

		Complex res;
		for(size_t i(start+1); i<stop; i++)
		{
 			Complex fim1 = APVector[i-1] * _responseModel->evalAP(TVector[i-1], T0, AP0).conj() * _responseModel->evalAP(T0, TVector[i-1], AP0).conj();
 			Complex fi = APVector[i] * _responseModel->evalAP(TVector[i], T0, AP0).conj() * _responseModel->evalAP(T0, TVector[i], AP0).conj();

 			res += (TVector[i] - TVector[i-1])*0.5*(fi + fim1);

		}

		return res/(TVector[stop-1] - TVector[start]);
	}


	//////////////////////////////////////////////////////////////////////////
	bool Optimizer::workPart(const ScaledData &scaledData)
	{
		if(_prevScaledDataPushCounter == scaledData.getPushCounter())
		{
			return false;
		}
		size_t samplesOnWin = (_settingsOptimizer._samplesOnPeriod * _settingsOptimizer._periodsOnWin);
		size_t samplesOnLevel = getNeedDataSamples();

		//////////////////////////////////////////////////////////////////////////
		size_t steps2Process;
		real speed = _workSpeedMeter.getSpeed();
		if(speed)
		{
			steps2Process = size_t(speed * _settingsOptimizer._analyzeBestTime);
		}
		else
		{
			steps2Process = 1;
		}

		real threadTime = getThreadTime();

		TVReal threadStorage;
		size_t localIterationsAmount = 0;

		TVReal alignedValues;

		while(_levelPosition2Process < _levelPositionsTotal)
		{
			const Level &level0 = scaledData.getLevel(_levelPosition2Process);

			if(level0.size() < samplesOnLevel)
			{
				//no data
				break;
			}

			//5 это было добавлено в уровне на запас выравнивания
			level0.makeAligned(samplesOnWin, _x0, &alignedValues);
			_APVector[_levelPosition2Process] = probeOne(&alignedValues.front());

			_levelPosition2Process++;

			//////////////////////////////////////////////////////////////////////////
			//окончание отработки одного окна, проверить условие выхода
			localIterationsAmount++;
			if(localIterationsAmount >= steps2Process)
			{
				real probeThreadTime = getThreadTime() - threadTime;

				if(probeThreadTime > 0.1)
				{
					break;
				}
			}
		}

		//засеч потраченное процессорное время
		threadTime = getThreadTime() - threadTime;

		
		//_workSpeedMeter.push(probedIterationsAmount, threadTime);
		_workSpeedMeter.push(localIterationsAmount, threadTime);


		//вернуть флаг о необходимости еще работы
		return _levelPosition2Process < _levelPositionsTotal;
	}

	//////////////////////////////////////////////////////////////////////////
	real Optimizer::getCompletePercent() const
	{
		return _levelPositionsTotal?(real(_levelPosition2Process) / _levelPositionsTotal) : 0;
	}

	//////////////////////////////////////////////////////////////////////////
	real Optimizer::getSpeed() const
	{
		return _workSpeedMeter.getSpeed();
	}

	//////////////////////////////////////////////////////////////////////////
	real Optimizer::getEstimateTime() const
	{
		real speed = _workSpeedMeter.getSpeed();

		if(!speed)
		{
			return 0;
		}

		return (_levelPositionsTotal - _levelPosition2Process) / speed;
	}

	//////////////////////////////////////////////////////////////////////////
	real Optimizer::getElapsedTime() const
	{
		return _workSpeedMeter.getElapsedTime();
	}


	//////////////////////////////////////////////////////////////////////////
	size_t Optimizer::fillExtrap(
		const ScaledData &scaledData,
		const real *time,
		real *value,
		size_t size)
	{
		real pastConstWndPeriods = _settingsOptimizer._periodsOnWin*0.9;
		real pastCosWndPeriods = _settingsOptimizer._periodsOnWin*1.1;

		real futConstWndPeriods = _settingsOptimizer._extrapConstPeriods;
		real futCosWndPeriods = _settingsOptimizer._extrapPeriods;


		TVReal speriod = _spectrumPeriod;
		TVReal samplitude(speriod.size());
		TVReal sphase(speriod.size());

		for(size_t i(0); i<speriod.size(); i++)
		{
			samplitude[i] = _spectrumValue[i].a();
			sphase[i] = _spectrumValue[i].p();
		}

		//_spectrStore.mkSlice(_x0Spectrum, 1, speriod, samplitude, sphase);





		for(size_t i(0); i<size; i++)
		{
			real v = 0;
			real t = time[i];
			for(size_t j(0); j<speriod.size(); j++)
			{
				if(speriod[j] < _responseModel->getMaxSignificantPeriod(_TVector.front()) || speriod[j] > _responseModel->getMinSignificantPeriod(_TVector.back()))
				{
					continue;
				}
				real tPeriods = (t - _x0Spectrum)/speriod[j];
				real wnd = 1;

				if(tPeriods>0)
				{//fut
					if(tPeriods > futCosWndPeriods)
					{
						//wnd = 0
						continue;
					}

					if(tPeriods > futConstWndPeriods)
					{
						wnd = cos((tPeriods-futConstWndPeriods)/(futCosWndPeriods-futConstWndPeriods)*c_pi)/2+0.5;
					}
					else
					{
						wnd = 1;
					}
				}
				else
				{//past
					if(tPeriods < -pastCosWndPeriods)
					{
						//wnd = 0
						continue;
					}

					if(tPeriods < -pastConstWndPeriods)
					{
						tPeriods = -tPeriods;
						wnd = cos((tPeriods-pastConstWndPeriods)/(pastCosWndPeriods-pastConstWndPeriods)*c_pi)/2+0.5;
					}
					else
					{
						wnd = 1;
					}
				}

				v += samplitude[j]*cos(c_2Pi*(t-_x0Spectrum)/speriod[j]+sphase[j]) * wnd;
			}

			value[i] = v;
		}


		return 0;
	}
}

