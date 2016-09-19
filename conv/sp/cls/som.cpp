#include "sp/cls/som.hpp"

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
        _somShapes.resize(shapes);
        for(std::size_t idx(0); idx<shapes; ++idx)
        {
            SOMShape &somShape = _somShapes[idx];

            somShape._values.resize(shapeCols * shapeRows);

            for(auto &v : somShape._values)
            {
                v.re() = real(rand())/RAND_MAX;
                v.im() = real(rand())/RAND_MAX;
            }
        }

        _kdTreePtr.reset(new KDTree(-1, _somShapes));
    }

    void SOM::push4Learn(ShapePtr shape)
    {
        std::vector<std::size_t> indices(10);
        std::vector<real> distances(indices.size());

        _kdTreePtr->query(*shape, indices.size(), &indices[0], &distances[0]);

        real weight(1);

        for(auto index : indices)
        {
            SOMShape &somShape  = _somShapes[index];

            SOMShape::Shape4Learn shape4Learn{shape, weight};
            somShape._shapes4Learn.emplace_back(shape4Learn);

            weight *= 0.7;
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
        }


        SOMShape &somShapeMax = _somShapes[somShapeIndexMax];

        SOMShape somShapeMax2;
        somShapeMax2._learnedAmount = somShapeMax._learnedAmount/=2;
        somShapeMax2._values = somShapeMax._values;

        for(std::size_t idx(0); idx<somShapeMax._values.size(); ++idx)
        {
            complex delta = somShapeMax._values[idx] * (real(0.2) * rand()/RAND_MAX - 0.1);

            somShapeMax._values[idx] += delta;
            somShapeMax2._values[idx] -= delta;
        }
        _somShapes.push_back(somShapeMax2);

        if(_somShapes.size() > 100)
        {
            _somShapes.erase(_somShapes.begin() + somShapeIndexMin);
        }



        _kdTreePtr.reset(new KDTree(-1, _somShapes));
    }
}}
