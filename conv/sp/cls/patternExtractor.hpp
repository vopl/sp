#pragma once
#include "sp/cls/kdTree.hpp"
#include <memory>
#include <iostream>
#include <QtGui/QImage>

namespace sp { namespace cls
{
    template <class Shape>
    class PatternExtractor
    {
    public:
        using real = typename Shape::real;

    public:
        PatternExtractor(std::size_t initialShapes=1);
        ~PatternExtractor();

        void push4Learn(typename Shape::Ptr shape);

        void fixLearn(real rate = 0.01);

    private:
        struct LocalShape
            : public Shape
        {
            struct Shape4Learn
            {
                Shape   *_shape;
                real     _weight;
            };

            std::vector<Shape4Learn> _shapes4Learn;

            real _learnedAmount = 0;
        };

        Shape   _window;

        std::vector<LocalShape> _localShapes;
        std::vector<typename Shape::Ptr> _shapesInLearn;

        std::unique_ptr<KDTree<LocalShape>> _kdTreePtr;

    };








    template <class Shape>
    PatternExtractor<Shape>::PatternExtractor(std::size_t initialShapes)
    {
        for(std::size_t row(0); row<Shape::rows; ++row)
        {
            real vw = 0.5*(1.0 - cos(real(M_PIl*2)*(row+1)/(Shape::rows+1)));
            for(std::size_t col(0); col<Shape::cols; ++col)
            {
                real hw = 0.5*(1.0 - cos(real(M_PIl*2)*(col+1)/(Shape::cols+1)));

                _window.data()[row + Shape::rows * col] = hw*vw;
            }
        }

        for(std::size_t i(0); i<initialShapes; ++i)
        {
            _localShapes.push_back(LocalShape());
            _localShapes.back().randomize();
        }

        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
    }

    template <class Shape>
    PatternExtractor<Shape>::~PatternExtractor()
    {
    }

    template <class Shape>
    void PatternExtractor<Shape>::push4Learn(typename Shape::Ptr shape)
    {
        shape->normilize();

        LocalShape mshape;

        real bestShapeDistance(1e100);
        Shape bestShape;

        std::vector<std::size_t> indices(1);
        std::vector<typename Shape::real> distances(indices.size());

        for(std::size_t rphase(0); rphase<8; rphase+=1)
        {

            real phase = real(M_PIl*2)*rphase/8;

            for(std::size_t i(0); i<Shape::_valuesAmount; ++i)
            {
                mshape.data()[i] = shape->data()[i].rotate(phase);
            }

            mshape *= _window;



            _kdTreePtr->query(mshape.rawdata(), indices.size(), &indices[0], &distances[0]);

            if(distances[0] < bestShapeDistance)
            {
                bestShapeDistance = distances[0];
                bestShape = mshape;
            }
        }

        *shape = bestShape;

        indices.resize(10);
        distances.resize(indices.size());

        std::size_t foundAmount = _kdTreePtr->query(shape->rawdata(), indices.size(), &indices[0], &distances[0]);

        real weight(1);

        _shapesInLearn.push_back(std::move(shape));

        for(std::size_t idx(0); idx<foundAmount; ++idx)
        {
            std::size_t index = indices[idx];

            LocalShape &localShape  = _localShapes[index];

            typename LocalShape::Shape4Learn shape4Learn{_shapesInLearn.back().get(), weight};
            localShape._shapes4Learn.emplace_back(shape4Learn);

            weight *= 0.75;
        }
    }

    template <class Shape>
    void PatternExtractor<Shape>::fixLearn(typename Shape::real rate)
    {
        for(std::size_t localShapeIndex(0); localShapeIndex<_localShapes.size(); ++localShapeIndex)
        {
            LocalShape &localShape = _localShapes[localShapeIndex];
            for(typename LocalShape::Shape4Learn &shape4Learn : localShape._shapes4Learn)
            {
                real localRate = rate * shape4Learn._weight;

                for(std::size_t idx(0); idx<localShape._valuesAmount; ++idx)
                {
                    localShape.data()[idx] = localShape.data()[idx]*(1-localRate) + shape4Learn._shape->data()[idx]*localRate;
                }

                localShape._learnedAmount += localRate;
            }

            localShape._shapes4Learn.clear();
            localShape._learnedAmount *= 0.99;
        }
        _shapesInLearn.clear();

        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b){
            return a._learnedAmount >= b._learnedAmount;
        });

        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));

        std::cout<<"learned"<<std::endl;


        //////////////////////////////////////////////////////////////////////
        std::size_t cols = sqrt(real(_localShapes.size()))*3/2+0.5;
        std::size_t rows = _localShapes.size()/cols;
        while(cols*rows < _localShapes.size())
        {
            rows++;
        }

        const std::size_t k = 4;

        QImage imgRe((Shape::cols*k+1)*cols, (Shape::rows*k+1)*rows, QImage::Format_RGB32);
        QImage imgIm((Shape::cols*k+1)*cols, (Shape::rows*k+1)*rows, QImage::Format_RGB32);
        QImage imgA ((Shape::cols*k+1)*cols, (Shape::rows*k+1)*rows, QImage::Format_RGB32);
        imgRe.fill(QColor::fromRgb(255,255,255));
        imgIm.fill(QColor::fromRgb(255,255,255));
        imgA .fill(QColor::fromRgb(255,255,255));

        for(std::size_t localShapeIndex(0); localShapeIndex<_localShapes.size(); ++localShapeIndex)
        {
            LocalShape &localShape = _localShapes[localShapeIndex];

            std::size_t col = localShapeIndex%cols;
            std::size_t row = localShapeIndex/cols;

            localShape.draw(imgRe, col*(Shape::cols*k+1), row*(Shape::rows*k+1), k, true, false);
            localShape.draw(imgIm, col*(Shape::cols*k+1), row*(Shape::rows*k+1), k, false, true);
            localShape.draw(imgA , col*(Shape::cols*k+1), row*(Shape::rows*k+1), k, true, true);
        }

        imgRe.save("somShape/lastLearn_re.png");
        imgIm.save("somShape/lastLearn_im.png");
        imgA .save("somShape/lastLearn_a.png");

    }

}}
