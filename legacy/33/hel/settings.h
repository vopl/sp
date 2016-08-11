#ifndef _hel_settings_h_
#define _hel_settings_h_

#include "types.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	struct SettingsScaledData
	{
		//������ ������� ������
		real	_zeroPeriod;

		//��������� �������� ��������
		real	_periodMult;

		//���� � ������ ���������������� �������. ��� ��������� �������� ������� �������� ���������� �� ����������� �������� �� ��������� ������
		bool	_linearFillWhenPush;

		//�������� ��������
		bool	_dropWeekends;

		//������ ����� ������ � ������ ������
		real	_weekendBegin;

		real	_timeMostPast;
		real	_timeZero;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SettingsOptimizer
	{
		//���������� �������� �� ������������� ����
		size_t	_periodsOnWin;

		//�������� �� ������ ��� ����������� (�������� �� ���� = _samplesOnPeriod*_periodsOnWin)
		size_t	_samplesOnPeriod;

		//��� ����������� ������ ����������� ����������� ��������� ����������������� � ��������
		real	_analyzeBestTime;

		//���������� �������� ��������
		size_t	_polyPow;

		//��� ������� �������� ����
		real	_kaizerBeta;

		//////////////////////////////////////////////////////////////////////////
		//��� �����������
		real	_calibratorMinPeriod;
		real	_calibratorMaxPeriod;
		size_t	_calibratorStepsPeriod;
		size_t	_calibratorStepsPhase;
		real	_calibratorMultPeriodLevel2;

		//��� �������������� ��������
		real	_extrapConstPeriods;//���������� �������� ��� 1:1
		real	_extrapPeriods;//���������� �������� �����
	};
}

#endif
