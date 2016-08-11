#ifndef _hel_settings_h_
#define _hel_settings_h_

#include "types.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct SettingsScaledData
	{
		//период первого уровн€
		real	_zeroPeriod;

		//множитель масштаба периодов
		real	_periodMult;

		//дыры в данных аппроксимировать линейно. Ёто позвол€ет получать аппрокс результы равномерно но увеличивает нагрузку на контейнер данных
		bool	_linearFillWhenPush;

		//вырезать выходные
		bool	_dropWeekends;

		//начало конца недели с начала недели
		real	_weekendBegin;

		real	_timeMostPast;
		real	_timeZero;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SettingsOptimizer
	{
		//количество периодов на опимизируемое окно
		size_t	_periodsOnWin;

		//отсчетов на период при оптимизации (отсчетов на окно = _samplesOnPeriod*_periodsOnWin)
		size_t	_samplesOnPeriod;

		//при автоподборе объема непрерывной оптимизации идеальна€ продолжительность в секундах
		real	_analyzeBestTime;

		//управление степенью полинома
		size_t	_polyPow;

		//дл€ фильтра параметр окна
		real	_kaizerBeta;

		//////////////////////////////////////////////////////////////////////////
		//дл€ калибратора
		real	_calibratorMinPeriod;
		real	_calibratorMaxPeriod;
		size_t	_calibratorStepsPeriod;
		size_t	_calibratorStepsPhase;
		real	_calibratorMultPeriodLevel2;

		//дл€ восстановлени€ экстрапа
		real	_extrapConstPeriods;//количество периодов дл€ 1:1
		real	_extrapPeriods;//количество периодов всего
	};
}

#endif
