#include <stdafx.h>
#include "hel/spectrStore.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	SpectrStore::SpectrStore()
		: _periodsOnWin(1)
		, _periodsOnColumn(1)
		, _maxWidth(1)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	SpectrStore::~SpectrStore()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void SpectrStore::setup(
		real periodsOnWin,
		real periodsOnColumn,
		real maxWidth)
	{
		_periodsOnWin = periodsOnWin;
		_periodsOnColumn = periodsOnColumn;
		_maxWidth = maxWidth;
	}

	//////////////////////////////////////////////////////////////////////////
	void SpectrStore::push(
		real x0, 
		real x0Step, 
		real error,
		const TVReal &tlog, 
		const TVComplex &value)
	{
		assert(tlog.size() == value.size());

		while(_lines.size() < tlog.size())
		{
			size_t idx = _tlog.size();

			_tlog.push_back(tlog[idx]);
			real t = exp(tlog[idx]);

			SpectrStoreLinePtr line(new SpectrStoreLine);

			line->_xStep = t * _periodsOnColumn;
			line->_data.push_back(SpectrStoreCell(line->idxFor(x0)));
			_lines.push_back(line);
		}

		//////////////////////////////////////////////////////////////////////////
		//dump
		static std::ofstream out;
		if(!out.is_open())
		{
			out.open("spectrStore");
			out.precision(16);
			out.setf(std::ios::scientific);
			out<<"time2\tperiod2\treal2\timag2\n";
		}
		//////////////////////////////////////////////////////////////////////////


		real x0Start = x0;
		real x0Stop = x0+x0Step;
		real minTime = x0 - _maxWidth;
		for(size_t i(0); i<_lines.size(); i++)
		{
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
						line->_data.back()._value /= line->_lastAmount;
						line->_data.back()._error /= line->_lastAmount;
						line->_lastAmount = 0;
					}

					//////////////////////////////////////////////////////////////////////////
					//dump
					real t = exp(_tlog[i]);
					out<<line->xFor(line->_data.back()._idx)<<"\t";
					out<<t<<"\t";
					out<<line->_data.back()._value.re()<<"\t";
					out<<line->_data.back()._value.im()<<"\n";
					out.flush();
					//////////////////////////////////////////////////////////////////////////

					line->_data.push_back(SpectrStoreCell(idx));
				}

				line->_data.back()._value += value[i]*amount;
				line->_data.back()._error += error*amount;
				line->_lastAmount += amount;
			}


			size_t minIdx = line->idxFor(minTime);
			while(line->_data.front()._idx < minIdx)
			{
				line->_data.pop_front();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void SpectrStore::buildCorrections()
	{
		if(_tlog.size()<2)
		{
			return;
		}
		_tlogStep = (_tlog.back()-_tlog.front()) / (_tlog.size()-1);
		std::vector<Rect> rects(_lines.size());

		for(size_t i(0); i<rects.size(); i++)
		{
			rects[i] = rectFor(i, 0);
		}

		Rect last = rects[rects.size()/2];
		std::vector<SpectrStoreCell*> cells; TVReal weights;

		//трассировать пр€моугольник
		//итерировать пр€моугольник по траектории
		for(size_t xIdx(1); ; xIdx++)
		{
			real x = last._xCenter - last._xWidth*xIdx;
			real tlog = log(exp(last._tlogCenter) - (last._xCenter - x)/_periodsOnWin);
			if(_isnan(tlog))
			{
				break;
			}

			Rect r;
			r._tlogCenter = tlog;
			r._xCenter = x;

			r._tlogHeight = last._tlogHeight;
			r._xWidth = last._xWidth;

			size_t amount = cellsFor(r, cells, weights);
			if(!amount)
			{
				break;
			}

			//TRATATA
			for(size_t i(0); i<amount; i++)
			{
				cells[i]->_value += Complex(weights[i], weights[i])*100;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//dump
		{
			std::ofstream out("visit\\ss.3d");
			out.precision(16);
			out.setf(std::ios::scientific);
			out<<"time2\tperiod2\treal2\timag2\n";

			for(size_t l(0); l<_lines.size(); l++)
			{
				real t = exp(_tlog[l]);

				for(size_t i(0); i<_lines[l]->_data.size(); i++)
				{
					real x = _lines[l]->xForData(i)+_lines[l]->_xStep/2;
					out<<x<<"\t";
					out<<t<<"\t";
					out<<_lines[l]->_data[i]._value.re()<<"\t";
					out<<_lines[l]->_data[i]._value.im()<<"\n";
				}
			}
			out.flush();
		}

		assert(0);
	}


	//////////////////////////////////////////////////////////////////////////
	SpectrStore::Rect SpectrStore::rectFor(size_t lineIdx, size_t cellReverceIdx)
	{
		Rect res = {nullReal,nullReal,nullReal,nullReal};

		if(lineIdx >= _lines.size() || _lines.size() < 2)
		{
			return res;
		}

		SpectrStoreLinePtr &line = _lines[lineIdx];
		res._tlogCenter = _tlog[lineIdx];
		res._tlogHeight = _tlogStep;

		if(cellReverceIdx >= line->_data.size())
		{
			return res;
		}
		res._xCenter = line->xForData(line->_data.size() - 1 - cellReverceIdx) + line->_xStep/2;
		res._xWidth = line->_xStep;

		return res;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t SpectrStore::cellsFor(const Rect &r, std::vector<SpectrStoreCell*> &cells, TVReal &weights)
	{
		cells.clear();
		weights.clear();

		real xStart = r._xCenter-r._xWidth/2;
		real xStop = r._xCenter+r._xWidth/2;
		real tlogStart = r._tlogCenter-r._tlogHeight/2;
		real tlogStop = r._tlogCenter+r._tlogHeight/2;

		//по лини€м
		size_t lineStartIdx = size_t((tlogStart - _tlog.front())/_tlogStep+0.5);
		if(lineStartIdx>=_tlog.size())
		{
			return cells.size();
		}
		size_t lineStopIdx = size_t((tlogStop - _tlog.front())/_tlogStep+0.5);
		if(lineStopIdx>=_tlog.size())
		{
			return cells.size();
		}

		for(size_t lineIdx(lineStartIdx); lineIdx<=lineStopIdx; lineIdx++)
		{
			real tlogStartLine = _tlog[lineIdx]-_tlogStep/2;
			real tlogStopLine = _tlog[lineIdx]+_tlogStep/2;
			real tlogStartInLine = tlogStart<tlogStartLine?tlogStartLine:tlogStart;
			real tlogStopInLine = tlogStop>tlogStopLine?tlogStopLine:tlogStop;
			real lineWeight = (tlogStopInLine - tlogStartInLine)/_tlogStep;

			SpectrStoreLinePtr &line = _lines[lineIdx];
			TDSpectrStoreCell &data = line->_data;
			real xStep = line->_xStep;

			//внутри линии по €чейкам
			size_t xStartIdx = size_t((xStart - line->xForData(0))/xStep);
			if(xStartIdx>=data.size())
			{
				continue;
			}
			size_t xStopIdx = size_t((xStop - line->xForData(0))/xStep);
			if(xStopIdx+1>=data.size())
			{
				xStopIdx--;
			}

			for(size_t xIdx(xStartIdx); xIdx<=xStopIdx; xIdx++)
			{
				real xStartData = line->xForData(xIdx);
				real xStopData = line->xForData(xIdx)+xStep;
				real xStartInData = xStart<xStartData?xStartData:xStart;
				real xStopInData = xStop>xStopData?xStopData:xStop;
				real dataWeight = (xStopInData - xStartInData)/xStep;

				cells.push_back(&data[xIdx]);
				weights.push_back(dataWeight * lineWeight);
			}
		}

		//нормировать общий вес на 1
		if(weights.size())
		{
			real w = std::accumulate(weights.begin(), weights.end(), 0.0);
			if(w)
			{
				for(size_t i(0); i<weights.size(); i++)
				{
					weights[i] /= w;
				}
			}
		}

		return cells.size();
	}

}