#pragma once

#include "sp/complex.hpp"
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
                v.re() = real(rand())/RAND_MAX;
                v.im() = real(rand())/RAND_MAX;
            }

            normilize();
        }

        void normilize()
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


        void draw(QImage &img, std::size_t x, std::size_t y, std::size_t mult, bool re, bool im)
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
                }
                else if(re)
                {
                    gray = _values[idx].re();
                }
                else
                {
                    gray = _values[idx].im();
                }

                gray = (maxa ? gray/maxa : 0)/2+0.5;

                gray = std::min(gray, real(1));


                QColor color = QColor::fromRgbF(gray, gray, gray);

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
    };

}}
