#pragma once
#include "sp/cls/kdTree.hpp"
#include "sp/complex.hpp"
#include "sp/math.hpp"
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
        using complex = typename Shape::complex;

        void pca();
        void pca2();

    public:
        PatternExtractor(std::size_t initialShapes=1, bool withSmoothingWindow = true);
        ~PatternExtractor();

        std::size_t pushBasis(const Shape &shape);
        std::size_t push4Learn(typename Shape::Ptr shape);

        void fixLearn(real rate);
        void export_(std::vector<Shape> &learnedShapes4Export);

        void mergeSames(real eps);//линейная дистанция нормированная на одну ось
        void replaceNulls();

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
    void PatternExtractor<Shape>::pca()
    {
        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
            return a._learnedAmount > b._learnedAmount;
        });

        _localShapes.erase(_localShapes.begin()+_localShapes.size()*0.85, _localShapes.end());


        std::vector<LocalShape> x;
        x.swap(_localShapes);

        std::vector<LocalShape> &as = _localShapes;
        std::vector<std::vector<real>> bs;

        //normilize x
        for(std::size_t i(0); i<x.size(); ++i)
        {
            x[i].normalize();
        }

        for(;;)
        {
            as.push_back(LocalShape());
            as.back().randomize();
            as.back().normalize();

            bs.push_back(std::vector<real>(x.size()));
            std::vector<real> &b = bs.back();

            Shape &a = as.back();

            for(std::size_t iter(0); iter<200; ++iter)
            {

                //b
                {
                    Summator<real> denom = 0;
                    for(std::size_t j(0); j<Shape::_valuesAmount*2; ++j)
                    {
                        denom += a.rawdata()[j] * a.rawdata()[j];
                    }

                    for(std::size_t i(0); i<b.size(); ++i)
                    {
                        Summator<real> nom = 0;

                        for(std::size_t j(0); j<Shape::_valuesAmount*2; ++j)
                        {
                            nom += x[i].rawdata()[j] * a.rawdata()[j];
                        }

                        b[i] = nom/denom;
                    }
                }

                //a
                {
                    Summator<real> denom = 0;
                    for(std::size_t i(0); i<b.size(); ++i)
                    {
                        denom += b[i] * b[i];
                    }

                    for(std::size_t j(0); j<Shape::_valuesAmount*2; ++j)
                    {
                        Summator<real> nom = 0;

                        for(std::size_t i(0); i<b.size(); ++i)
                        {
                            nom += b[i] * x[i].rawdata()[j];
                        }

                        a.rawdata()[j] = nom/denom;
                    }
                    a.normalize();
                }
            }

            //sub
            for(std::size_t i(0); i<b.size(); ++i)
            {
                for(std::size_t j(0); j<Shape::_valuesAmount*2; ++j)
                {
                    x[i].rawdata()[j] -= a.rawdata()[j] * b[i];
                }
                x[i].normalize();
            }

            save("pca.iter.as");

            x.swap(_localShapes);
            save("pca.iter.x");

            x.swap(_localShapes);
        }


        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
    }


    template <class Shape>
    void PatternExtractor<Shape>::pca2()
    {
        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
            return a._learnedAmount > b._learnedAmount;
        });

