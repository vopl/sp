#ifndef _hel_timeMapper_h_
#define _hel_timeMapper_h_

typedef double real;

class TimeMapper
{

public://TRATATA private
	real _mult;
	real _offset;
	bool _dropWekends;
	real _weekendBegin;

public:
	TimeMapper();
	void setup(real externalStart, real externalStop, real internalStart, real internalStop, bool dropWekends, real weekendBegin);
	bool isExternalDropable(real) const;
	real external2Internal(real) const;
	real internal2External(real) const;
	real external2InternalOffset(real) const;
	real internal2ExternalOffset(real) const;

};

#endif
