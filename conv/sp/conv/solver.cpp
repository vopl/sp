#include "sp/conv/solver.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

#include "Eigen/SVD"
#include "Eigen/Dense"


namespace sp { namespace conv
{
    //complex Point::_v[Point::_amplitudeKAmount][Point::_periodKAmount];

    constexpr real Point::_amplitudeK0[Point::_amplitudeKAmount];
    constexpr real Point::_amplitudeK1[Point::_amplitudeKAmount];

    constexpr real Point::_periodK0[Point::_periodKAmount];
    constexpr real Point::_periodK1[Point::_periodKAmount];

    Solver::Solver()
    {

    }

    Solver::~Solver()
    {

    }

    void Solver::setup(
            real ppw,
            const TVReal &periods,
            real signalSampleStep,
            std::size_t signalBucketSize)
    {
        _ppw = ppw;
        _periods = periods;
        _signalSampleStep = signalSampleStep;
        _signalBucketSize = signalBucketSize;

        _signal.resize(signalSize4Period(periods.back()));

        _points.resize(_periods.size());
        _coveredSignal.resize(_signal.size());
    }

    void Solver::pushSignal(const real *signalBucketSamples)
    {
        std::move(
                    _signal.begin() + _signalBucketSize,
                    _signal.end(),
                    _signal.begin());

        std::copy(
                    signalBucketSamples,
                    signalBucketSamples+_signalBucketSize,
                    _signal.end() - _signalBucketSize);

        _signalSamplesPushed += _signalBucketSize;
    }

    void Solver::update()
    {
//        if(0)
//        {
//            std::ofstream out("basis");


//            for(real x(0); x<10; x+=0.01)
//            {
//                real wx = x/10;

//                for(std::size_t pi(0); pi<Point::_periodKAmount; ++pi)
//                {
//                    for(std::size_t ai(0); ai<Point::_amplitudeKAmount; ++ai)
//                    {
//                        Point p;
//                        p._v[ai][pi] = 1;

//                        out<<p.eval(1.0, x, wx)<<", ";

//                    }
//                }
//                out<<std::endl;
//            }

//            exit(0);
//        }

        std::fill(_coveredSignal.begin(), _coveredSignal.end(), real(0));

        for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
        {
            _points[periodIndex] = Point();
            //_points[periodIndex]._v = 0;
        }


        for(;;)
        {
            TVPoint points(_points.size());
            fetchEcho(points);

            //update covered signal
            real err = applyEchoPart(points);
            //std::cout<<err<<std::endl;

            {
                std::ofstream out("s2");

                for(std::size_t sampleIndex(_signal.size()-1000); sampleIndex<_signal.size(); ++sampleIndex)
                {
                    out<<_signal[sampleIndex]<<", "<<_coveredSignal[sampleIndex]<<std::endl;
                }
            }


            {
                std::ofstream out("spectr");

                for(std::size_t periodIndex(0); periodIndex<_points.size(); ++periodIndex)
                {
                    out<<_points[periodIndex]<<std::endl;
                }
            }
            int k = 220;

        }

    }

















