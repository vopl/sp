#ifndef _market_palayer_h_
#define _market_palayer_h_

#include "hel/types.h"
#include "hel/icore.h"
#include "hel/timeMapper.h"
#include "statist/contract.h"

namespace market
{
	//////////////////////////////////////////////////////////////////////////
	using namespace hel;
	using namespace statist;
	struct SettingsPlayer
	{
		real	_signaledPahseRange;
		real	_distanceBound;
		real	_minPeriod;
		real	_maxPeriod;
		size_t	_extrapsAmount;

		bool	_dropWeekends;
		time_t	_weekendBegin;

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _signaledPahseRange;
			ar & _distanceBound;
			ar & _minPeriod;
			ar & _maxPeriod;
			ar & _extrapsAmount;

			ar & _dropWeekends;
			ar & _weekendBegin;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	class Player
	{

	public:
		Player(){}
		Player(const SettingsPlayer &settings);
		virtual ~Player(){}

		void pushData(const TVTimedValue &prices);
		TContracts step(hel::ICore *helCore);

	private:
		TVTimedValue	_prices;
		size_t			_thickGen;
		SettingsPlayer	_settings;
		TimeMapper		_timeMapper;

		TContracts		_contractsInProgress;
		TContracts		_contractsCompleted;

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _prices;
			ar & _thickGen;
			ar & _settings;
			ar & _timeMapper;

			ar & _contractsInProgress;
			ar & _contractsCompleted;
		}

	private:
		void checkOpen(hel::ICore *helCore, const TimedValue &price);
		void checkClose(const TVTimedValue &prices);

		bool alreadyOpened(const Contract &c);
	};
}

#endif
