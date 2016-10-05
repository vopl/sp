#pragma once

#include "sp/complex.hpp"
#include "sp/math.hpp"
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
        using complex = complex_tmpl<real>;
        using Ptr = std::unique_ptr<Shape>;
        static constexpr std::size_t cols = cols_;
        static constexpr std::size_t rows = rows_;
        static constexpr std::size_t _valuesAmount = cols*rows;

        complex *data(){return _values;}
        const complex *data() const {return _values;}

        real *rawdata(){return &_values[0].re();}
        const real *rawdata() const {return &_values[0].re();}

        void randomize()
        {
            for(auto &v : _values)
            {
                v.re() = real(rand())/RAND_MAX-0.5;
                v.im() = real(rand())/RAND_MAX-0.5;
            }

            normalize();
        }

        void dropSmalls(real part)
        {
            real middleA = 0;
            for(const complex &v : _values)
            {
                middleA += v.a();
            }
            middleA /= _valuesAmount;

            for(complex &v : _values)
            {
                if(v.a() < middleA*part)
                {
                    v = 0;
                }
            }
        }

        real w() const
        {
            real sum = 0;
            for(const complex &v : _values)
            {
                sum += v.a() * v.a();
            }

            return sum;
        }

        real symmetry() const
        {
            real sv(0), sh(0);

            {
                real sum = 0;
                for(std::size_t col(0); col<cols; ++col)
                {
                    for(std::size_t row(0); row<rows; ++row)
                    {
                        complex v = _values[col*rows + row];

                        real wh = pow(
                                     real(rows-1)/4 - fabs(real(rows-1)/2 - real(row)),
                                     1);

                        real wv = pow(
                                     real(cols-1)/4 - fabs(real(cols-1)/2 - real(col)),
                                     1);


                        sh += wh*v.a();
                        sv += wv*v.a();

                        sum += v.a();
                    }
                }

                sh /= sum;
                sv /= sum;
            }

            return /*sv +*/ sh;
        }

        void normalize(bool withPhase = true)
        {
            //выравнять амплитуду на 1 и фазу на 0
            Summator<complex> middleSum;
            Summator<complex> normaSum;
            for(const complex &v : _values)
            {
                middleSum += v;
                normaSum += v.sqr();
            }
            real norma = sqrt(normaSum.v().a());
            complex middle = middleSum.v() / real(_valuesAmount);

            if(norma < std::numeric_limits<real>::epsilon())
            {
                return;
            }

            if(withPhase && middle.a()>std::numeric_limits<real>::epsilon())
            {
                middle = middle*norma/middle.a();
            }
            else
            {
                middle = norma;
            }

            for(complex &v : _values)
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

            complex copy[_valuesAmount];
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

            for(int row(0); row<rows; ++row)
            {
                for(int col(0); col<cols; ++col)
                {
                    int c2 = col - int(ccol) + int(cols/2);
                    int r2 = row - int(crow) + int(rows/2);

                    if(c2 >=0 && c2<cols &&
                       r2 >=0 && r2<rows)
                    {
                        _values[col*rows + row] = copy[c2*rows + r2];
                    }
                    else
                    {
                        _values[col*rows + row] = 0;
                    }
                }
            }
        }

        void smooth()
        {
            complex copy[_valuesAmount];
            for(std::size_t i(0); i<_valuesAmount; ++i)
            {
                copy[i] = _values[i];
            }

            for(int row(0); row<rows; ++row)
            {
                for(int col(0); col<cols; ++col)
                {
                    for(int drow(-1); drow<2; ++drow)
                    {
                        for(int dcol(-1); dcol<2; ++dcol)
                        {
                            //if(drow && dcol)
                            {
                                _values[(col%cols)*rows + (row)%rows] += copy[((col+dcol)%cols)*rows + (row+drow)%rows];
                            }
                        }
                    }
                }
            }

            normalize();
        }

        void draw(QImage &img, std::size_t x, std::size_t y, std::size_t mult, real weight, bool re, bool im)
        {
            real maxa(0);
            for(const auto &v : _values)
            {
                maxa = std::max(maxa, v.a());
            }
            //maxa = 2;

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

                //gray *= pow(weight, 0.25);

                gray = std::min(gray, real(1));
                gray = std::max(gray, real(0));

//                QColor color = QColor::fromRgbF(gray, gray, gray);
                real accent = std::min(weight/4+gray, real(1));
                accent = std::min(accent, real(1));
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
        complex _values[_valuesAmount];

        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            (void) version;

            ar & SER_NVP(values, _values);
        }

    };

}}
