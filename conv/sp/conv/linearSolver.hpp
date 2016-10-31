#pragma once

#include "sp/math.hpp"

#include <memory>
#include "Eigen/SVD"
#include "Eigen/Dense"

namespace sp { namespace conv
{
    using Matrix = Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic>;
    using Vector = Eigen::Matrix<real, Eigen::Dynamic, 1>;
    using RowVector = Eigen::Matrix<real, 1, Eigen::Dynamic>;

    struct LinearSolverLDLT
        : public Eigen::LDLT<Matrix>
    {
        using Base = Eigen::LDLT<Matrix>;
        static const std::size_t _solverId = 1;

        LinearSolverLDLT();
        LinearSolverLDLT(const Matrix &m);
        ~LinearSolverLDLT();

        bool load(std::size_t dim, std::istream &in);
        bool save(std::ostream &out);
    };

    struct LinearSolverFullPivHouseholderQR
        : public Eigen::FullPivHouseholderQR<Matrix>
    {
        using Base = Eigen::FullPivHouseholderQR<Matrix>;
        static const std::size_t _solverId = 2;

        LinearSolverFullPivHouseholderQR();
        LinearSolverFullPivHouseholderQR(const Matrix &m);
        ~LinearSolverFullPivHouseholderQR();

        bool load(std::size_t dim, std::istream &in);
        bool save(std::ostream &out);
    };

    struct LinearSolverJacobiSVD
        : public Eigen::JacobiSVD<Matrix, Eigen::ColPivHouseholderQRPreconditioner>
    {
        using Base = Eigen::JacobiSVD<Matrix, Eigen::ColPivHouseholderQRPreconditioner>;
        static const std::size_t _solverId = 3;

        LinearSolverJacobiSVD();
        LinearSolverJacobiSVD(const Matrix &m);
        ~LinearSolverJacobiSVD();

        bool load(std::istream &in);
        bool save(std::ostream &out);
    };

    //using LinearSolver = LinearSolverLDLT;
    //using LinearSolver = LinearSolverFullPivHouseholderQR;
    using LinearSolver = LinearSolverJacobiSVD;

    using LinearSolverPtr = std::unique_ptr<LinearSolver>;

}}