    void Solver::update2()
    {
        using Matrix = Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic>;
        using Vector = Eigen::Matrix<real, Eigen::Dynamic, 1>;
        using RowVector = Eigen::Matrix<real, 1, Eigen::Dynamic>;

        Eigen::Map<const Vector> signal(&_signal[0], _signal.size());
        Eigen::Map<RowVector> points(_points[0].data(), _points.size()*Point::dataSize());

        Matrix kern;
        kern.setZero(signal.size(), points.size());


        real xStop = _signalSampleStep;
        real xStart = xStop -(_signal.size()*_signalSampleStep);
        std::size_t minPeriodIndex = _periods.size();

        std::cout<<"fill "<<kern.rows()<<" x "<<kern.cols()<<std::endl;
        for(std::size_t sampleIndex(0); sampleIndex<kern.rows(); ++sampleIndex)
        {
            while(minPeriodIndex>0 && sampleIndex >= kern.rows() - signalSize4Period(_periods[minPeriodIndex-1]))
            {
                minPeriodIndex--;
            }

            real x = xStart + sampleIndex*_signalSampleStep;

            for(std::size_t periodIndex(minPeriodIndex); periodIndex<_periods.size(); ++periodIndex)
            {
                real period = _periods[periodIndex];

                real wx = 1.0 - (xStop - x)/period/_ppw;

                for(std::size_t bidx(0); bidx<Point::dataSize(); ++bidx)
                {
                    Point p;
                    p.data()[bidx] = 1.0;
                    kern(sampleIndex, (periodIndex)*Point::dataSize()+bidx) = p.eval(period, x, wx);
                }
            }
        }










//        //Ax=b lsq
//        std::cout<<"transpose"<<std::endl;
//        Matrix kernT = kern.transpose();

//        std::cout<<"mult"<<std::endl;
//        Matrix kernTKern = kernT * kern;

//        std::cout<<"solver"<<std::endl;
//        //Eigen::JacobiSVD<Matrix, Eigen::ColPivHouseholderQRPreconditioner> solver(kernTKern, Eigen::ComputeThinU | Eigen::ComputeThinV);
//        Eigen::FullPivHouseholderQR<Matrix> solver(kernTKern);


//        std::cout<<"solve"<<std::endl;
//        points = solver.solve(kernT * signal);


















        //Ax=b
        std::cout<<"solver"<<std::endl;
        Eigen::JacobiSVD<Matrix, Eigen::ColPivHouseholderQRPreconditioner> solver(kern, Eigen::ComputeThinU | Eigen::ComputeThinV);
        //Eigen::FullPivHouseholderQR<Matrix> solver(kern);


        std::cout<<"solve"<<std::endl;
        points = solver.solve(signal);















        std::cout<<"dump"<<std::endl;

        minPeriodIndex = _periods.size();
        for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
        {
            while(minPeriodIndex>0 && sampleIndex >= _signal.size() - signalSize4Period(_periods[minPeriodIndex-1]))
            {
                minPeriodIndex--;
            }


            real x = xStart + sampleIndex*_signalSampleStep;
            real &v = _coveredSignal[sampleIndex];
            v = 0;

            for(std::size_t periodIndex(minPeriodIndex); periodIndex<_periods.size(); ++periodIndex)
            {
                real period = _periods[periodIndex];
                real wx = 1.0 - (xStop - x)/period/_ppw;
                v += _points[periodIndex].eval(period, x, wx);
            }
        }













        {
            std::ofstream out("s2");

            for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
            {
                out<<_signal[sampleIndex]<<", "<<_coveredSignal[sampleIndex]<<std::endl;
            }
        }


        {
            std::ofstream out("spectr");

            for(std::size_t periodIndex(0); periodIndex<_points.size(); ++periodIndex)
            {
                out<<_points[periodIndex]<<std::endl;
            }
        }
        int k = 220;

        exit(0);
    }



    const TVPoint &Solver::points()
    {
        return _points;
    }

    std::size_t Solver::signalSize4Period(real period)
    {
        return std::size_t(period * _ppw / _signalSampleStep+0.5);
    }

    void Solver::fetchEcho(TVPoint &points)
    {
        std::vector<std::size_t> amounts(points.size());

        real xStop = _signalSamplesPushed*_signalSampleStep;
        real xStart = xStop - _signal.size()*_signalSampleStep;

        std::size_t minPeriodIndex = _periods.size();

        for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
        {
            while(minPeriodIndex>0 && sampleIndex >= _signal.size() - signalSize4Period(_periods[minPeriodIndex-1]))
            {
                minPeriodIndex--;
            }

            real x = xStart + sampleIndex*_signalSampleStep;
            real v = _signal[sampleIndex] - _coveredSignal[sampleIndex];

            for(std::size_t periodIndex(minPeriodIndex); periodIndex<_periods.size(); ++periodIndex)
            {
                real period = _periods[periodIndex];

                //real dp = x*g_2pi/period;
                //points[periodIndex]._v += complex(v).rotate(dp);

                real wx = 1.0 - (xStop - x)/period/_ppw;
                points[periodIndex].accumuleConv(v, period, x, wx);

                amounts[periodIndex]++;
            }
        }

        for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
        {
            std::size_t amount = amounts[periodIndex];
            if(amount)
            {
                //points[periodIndex]._v /= amount;
                points[periodIndex].finalizeConv(amount);
            }
        }

        int k = 220;
    }

