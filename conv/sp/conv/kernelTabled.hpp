#pragma once

#include "sp/math.hpp"
#include "sp/conv/signalConvolver.hpp"
#include <string>
#include <map>

#include "Eigen/SVD"
#include "Eigen/Dense"

namespace sp { namespace conv
{
    class KernelTabled
    {
    public:
        KernelTabled(real ppw, std::size_t samplesPerLevelPeriod);
        ~KernelTabled();


        EchoPoint eval(real t, real st, const SpectrPoint &sv);

        void deconvolve(
            size_t esize, const real *et, const EchoPoint   *ev, //отклик
            size_t ssize, const real *st,       SpectrPoint *sv//спектр
        );

        void flush();

    public:
        void evalKernel(real t, KernelPoint &kv);

    private:
        void buildValue(const real &period, KernelPoint &kv);

    private:
        std::string stateFileName();
        bool load();
        bool save();

        SignalConvolver &getSignalConvolver();

    private:
        real _ppw;
        std::size_t _samplesPerLevelPeriod;

        using SignalConvolverPtr = std::unique_ptr<SignalConvolver>;
        SignalConvolverPtr _scp;

    private:
        using ValuesByPeriod = std::map<
            real,
            KernelPoint,
            std::less<real>,
            Eigen::aligned_allocator<std::pair<const real, KernelPoint>>
        >;

        ValuesByPeriod _valuesByPeriod;
        std::size_t _addedValuesAmount = 0;

    private://deconvolve2
        using Matrix = Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic>;
        using Vector = Eigen::Matrix<real, Eigen::Dynamic, 1>;
        using RowVector = Eigen::Matrix<real, 1, Eigen::Dynamic>;

        struct SolverLDLT
            : public Eigen::LDLT<Matrix>
        {
            using Base = Eigen::LDLT<Matrix>;
            static const std::size_t _solverId = 1;

            SolverLDLT();
            SolverLDLT(const Matrix &m);
            ~SolverLDLT();

            bool load(std::size_t dim, std::istream &in);
            bool save(std::ostream &out);
        };

        struct SolverFullPivHouseholderQR
            : public Eigen::FullPivHouseholderQR<Matrix>
        {
            using Base = Eigen::FullPivHouseholderQR<Matrix>;
            static const std::size_t _solverId = 2;

            SolverFullPivHouseholderQR();
            SolverFullPivHouseholderQR(const Matrix &m);
            ~SolverFullPivHouseholderQR();

            bool load(std::size_t dim, std::istream &in);
            bool save(std::ostream &out);
        };

        struct SolverJacobiSVD
            : public Eigen::JacobiSVD<Matrix, Eigen::ColPivHouseholderQRPreconditioner>
        {
            using Base = Eigen::JacobiSVD<Matrix, Eigen::ColPivHouseholderQRPreconditioner>;
            static const std::size_t _solverId = 3;

            SolverJacobiSVD();
            SolverJacobiSVD(const Matrix &m);
            ~SolverJacobiSVD();

            bool load(std::size_t dim, std::istream &in);
            bool save(std::ostream &out);
        };

        using Solver = SolverLDLT;
        //using Solver = SolverFullPivHouseholderQR;
        //using Solver = SolverJacobiSVD;

        using SolverPtr = std::unique_ptr<Solver>;

        Matrix      _kernT;
        SolverPtr   _solver;

        using D2Key = std::tuple<
            real,       //ppw
            std::size_t,//splp

            real,       //minTE
            real,       //maxTE
            std::size_t,//stepsTE

            real,       //minTS
            real,       //maxTS
            std::size_t //stepsTS
        >;

        D2Key _d2Key;

        std::string d2FileName();
        bool d2Load();
        bool d2Save();

    };
}}
