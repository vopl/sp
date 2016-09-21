#pragma once
#include "sp/cls/kdTree.hpp"
#include "sp/complex.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <QtGui/QImage>

#include "sp/serialization.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>



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

        std::size_t push4Learn(typename Shape::Ptr shape);

        void fixLearn(real rate, std::vector<Shape> &learnedShapes4Export);

        void mergeSames(real eps);//линейная дистанция нормированная на одну ось

    public:
        void save(const std::string &prefix);
        void load(const std::string &prefix);

    private:
        struct LocalShape
            : public Shape
        {
            struct Shape4Learn
            {
                Shape   *_shape;
                real     _weight;

                template<class Archive>
                void serialize(Archive & ar, const unsigned int version)
                {
                    (void) version;

                    ar & SER_NVP(shape, _shape);
                    ar & SER_NVP(weight, _weight);
                }
            };

            std::vector<Shape4Learn> _shapes4Learn;

            real _learnedAmount = 0;

            friend class boost::serialization::access;

            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                (void) version;

                ar & SER_NVP(Shape, static_cast<Shape&>(*this));
                ar & SER_NVP(shapes4Learn, _shapes4Learn);
                ar & SER_NVP(learnedAmount, _learnedAmount);
            }
        };

        Shape   _window;

        std::vector<LocalShape> _localShapes;
        std::vector<typename Shape::Ptr> _shapesInLearn;

        std::unique_ptr<KDTree<LocalShape>> _kdTreePtr;


        friend class boost::serialization::access;
        BOOST_SERIALIZATION_SPLIT_MEMBER()

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            (void) version;

            ar & SER_NVP(localShapes, _localShapes);
            ar & SER_NVP(shapesInLearn, _shapesInLearn);
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            (void) version;

            ar & SER_NVP(localShapes, _localShapes);
            ar & SER_NVP(shapesInLearn, _shapesInLearn);

            _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
        }


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
            _localShapes.back() *= _window;
            _localShapes.back()._learnedAmount = 0;
        }

        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
    }

    template <class Shape>
    PatternExtractor<Shape>::~PatternExtractor()
    {
    }

    template <class Shape>
    std::size_t PatternExtractor<Shape>::push4Learn(typename Shape::Ptr shape)
    {
        shape->normilize();

        LocalShape mshape;

        real bestShapeDistance(1e100);
        Shape bestShape;

        std::vector<std::size_t> indices(1);
        std::vector<typename Shape::real> distances(indices.size());

        for(std::size_t rphase(0); rphase<36; rphase+=1)
        {

            real phase = real(M_PIl*2)*rphase/36;

            for(std::size_t i(0); i<Shape::_valuesAmount; ++i)
            {
                mshape.data()[i] = shape->data()[i].rotate(phase);
            }

//            mshape *= _window;
//            mshape.normilize();

            _kdTreePtr->query(mshape.rawdata(), indices.size(), &indices[0], &distances[0]);

            if(distances[0] < bestShapeDistance)
            {
                bestShapeDistance = distances[0];
                bestShape = mshape;
            }
        }

        *shape = bestShape;

        indices.resize(8);
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

        return _shapesInLearn.size();
    }

    template <class Shape>
    void PatternExtractor<Shape>::fixLearn(typename Shape::real rate, std::vector<Shape> &learnedShapes4Export)
    {
        for(std::size_t localShapeIndex(0); localShapeIndex<_localShapes.size(); ++localShapeIndex)
        {
            LocalShape &localShape = _localShapes[localShapeIndex];

            if(!localShape._shapes4Learn.empty())
            {
                real amount = 0;
                for(typename LocalShape::Shape4Learn &shape4Learn : localShape._shapes4Learn)
                {
                    real localRate = rate * shape4Learn._weight / localShape._shapes4Learn.size();

                    for(std::size_t idx(0); idx<localShape._valuesAmount; ++idx)
                    {
                        localShape.data()[idx] += shape4Learn._shape->data()[idx]*localRate;
                    }
                    amount += localRate;
                }
                localShape._learnedAmount += amount;
                localShape.normilize();
                localShape._shapes4Learn.clear();
            }

            localShape._learnedAmount *= 0.99;
        }
        _shapesInLearn.clear();

        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
            return a._learnedAmount > b._learnedAmount;
        });

        {
            if(learnedShapes4Export.size() > _localShapes.size())
            {
                learnedShapes4Export.resize(_localShapes.size());
            }

            for(std::size_t i(0); i<learnedShapes4Export.size(); ++i)
            {
                learnedShapes4Export[i] = _localShapes[i];
                _localShapes[i].randomize();
                _localShapes[i] *= _window;
                _localShapes[i]._learnedAmount = 0;
            }
        }

        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
            return a._learnedAmount > b._learnedAmount;
        });

        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));

