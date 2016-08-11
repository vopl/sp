#ifndef _hel_speedMeter_h_
#define _hel_speedMeter_h_

namespace hel
{
	template <size_t _bufLen=100>
	class SpeedMeter
	{
		real _amountBuf[_bufLen];
		real _timeBuf[_bufLen];
		real _sumAmount;
		real _sumTime;
		real _totalSumTime;
		size_t _index;
		size_t _pushAmount;
	public:
		SpeedMeter()
			: _sumAmount(0)
			, _sumTime(0)
			, _totalSumTime(0)
			, _index(0)
			, _pushAmount(0)
		{
			std::fill(_amountBuf, _amountBuf+_bufLen, 0);
			std::fill(_timeBuf, _timeBuf+_bufLen, 0);
		}

		void push(real amount, real time)
		{
			if(_pushAmount++ < _bufLen*10)
			{
				_sumTime -= _timeBuf[_index];
				_sumAmount -= _amountBuf[_index];

				_sumTime += time;
				_sumAmount += amount;

				_timeBuf[_index] = time;
				_amountBuf[_index] = amount;
				_index = (_index+1) % _bufLen;
			}
			else
			{
				_pushAmount = 0;

				_timeBuf[_index] = time;
				_amountBuf[_index] = amount;
				_index = (_index+1) % _bufLen;

				_sumTime = std::accumulate(_timeBuf, _timeBuf+_bufLen, 0.0);
				_sumAmount = std::accumulate(_amountBuf, _amountBuf+_bufLen, 0.0);
			}
			_totalSumTime += time;
		}

		real getSpeed()const
		{
			return _sumTime?(_sumAmount/_sumTime):0;
		}

		real getElapsedTime()const
		{
			return _totalSumTime;
		}
	};
}
#endif