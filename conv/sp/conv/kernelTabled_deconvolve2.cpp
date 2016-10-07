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

            //значения отклика, если задано
            //длина n/2
            const complex   *_ev;

            KernelTabled    *_kernelTabled;
        };

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        void evalEchoTail(const real *p, real *hx, std::size_t m, std::size_t n, void *vparams)
        {
            Params *params = reinterpret_cast<Params *>(vparams);

            for(std::size_t i(0); i<n/2; i++)
            {
                real et = params->_et[i];
                Summator<real> re;
                Summator<real> im;

                for(std::size_t j(0); j<m/2; j++)
                {
                    real st = params->_st[j];

                    real rr, ri, ir, ii;
                    params->_kernelTabled->evalKernel(et/st, rr, ri, ir, ii);

                    //assert(std::isfinite(rr) && std::isfinite(ri));
                    //assert(std::isfinite(ir) && std::isfinite(ii));

                    real sr = p[j*2+0];
                    real si = p[j*2+1];

                    re += sr*rr - si*ri;
                    im += sr*ir - si*ii;
                }

                //assert(std::isfinite(re) && std::isfinite(im));

                re += -params->_ev[i].re();
                im += -params->_ev[i].im();

                hx[i*2+0] = -real(re.v());
                hx[i*2+1] = -real(im.v());
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void evalKernel(real *jx, std::size_t m, std::size_t n, void *vparams)
        {
            Params *params = reinterpret_cast<Params *>(vparams);

            for(std::size_t i(0); i<n/2; i++)
            {
                real et = params->_et[i];

                for(std::size_t j(0); j<m/2; j++)
                {
                    real st = params->_st[j];

                    real rr,  ri,  ir,  ii;
                    params->_kernelTabled->evalKernel(et/st, rr, ri, ir, ii);

                    jx[(i*2+0)+(j*2+0)*n] = real(rr);
                    jx[(i*2+0)+(j*2+1)*n] = real(-ri);

                    jx[(i*2+1)+(j*2+0)*n] = real(ir);
                    jx[(i*2+1)+(j*2+1)*n] = real(-ii);
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void KernelTabled::deconvolve2(
            size_t esize, const real *et, const complex *ev, //отклик
            size_t ssize, const real *st,       complex *sv, //спектр
            size_t &iters, //макс итераций
            real initialMu,
            real &error0,
            real &error1)
    {
        Params params;
        params._et = et;
        params._ev = ev;
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
                Matrix kern(esize*2, ssize*2);
                conv::evalKernel(kern.data(), ssize*2, esize*2, &params);
                _kernT = kern.transpose();
                Matrix kernTKern = _kernT * kern;

                _solver.reset(new Solver(kernTKern));

                std::cerr<<"ok"<<std::endl;
                d2Save();
            }
        }

        Eigen::Map<const Vector> echo(&ev->re(), esize*2, 1);
        Eigen::Map<Vector> spectr(&sv->re(), ssize*2, 1);

        spectr = _solver->solve(_kernT * echo);
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

        _kernT.resize(std::get<7>(_d2Key)*2, std::get<4>(_d2Key)*2);
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

}}
