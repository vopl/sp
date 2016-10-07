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


        complex eval(real t, real st, const complex &sv);

        void deconvolve(
            size_t esize, const real *et, const complex *ev,//отклик
            size_t ssize, const real *st,       complex *sv,//спектр
            size_t &iters,//макс итераций
            real initialMu,
            real &error0,
            real &error1);

        void deconvolve2(
            size_t esize, const real *et, const complex *ev,//отклик
            size_t ssize, const real *st,       complex *sv,//спектр
            size_t &iters,//макс итераций
            real initialMu,
            real &error0,
            real &error1);

        void flush();

    public:
        void evalKernel(real t, real &rr, real &ri, real &ir, real &ii);

    private:
        void buildValue(const real &period, complex &re, complex &im);

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
        struct Value
        {
            complex _re;
            complex _im;
        };

        using ValuesByPeriod = std::map<real, Value>;
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
            static const std::size_t _solverId = 123456;

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
            static const std::size_t _solverId = 234567;

            SolverFullPivHouseholderQR();
            SolverFullPivHouseholderQR(const Matrix &m);
            ~SolverFullPivHouseholderQR();

            bool load(std::size_t dim, std::istream &in);
            bool save(std::ostream &out);
        };


        //using Solver = SolverLDLT;
        using Solver = SolverFullPivHouseholderQR;
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
