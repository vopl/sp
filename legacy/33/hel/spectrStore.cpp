#include <stdafx.h>
#include "hel/spectrStore.h"

namespace hel
{
	//////////////////////////////////////////////////////////////////////////
	void SpectrStore::push(
		real x0, 
		real error,
		const TVReal &period, 
		const TVComplex &value)
	{
		LinePtr line(new Line(x0, error, period, value));
		_lines.insert(_lines.begin() + lineIdx(line), line);
	}

	//////////////////////////////////////////////////////////////////////////
	void SpectrStore::mkSlice(
		real x0, 
		real pow,
		TVReal &period, 
		TVReal &amplitude, 
		TVReal &phase)
	{
		period.clear();
		amplitude.clear();
		phase.clear();
		if(_lines.empty())
		{
			return;
		}

		real xMin = _lines.front()->_x0;
		size_t x0Idx = lineIdx(x0);

		for(size_t li(0); li<_lines.size(); li++)
		{
			Line &line = *(_lines[li].get());
			if(line._x0 > x0)
			{
				continue;
			}

			real tMin = (x0 - line._x0)/pow;

			size_t tMinIdx = std::lower_bound(line._period.begin(), line._period.end(), tMin) - line._period.begin();

			for(size_t tIdx(tMinIdx); tIdx<line._period.size(); tIdx++)
			{
				//////////////////////////////////////////////////////////////////////////
				period.push_back(line._period[tIdx]);

				const Complex &v = line._value[tIdx];
				//////////////////////////////////////////////////////////////////////////
				real lxMin = x0 - line._period[tIdx]*pow;
				size_t lxMinIdx = lineIdx(lxMin);
				real norma = 1.0/(x0Idx - lxMinIdx + 1);
				amplitude.push_back(v.a() * norma);

				//////////////////////////////////////////////////////////////////////////
				real dp = c_2Pi*(x0-line._x0)/line._period[tIdx];
				phase.push_back(v.p() + dp);
			}
		}
	}

}