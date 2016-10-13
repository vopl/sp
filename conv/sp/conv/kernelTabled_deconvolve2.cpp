#include "sp/conv/kernelTabled.hpp"
#include "sp/math.hpp"

#include <iostream>
#include <functional>
#include <fstream>

namespace sp { namespace conv
{
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct Params
        {
            //сетка периода, должна быть задана всегда
            //отклика - длина n/2
            const real      *_et;

            //спектра - длина m/2
            const real      *_st;

//            //значения отклика, если задано
//            //длина n/2
//            const EchoPoint *_ev;

            KernelTabled    *_kernelTabled;
        };

//        //////////////////////////////////////////////////////////////////////////
//        //////////////////////////////////////////////////////////////////////////
//        //////////////////////////////////////////////////////////////////////////
//        void evalEchoTail(const SpectrPoint *p, EchoPoint *hx, std::size_t m, std::size_t n, void *vparams)
//        {
//            Params *params = reinterpret_cast<Params *>(vparams);

//            for(std::size_t i(0); i<n; i++)
//            {
//                real et = params->_et[i];
//                Summator<EchoPoint> res;

//                for(std::size_t j(0); j<m; j++)
//                {
//                    real st = params->_st[j];

//                    KernelPoint k;
//                    params->_kernelTabled->evalKernel(et/st, k);

//                    //assert(std::isfinite(rr) && std::isfinite(ri));
//                    //assert(std::isfinite(ir) && std::isfinite(ii));

//                    res += p[j] * k;
//                }

//                //assert(std::isfinite(re) && std::isfinite(im));

//                res += -params->_ev[i];

//                hx[i] = res;
//            }
//        }