    real Solver::applyEchoPart(TVPoint &echoPoints)
    {
//        real res = 0;
//        for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
//        {
//            //_points[periodIndex]._v += points[periodIndex]._v * part;
//            _points[periodIndex].add(points[periodIndex], part);
//        }

        real wt = 0;
        real wc = 0;
        real ws = 0;
        real wd = 0;
        TVReal signalFromEcho(_signal.size());

        real xStop = _signalSamplesPushed*_signalSampleStep;
        real xStart = xStop - _signal.size()*_signalSampleStep;

        std::size_t minPeriodIndex = _periods.size();

        real maxPointA = 0;
        std::size_t maxPointPeriodIndex=0;

        for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
        {
            if(maxPointA < echoPoints[periodIndex].maxA())
            {
                maxPointA = echoPoints[periodIndex].maxA();
                maxPointPeriodIndex = periodIndex;
            }

        }


        for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
        {
            while(minPeriodIndex>0 && sampleIndex >= _signal.size() - signalSize4Period(_periods[minPeriodIndex-1]))
            {
                minPeriodIndex--;
            }


            real x = xStart + sampleIndex*_signalSampleStep;
            real &v = signalFromEcho[sampleIndex];
            v = 0;

            //for(std::size_t periodIndex(minPeriodIndex); periodIndex<_periods.size(); ++periodIndex)
            if(maxPointPeriodIndex >= minPeriodIndex)
            {
                real period = _periods[maxPointPeriodIndex];

                //real dp = x*g_2pi/period;
                //v += _points[periodIndex]._v.rotate(-dp).re();

                real wx = 1.0 - (xStop - x)/period/_ppw;
                v += echoPoints[maxPointPeriodIndex].getMax().eval(period, x, wx)
                     //* (period)//это вес, для того чтобы усилить низкие частоты
                     ;
            }


//            wt += sqr(_signal[sampleIndex]*wAmount_);
//            wc += sqr(_coveredSignal[sampleIndex]*wAmount_);
//            ws += sqr((_signal[sampleIndex] - _coveredSignal[sampleIndex])*wAmount_);
//            wd += sqr(signalFromEcho[sampleIndex]*wAmount_);

            //wt += sqr(_signal[sampleIndex]);
            //wc += sqr(_coveredSignal[sampleIndex]*wAmount_);
            ws += sqr((_signal[sampleIndex] - _coveredSignal[sampleIndex]));
            wd += sqr(signalFromEcho[sampleIndex]);
        }

//        wt = sqrt(wt/wAmount);
//        wc = sqrt(wc/wAmount);
//        ws = sqrt(ws/wAmount);
//        wd = sqrt(wd/wAmount);

        //wt = sqrt(wt)/wAmount;
        //wc = sqrt(wc)/wAmount;
        ws = sqrt(ws);
        wd = sqrt(wd);


        ///////////////////////////////////////////////////////////////

        real part = 1.0;//(ws/wd);
        //real part2 = 0.0045;
        //part = std::min(part, real(0.023));
        //part = 0.33;


        //for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
        {
            _points[maxPointPeriodIndex].add(echoPoints[maxPointPeriodIndex].getMax(), part);
        }

        for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
        {
            _coveredSignal[sampleIndex] += signalFromEcho[sampleIndex]*part;
        }


        std::cout<<"part: "<<part<<", wd:"<<wd<<", ws:"<<ws<<", wt:"<<wt<<", wc:"<<wc<<std::endl;
        return part;
    }

}}