//        save("pca.init");
//        exit(0);


        std::vector<LocalShape> x;
        x.swap(_localShapes);

        std::vector<LocalShape> &as = _localShapes;
        std::vector<std::vector<complex>> bs;

        {
            //normilize x
            Summator<complex> vSum;
            for(std::size_t i(0); i<x.size(); ++i)
            {
                for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                {
                    vSum += x[i].data()[j];
                }
            }
            complex vMean = vSum.v() / (real(x.size()) * Shape::_valuesAmount);
            Summator<real> aSum;
            for(std::size_t i(0); i<x.size(); ++i)
            {
                for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                {
                    x[i].data()[j] -= vMean;
                    aSum += x[i].data()[j].a();
                }
            }
            real aMean = aSum.v() / (real(x.size()) * Shape::_valuesAmount);
            for(std::size_t i(0); i<x.size(); ++i)
            {
                for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                {
                    x[i].data()[j] /= aMean;
                }
            }
        }

        for(;;)
        {
            as.push_back(LocalShape());
            as.back().randomize();
            as.back().normalize();

            bs.push_back(std::vector<complex>(x.size()));
            std::vector<complex> &b = bs.back();

            Shape &a = as.back();

            complex prevF = 0;

            for(std::size_t iter(0); iter<500; ++iter)
            {
                //F
                {
                    Summator<complex> F;

                    for(std::size_t i(0); i<b.size(); ++i)
                    {
                        for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                        {
                            F += x[i].data()[j];
                            F += -b[i]*a.data()[j];
                        }
                    }

                    if(F.v().a() < 1e-40)
                    {
                        std::cout<<iter<<", "<<F.v().a()<<", "<<"unk"<<std::endl;
                        break;
                    }

                    complex dFdF = (F.v()-prevF)/F.v();
                    std::cout<<iter<<", "<<F.v().a()<<", "<<dFdF.a()<<std::endl;

                    prevF = F;

                    if(dFdF.a() < 1e-10)
                    {
                        break;
                    }
                }


                //b
                {
                    Summator<complex> denom;
                    for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                    {
                        denom += a.data()[j] * a.data()[j];
                    }

                    for(std::size_t i(0); i<b.size(); ++i)
                    {
                        Summator<complex> nom;

                        for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                        {
                            nom += x[i].data()[j] * a.data()[j];
                        }

                        b[i] = nom.v()/denom.v();
                    }
                }

                //a
                {
                    Summator<complex> denom;
                    for(std::size_t i(0); i<b.size(); ++i)
                    {
                        denom += b[i] * b[i];
                    }

                    for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                    {
                        Summator<complex> nom;

                        for(std::size_t i(0); i<b.size(); ++i)
                        {
                            nom += b[i] * x[i].data()[j];// * sqrt(x[i].w());
                        }

                        a.data()[j] = nom.v()/denom.v();
                    }
                    a.normalize(false);
                }
            }

            //sub
            for(std::size_t i(0); i<b.size(); ++i)
            {
                for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                {
                    x[i].data()[j] -= a.data()[j] * b[i];
                }
            }

            {
                //normilize x
                Summator<complex> vSum;
                for(std::size_t i(0); i<x.size(); ++i)
                {
                    for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                    {
                        vSum += x[i].data()[j];
                    }
                }
                complex vMean = vSum.v() / (real(x.size()) * Shape::_valuesAmount);
                Summator<real> aSum;
                for(std::size_t i(0); i<x.size(); ++i)
                {
                    for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                    {
                        x[i].data()[j] -= vMean;
                        aSum += x[i].data()[j].a();
                    }
                }
                real aMean = aSum.v() / (real(x.size()) * Shape::_valuesAmount);
                for(std::size_t i(0); i<x.size(); ++i)
                {
                    for(std::size_t j(0); j<Shape::_valuesAmount; ++j)
                    {
                        x[i].data()[j] /= aMean;
                    }
                }
            }

            save("pca.iter.as2");

            x.swap(_localShapes);
            save("pca.iter.x2");

            x.swap(_localShapes);
        }


        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
    }

    template <class Shape>
    PatternExtractor<Shape>::PatternExtractor(std::size_t initialShapes, bool withSmoothingWindow)
    {
        for(std::size_t row(0); row<Shape::rows; ++row)
        {
            real vw = 0.5*(1.0 - cos(real(M_PIl*2)*(row+1)/(Shape::rows+1)));
            for(std::size_t col(0); col<Shape::cols; ++col)
            {
                real hw = 0.5*(1.0 - cos(real(M_PIl*2)*(col+1)/(Shape::cols+1)));

                _window.data()[row + Shape::rows * col] = withSmoothingWindow ? hw*vw : 1;
            }
        }

        _localShapes.reserve(1000*1000*60);
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
    std::size_t PatternExtractor<Shape>::pushBasis(const Shape &shape)
    {
        _localShapes.resize(_localShapes.size() + 1);
        static_cast<Shape&>(_localShapes.back()) = shape;
        _localShapes.back()._learnedAmount = 0;

        return _localShapes.size();
    }

    template <class Shape>
    std::size_t PatternExtractor<Shape>::push4Learn(typename Shape::Ptr shape)
    {
        *shape *= _window;
        //shape->centrate();
        shape->normalize();

        std::vector<std::size_t> indices(20);
        std::vector<typename Shape::real> distances(indices.size());
        std::size_t foundAmount = _kdTreePtr->query(shape->rawdata(), indices.size(), &indices[0], &distances[0]);

        if(foundAmount)
        {
            real weightNorma = sqrt(distances[0]);

            _shapesInLearn.push_back(std::move(shape));

            for(std::size_t idx(0); idx<foundAmount; ++idx)
            {
                std::size_t index = indices[idx];

                LocalShape &localShape  = _localShapes[index];

                typename LocalShape::Shape4Learn shape4Learn{_shapesInLearn.back().get(), weightNorma/real(sqrt(distances[idx]))};
                localShape._shapes4Learn.emplace_back(shape4Learn);
            }
        }

        return _shapesInLearn.size();
    }

    template <class Shape>
    void PatternExtractor<Shape>::fixLearn(typename Shape::real rate)
    {
        for(std::size_t localShapeIndex(0); localShapeIndex<_localShapes.size(); ++localShapeIndex)
        {
            LocalShape &localShape = _localShapes[localShapeIndex];
            localShape._learnedAmount *= 0.95;

            if(!localShape._shapes4Learn.empty())
            {
                real amount = 0;
                for(std::size_t idx(0); idx<localShape._valuesAmount; ++idx)
                {
                    localShape.data()[idx] *= real(localShape._shapes4Learn.size());
                }

                for(typename LocalShape::Shape4Learn &shape4Learn : localShape._shapes4Learn)
                {
                    real localRate = rate * shape4Learn._weight;

                    for(std::size_t idx(0); idx<localShape._valuesAmount; ++idx)
                    {
                        localShape.data()[idx] += shape4Learn._shape->data()[idx]*localRate;
                    }
                    amount += localRate;
                }
                localShape._learnedAmount += amount / real(localShape._shapes4Learn.size());
                localShape.normalize();
                localShape._shapes4Learn.clear();
            }
        }
        _shapesInLearn.clear();

        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
            return a._learnedAmount > b._learnedAmount;
        });

        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));

