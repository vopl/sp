#include "sp/conv/drawer.hpp"
#include <cassert>
#include <set>

namespace sp { namespace conv
{
    Drawer::Drawer()
    {

    }

    Drawer::~Drawer()
    {

    }

    void Drawer::setup(
            real ppw,
            const TVReal &periods,
            real signalSampleStep,
            std::size_t signalBucketSize)
    {
        _ppw = ppw;
        _periods = periods;
        _signalSampleStep = signalSampleStep;
        _signalBucketSize = signalBucketSize;

        std::size_t size4MaxPeriod = _ppw* _periods.back() / (_signalSampleStep * _signalBucketSize)+0.5;

        while(_columns.size() < size4MaxPeriod)
        {
            _columns.push_back(Column{});
            Column &lastCol = _columns.back();

            lastCol._values.resize(_height);
            lastCol._amounts.resize(_height);
        }

    }

    void Drawer::push(const TVPoint &points)
    {
        {
            _columns.push_back(Column{});
            Column &lastCol = _columns.back();

            lastCol._values.resize(_height);
            lastCol._amounts.resize(_height);
        }

        std::size_t size4MaxPeriod = _ppw* _periods.back() / (_signalSampleStep * _signalBucketSize)+0.5;
        std::size_t middleIndex = _columns.size() - size4MaxPeriod/2;
        real middleX = _signalSampleStep * _signalBucketSize * middleIndex;

        std::set<std::size_t *> touchedAmounts;

        for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
        {
            real period = _periods[periodIndex];

            real periodLog2ImgMult = _height / (log(_periods.back()) - log(_periods.front()));

            real x0 = middleX - _ppw*period/2;
            real x1 = middleX + _ppw*period/2;

            int gx0 = x0/(_signalSampleStep * _signalBucketSize)+0.5;
            int gx1 = x1/(_signalSampleStep * _signalBucketSize)+0.5;

            const Point &p = points[periodIndex];

            for(std::size_t ia(0); ia<Point::_amplitudeKAmount; ++ia)
            {
                real a0 = _basis._amplitudeK0[ia];
                real a1 = _basis._amplitudeK1[ia];

                for(std::size_t ip(0); ip<Point::_periodKAmount; ++ip)
                {
                    real t0 = _basis._periodK0[ip]*period;
                    real t1 = _basis._periodK1[ip]*period;

                    real v = p._v[ia][ip].a();

                    for(int gx(gx0); gx<gx1; ++gx)
                    {
                        real lx01 = real(gx-gx0) / (gx1 - gx0);
                        real t = t0 * (1-lx01) + t1 * lx01;
                        real value = v* (a0 * (1.0-lx01) + a1 * lx01);

                        int gt = (log(t) - log(_periods.front())) * periodLog2ImgMult;

                        if(gt>=0 && gt<_height)
                        {
                            Column &col = _columns[gx];

                            col._values[gt] += value;
                            touchedAmounts.insert(&col._amounts[gt]);
                        }
                    }
                }
            }

        }

        for(std::size_t *ta : touchedAmounts)
        {
            *ta += 1;
        }
    }

    static const real POWFactor = 1.0;

    QImage Drawer::dump()
    {
        QImage res(_columns.size(), _height, QImage::Format_RGB32);
        res.fill(0);


        real max = 0;
        for(std::size_t x(0); x<_columns.size(); ++x)
        {
            const Column &c = _columns[x];
            for(std::size_t y(0); y<_height; ++y)
            {
                if(c._amounts[y])
                {
                    max = std::max(max, pow(c._values[y]/c._amounts[y], POWFactor));
                }
            }
        }

        if(max)
        {
            for(std::size_t x(0); x<_columns.size(); ++x)
            {
                const Column &c = _columns[x];
                for(std::size_t y(0); y<_height; ++y)
                {
                    if(c._amounts[y])
                    {
                        real gray = pow(c._values[y]/c._amounts[y], POWFactor)/max;

                        if(gray<0 || gray>1)
                        {
                            assert(0);
                        }

                        gray = std::min(real(1), std::max(real(0), gray));

                        res.setPixelColor(x,y,QColor::fromRgbF(gray,gray,gray));
                    }
                }
            }

        }


        return res;
    }

}}
