#ifndef _hel_settings_h_
#define _hel_settings_h_

#include "types.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct SettingsTimeMapper
	{
		//�������� ��������
		bool	_dropWeekends;

		//������ ����� ������ � ������ ������
		real	_weekendBegin;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SettingsScaledData
	{
		//����� ������ �������� ������
		real	_zeroPeriod;

		//���������� ����������� � ����������� ������ �������
		size_t	_smoothLength;

		//���������� ����� � �������
		size_t	_maxLength;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SettingsOptimizer
	{
		//���������� �������� �� ������������� ����
		size_t	_periodsOnWin;

		//�������� ��������
		real	_minPeriodE;
		real	_maxPeriodE;

		//�������� �������
		size_t	_stepsPeriodE;

		//�������� ��������
		real	_minPeriodS;
		real	_maxPeriodS;

		//�������� �������
		size_t	_stepsPeriodS;
	};
}

#endif
