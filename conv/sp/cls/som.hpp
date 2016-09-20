#pragma once
#include "sp/math.hpp"
#include "sp/cls/nanoflann.hpp"
#include <memory>

namespace sp { namespace cls
{

    class ShapeOld
    {
    public:
        TVComplex   _values;//periods X frames

        ShapeOld &operator *= (const ShapeOld &to)
        {
            assert(_values.size() == to._values.size());

            for(std::size_t i(0); i<_values.size(); ++i)
            {
                _values[i] *= to._values[i];
            }

            return *this;
        }

//        std::size_t _frameRotor = 0;
//        std::size_t _periodRotor = 0;

//        real        _phaseRotor = 0;
//        real        _amplitudeOffset = 0;
//        real        _amplitudeMult = 1;
    };
    using ShapeOldPtr = std::shared_ptr<ShapeOld>;

    class SOM
    {
    public:
        SOM();
        ~SOM();

        void init(std::size_t shapeCols, std::size_t shapeRows, std::size_t shapes);

        void push4Learn(ShapeOldPtr shape);

        void fixLearn(real rate = 0.01);

    private:

        std::size_t _shapeCols;
        std::size_t _shapeRows;

        struct SOMShape
            : public ShapeOld
        {
            struct Shape4Learn
            {
                ShapeOldPtr    _shape;
                real        _weight;
            };

            std::vector<Shape4Learn> _shapes4Learn;

            real _learnedAmount = 0;
        };

        ShapeOld _window;

        std::vector<SOMShape> _somShapes;



    private:
        struct KDTree
        {
            using num_t = real;
            static const int DIM = -1;
            using Distance = nanoflann::metric_L2;
            using IndexType = size_t;
            using VectorOfVectorsType = const std::vector<SOMShape>;

            typedef KDTree self_t;
            typedef typename Distance::template traits<num_t,self_t>::distance_t metric_t;
            typedef nanoflann::KDTreeSingleIndexAdaptor< metric_t,self_t,DIM,IndexType>  index_t;

            index_t* index; //! The kd-tree index for the user to call its methods as usual with any other FLANN index.

            /// Constructor: takes a const ref to the vector of vectors object with the data points
            KDTree(const int dimensionality, const VectorOfVectorsType &mat, const int leaf_max_size = 1) : m_data(mat)
            {
                assert(mat.size()!=0 && mat[0]._values.size()!=0);
                const size_t dims = mat[0]._values.size()*2;
                if (DIM>0 && static_cast<int>(dims)!=DIM)
                    throw std::runtime_error("Data set dimensionality does not match the 'DIM' template argument");
                index = new index_t( dims, *this /* adaptor */, nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size ) );
                index->buildIndex();
            }

            ~KDTree() {
                delete index;
            }

            const VectorOfVectorsType &m_data;

            /** Query for the \a num_closest closest points to a given point (entered as query_point[0:dim-1]).
              *  Note that this is a short-cut method for index->findNeighbors().
              *  The user can also call index->... methods as desired.
              * \note nChecks_IGNORED is ignored but kept for compatibility with the original FLANN interface.
              */
            inline void query(const ShapeOld &query_point, const size_t num_closest, IndexType *out_indices, num_t *out_distances_sq, const int nChecks_IGNORED = 10) const
            {
                nanoflann::KNNResultSet<num_t,IndexType> resultSet(num_closest);
                resultSet.init(out_indices, out_distances_sq);
                //AHTUNG complex used as array of two reals
                index->findNeighbors(resultSet, &query_point._values[0].re(), nanoflann::SearchParams());
            }

            /** @name Interface expected by KDTreeSingleIndexAdaptor
              * @{ */

            const self_t & derived() const {
                return *this;
            }
            self_t & derived()       {
                return *this;
            }

            // Must return the number of data points
            inline size_t kdtree_get_point_count() const {
                return m_data.size();
            }

            // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
            inline num_t kdtree_distance(const num_t *p1, const size_t idx_p2,size_t size) const
            {
                num_t s=0;
                for (size_t i=0; i<size; i++) {
                    const num_t d= p1[i]-(&m_data[idx_p2]._values[0].re())[i];
                    s+=d*d;
                }
                return s;
            }

            // Returns the dim'th component of the idx'th point in the class:
            inline num_t kdtree_get_pt(const size_t idx, int dim) const {
                return (&m_data[idx]._values[0].re())[dim];
            }

            // Optional bounding-box computation: return false to default to a standard bbox computation loop.
            //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
            //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
            template <class BBOX>
            bool kdtree_get_bbox(BBOX & /*bb*/) const {
                return false;
            }

            /** @} */

        }; // end of KDTree
        using KDTreePtr = std::unique_ptr<KDTree>;

        KDTreePtr _kdTreePtr;



    };

}}
