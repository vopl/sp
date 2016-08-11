#include "stdafx.h"
#include "hel/core.h"
#include <Windows.h>
#undef min
#undef max

//#include "hel/mtTypes.h"
//#include "mpfrc++/mpreal.h"

BOOST_CLASS_IMPLEMENTATION(hel::SettingsTimeMapper, boost::serialization::primitive_type);
BOOST_CLASS_IMPLEMENTATION(hel::SettingsScaledData, boost::serialization::primitive_type);
BOOST_CLASS_IMPLEMENTATION(hel::SettingsOptimizer, boost::serialization::primitive_type);
BOOST_CLASS_IMPLEMENTATION(hel::TimedValue, boost::serialization::primitive_type);

namespace hel
{


	//////////////////////////////////////////////////////////////////////////
	Core::~Core()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::setup(const SettingsTimeMapper &stm, const SettingsScaledData &ssd)
	{
		_optimizers.clear();
		_optimizerWeights.clear();

		_scaledData.setup(ssd);
		_timeMapper.setup(stm);

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::addOptimizer(const SettingsOptimizer &settings)
	{
		_optimizers.push_back(OptimizerPtr(new Optimizer));
		_optimizers.back()->setup(_scaledData, settings);

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	size_t Core::getOptimizersAmount()
	{
		return _optimizers.size();
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::load(CSZ fName)
	{
		try
		{
			{
				std::ifstream ifs(fName, std::ios::binary);
				boost::archive::binary_iarchive ia(ifs);

				ia >> _timeMapper;
				ia >> _scaledData;
				ia >> _optimizers;
				ia >> _lastUpdateX0;
			}
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::save(CSZ fName)
	{
		try
		{
			{
				std::string tmp = fName;
				tmp += ".tmp";
				std::ofstream ofs(tmp.c_str(), std::ios::binary);
				boost::archive::binary_oarchive oa(ofs);

				oa << _timeMapper;
				oa << _scaledData;
				oa << _optimizers;
				oa << _lastUpdateX0;

				ofs.close();

 				if(boost::filesystem::is_regular_file(fName))
 				{
 					boost::filesystem::remove(fName);
 				}
				boost::filesystem::rename(tmp, fName);
			}
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	void Core::pushData(const TimedValue *data, size_t size)
	{
		for(size_t i(0); i<size; i++)
		{
			if(!_timeMapper.isExternalDropable(data[i]._time))
			{
				TimedValue tv;
				tv._value = data[i]._value;
				tv._time = _timeMapper.external2Internal(data[i]._time);
				_scaledData.pushData(&tv, 1);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	real Core::getLastUpdateDataTime()
	{
		return _timeMapper.internal2External(_lastUpdateX0);
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::needUpdate()
	{
		return _lastUpdateX0 < _scaledData.getLastTime();
	}

	//////////////////////////////////////////////////////////////////////////
	bool Core::update(real step, bool response, bool spectr)
	{
		if(_lastUpdateX0 == nullReal)
		{
			_lastUpdateX0 = _scaledData.getLastTime();
		}
		else
		{
			if(_lastUpdateX0 >= _scaledData.getLastTime())
			{
				return true;
			}

			_lastUpdateX0 = _scaledData.getNearTime(_lastUpdateX0 + step);
		}

		if(response && _optimizers.size())
		{
			for(size_t i(0); i<_optimizers.size(); i++)
			{
				_optimizers[i]->update(_scaledData, _lastUpdateX0, step, spectr);
			}
		}

		return _lastUpdateX0 == _scaledData.getLastTime();
	}

	//////////////////////////////////////////////////////////////////////////
	size_t Core::fillResponse(
		size_t optimizerIdx,
		real *period,
		Complex *value,
		size_t size)
	{
		if(optimizerIdx >= _optimizers.size())
		{
			return 0;
		}

		const TVReal &p = _optimizers[optimizerIdx]->getResponsePeriodLog();
		if(!size)
		{
			return p.size();
		}
		const TVComplex &v = _optimizers[optimizerIdx]->getResponseValue();

		size_t s = std::min(size, p.size());

		for(size_t i(0); i<s; i++)
		{
			period[i] = exp(p[i]);
			value[i] = v[i];
		}

		for(size_t i(s); i<size; i++)
		{
			period[i] = 0;
			value[i].set(0,0);
		}

		return s;
	}


	//////////////////////////////////////////////////////////////////////////
	size_t Core::fillSpectr(
		size_t optimizerIdx,
		real *period,
		Complex *value,
		size_t size)
	{
		if(optimizerIdx >= _optimizers.size())
		{
			return 0;
		}

		const TVReal &p = _optimizers[optimizerIdx]->getSpectrPeriodLog();
		if(!size)
		{
			return p.size();
		}
		const TVComplex &v = _optimizers[optimizerIdx]->getSpectrValue();

		size_t s = std::min(size, p.size());

		for(size_t i(0); i<s; i++)
		{
			period[i] = exp(p[i]);
			value[i] = v[i];
		}

		for(size_t i(s); i<size; i++)
		{
			period[i] = 0;
			value[i].set(0,0);
		}

		return s;
	}

	//////////////////////////////////////////////////////////////////////////
	real Core::getSeparatorError(size_t optimizerIdx)
	{
		if(optimizerIdx >= _optimizers.size())
		{
			return 0;
		}
		return _optimizers[optimizerIdx]->getSeparatorError();
	}

	//////////////////////////////////////////////////////////////////////////
	Core::Core()
		: _lastUpdateX0(nullReal)
	{
	}

}
