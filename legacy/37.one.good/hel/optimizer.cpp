#include "stdafx.h"
#include "hel/optimizer.h"
#include "hel/workTimeUtils.h"
//#include "hel/levmar.h"
//#include "hel/fft.h"
//#include "mpfrc++/mpreal.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	Optimizer::Optimizer()
		: _separatorError(0)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void Optimizer::setup(ScaledData &scaledData, const SettingsOptimizer &settingsOptimizer)
	{
		_settingsScaledData = scaledData.getSettings();
		_settingsOptimizer = settingsOptimizer;


		_signalProcessor.setup(
			_settingsOptimizer._minPeriodE, 
			_settingsOptimizer._maxPeriodE,
			_settingsOptimizer._stepsPeriodE,
			_settingsOptimizer._periodsOnWin);

		real minTLog = log(_settingsOptimizer._minPeriodS);
		real maxTLog = log(_settingsOptimizer._maxPeriodS);
		real logtStep = (maxTLog - minTLog) / (_settingsOptimizer._stepsPeriodS);
		_periodLogS.resize(_settingsOptimizer._stepsPeriodS);
		for(size_t k(0); k<_settingsOptimizer._stepsPeriodS; k++)
		{
			_periodLogS[k] = minTLog+k*logtStep;
		}
		_valueS.resize(_periodLogS.size());

		_rm.reset(new ResponseModel);
		_rm->setup(
			1e10,	// real maxT,
			100000,	// size_t stepsT_l1,
			100000,	// size_t stepsT_ge1,
			_settingsOptimizer._periodsOnWin);
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
	void Optimizer::update(const ScaledData &scaledData, real x0, real x0Step, bool spectr)
	{
		TDTimedValue data;
		scaledData.fillSmoothedDischarged(x0, data, 100, _settingsOptimizer._periodsOnWin, true);
		_signalProcessor.update(data);

// 		//////////////////////////////////////////////////////////////////////////
// 		{
// 			const TVReal &tlog = _signalProcessor.getPeriodLogE();
// 			const TVComplex &v = _signalProcessor.getValueE();
// 			std::ofstream out("echo");
// 			out.precision(16);
// 			out.setf(std::ios::scientific);
// 
// 			const TVReal &lts = tlog;
// 			const TVComplex &vs = v;
// 
// 			for(size_t i(0); i<lts.size(); i++)
// 			{
// 				out<<exp(lts[i])<<"\t";
// 				out<<vs[i].re()<<"\t";
// 				out<<vs[i].im()<<"\n";
// 			}
// 			out.flush();
// 			exit(0);
// 		}
// 		//////////////////////////////////////////////////////////////////////////

// 		_signalProcessor.update(scaledData.getSmoothed());

		_separatorError = deconvolve(_signalProcessor.getPeriodLogE(), _signalProcessor.getValueE());
	}


	//////////////////////////////////////////////////////////////////////////
	real Optimizer::getSeparatorError()
	{
		return _separatorError;
	}


	//////////////////////////////////////////////////////////////////////////
	real Optimizer::deconvolve(const TVReal &tlog, const TVComplex &v)
	{
		//		_rm->dump("out");

		real normak, normab;
		updateNorma(tlog, v, normak, normab);
		real initError = calcError(TVReal(), TVComplex(), tlog, v, normak, normab);
		if(!initError)
		{
			initError = 1;
		}
		real error = 1;

		int iters = _rm->solve_v(
			tlog.size(), &tlog[0], &v[0],
			_periodLogS.size(), &_periodLogS[0], &_valueS[0],
			25, _work);

		error = calcError(_periodLogS, _valueS, tlog, v, normak, normab) / initError;
		return error;
	}

	//////////////////////////////////////////////////////////////////////////
	void Optimizer::updateNorma(const TVReal &lts, const TVComplex &vs, real &normak, real &normab)
	{
		TVReal as(vs.size());
		real mn=std::numeric_limits<real>::max();
		for(size_t i(0); i<vs.size(); i++)
		{
			as[i] = log(vs[i].a());
			if(as[i] > std::numeric_limits<real>::max() || as[i] < -std::numeric_limits<real>::max())
			{
				//inf
			}
			else
			{
				if(mn>as[i]) mn = as[i];
			}
		}
		for(size_t i(0); i<vs.size(); i++)
		{
			if(as[i] > std::numeric_limits<real>::max() || as[i] < -std::numeric_limits<real>::max())
			{
				as[i] = mn;
			}
		}

		if(mn == std::numeric_limits<real>::max())
		{
			normak = 0;
			normab = 0;
			return;
		}

		llsq(&lts[0], &as[0], as.size(), normak, normab);

		if(	_isnan(normak) || _isnan(normab) ||
			normak > std::numeric_limits<real>::max() || normak < -std::numeric_limits<real>::max()||
			normab > std::numeric_limits<real>::max() || normab < -std::numeric_limits<real>::max())
		{
			normak = 0;
			normab = 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	real Optimizer::calcError(const TVReal &lts, const TVComplex &vs, const TVReal &lte, const TVComplex &ve, real normak, real normab)
	{
		real err = 0;
		for(size_t i(0); i<ve.size(); i++)
		{
			Complex v = ve[i];
			for(size_t j(0); j<lts.size(); j++)
			{
				v -= _rm->eval_tabled(lte[i], lts[j], vs[j]);
			}
			err += v.a() / exp(lte[i]*normak+normab);
		}
		return err;
	}


}

