#ifndef _hel_optimizer_h_
#define _hel_optimizer_h_

#include "hel/settings.h"
#include "hel/scaledData.h"
#include "hel/speedMeter.h"
#include "hel/responseModel.h"
#include "hel/spectrStore.h"

namespace hel
{
	class Optimizer
	{
	private:
		struct LevelPos
		{
			size_t _droppedAmount;

			LevelPos()
				: _droppedAmount(-1)
			{
			}
		};
		std::vector<LevelPos> _levelPositions;

		//size_t _levelPosition2Process;
		size_t _prevScaledDataPushCounter;
		size_t _iterations2Process;
		size_t _iterationsProcessed;

		SpectrStore _spectrStore;

	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _responseModel;
			ar & _settingsScaledData;
			ar & _settingsOptimizer;

			ar & _levelPositions;
			ar & _levelPosition2Process;
			ar & _prevScaledDataPushCounter;
			ar & _iterations2Process;
			ar & _iterationsProcessed;

			ar & _wnd;
			ar & _TVector;
			ar & _APVector;

			ar & _spectrumPeriod;
			ar & _spectrumValue;
			ar & _x0;
			ar & _x0Spectrum;

			ar & _spectrStore;
		}
	private:
		ResponseModelPtr	_responseModel;
		SettingsScaledData	_settingsScaledData;
		SettingsOptimizer	_settingsOptimizer;

		size_t	_levelPosition2Process;
		size_t	_levelPositionsTotal;
		TVReal	_values4Prober;
		TVReal	_wnd;
		TVReal	_TVector;
		std::vector<Complex> _APVector;
		SpeedMeter<100>	_workSpeedMeter;

		TVReal		_spectrumPeriod;
		TVComplex	_spectrumValue;
		TVReal		_spectrumPeriod2;
		TVComplex	_spectrumValue2;
		real	_x0;
		real	_x0Spectrum;


	private:

	public:
		Complex probeOne(real *values);

	private:
		void lowPassFir(
			real Fs,
			real Fd,
			size_t n,
			TVReal &A);
		real kaizer(real beta, int n, int N);
		void applyWnd(TVReal &s, real beta);

		Complex evalConvolution(const TVReal &TVector, const TVComplex &APVector, size_t idx, real phase);

	public:
		Optimizer();
		void setup(ScaledData &scaledData, const SettingsOptimizer &settingsOptimizer);
		bool processCalibrator(size_t steps);
		real getProcessCalibratorState();
		const SettingsOptimizer &getSettings()const;
		const SettingsScaledData &getSettingsScaledData()const;
		size_t getNeedDataSamples() const;
		void prepareWork(const ScaledData &scaledData);
		void terminateWork(const ScaledData &scaledData);


		void separateSpectrum(const ScaledData &scaledData);


		bool workPart(const ScaledData &scaledData);
		real getCompletePercent() const;
		real getSpeed() const;
		real getEstimateTime() const;
		real getElapsedTime() const;



		size_t fillExtrap(
			const ScaledData &scaledData,
			const real *time,
			real *value,
			size_t size);

		TVReal &getSpectrPeriod()
		{
			return _spectrumPeriod2;
		}
		TVComplex &getSpectrValue()
		{
			return _spectrumValue2;
		}
	};

}

BOOST_CLASS_IMPLEMENTATION(hel::Optimizer::LevelPos, boost::serialization::primitive_type);
BOOST_CLASS_VERSION(hel::Optimizer, 0);

#endif