        //////////////////////////////////////////////////////////////////////////
        void evalKernel(real tMult, Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic> &jx, std::size_t m, std::size_t n, void *vparams)
        {
            Params *params = reinterpret_cast<Params *>(vparams);

            for(std::size_t i(0); i<n; i++)
            {
                real et = params->_et[i];

                for(std::size_t j(0); j<m; j++)
                {
                    real st = params->_st[j];

                    KernelPoint k;
                    params->_kernelTabled->evalKernel(tMult*et/st, k);

                    jx.block<KernelPoint::ColsAtCompileTime, KernelPoint::RowsAtCompileTime>(
                                KernelPoint::ColsAtCompileTime*i, KernelPoint::RowsAtCompileTime*j)

                            = k.transpose();
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void KernelTabled::deconvolve(
            size_t esize, const real *et, const EchoPoint   *ev,
            size_t ssize, const real *st,       SpectrPoint *sv)
    {
        Params params;
        params._et = et;
//        params._ev = ev;
        params._st = st;
        params._kernelTabled = this;



        D2Key d2Key = std::make_tuple(
                    _ppw,
                    _samplesPerLevelPeriod,

                    et[0],
                    et[esize-1],
                    esize,

                    st[0],
                    st[ssize-1],
                    ssize);

        if(d2Key != _d2Key)
        {
            _d2Key = d2Key;

            if(!d2Load())
            {
                std::cerr<<"build d2...";std::cerr.flush();

                Matrix kern(esize*KernelPoint::ColsAtCompileTime, ssize*KernelPoint::RowsAtCompileTime);

                conv::evalKernel(400, kern, ssize, esize, &params);
                _kernT = kern.transpose();
                Matrix kernTKern = _kernT * kern;

                _solver.reset(new Solver(kernTKern));

                conv::evalKernel(1, kern, ssize, esize, &params);
                _kernT = kern.transpose();


                std::cerr<<"ok"<<std::endl;
                d2Save();
            }
        }

#if 1
        Eigen::Map<const Vector> echo(ev->data(), esize*ev->size(), 1);
        Eigen::Map<Vector> spectr(sv->data(), ssize*sv->size(), 1);

        spectr.noalias() = _solver->solve(_kernT * echo);
#else
        Vector echoTail(esize*2);
        Eigen::Map<Vector> spectr(&sv->re(), ssize*2, 1);

        evalEchoTail(spectr.data(), echoTail.data(), ssize*2, esize*2, &params);
        error1 = error0 = echoTail.norm();

        std::size_t iter(0);
        for(; iter < iters; ++iter)
        {
            spectr += _solver->solve(_kernT * echoTail);

            evalEchoTail(spectr.data(), echoTail.data(), ssize*2, esize*2, &params);
            real error = echoTail.norm();

            if(error >= error1)
            {
                error1 = error;
                break;
            }

            error1 = error;
        }

        iters = iter;
#endif

    }

    namespace
    {
        template <typename TT>
        struct hash
        {
            size_t
            operator()(TT const& tt) const
            {
                return std::hash<TT>()(tt);
            }
        };

        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v)
        {
            seed ^= hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
            static void apply(size_t& seed, Tuple const& tuple)
            {
                HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
                hash_combine(seed, std::get<Index>(tuple));
            }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple,0>
        {
            static void apply(size_t& seed, Tuple const& tuple)
            {
                hash_combine(seed, std::get<0>(tuple));
            }
        };

        template <typename ... TT>
        struct hash<std::tuple<TT...>>
        {
            size_t
            operator()(std::tuple<TT...> const& tt) const
            {
                size_t seed = 0;
                HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
                return seed;
            }
        };
    }

    //////////////////////////////////////////////////////////////////////////
    std::string KernelTabled::d2FileName()
    {
        char tmp[4096];
        std::size_t h = 0;

        hash_combine(h, sizeof(real));
        hash_combine(h, hash<D2Key>()(_d2Key));
        hash_combine(h, Solver::_solverId);

        sprintf(tmp, "kt_d2_%zx.bin", h);
        return tmp;

    }

    //////////////////////////////////////////////////////////////////////////
    bool KernelTabled::d2Load()
    {
        std::cerr<<"load d2: "<<d2FileName()<<"...";

        std::ifstream in(d2FileName().c_str(), std::ios::in|std::ios::binary);
        if(!in)
        {
            std::cerr<<"no file"<<std::endl;
            return false;
        }

        _kernT.resize(std::get<7>(_d2Key)*KernelPoint::RowsAtCompileTime, std::get<4>(_d2Key)*KernelPoint::ColsAtCompileTime);
        in.read( (char *) _kernT.data(), _kernT.rows()*_kernT.cols()*sizeof(typename Matrix::Scalar) );
        if(!in)
        {
            std::cerr<<"bad file"<<std::endl;
            return false;
        }

        _solver.reset(new Solver());
        _solver->load(_kernT.rows(), in);

        char magic[4];
        in.read((char*) magic, sizeof(magic) );

        if('E'!= magic[0] || 'N'!= magic[1] || 'D'!= magic[2] || '.'!= magic[3])
        {
            std::cerr<<"bad file"<<std::endl;
            return false;
        }
        in.close();

        std::cerr<<"ok"<<std::endl;

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    bool KernelTabled::d2Save()
    {
        std::string fname = d2FileName();
        std::cerr<<"save d2: "<<fname<<"...";
        std::ofstream out((fname+".tmp").c_str(), std::ios::out|std::ios::binary);
        if(!out)
        {
            std::cerr<<"unable to open file: "<<fname<<std::endl;
            abort();
            return false;
        }

        out.write((char*) _kernT.data(), _kernT.rows()*_kernT.cols()*sizeof(typename Matrix::Scalar) );

        _solver->save(out);

        char magic[4] = {'E', 'N', 'D', '.'};
        out.write((char*) magic, sizeof(magic) );

        out.flush();
        bool res = out.good();
        out.close();

        if(!res)
        {
            std::cerr<<"bad file"<<std::endl;
            return res;
        }

        if(0 != rename((fname+".tmp").c_str(), (fname).c_str()))
        {
            std::cerr<<"unable to rename file: "<<fname<<".tmp -> "<<fname<<std::endl;
            abort();
            return false;
        }

        std::cerr<<"ok"<<std::endl;

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    const std::size_t KernelTabled::SolverLDLT::_solverId;

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverLDLT::SolverLDLT()
        : Base()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverLDLT::SolverLDLT(const Matrix &m)
        : Base(m)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverLDLT::~SolverLDLT()
    {
    }


    //////////////////////////////////////////////////////////////////////////
    bool KernelTabled::SolverLDLT::load(std::size_t dim, std::istream &in)
    {
        m_matrix.resize(dim, dim);
        in.read((char*) m_matrix.data(), m_matrix.rows()*m_matrix.cols()*sizeof(typename Matrix::Scalar) );
        in.read((char*) &m_l1_norm, sizeof(m_l1_norm) );

        m_transpositions.resize(dim);
        in.read((char*) m_transpositions.indices().data(), m_transpositions.indices().rows()*m_transpositions.indices().cols()*sizeof(typename TranspositionType::IndicesType::Scalar) );

        in.read((char*) &m_sign, sizeof(m_sign) );
        in.read((char*) &m_isInitialized, sizeof(m_isInitialized) );

        m_temporary.resize(dim, 1);

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    bool KernelTabled::SolverLDLT::save(std::ostream &out)
    {
        out.write((char*) m_matrix.data(), m_matrix.rows()*m_matrix.cols()*sizeof(typename MatrixType::Scalar) );
        out.write((char*) &m_l1_norm, sizeof(m_l1_norm) );
        out.write((char*) m_transpositions.indices().data(), m_transpositions.indices().rows()*m_transpositions.indices().cols()*sizeof(typename TranspositionType::IndicesType::Scalar) );
        out.write((char*) &m_sign, sizeof(m_sign) );
        out.write((char*) &m_isInitialized, sizeof(m_isInitialized) );

        return out.good();
    }

    //////////////////////////////////////////////////////////////////////////
    const std::size_t KernelTabled::SolverFullPivHouseholderQR::_solverId;

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverFullPivHouseholderQR::SolverFullPivHouseholderQR()
        : Base()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverFullPivHouseholderQR::SolverFullPivHouseholderQR(const Matrix &m)
        : Base(m)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverFullPivHouseholderQR::~SolverFullPivHouseholderQR()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    bool KernelTabled::SolverFullPivHouseholderQR::load(std::size_t dim, std::istream &in)
    {
        //m_cols_permutation
        m_cols_permutation.indices().resize(dim);
        in.read((char*) m_cols_permutation.indices().data(), m_cols_permutation.indices().rows()*m_cols_permutation.indices().cols()*sizeof(typename PermutationType::IndicesType::Scalar) );

        //m_cols_transpositions
        m_cols_transpositions.resize(dim);
        in.read((char*) m_cols_transpositions.data(), m_cols_transpositions.rows()*m_cols_transpositions.cols()*sizeof(typename IntDiagSizeVectorType::Scalar) );

        //m_det_pq
        in.read((char*) &m_det_pq, sizeof(m_det_pq) );

        //m_hCoeffs
        m_hCoeffs.resize(dim);
        in.read((char*) m_hCoeffs.data(), m_hCoeffs.rows()*m_hCoeffs.cols()*sizeof(typename HCoeffsType::Scalar) );

        //m_isInitialized
        in.read((char*) &m_isInitialized, sizeof(m_isInitialized) );

        //m_maxpivot
        in.read((char*) &m_maxpivot, sizeof(m_maxpivot) );

        //m_nonzero_pivots
        in.read((char*) &m_nonzero_pivots, sizeof(m_nonzero_pivots) );

        //m_precision
        in.read((char*) &m_precision, sizeof(m_precision) );

        //m_prescribedThreshold
        in.read((char*) &m_prescribedThreshold, sizeof(m_prescribedThreshold) );

        //m_qr
        m_qr.resize(dim, dim);
        in.read((char*) m_qr.data(), m_qr.rows()*m_qr.cols()*sizeof(typename MatrixType::Scalar) );

        //m_rows_transpositions
        m_rows_transpositions.resize(dim);
        in.read((char*) m_rows_transpositions.data(), m_rows_transpositions.rows()*m_rows_transpositions.cols()*sizeof(typename IntDiagSizeVectorType::Scalar) );

        //m_temp
        m_temp.resize(dim);

        //m_usePrescribedThreshold
        in.read((char*) &m_usePrescribedThreshold, sizeof(m_usePrescribedThreshold) );

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    bool KernelTabled::SolverFullPivHouseholderQR::save(std::ostream &out)
    {

        //m_cols_permutation
        out.write((char*) m_cols_permutation.indices().data(), m_cols_permutation.indices().rows()*m_cols_permutation.indices().cols()*sizeof(typename PermutationType::IndicesType::Scalar) );

        //m_cols_transpositions
        out.write((char*) m_cols_transpositions.data(), m_cols_transpositions.rows()*m_cols_transpositions.cols()*sizeof(typename IntDiagSizeVectorType::Scalar) );

        //m_det_pq
        out.write((char*) &m_det_pq, sizeof(m_det_pq) );

        //m_hCoeffs
        out.write((char*) m_hCoeffs.data(), m_hCoeffs.rows()*m_hCoeffs.cols()*sizeof(typename HCoeffsType::Scalar) );

        //m_isInitialized
        out.write((char*) &m_isInitialized, sizeof(m_isInitialized) );

        //m_maxpivot
        out.write((char*) &m_maxpivot, sizeof(m_maxpivot) );

        //m_nonzero_pivots
        out.write((char*) &m_nonzero_pivots, sizeof(m_nonzero_pivots) );

        //m_precision
        out.write((char*) &m_precision, sizeof(m_precision) );

        //m_prescribedThreshold
        out.write((char*) &m_prescribedThreshold, sizeof(m_prescribedThreshold) );

        //m_qr
        out.write((char*) m_qr.data(), m_qr.rows()*m_qr.cols()*sizeof(typename MatrixType::Scalar) );

        //m_rows_transpositions
        out.write((char*) m_rows_transpositions.data(), m_rows_transpositions.rows()*m_rows_transpositions.cols()*sizeof(typename IntDiagSizeVectorType::Scalar) );

        //m_temp

        //m_usePrescribedThreshold
        out.write((char*) &m_usePrescribedThreshold, sizeof(m_usePrescribedThreshold) );

        return true;
    }


































    //////////////////////////////////////////////////////////////////////////
    const std::size_t KernelTabled::SolverJacobiSVD::_solverId;

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverJacobiSVD::SolverJacobiSVD()
        : Base()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverJacobiSVD::SolverJacobiSVD(const Matrix &m)
        : Base(m, Eigen::ComputeThinU | Eigen::ComputeThinV)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    KernelTabled::SolverJacobiSVD::~SolverJacobiSVD()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    bool KernelTabled::SolverJacobiSVD::load(std::size_t dim, std::istream &in)
    {
        //m_cols
        in.read((char*) &m_cols, sizeof(m_cols) );

        //m_computationOptions
        in.read((char*) &m_computationOptions, sizeof(m_computationOptions) );

        //m_computeFullU
        in.read((char*) &m_computeFullU, sizeof(m_computeFullU) );

        //m_computeFullV
        in.read((char*) &m_computeFullV, sizeof(m_computeFullV) );

        //m_computeThinU
        in.read((char*) &m_computeThinU, sizeof(m_computeThinU) );

        //m_computeThinV
        in.read((char*) &m_computeThinV, sizeof(m_computeThinV) );

        //m_diagSize
        in.read((char*) &m_diagSize, sizeof(m_diagSize) );

        //m_isAllocated
        in.read((char*) &m_isAllocated, sizeof(m_isAllocated) );

        //m_isInitialized
        in.read((char*) &m_isInitialized, sizeof(m_isInitialized) );

        //m_matrixU
        m_matrixU.resize(dim, dim);
        in.read((char*) m_matrixU.data(), m_matrixU.rows()*m_matrixU.cols()*sizeof(typename MatrixUType::Scalar) );

        //m_matrixV
        m_matrixV.resize(dim, dim);
        in.read((char*) m_matrixV.data(), m_matrixV.rows()*m_matrixV.cols()*sizeof(typename MatrixVType::Scalar) );

        //m_nonzeroSingularValues
        in.read((char*) &m_nonzeroSingularValues, sizeof(m_nonzeroSingularValues) );

        //m_prescribedThreshold
        in.read((char*) &m_prescribedThreshold, sizeof(m_prescribedThreshold) );

        //m_rows
        in.read((char*) &m_rows, sizeof(m_rows) );

        //m_singularValues
        m_singularValues.resize(dim);
        in.read((char*) m_singularValues.data(), m_singularValues.rows()*m_singularValues.cols()*sizeof(typename SingularValuesType::Scalar) );

        //m_usePrescribedThreshold
        in.read((char*) &m_usePrescribedThreshold, sizeof(m_usePrescribedThreshold) );

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    bool KernelTabled::SolverJacobiSVD::save(std::ostream &out)
    {
        //m_cols
        out.write((char*) &m_cols, sizeof(m_cols) );

        //m_computationOptions
        out.write((char*) &m_computationOptions, sizeof(m_computationOptions) );

        //m_computeFullU
        out.write((char*) &m_computeFullU, sizeof(m_computeFullU) );

        //m_computeFullV
        out.write((char*) &m_computeFullV, sizeof(m_computeFullV) );

        //m_computeThinU
        out.write((char*) &m_computeThinU, sizeof(m_computeThinU) );

        //m_computeThinV
        out.write((char*) &m_computeThinV, sizeof(m_computeThinV) );

        //m_diagSize
        out.write((char*) &m_diagSize, sizeof(m_diagSize) );

        //m_isAllocated
        out.write((char*) &m_isAllocated, sizeof(m_isAllocated) );

        //m_isInitialized
        out.write((char*) &m_isInitialized, sizeof(m_isInitialized) );

        //m_matrixU
        out.write((char*) m_matrixU.data(), m_matrixU.rows()*m_matrixU.cols()*sizeof(typename MatrixUType::Scalar) );

        //m_matrixV
        out.write((char*) m_matrixV.data(), m_matrixV.rows()*m_matrixV.cols()*sizeof(typename MatrixVType::Scalar) );

        //m_nonzeroSingularValues
        out.write((char*) &m_nonzeroSingularValues, sizeof(m_nonzeroSingularValues) );

        //m_prescribedThreshold
        out.write((char*) &m_prescribedThreshold, sizeof(m_prescribedThreshold) );

        //m_rows
        out.write((char*) &m_rows, sizeof(m_rows) );

        //m_singularValues
        out.write((char*) m_singularValues.data(), m_singularValues.rows()*m_singularValues.cols()*sizeof(typename SingularValuesType::Scalar) );

        //m_usePrescribedThreshold
        out.write((char*) &m_usePrescribedThreshold, sizeof(m_usePrescribedThreshold) );

        return true;
    }

}}
