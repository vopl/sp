#include "sp/cls/som.hpp"
#include <QtGui/QImage>

namespace sp { namespace cls
{

    SOM::SOM()
    {

    }

    SOM::~SOM()
    {

    }

    void SOM::init(std::size_t shapeCols, std::size_t shapeRows, std::size_t shapes)
    {
        _shapeCols = shapeCols;
        _shapeRows = shapeRows;

        _window._values.resize(shapeCols*shapeRows);

        for(std::size_t row(0); row<shapeRows; ++row)
        {
            real vw = 0.5*(1.0 - cos(g_2pi*(row+1)/(shapeRows+1)));
            for(std::size_t col(0); col<shapeCols; ++col)
            {
                real hw = 0.5*(1.0 - cos(g_2pi*(col+1)/(shapeCols+1)));

                _window._values[row + _shapeRows * col] = 1;//hw*vw;
            }
        }


        _somShapes.resize(shapes);
        for(std::size_t idx(0); idx<shapes; ++idx)
        {
            SOMShape &somShape = _somShapes[idx];

            somShape._values.resize(shapeCols * shapeRows);

            for(auto &v : somShape._values)
            {
                v.re() = real(rand())/RAND_MAX-0.5;
                v.im() = real(rand())/RAND_MAX-0.5;
            }

            somShape *= _window;
        }

        _kdTreePtr.reset(new KDTree(-1, _somShapes));
    }

    void SOM::push4Learn(ShapeOldPtr shape)
    {
        assert(shape->_values.size() == _shapeCols*_shapeRows);

        //выровнять амплитуду на 1 и фазу на 0
        complex middle;
        for(const complex &v : shape->_values)
        {
            middle += v;
        }
        middle /= shape->_values.size();

        if(middle.a() < std::numeric_limits<real>::epsilon())
        {
            return;
        }

        for(complex &v : shape->_values)
        {
            v /= middle;
        }


        ShapeOldPtr mshape(new ShapeOld);
        mshape->_values.resize(shape->_values.size());

        real bestShapeDistance(1e100);
        ShapeOld bestShape;

        std::vector<std::size_t> indices(1);
        std::vector<real> distances(indices.size());

        for(std::size_t rphase(0); rphase<8; rphase+=1)
        {

            real phase = g_2pi*rphase/8;

            for(std::size_t i(0); i<mshape->_values.size(); ++i)
            {
                mshape->_values[i] = shape->_values[i].rotate(phase);
            }

            *mshape *= _window;


            _kdTreePtr->query(*mshape, indices.size(), &indices[0], &distances[0]);

            if(distances[0] < bestShapeDistance)
            {
                bestShapeDistance = distances[0];
                bestShape = *mshape;
            }
        }

        *shape = bestShape;

        indices.resize(10);
        distances.resize(indices.size());

        _kdTreePtr->query(*shape, indices.size(), &indices[0], &distances[0]);

        real weight(1);

        for(std::size_t idx(0); idx<indices.size(); ++idx)
        {
            std::size_t index = indices[idx];

            SOMShape &somShape  = _somShapes[index];

            SOMShape::Shape4Learn shape4Learn{shape, weight};
            somShape._shapes4Learn.emplace_back(shape4Learn);

            weight *= 0.75;
        }
    }

