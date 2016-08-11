#ifndef _hel_types_h_
#define _hel_types_h_

namespace hel
{
	typedef const char *CSZ;

	//////////////////////////////////////////////////////////////////////////
	typedef double real;
	static const real nullReal = -std::numeric_limits<real>::max();
	typedef std::vector<real> TVReal;

	//////////////////////////////////////////////////////////////////////////
	struct TimedValue
	{
		real	_value;
		real	_time;

		bool operator <(const TimedValue &a) const
		{
			return _time < a._time;
		}
	};
	typedef std::vector<TimedValue> TVTimedValue;

#ifdef  _WIN64
	typedef __int64    signed_t;
#else
	typedef int   signed_t;
#endif

	struct AP
	{
		real _A;
		real _P;

		AP()
			: _A(0)
			, _P(0)
		{
		}

		real re() const
		{
			return _A*cos(_P);
		}
		real im() const
		{
			return _A*sin(_P);
		}

		AP operator+(const AP &ap) const
		{
			real re_ = re()+ap.re();
			real im_ = im()+ap.im();

			AP res;
			res._A = sqrt(re_*re_+im_*im_);
			res._P = atan2(im_, re_);
			return res;
		}

		AP operator-(const AP &ap) const
		{
			real re_ = re()-ap.re();
			real im_ = im()-ap.im();

			AP res;
			res._A = sqrt(re_*re_+im_*im_);
			res._P = atan2(im_, re_);
			return res;
		}

		AP operator*(const AP &ap) const
		{
			AP res;
			res._A = _A*ap._A;
			res._P = _P+ap._P;
			return res;
		}

		AP operator/(const AP & ap) const
		{
			AP res;
			res._A = _A/ap._A;
			res._P = _P-ap._P;
			return res;
		}

		AP &operator+=(const AP & ap) {*this = *this+ap; return *this;}
		AP &operator-=(const AP & ap) {*this = *this-ap; return *this;}
		AP &operator*=(const AP & ap) {*this = *this*ap; return *this;}
		AP &operator/=(const AP & ap) {*this = *this/ap; return *this;}
	};
}

#endif
