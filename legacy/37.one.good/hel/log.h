#ifndef _hel_log_h_
#define _hel_log_h_


#include <iomanip>
#include <sys/timeb.h>
#include <time.h>

namespace msoi
{
	//////////////////////////////////////////////////////////////////////////
	class KLogHelper
	{
		std::ofstream	&_out;
		std::strstream	_logstrstream;
	public:
		KLogHelper(std::ofstream &out)
			: _out(out)
		{
		}
		~KLogHelper()
		{
			if(_out)
			{
				std::string str = _logstrstream.str();
				_out.write(str.c_str(), str.length());
			}
		};
		KLogHelper &operator<<(size_t t)
		{
			_logstrstream<<(unsigned int)t;
			return *this;
		}
		KLogHelper &operator<<(CSZ t)
		{
			if(t)
			{
				_logstrstream<<t;
			}
			else
			{
				_logstrstream<<"(null)";
			}
			return *this;
		}
		KLogHelper &operator<<(const std::strstream &lss)
		{
			_logstrstream<<lss.str();
			return *this;
		}

		template <class T>
			KLogHelper &operator<<(T t)
		{
			_logstrstream<<t;
			return *this;
		}
	};

	inline std::string time()
	{
		struct timeb timebuffer;
		ftime( &timebuffer );
		tm *ptm = localtime(&timebuffer.time);
		char buf[64];
		sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%002d", 
			ptm->tm_year + 1900, 
			ptm->tm_mon, 
			ptm->tm_mday,
			ptm->tm_hour,
			ptm->tm_min,
			ptm->tm_sec,
			timebuffer.millitm);
		return std::string(buf);
	}
}

extern std::ofstream g_logOfstream_global;
#define GLOG_CLOSE() g_logOfstream_global.close();g_logOfstream_global.clear()
#define GLOG_OPEN(fname) GLOG_CLOSE(); g_logOfstream_global.open(fname, std::ios::app);GLOG<<"start log in "<<fname
#define GLOG (KLogHelper(g_logOfstream_global))


#endif
