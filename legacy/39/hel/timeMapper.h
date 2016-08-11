#ifndef _hel_timeMapper_h_
#define _hel_timeMapper_h_

#include "hel/types.h"
#include "hel/settings.h"

namespace hel
{
	class TimeMapper
		: private SettingsTimeMapper
	{

	private:

		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version)
		{
			ar & (SettingsTimeMapper)*this;
		}

	public:
		TimeMapper();
		void setup(const SettingsTimeMapper &settings);
		bool isExternalDropable(real) const;
		real external2Internal(real) const;
		real internal2External(real) const;
	};
}
#endif
