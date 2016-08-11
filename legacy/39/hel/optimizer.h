#ifndef _hel_optimizer_h_
#define _hel_optimizer_h_

#include "hel/settings.h"
#include "hel/scaledData.h"
#include "hel/responseModel.h"
#include "hel/signalProcessor.h"

namespace hel
{
	class Optimizer
	{
	public:
		SettingsScaledData	_settingsScaledData;
		SettingsOptimizer	_settingsOptimizer;

		SignalProcessor _signalProcessor;

		ResponseModelPtr			_rm;
		TVReal						_periodLogS;
		TVComplex					_valueS;
		real						_separatorError;
		TVReal						_work;

	public:
		Optimizer();
		virtual ~Optimizer(){}

		void setup(ScaledData &scaledData, const SettingsOptimizer &settingsOptimizer);
		const SettingsOptimizer &getSettings()const;
		const SettingsScaledData &getSettingsScaledData()const;

		void update(const ScaledData &scaledData, real x0, real x0Step, bool spectr);

		real getSeparatorError();

		const TVReal &getResponsePeriodLog()
		{
			return _signalProcessor.getPeriodLogE();
		}
		const TVComplex &getResponseValue()
		{
			return _signalProcessor.getValueE();
		}
		const TVReal &getSpectrPeriodLog()
		{
			return _periodLogS;
		}
		const TVComplex &getSpectrValue()
		{
			return _valueS;
		}

	private:
		real deconvolve(const TVReal &tlog, const TVComplex &v);
		void updateNorma(const TVReal &tlog, const TVComplex &v, real &normak, real &normab);
		real calcError(const TVReal &tlogs, const TVComplex &vs, const TVReal &tloge, const TVComplex &ve, real normak, real normab);

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _settingsScaledData;
			ar & _settingsOptimizer;

			ar & _signalProcessor;

			ar & _rm;
			ar & _periodLogS;
			ar & _valueS;
			ar & _separatorError;
		}
	private:
	};

	typedef boost::shared_ptr<Optimizer> OptimizerPtr;

}

BOOST_CLASS_VERSION(hel::Optimizer, 0);

#endif
