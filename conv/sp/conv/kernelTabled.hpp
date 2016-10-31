#pragma once

#include "sp/math.hpp"
#include "sp/conv/signalConvolver.hpp"
#include "sp/conv/linearSolver.hpp"
#include <string>
#include <map>


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

        Matrix      _kernT;
        LinearSolverPtr   _solver;

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
