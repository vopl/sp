#ifndef _hel_signalProcessor_h_
#define _hel_signalProcessor_h_

#include "hel/types.h"
#include "hel/complex.h"
#include "hel/responseModel.h"
#include "hel/convolCore_meandr.h"
#include "hel/convolCore_trig.h"
#include "hel/convolCore_trig2.h"
#include "hel/convolCore_triangle.h"

namespace hel
{

	//////////////////////////////////////////////////////////////////////////
	class OnePeriodProcessor
	{
		real _period;
		real _winLen;
		real _step0;

		typedef ConvolCore_trig2 Part;

		ConvolCore_trig		_part_m1;
		ConvolCore_trig		_part_p1;

		Complex _value;

	public:
		OnePeriodProcessor();
		void setup(real period, real periodsOnWin);
		Complex eval(const TDTimedValue &signal);

	private:

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _period;
			ar & _winLen;
			ar & _step0;

			ar & _part_m1;
			ar & _part_p1;
		}


	};

	//////////////////////////////////////////////////////////////////////////
	class SignalProcessor
	{
		real _periodsOnWin;
		TVReal _periodLogE;
		TVComplex _valueE;

		typedef std::vector<OnePeriodProcessor> TVProcessors;
		TVProcessors _processors;
	public:
		SignalProcessor();
		void setup(
			real minPeriodE, 
			real maxPeriodE, 
			size_t stepsPeriodE, 
			real periodsOnWin);
		void update(const TDTimedValue &signal);

		const TVReal &getPeriodLogE() const;
		const TVComplex &getValueE() const;
	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _periodsOnWin;
			ar & _periodLogE;
			ar & _valueE;
			ar & _processors;
		}

	};
}
#endif
