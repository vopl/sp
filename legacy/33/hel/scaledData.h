#ifndef _hel_scaledData_h_
#define _hel_scaledData_h_

#include "hel/settings.h"
#include "hel/level.h"
#include "hel/timeMapper.h"

namespace hel
{
	class ScaledData
	{
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _settings;
			ar & _originalData;
			ar & _lastTimeExternal;
			ar & _lastValue;
			ar & _levels;
			ar & _pushCounter;
			ar & _timeMapper;

			ar & _lastDerivative;
			ar & _lastDerivative2;

			ar & _lastITime;
			ar & _lastIValue;
			ar & _lastPushedTime;
		}
	private:
		SettingsScaledData	_settings;
		Level			_originalData;
		real			_lastTimeExternal;
		real			_lastValue;
		typedef std::vector<LevelPtr> TVLevels;
		TVLevels		_levels;

		LevelPtr _getLevel(size_t levelIdx);

		size_t			_pushCounter;

		TimeMapper		_timeMapper;
		real			_lastITime;
		real			_lastIValue;
		real			_lastDerivative;
		real			_lastDerivative2;
		real			_lastPushedTime;

	private:
		void pushDataIntoLevels(const TimedValue *data, size_t size, size_t keepSamples, real internalZeroPeriod);

	public:
		ScaledData();
		void setup(const SettingsScaledData &settings);
		const SettingsScaledData &getSettings()const;
		void pushData(const TimedValue *data, size_t size, size_t keepSamples);
		real getLastTime() const;
		real getLastTimeExternal() const;
		real getLastValue() const;

		size_t getPushCounter() const;

		size_t getLevelsAmount() const;
		const Level &getLevel(size_t index) const;
		const Level &getOriginal() const;

		const TimeMapper &getTimeMapper() const;
	};

}
BOOST_CLASS_VERSION(hel::ScaledData, 0);

#endif
