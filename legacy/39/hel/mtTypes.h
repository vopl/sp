#ifndef _hel_mtTypes_h_
#define _hel_mtTypes_h_


namespace hel
{
	struct HistoryHeader
	{
		int               version;            // версия базы
		char              copyright[64];      // копирайт
		char              symbol[12];         // инструмент
		int               period;             // период инструмента
		int               digits;             // число знаков после запятой в инструменте
		long            timesign;           // временной отпечаток создания базы
		long            last_sync;          // время последней синхронизации
		int               unused[13];         // для будущего использования
	};

#pragma pack(push,1)
	//---- Стандартное представление котировки в базе
	struct RateInfo
	{
		long            ctm;                // текущее время в секундах
		double            open;
		double            low;
		double            high;
		double            close;
		double            vol;
	};
#pragma pack(pop)

}

#endif
