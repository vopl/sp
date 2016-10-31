#include "sp/conv/linearSolver.hpp"

namespace sp { namespace conv
{

        //////////////////////////////////////////////////////////////////////////
        const std::size_t LinearSolverLDLT::_solverId;

        //////////////////////////////////////////////////////////////////////////
        LinearSolverLDLT::LinearSolverLDLT()
            : Base()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        LinearSolverLDLT::LinearSolverLDLT(const Matrix &m)
            : Base(m)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        LinearSolverLDLT::~LinearSolverLDLT()
        {
        }


        //////////////////////////////////////////////////////////////////////////
        bool LinearSolverLDLT::load(std::size_t dim, std::istream &in)
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
        bool LinearSolverLDLT::save(std::ostream &out)
        {
            out.write((char*) m_matrix.data(), m_matrix.rows()*m_matrix.cols()*sizeof(typename MatrixType::Scalar) );
            out.write((char*) &m_l1_norm, sizeof(m_l1_norm) );
            out.write((char*) m_transpositions.indices().data(), m_transpositions.indices().rows()*m_transpositions.indices().cols()*sizeof(typename TranspositionType::IndicesType::Scalar) );
            out.write((char*) &m_sign, sizeof(m_sign) );
            out.write((char*) &m_isInitialized, sizeof(m_isInitialized) );

            return out.good();
        }

        //////////////////////////////////////////////////////////////////////////
        const std::size_t LinearSolverFullPivHouseholderQR::_solverId;

        //////////////////////////////////////////////////////////////////////////
        LinearSolverFullPivHouseholderQR::LinearSolverFullPivHouseholderQR()
            : Base()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        LinearSolverFullPivHouseholderQR::LinearSolverFullPivHouseholderQR(const Matrix &m)
            : Base(m)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        LinearSolverFullPivHouseholderQR::~LinearSolverFullPivHouseholderQR()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        bool LinearSolverFullPivHouseholderQR::load(std::size_t dim, std::istream &in)
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
        bool LinearSolverFullPivHouseholderQR::save(std::ostream &out)
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
        const std::size_t LinearSolverJacobiSVD::_solverId;

        //////////////////////////////////////////////////////////////////////////
        LinearSolverJacobiSVD::LinearSolverJacobiSVD()
            : Base()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        LinearSolverJacobiSVD::LinearSolverJacobiSVD(const Matrix &m)
            : Base(m, Eigen::ComputeThinU | Eigen::ComputeThinV)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        LinearSolverJacobiSVD::~LinearSolverJacobiSVD()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        bool LinearSolverJacobiSVD::load(std::istream &in)
        {
            //m_rows
            in.read((char*) &m_rows, sizeof(m_rows) );

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
            m_matrixU.resize(m_rows, m_cols);
            in.read((char*) m_matrixU.data(), m_matrixU.rows()*m_matrixU.cols()*sizeof(typename MatrixUType::Scalar) );

            //m_matrixV
            m_matrixV.resize(m_cols, m_cols);
            in.read((char*) m_matrixV.data(), m_matrixV.rows()*m_matrixV.cols()*sizeof(typename MatrixVType::Scalar) );

            //m_nonzeroSingularValues
            in.read((char*) &m_nonzeroSingularValues, sizeof(m_nonzeroSingularValues) );

            //m_prescribedThreshold
            in.read((char*) &m_prescribedThreshold, sizeof(m_prescribedThreshold) );

            //m_singularValues
            m_singularValues.resize(m_cols);
            in.read((char*) m_singularValues.data(), m_singularValues.rows()*m_singularValues.cols()*sizeof(typename SingularValuesType::Scalar) );

            //m_usePrescribedThreshold
            in.read((char*) &m_usePrescribedThreshold, sizeof(m_usePrescribedThreshold) );

            return true;
        }

        //////////////////////////////////////////////////////////////////////////
        bool LinearSolverJacobiSVD::save(std::ostream &out)
        {
            //m_rows
            out.write((char*) &m_rows, sizeof(m_rows) );

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

            //m_singularValues
            out.write((char*) m_singularValues.data(), m_singularValues.rows()*m_singularValues.cols()*sizeof(typename SingularValuesType::Scalar) );

            //m_usePrescribedThreshold
            out.write((char*) &m_usePrescribedThreshold, sizeof(m_usePrescribedThreshold) );

            return true;
        }

}}
