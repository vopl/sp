#ifndef _hel_mtTypes_h_
#define _hel_mtTypes_h_


namespace hel
{
	struct HistoryHeader
	{
		int               version;            // ������ ����
		char              copyright[64];      // ��������
		char              symbol[12];         // ����������
		int               period;             // ������ �����������
		int               digits;             // ����� ������ ����� ������� � �����������
		long            timesign;           // ��������� ��������� �������� ����
		long            last_sync;          // ����� ��������� �������������
		int               unused[13];         // ��� �������� �������������
	};

#pragma pack(push,1)
	//---- ����������� ������������� ��������� � ����
	struct RateInfo
	{
		long            ctm;                // ������� ����� � ��������
		double            open;
		double            low;
		double            high;
		double            close;
		double            vol;
	};
#pragma pack(pop)

}

#endif