//        std::cout<<"learned"<<std::endl;
    }

    template <class Shape>
    void PatternExtractor<Shape>::export_(std::vector<Shape> &learnedShapes4Export)
    {
        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
            return a._learnedAmount > b._learnedAmount;
        });

        std::size_t exportIndex = 0;

        for(std::size_t i(0); i<_localShapes.size(); ++i)
        {
            if(exportIndex >= learnedShapes4Export.size())
            {
                break;
            }

            if(_localShapes[i]._learnedAmount>0)
            {
                learnedShapes4Export[exportIndex] = _localShapes[i];
                _localShapes[i].randomize();
                _localShapes[i] *= _window;
                _localShapes[i]._learnedAmount = 0;
                exportIndex++;
            }
        }

        learnedShapes4Export.resize(exportIndex);

//        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
//            return a._learnedAmount > b._learnedAmount;
//        });

        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
    }


    template <class Shape>
    void PatternExtractor<Shape>::mergeSames(real eps)
    {
        real boundSameDistance = eps*eps;

        std::vector<std::size_t> indices(2);
        std::vector<typename Shape::real> distances(indices.size());

        bool doWork = true;

        while(doWork)
        {
            doWork = false;

            for(std::size_t localShapeIndex(0); localShapeIndex<_localShapes.size(); ++localShapeIndex)
            {
                real sameShapeDistance(1e100);
                std::size_t sameShapeIndex = 0;

                LocalShape &target = _localShapes[localShapeIndex];

                if(2 <= _kdTreePtr->query(target.rawdata(), indices.size(), &indices[0], &distances[0]))
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
                            sameShapeIndex = indices[iidx];
                        }
                    }
                }

                if(sameShapeDistance < boundSameDistance)
                {
                    //std::cout<<"merge "<<localShapeIndex<<" <- "<<sameShapeIndex<<", dist2: "<<sqrt(sameShapeDistance)<<" < "<<sqrt(boundSameDistance)<<std::endl;

                    LocalShape &same = _localShapes[sameShapeIndex];
                    for(std::size_t i(0); i<Shape::_valuesAmount; ++i)
                    {
                        target.data()[i] += same.data()[i];
                        same.data()[i] = 0;
                    }
                    target.normalize();
                    target._learnedAmount += same._learnedAmount;
                    target._learnedAmount /= 2;

                    std::swap(same, _localShapes.back());
                    _localShapes.erase(_localShapes.end()-1);
//                    same.randomize();
//                    same *= _window;
//                    same._learnedAmount = 0;

                    _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));

                    doWork = true;
                }
            }
        }

//        std::sort(_localShapes.begin(), _localShapes.end(), [](const LocalShape &a, const LocalShape &b)->bool{
//            return a._learnedAmount > b._learnedAmount;
//        });

//        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
    }

    template <class Shape>
    void PatternExtractor<Shape>::replaceNulls()
    {
        for(std::size_t localShapeIndex(0); localShapeIndex<_localShapes.size(); ++localShapeIndex)
        {
            LocalShape &target = _localShapes[localShapeIndex];

            if(target._learnedAmount<=0)
            {
                target.randomize();
                target *= _window;
                target._learnedAmount = 0;
            }
        }

        _kdTreePtr.reset(new KDTree<LocalShape>(-1, _localShapes));
    }

    template <class Shape>
    void PatternExtractor<Shape>::save(const std::string &prefix)
    {
        if(0)
        {
            std::ofstream ofs(prefix+"/pe.ser.tmp");
            if(!ofs)
            {
                std::cerr<<"unable to create file "<<prefix+"/pe.ser.tmp"<<std::endl;
                return;
            }

            boost::archive::text_oarchive oa(ofs);

            oa << SER_NVP(pe, *this);

            ofs.close();

            rename((prefix+"/pe.ser.tmp").c_str(), (prefix+"/pe.ser").c_str());
        }


        //images
        {
            std::size_t imagesAmount = std::min(std::size_t(10000), _localShapes.size());

            real learnedAmountMax = std::numeric_limits<typename Shape::real>::min();
            for(std::size_t localShapeIndex(0); localShapeIndex<imagesAmount; ++localShapeIndex)
            {
                LocalShape &localShape = _localShapes[localShapeIndex];
                learnedAmountMax = std::max(learnedAmountMax, localShape._learnedAmount);
            }

            std::size_t cols = sqrt(real(imagesAmount))*3/2+0.5;
            std::size_t rows = imagesAmount/cols;
            while(cols*rows < imagesAmount)
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

            for(std::size_t localShapeIndex(0); localShapeIndex<imagesAmount; ++localShapeIndex)
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
