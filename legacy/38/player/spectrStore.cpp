#include <stdafx.h>
#include "spectrStore.h"

namespace hel_player
{
	//////////////////////////////////////////////////////////////////////////
	SpectrStore::SpectrStore(
		real periodsOnColumn,
		real periodsOnRow)
		: _periodsOnColumn(1)
		, _periodsOnRow(1)
	{
		_periodsOnColumn = periodsOnColumn;
		_periodsOnRow = periodsOnRow;
	}

	//////////////////////////////////////////////////////////////////////////
	SpectrStore::~SpectrStore()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void SpectrStore::push(
		real x0, 
		real x0Step, 
		const TVReal &ts, 
		const TVComplex &vs)
	{
		assert(ts.size() == vs.size());

		while(_lines.size() < ts.size())
		{
			size_t idx = _ts.size();

			_ts.push_back(ts[idx]);
			real t = ts[idx];

			SpectrStoreLinePtr line(new SpectrStoreLine);

			line->_xStep = t * _periodsOnColumn;
			line->_data.push_back(SpectrStoreCell(line->idxFor(x0)));
			_lines.push_back(line);
		}

		real x0Start = x0;
		real x0Stop = x0+x0Step;
		for(size_t i(0); i<_lines.size(); i++)
		{
			real minTime = x0 - _periodsOnRow*_ts[i];
			SpectrStoreLinePtr &line = _lines[i];

			size_t idxStart = line->idxFor(x0Start);
			size_t idxStop = line->idxFor(x0Stop);

			for(size_t idx=idxStart; idx<=idxStop; idx++)
			{
				real xStart = line->xFor(idx);
				if(xStart < x0Start) xStart = x0Start;
				real xStop = line->xFor(idx+1);
				if(xStop > x0Stop) xStop = x0Stop;
				real amount = xStop - xStart;

				if(line->_data.back()._idx != idx)
				{
					if(line->_lastAmount)
					{
						line->_lastAmount = 0;
						line->_lastSum = 0;
					}

					line->_data.push_back(SpectrStoreCell(idx));
				}

				line->_lastSum += vs[i]*amount;
				line->_lastAmount += amount;

				if(line->_lastAmount)
				{
					line->_data.back()._value = line->_lastSum / line->_lastAmount;
				}
			}


			size_t minIdx = line->idxFor(minTime);
			while(line->_data.front()._idx < minIdx)
			{
				line->_data.pop_front();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void SpectrStore::getMarks(TVReal &marks)
	{
		marks.resize(_ts.size());

		TVReal mdata;
		for(size_t i(0); i<_ts.size(); i++)
		{
			TDSpectrStoreCell &data = _lines[i]->_data;
			if(data.empty())
			{
				marks[i] = 0;
				continue;
			}

			real dist = (_lines[i]->xForData(data.size()-1) - _lines[i]->xForData(0)) / _ts[i];

			if(dist < _periodsOnRow*0.9)
			{
				marks[i] = 0;
				continue;
			}

			mdata.resize(data.size());

			for(size_t j(0); j<data.size(); j++)
			{
				mdata[j] = data[j]._value.a();
			}

			std::sort(mdata.begin(), mdata.end());

			size_t bnd = mdata.size()*1/10;
			real k,b;
			llsq(NULL, &mdata[bnd], mdata.size()-bnd*2, k, b);

			marks[i] = (k*(mdata.size()-bnd*2.0)/2 + b) / k;
		}


	}

	//////////////////////////////////////////////////////////////////////////
	bool SpectrStore::fillNNOut(size_t tMiddle, size_t tRange, size_t xPowBase, size_t xRange, TVReal &dataOut)
	{
		if(tMiddle<tRange || tMiddle>=_ts.size()-tRange)
		{
			return false;
		}

		size_t xRangeMax = size_t(::pow(real(xPowBase), int(xRange)) + 0.5)-1;
		size_t tRangeAll = tRange*2+1;
		dataOut.resize(tRangeAll * (xRange+1) );

		for(size_t t(0); t<tRangeAll; t++)
		{
			size_t ti = tMiddle+t-tRange;
			TDSpectrStoreCell &data = _lines[ti]->_data;
			if(data.size() <= xRangeMax)
			{
				return false;
			}

			for(size_t x(0); x<=xRange; x++)
			{
				size_t xi = data.size() - 1 - size_t(::pow(real(xPowBase), int(x)) + 0.5)+1;
				Complex v = data[xi]._value;
				dataOut[x*tRangeAll + t] = v.rotate(c_piDiv2).re() - v.re();
			}
		}

		return true;
	}

}