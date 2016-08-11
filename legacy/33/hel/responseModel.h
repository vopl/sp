#ifndef _hel_responseModel_h_
#define _hel_responseModel_h_

#include "hel/types.h"
#include "hel/complex.h"

namespace hel
{
	class Optimizer;
	class ResponseModel
	{
	public:
		ResponseModel();
		virtual ~ResponseModel();
		bool setup(Optimizer *optimizer, real minPeriod, real maxPeriod, size_t stepsPeriod, size_t stepsPhase);
		real getCompletePercent();
		bool build(Optimizer *optimizer, size_t tiAmount);
		real getMinPeriod();
		real getMaxPeriod();

		real getMinSignificantPeriod(real T0);
		real getMaxSignificantPeriod(real T0);
		real getMinSignificantPeriodMult();
		real getMaxSignificantPeriodMult();

	private:
		bool buildMatrix(Optimizer *optimizer, size_t tiAmount);
		void storeAPLine(const std::vector<Complex> &apLine);

		void buildApprox();
		void alignMDPhase(TVReal &p, TVReal &a, size_t centerIdx);
		void smoothPolynomial(
			const TVReal &period, 
			size_t beginSignificantZone,
			size_t endSignificantZone,
			const TVReal &periodLevel2, 
			TVReal &am,
			real winLen,
			TVReal &ams);


		real _minPeriod;
		real _maxPeriod;
		real _calibratorMultPeriodLevel2;
		size_t _samplesOnLevel;
		size_t _samplesOnWin;
		size_t _periodsOnWin;
		real _valuesPeriod;
		real _x0;
		real _T0;
		real _A0;

		TVReal _storeAPLineArgs;

		size_t _stepsPhase;
		size_t _stepsPeriod;

		bool _matrixReady;
		bool _approxReady;
		size_t _ti;


		TVReal _threadStorage;



		//////////////////////////////////////////////////////////////////////////
	private:
		//первый уровень разложения - реальная и мнимая часть отклика по переменной фазе
		//от minPeriod до maxPeriod по stepsPeriod
		std::vector<Complex> _realLevel1;
		std::vector<Complex> _imagLevel1;

		//индекс периода, ближайшего к центру симметрии
		size_t _centerIdx;
		size_t _beginSignificantZone;
		size_t _endSignificantZone;

		size_t	_stepsPeriodLevel2;
		TVReal	_periodLevel2;
		real	_periodLevel2Step;
		std::vector<Complex> _realLevel2;
		std::vector<Complex> _imagLevel2;
		std::vector<Complex> _realLevel2Diff;
		std::vector<Complex> _imagLevel2Diff;

		real _minSignificantPeriod;
		real _maxSignificantPeriod;

		real _level2AmpNorma4rating;

	private:



	public://новый интерфейс
		Complex evalAP(real T, real T0, const Complex &AP0);
		void evalAP_grad(real T, real T0, const Complex &AP0,
			Complex &dt,
			Complex &dapRe,
			Complex &dapIm);
		Complex evalAP0(real T, real T0, const Complex &AP);

		Complex evalAP_rating(real T, real T0, const Complex &AP);



	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{

			ar & _minPeriod;
			ar & _maxPeriod;
			ar & _calibratorMultPeriodLevel2;
			ar & _samplesOnLevel;
			ar & _samplesOnWin;
			ar & _periodsOnWin;
			ar & _valuesPeriod;
			ar & _x0;
			ar & _T0;
			ar & _A0;

			ar & _storeAPLineArgs;

			ar & _stepsPhase;
			ar & _stepsPeriod;
			ar & _ti;

			ar & _matrixReady;
			ar & _approxReady;

			//////////////////////////////////////////////////////////////////////////
			ar & _realLevel1;
			ar & _imagLevel1;

			ar & _centerIdx;
			ar & _beginSignificantZone;
			ar & _endSignificantZone;

			ar & _stepsPeriodLevel2;
			ar & _periodLevel2;
			ar & _periodLevel2Step;

			ar & _realLevel2;
			ar & _imagLevel2;
			ar & _realLevel2Diff;
			ar & _imagLevel2Diff;

			ar & _minSignificantPeriod;
			ar & _maxSignificantPeriod;

			ar & _level2AmpNorma4rating;





			//_approxReady = false;

		}
	};

	typedef boost::shared_ptr<ResponseModel> ResponseModelPtr;
}
BOOST_CLASS_VERSION(hel::ResponseModel, 0);


#endif
