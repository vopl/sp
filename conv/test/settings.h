#ifndef _hel_settings_h_
#define _hel_settings_h_

#include "types.h"

namespace hel
{
    //////////////////////////////////////////////////////////////////////////
    struct SettingsTimeMapper
    {
        //вырезать выходные
        bool    _dropWeekends;

        //начало конца недели с начала недели
        real    _weekendBegin;
    };

    //////////////////////////////////////////////////////////////////////////
    struct SettingsScaledData
    {
        //длина сэмпла нулевого уровня
        real    _zeroPeriod;

        //количество сглаживания в минимальных частях спектра
        size_t    _smoothLength;

        //количество серии к анализу
        size_t    _maxLength;
    };

    //////////////////////////////////////////////////////////////////////////
    struct SettingsOptimizer
    {
        //количество периодов на опимизируемое окно
        size_t    _periodsOnWin;

        //диапазон периодов
        real    _minPeriodE;
        real    _maxPeriodE;

        //отсчетов периода
        size_t    _stepsPeriodE;

        //диапазон периодов
        real    _minPeriodS;
        real    _maxPeriodS;

        //отсчетов периода
        size_t    _stepsPeriodS;
    };
}

#endif
