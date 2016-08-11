#ifndef _helPlayer_spectrStore_h_
#define _helPlayer_spectrStore_h_

#include "hel/types.h"
#include "hel/complex.h"

namespace hel_player
{
	using namespace hel;
	//////////////////////////////////////////////////////////////////////////
	class SpectrStoreCell
	{
	public:
		Complex	_value;
		size_t	_idx;
	public:
		SpectrStoreCell()
			: _idx(0)
		{
		}

		SpectrStoreCell(size_t idx)
			: _idx(idx)
		{
		}

		~SpectrStoreCell()
		{

		}
	};
	typedef std::deque<SpectrStoreCell> TDSpectrStoreCell;

	//////////////////////////////////////////////////////////////////////////
	struct SpectrStoreLine//спектр дл€ одного x0
	{
		real _xStep;//ширина €чейки

		TDSpectrStoreCell _data;

		Complex _lastSum;//количество
		real _lastAmount;//количество x в последней €чейке

		SpectrStoreLine()
			: _lastAmount(0)
			, _xStep(1)
		{
		}
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
	};
	typedef boost::shared_ptr<SpectrStoreLine> SpectrStoreLinePtr;

	//////////////////////////////////////////////////////////////////////////
	class SpectrStore
	{
		real _periodsOnColumn;
		real _periodsOnRow;

		typedef std::deque<SpectrStoreLinePtr> TDSpectrStoreLinePtr;
		TDSpectrStoreLinePtr _lines;
		TVReal _ts;
	public:
		SpectrStore(real periodsOnColumn,
			real periodsOnRow);
		~SpectrStore();

		void push(
			real x0,
			real x0Step,
			const TVReal &ts, 
			const TVComplex &vs);
		void getMarks(TVReal &marks);

		bool fillNNOut(size_t ti, size_t tRange, size_t xPowBase, size_t xRange, TVReal &data);
	};
}
#endif
