#ifndef _hel_spectrStore_h_
#define _hel_spectrStore_h_

#include "hel/types.h"
#include "hel/complex.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	class SpectrStore
	{
		struct Line
		{
			real _x0;
			real _error;
			TVReal _period;
			TVComplex _value;

			Line()
				: _x0(0)
				, _error(0)
			{
			}

			Line(
				real x0, 
				real error,
				const TVReal &period, 
				const TVComplex &value)
				: _x0(x0)
				, _error(error)
				, _period(period)
				, _value(value)
			{
			}

			virtual ~Line(){};
		private:
			friend class boost::serialization::access;
			template<class Archive> void serialize(Archive & ar, const unsigned int version)
			{
				ar & _x0;
				ar & _error;
				ar & _period;
				ar & _value;
			}
		};
		typedef boost::shared_ptr<Line> LinePtr;
		struct LinePtrXPred
		{
			bool operator()(const LinePtr &l1, const LinePtr &l2)
			{
				return l1->_x0 < l2->_x0;
			}
			bool operator()(const real &l1, const LinePtr &l2)
			{
				return l1 < l2->_x0;
			}
			bool operator()(const LinePtr &l1, const real &l2)
			{
				return l1->_x0 < l2;
			}
		};

		size_t lineIdx(real x)
		{
			return std::lower_bound(_lines.begin(), _lines.end(), x, LinePtrXPred()) - _lines.begin();
		}
		size_t lineIdx(const LinePtr &l)
		{
			return std::lower_bound(_lines.begin(), _lines.end(), l, LinePtrXPred()) - _lines.begin();
		}

		typedef std::deque<LinePtr> TVLine;
		TVLine	_lines;
	public:
		void push(
			real x0, 
			real error,
			const TVReal &period, 
			const TVComplex &value);

		void mkSlice(
			real x0, 
			real pow,
			TVReal &period, 
			TVReal &amplitude, 
			TVReal &phase);

	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _lines;
		}
	};
}
#endif
