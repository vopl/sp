#pragma once
#include "sp/conv/basis.hpp"
#include "sp/conv/point.hpp"

#include <deque>
#include <QImage>

namespace sp { namespace conv
{
    class Drawer
    {
    public:
        Drawer();
        ~Drawer();

        void setup(
                real ppw,
                const TVReal &periods,
                real signalSampleStep,
                std::size_t signalBucketSize);

        void push(const TVPoint &points);

        QImage dump();

    private:
        real                            _ppw = 0;
        TVReal                          _periods;
        real                            _signalSampleStep = 0;
        std::size_t                     _signalBucketSize = 0;

        std::size_t _height = 200;

        Basis   _basis;

    private:
        struct Column
        {
            TVReal                      _values;
            std::vector<std::size_t>    _amounts;
        };

        using Columns = std::deque<Column>;

        Columns _columns;


    };

}}
