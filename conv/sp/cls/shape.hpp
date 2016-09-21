#pragma once

#include "sp/complex.hpp"
#include "sp/serialization.hpp"
#include <limits>
#include <memory>
#include <QtGui/QImage>

namespace sp { namespace cls
{
    template <std::size_t cols_, std::size_t rows_, typename real_>
    class Shape
    {
    public:
        using real = real_;
        using Ptr = std::unique_ptr<Shape>;
        static constexpr std::size_t cols = cols_;
        static constexpr std::size_t rows = rows_;
        static constexpr std::size_t _valuesAmount = cols*rows;

        complex_tmpl<real> *data(){return _values;}
        const complex_tmpl<real> *data() const {return _values;}

        real *rawdata(){return &_values[0].re();}
        const real *rawdata() const {return &_values[0].re();}

        void randomize()
        {
            for(auto &v : _values)
            {
                v.re() = real(rand())/RAND_MAX-0.5;
                v.im() = real(rand())/RAND_MAX-0.5;
            }

            normilize();
        }

        void normilize(bool withPhase = false)
        {
            //выровнять амплитуду на 1 и фазу на 0
            complex_tmpl<real> middle;
            for(const complex_tmpl<real> &v : _values)
            {
                middle += v;
            }
            middle /= rows*cols;

            if(!middle.a())
            {
                return;
            }

            if(!withPhase)
            {
                middle = middle.a();
            }

            for(complex_tmpl<real> &v : _values)
            {
                v /= middle;
            }
        }

        Shape &operator *= (const Shape &to)
        {
            for(std::size_t i(0); i<_valuesAmount; ++i)
            {
                _values[i] *= to._values[i];
            }

            return *this;
        }

        void centrate()
        {
            real mid = 0;
            for(std::size_t i(0); i<_valuesAmount; ++i)
            {
                mid += _values[i].a();
            }
            mid /= _valuesAmount;


            real maxValue = 0;
            std::size_t maxIndex = 0;

            complex_tmpl<real> copy[_valuesAmount];
            for(std::size_t i(0); i<_valuesAmount; ++i)
            {
                real v = fabs(mid - _values[i].a());

                if(maxValue < v)
                {
                    maxValue = v;
                    maxIndex = i;
                }

                copy[i] = _values[i];
            }

            std::size_t ccol = maxIndex / rows;
            std::size_t crow = maxIndex % rows;

            for(std::size_t row(0); row<rows; ++row)
            {
                for(std::size_t col(0); col<cols; ++col)
                {
                    _values[((col+cols/2)%cols)*rows + (row+rows/2)%rows] = copy[((col+ccol)%cols)*rows + (row+crow)%rows];
                }
            }
        }

        void draw(QImage &img, std::size_t x, std::size_t y, std::size_t mult, real weight, bool re, bool im)
        {
            real maxa(0);
            for(const auto &v : _values)
            {
                maxa = std::max(maxa, v.a());
            }

            for(std::size_t idx(0); idx<_valuesAmount; ++idx)
            {
                real gray;
                if(re && im)
                {
                    gray = _values[idx].a();
                    gray = (maxa ? gray/maxa : 0);
                }
                else if(re)
                {
                    gray = _values[idx].re();
                    gray = (maxa ? gray/maxa : 0)/2+0.5;
                }
                else
                {
                    gray = _values[idx].im();
                    gray = (maxa ? gray/maxa : 0)/2+0.5;
                }

                gray = std::min(gray, real(1));
                gray = std::max(gray, real(0));

                real accent = std::min(weight/4+gray, real(1));
                accent = std::max(accent, real(0));

                QColor color = QColor::fromRgbF(gray, gray, accent);

                std::size_t shapeCol = idx/rows;
                std::size_t shapeRow = idx%rows;

                for(std::size_t i(0); i<mult; i++)
                {
                    for(std::size_t j(0); j<mult; j++)
                    {
                        img.setPixelColor(
                                    x + (shapeCol)*mult + i,
                                    y + (shapeRow)*mult + j,
                                    color);
                    }
                }
            }
        }


    private:
        complex_tmpl<real> _values[_valuesAmount];

        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            (void) version;

            ar & SER_NVP(values, _values);
        }

    };

}}