    void SOM::fixLearn(real rate)
    {
        std::size_t somShapeIndexMax(0);
        real somShapeLearnedAmountMax(0);

        std::size_t somShapeIndexMin(0);
        real somShapeLearnedAmountMin(_somShapes[0]._learnedAmount);


        for(std::size_t somShapeIndex(0); somShapeIndex<_somShapes.size(); ++somShapeIndex)
        {
            SOMShape &somShape = _somShapes[somShapeIndex];
            for(SOMShape::Shape4Learn &shape4Learn : somShape._shapes4Learn)
            {
                real localRate = rate * shape4Learn._weight;

                assert(shape4Learn._shape->_values.size() == somShape._values.size());

                for(std::size_t idx(0); idx<somShape._values.size(); ++idx)
                {
                    somShape._values[idx] = somShape._values[idx]*(1-localRate) + shape4Learn._shape->_values[idx]*localRate;
                }

                somShape._learnedAmount += localRate;
            }
            somShape._shapes4Learn.clear();

            if(somShapeLearnedAmountMax < somShape._learnedAmount)
            {
                somShapeLearnedAmountMax = somShape._learnedAmount;
                somShapeIndexMax = somShapeIndex;
            }

            if(somShapeLearnedAmountMin > somShape._learnedAmount)
            {
                somShapeLearnedAmountMin = somShape._learnedAmount;
                somShapeIndexMin = somShapeIndex;
            }

            somShape._learnedAmount *= 0.99;
        }


//        SOMShape &somShapeMax = _somShapes[somShapeIndexMax];

//        SOMShape somShapeMax2;
//        somShapeMax2._learnedAmount = 0;
//        somShapeMax2._values = somShapeMax._values;

//        for(std::size_t idx(0); idx<somShapeMax._values.size(); ++idx)
//        {
//            somShapeMax2._values[idx] = complex(real(1) * rand()/RAND_MAX - 0.5, real(1) * rand()/RAND_MAX - 0.5);
//        }
//        somShapeMax2 *= _window;
//        _somShapes.push_back(somShapeMax2);

//        if(_somShapes.size() > 50*50)
//        {
//            _somShapes.erase(_somShapes.begin() + somShapeIndexMin);
//        }


        std::sort(_somShapes.begin(), _somShapes.end(), [](const SOMShape &a, const SOMShape &b){
            return a._learnedAmount >= b._learnedAmount;
        });

        /////////-/////////-/////////-/////////-/////////-/////////-/////////-/////////-
        {
            const std::size_t k = 5;

            std::size_t cols = sqrt(real(_somShapes.size()))*3/2+0.5;
            std::size_t rows = _somShapes.size()/cols;
            while(cols*rows < _somShapes.size())
            {
                rows++;
            }

            QImage imgRe((_shapeCols*k+1)*cols, (_shapeRows*k+1)*rows, QImage::Format_RGB32);
            QImage imgIm((_shapeCols*k+1)*cols, (_shapeRows*k+1)*rows, QImage::Format_RGB32);
            QImage imgA((_shapeCols*k+1)*cols, (_shapeRows*k+1)*rows, QImage::Format_RGB32);
            imgRe.fill(QColor::fromRgb(255,255,255));
            imgIm.fill(QColor::fromRgb(255,255,255));
            imgA.fill(QColor::fromRgb(255,255,255));

            for(std::size_t somShapeIndex(0); somShapeIndex<_somShapes.size(); ++somShapeIndex)
            {
                const ShapeOld &somShape = _somShapes[somShapeIndex];
                real maxa(0);
                for(const complex &v : somShape._values)
                {
                    maxa = std::max(maxa, v.a());
                }

                std::size_t col = somShapeIndex%cols;
                std::size_t row = somShapeIndex/cols;

                for(std::size_t idx(0); idx<somShape._values.size(); ++idx)
                {
                    real grayRe = (maxa ? somShape._values[idx].re()/maxa : 0)/2+0.5;
                    grayRe = std::min(grayRe, real(1));

                    real grayIm = (maxa ? somShape._values[idx].im()/maxa : 0)/2+0.5;
                    grayIm = std::min(grayIm, real(1));

                    real grayA = maxa ? somShape._values[idx].a()/maxa : 0;
                    grayA = std::min(grayA, real(1));

                    QColor colorRe = QColor::fromRgbF(grayRe, grayRe, grayRe);
                    QColor colorIm = QColor::fromRgbF(grayIm, grayIm, grayIm);
                    QColor colorA = QColor::fromRgbF(grayA, grayA, grayA);

                    std::size_t shapeCol = idx/_shapeRows;
                    std::size_t shapeRow = idx%_shapeRows;

                    for(std::size_t i(0); i<k; i++)
                    {
                        for(std::size_t j(0); j<k; j++)
                        {
                            imgRe.setPixelColor(
                                        col * (_shapeCols*k+1) + (shapeCol)*k + i,
                                        row * (_shapeRows*k+1) + (shapeRow)*k + j,
                                        colorRe);
                            imgIm.setPixelColor(
                                        col * (_shapeCols*k+1) + (shapeCol)*k + i,
                                        row * (_shapeRows*k+1) + (shapeRow)*k + j,
                                        colorIm);
                            imgA.setPixelColor(
                                        col * (_shapeCols*k+1) + (shapeCol)*k + i,
                                        row * (_shapeRows*k+1) + (shapeRow)*k + j,
                                        colorA);
                        }
                    }
                }
            }

            imgRe.save("somShape/lastLearn_re.png");
            imgIm.save("somShape/lastLearn_im.png");
            imgA.save("somShape/lastLearn_a.png");
        }

        _kdTreePtr.reset(new KDTree(-1, _somShapes));

        std::cout<<"learned"<<std::endl;
    }
}}
