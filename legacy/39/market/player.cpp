#include "stdafx.h"
#include "market/player.h"
#include "hel/math.h"

namespace market
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player(const SettingsPlayer &settings)
		: _settings(settings)
		, _thickGen(1)
	{
		_timeMapper.setup(0, 1, 0, 1, _settings._dropWeekends, real(_settings._weekendBegin));
	}

	//////////////////////////////////////////////////////////////////////////
	void Player::pushData(const TVTimedValue &prices)
	{
		_prices.insert(_prices.end(), prices.begin(), prices.end());
	}

	//////////////////////////////////////////////////////////////////////////
	TContracts Player::step(hel::ICore *helCore)
	{
		_contractsCompleted.clear();

		if(!_prices.empty())
		{
			//отработать открытые
			checkClose(_prices);

			if(helCore)
			{
				//посмотреть по хел надо ли открывать
				checkOpen(helCore, _prices.back());
			}
		}
		_prices.clear();

		return _contractsCompleted;
	}


	//////////////////////////////////////////////////////////////////////////
	void Player::checkOpen(hel::ICore *helCore, const TimedValue &price)
	{
		time_t time = time_t(_timeMapper.external2Internal(price._time)+0.5);
		const size_t size = 1000;
		TVReal period(size);
		TVReal amplitude(size);
		TVReal phase(size);

		double phaseBound = _settings._signaledPahseRange/2*c_2Pi;

		for(size_t extrapIndex = 0; extrapIndex < _settings._extrapsAmount; extrapIndex++)
		{
			size_t sizeCur = size;
			helCore->getTrajectories(
				extrapIndex, 
				price._time, 
				&period.front(), 
				&amplitude.front(), 
				&phase.front(), 
				sizeCur, 
				_settings._minPeriod, 
				_settings._maxPeriod);

			for(size_t trajIndex=0; trajIndex<sizeCur; trajIndex++)
			{
				Contract c = {};


				if(phase[trajIndex]<-c_pi+phaseBound || phase[trajIndex]>c_pi-phaseBound)
				{
					c._ect = ectLong;
				}
				else if(phase[trajIndex]>-phaseBound && phase[trajIndex]<+phaseBound)
				{
					c._ect = ectShort;
				}
				else
				{
					continue;
				}

				c._extrapolatorId = extrapIndex;
				c._openTime = time;
				c._length = time_t(0.5*period[trajIndex]+0.5);


				if(!alreadyOpened(c))
				{
					c._openPrice = price._value;
					c._minPrice = c._maxPrice = price._value;
					c._lots = 1;
					c._ticket = _thickGen++;

					c._amplitude = amplitude[trajIndex];
					c._phase = phase[trajIndex];
					c._period = period[trajIndex];

					_contractsInProgress.push_back(c);
				}
			}  
		}

	}

	//////////////////////////////////////////////////////////////////////////
	void Player::checkClose(const TVTimedValue &prices)
	{
		TVTimedValue::const_iterator piter = prices.begin();
		TVTimedValue::const_iterator pend = prices.end();

		for(; piter!=pend; piter++)
		{
			const TimedValue &price = *piter;
			time_t time = time_t(_timeMapper.external2Internal(price._time)+0.5);
			TContracts::iterator citer = _contractsInProgress.begin();
			TContracts::iterator cend = _contractsInProgress.end();

			for(; citer!=cend; )
			{
				Contract &c = *citer;
				c._minPrice = std::min(c._minPrice, price._value);
				c._maxPrice = std::max(c._maxPrice, price._value);
				if(c._openTime + c._length < time)
				{
					c._closePrice = price._value;
					c._closeTime = time;
					
					if(ectLong == c._ect)
					{
						c._profit = c._closePrice - c._openPrice;
					}
					else
					{
						c._profit = c._openPrice - c._closePrice;
					}

					_contractsCompleted.push_back(c);
					citer = _contractsInProgress.erase(citer);
					cend = _contractsInProgress.end();
					continue;
				}
				citer++;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool Player::alreadyOpened(const Contract &c)
	{
		//_distanceBound

		TContracts::reverse_iterator iter = _contractsInProgress.rbegin();
		TContracts::reverse_iterator end = _contractsInProgress.rend();

		for(; iter!=end; iter++)
		{
			const Contract & c2 = *iter;
			if(c._ect != c2._ect)
			{
				continue;
			}
			if(c._extrapolatorId != c2._extrapolatorId)
			{
				continue;
			}
			if(c.distance(*iter) < _settings._distanceBound)
			{
				return true;
			}
		}

		return false;
	}

}
