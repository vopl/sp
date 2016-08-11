#ifndef _hel_spectrStore_h_
#define _hel_spectrStore_h_

#include "hel/types.h"
#include "hel/complex.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	class SpectrStoreCell
	{
	public:
		Complex	_value;
		Complex	_correction;
		real	_error;

		size_t	_idx;
	public:
		SpectrStoreCell()
			: _idx(0)
			, _error(0)
		{
		}

		SpectrStoreCell(size_t idx)
			: _idx(idx)
			, _error(0)
		{
		}

		~SpectrStoreCell()
		{

		}

	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _value;
			ar & _correction;
			ar & _error;
			ar & _idx;
		}
	};
	typedef std::deque<SpectrStoreCell> TDSpectrStoreCell;

	//////////////////////////////////////////////////////////////////////////
	struct SpectrStoreLine//спектр дл€ одного x0
	{
		real _xStep;//ширина €чейки

		TDSpectrStoreCell _data;

		real _lastAmount;//количество x в последней €чейке

		virtual ~SpectrStoreLine(){}

		size_t idxFor(real x) const
		{
			assert(x >= 0);
			return size_t(x / _xStep);
		}
		real xFor(size_t idx) const
		{
			return idx*_xStep;
		}
		real xForData(size_t idx) const
		{
			assert(idx < _data.size());
			return _data[idx]._idx*_xStep;
		}

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _xStep;
			ar & _data;
			ar & _lastAmount;
		}
	};
	typedef boost::shared_ptr<SpectrStoreLine> SpectrStoreLinePtr;

	//////////////////////////////////////////////////////////////////////////
	class SpectrStore
	{
		real _periodsOnWin;
		real _periodsOnColumn;
		real _maxWidth;
		real _tlogStep;

		typedef std::deque<SpectrStoreLinePtr> TDSpectrStoreLinePtr;
		TDSpectrStoreLinePtr _lines;
		TVReal _tlog;
	public:
		SpectrStore();
		~SpectrStore();

		void setup(
			real periodsOnWin,
			real periodsOnColumn,
			real maxWidth);

		void push(
			real x0,
			real x0Step,
			real error,
			const TVReal &tlog, 
			const TVComplex &value);

		void buildCorrections();

	private:
		struct Rect
		{
			real _tlogCenter;
			real _tlogHeight;
			real _xCenter;
			real _xWidth;
		};
		Rect rectFor(size_t lineIdx, size_t cellReverceIdx);
		size_t cellsFor(const Rect &r, std::vector<SpectrStoreCell*> &cells, TVReal &weights);

	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & _periodsOnWin;
			ar & _periodsOnColumn;
			ar & _maxWidth;
			ar & _lines;
			ar & _tlog;
			ar & _tlogStep;
		}
	};
}
#endif