//        std::cout<<"learned"<<std::endl;
    }

    template <class Shape>
    void PatternExtractor<Shape>::mergeSames(real eps)
    {
        real boundSameDistance = eps*eps*Shape::_valuesAmount*Shape::_valuesAmount;

        std::vector<std::size_t> indices(2);
        std::vector<typename Shape::real> distances(indices.size());

        LocalShape mshape;

        for(std::size_t localShapeIndex(0); localShapeIndex<_localShapes.size(); ++localShapeIndex)
        {
            Shape localShapeRotated;
            real sameShapeDistance(1e100);
            std::size_t sameShapeIndex = 0;

            LocalShape &target = _localShapes[localShapeIndex];

            for(std::size_t rphase(0); rphase<36; rphase+=1)
            {
                real phase = real(M_PIl*2)*rphase/36;

                for(std::size_t i(0); i<Shape::_valuesAmount; ++i)
                {
                    mshape.data()[i] = target.data()[i].rotate(phase);
                }

                if(2 <= _kdTreePtr->query(mshape.rawdata(), indices.size(), &indices[0], &distances[0]))
                {
                    for(std::size_t iidx(0); iidx<indices.size(); iidx++)
                    {
                        if(localShapeIndex == indices[iidx])
                        {
                            continue;
                        }

                        if(distances[iidx] < sameShapeDistance)
                        {
                            sameShapeDistance = distances[iidx];
                            localShapeRotated = mshape;
                            sameShapeIndex = indices[iidx];
                        }
                    }
                }
            }

            if(sameShapeDistance < boundSameDistance)
            {
                LocalShape &same = _localShapes[sameShapeIndex];

                //std::cout<<"merge "<<localShapeIndex<<" - "<<sameShapeIndex<<" with eps "<<sameShapeDistance<<std::endl;

                for(std::size_t i(0); i<Shape::_valuesAmount; ++i)
                {
                    target.data()[i] = (localShapeRotated.data()[i] + same.data()[i])/real(2);
                }

                same.randomize();
                same *= _window;
                same._learnedAmount = 0;

                _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
            }
        }

        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
            return a._learnedAmount > b._learnedAmount;
        });
    }

    template <class Shape>
    void PatternExtractor<Shape>::save(const std::string &prefix)
    {
        {
            std::ofstream ofs(prefix+"/pe.ser");
            if(!ofs)
            {
                std::cerr<<"unable to create file "<<prefix+"/pe.ser";
                return;
            }

            boost::archive::text_oarchive oa(ofs);

            oa << SER_NVP(pe, *this);
        }


        //images
        {
            real learnedAmountMax = std::numeric_limits<typename Shape::real>::min();
            for(std::size_t localShapeIndex(0); localShapeIndex<_localShapes.size(); ++localShapeIndex)
            {
                LocalShape &localShape = _localShapes[localShapeIndex];
                learnedAmountMax = std::max(learnedAmountMax, localShape._learnedAmount);
            }

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

                localShape.draw(imgRe, col*(Shape::cols*k+1), row*(Shape::rows*k+1), k, localShape._learnedAmount/learnedAmountMax, true, false);
                localShape.draw(imgIm, col*(Shape::cols*k+1), row*(Shape::rows*k+1), k, localShape._learnedAmount/learnedAmountMax, false, true);
                localShape.draw(imgA , col*(Shape::cols*k+1), row*(Shape::rows*k+1), k, localShape._learnedAmount/learnedAmountMax, true, true);
            }

            imgRe.save((prefix+"/lastLearn_re.png").data());
            imgIm.save((prefix+"/lastLearn_im.png").data());
            imgA .save((prefix+"/lastLearn_a.png").data());
        }

        std::cout<<"saved "<<prefix<<std::endl;
    }

    template <class Shape>
    void PatternExtractor<Shape>::load(const std::string &prefix)
    {
        std::ifstream ifs(prefix+"/pe.ser");
        if(!ifs)
        {
            std::cerr<<"unable to open file "<<prefix+"/pe.ser"<<std::endl;
            return;
        }

        try
        {
            boost::archive::text_iarchive ia(ifs);
            ia >> SER_NVP(pe, *this);
        }
        catch(std::exception &e)
        {
            std::cerr<<"load failed: "<<e.what()<<std::endl;
            return;
        }

        std::cout<<"loaded "<<prefix<<std::endl;
    }

}}
