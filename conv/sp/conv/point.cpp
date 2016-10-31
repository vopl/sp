#include "sp/conv/point.hpp"

namespace sp { namespace conv
{
    real *Point::data()
    {
        return &_v[0][0].re();
    }

    std::size_t Point::dataSize()
    {
        return 2 * _amplitudeKAmount * _periodKAmount;
    }


    std::ostream &operator<<(std::ostream &out, const Point &p)
    {
        for(std::size_t ia(0); ia<Point::_amplitudeKAmount; ++ia)
        {
            for(std::size_t ip(0); ip<Point::_periodKAmount; ++ip)
            {
                out << p._v[ia][ip].re()<<", "<<p._v[ia][ip].im()<<", ";
            }
        }

        return out;
    }

}}
