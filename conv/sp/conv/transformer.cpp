#include "sp/conv/transformer.hpp"
#include "sp/conv/basis.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

#include "Eigen/SVD"
#include "Eigen/Dense"

#include <QPainter>

namespace sp { namespace conv
{
    Transformer::Transformer()
    {
    }

    Transformer::~Transformer()
    {

    }

    void Transformer::setup(
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

    void Transformer::pushSignal(const real *signalBucketSamples)
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











    void Transformer::update()
    {
//        real xStop = _signalSampleStep;
//        real xStart = xStop -(_signal.size()*_signalSampleStep);

//        std::size_t minPeriodIndex = _periods.size();

        Eigen::Map<const Vector> signal(&_signal[0], _signal.size());
        Eigen::Map<RowVector> points(_points[0].data(), _points.size()*Point::dataSize());

        if(!_linearSolver && !d2Load())
        {
            Matrix kern;
            kern.setZero(signal.size(), points.size());

            std::cout<<"fill "<<kern.rows()<<" x "<<kern.cols()<<std::endl;
//            for(std::size_t sampleIndex(0); sampleIndex<kern.rows(); ++sampleIndex)
//            {
//                while(minPeriodIndex>0 && sampleIndex >= kern.rows() - signalSize4Period(_periods[minPeriodIndex-1]))
//                {
//                    minPeriodIndex--;
//                }

//                real x = xStart + sampleIndex*_signalSampleStep;

//                for(std::size_t periodIndex(minPeriodIndex); periodIndex<_periods.size(); ++periodIndex)
//                {
//                    real period = _periods[periodIndex];

//                    real wx = 1.0 - (xStop - x)/period/_ppw;

//                    for(std::size_t bidx(0); bidx<Point::dataSize(); ++bidx)
//                    {
//                        Point p;
//                        p.data()[bidx] = 1.0;

//                        real v = _basis.eval(p, period, x, wx);

//                        kern(sampleIndex, (periodIndex)*Point::dataSize()+bidx) = v;
//                    }
//                }
//            }

            real xCenter = _signalSampleStep*_signal.size() / 2;

            for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
            {
                real period = _periods[periodIndex];

                real xStart = xCenter - period*_ppw/2;
                real xStop = xCenter + period*_ppw/2;

                std::size_t sampleIndexStart = xStart / _signalSampleStep-1.5;
                std::size_t sampleIndexStop = xStop / _signalSampleStep+1.5;

                if(sampleIndexStart >= kern.rows())
                {
                    sampleIndexStart = 0;
                }
                if(sampleIndexStop >= kern.rows())
                {
                    sampleIndexStop = kern.rows()-1;
                }

                for(std::size_t sampleIndex(sampleIndexStart); sampleIndex<sampleIndexStop; ++sampleIndex)
                {
                    real x = sampleIndex*_signalSampleStep;
                    real wx = (x-xStart) / (period*_ppw);
                    if(wx<0 || wx>1)
                    {
                        continue;
                    }

                    for(std::size_t bidx(0); bidx<Point::dataSize(); ++bidx)
                    {
                        Point p;
                        p.data()[bidx] = 1.0;

                        real v = _basis.eval(p, period, x-xCenter, wx);

                        kern(sampleIndex, (periodIndex)*Point::dataSize()+bidx) = v;
                    }
                }
            }










//            //Ax=b lsq
//            std::cout<<"transpose"<<std::endl;
//            _linearSolverKernT = kern.transpose();

//            std::cout<<"mult"<<std::endl;
//            Matrix kernTKern = _linearSolverKernT * kern;

//            std::cout<<"solver"<<std::endl;
//            _linearSolver.reset(new LinearSolver(kernTKern));






            //Ax=b
            std::cout<<"solver"<<std::endl;
            _linearSolver.reset(new LinearSolver(kern));

            d2Save();
        }


        std::cout<<"solve"<<std::endl;
        points.noalias() = _linearSolver->solve(/*_linearSolverKernT **/ signal);

        //points = Matrix::Constant(1, _points.size()*Point::dataSize(), 1.0);



        {
            std::cout<<"dump"<<std::endl;
            std::fill(_coveredSignal.begin(), _coveredSignal.end(), real(0));

            real xCenter = _signalSampleStep*_signal.size() / 2;

            for(std::size_t periodIndex(0); periodIndex<_periods.size(); ++periodIndex)
            {
                real period = _periods[periodIndex];

                real xStart = xCenter - period*_ppw/2;
                real xStop = xCenter + period*_ppw/2;

                std::size_t sampleIndexStart = xStart / _signalSampleStep-1.5;
                std::size_t sampleIndexStop = xStop / _signalSampleStep+1.5;

                if(sampleIndexStart >= _signal.size())
                {
                    sampleIndexStart = 0;
                }
                if(sampleIndexStop >= _signal.size())
                {
                    sampleIndexStop = _signal.size()-1;
                }

                for(std::size_t sampleIndex(sampleIndexStart); sampleIndex<sampleIndexStop; ++sampleIndex)
                {
                    real x = sampleIndex*_signalSampleStep;
                    real wx = (x-xStart) / (period*_ppw);
                    if(wx<0 || wx>1)
                    {
                        continue;
                    }

                    real v = _basis.eval(_points[periodIndex], period, x-xCenter, wx);
                    _coveredSignal[sampleIndex] += v;
                }
            }

        }

        {
            std::ofstream out("s2");

            for(std::size_t sampleIndex(0); sampleIndex<_signal.size(); ++sampleIndex)
            {
                out<<_signal[sampleIndex]<<", "<<_coveredSignal[sampleIndex]<<std::endl;
            }
        }

    }



    const TVPoint &Transformer::points()
    {
        return _points;
    }

    std::size_t Transformer::signalSize4Period(real period)
    {
        return std::size_t(period * _ppw / _signalSampleStep+0.5);
    }



    //////////////////////////////////////////////////////////////////////////
    std::string Transformer::d2FileName()
    {
        return "linearSolver.bin";
//        char tmp[4096];
//        std::size_t h = 0;

//        hash_combine(h, sizeof(real));
//        hash_combine(h, hash<D2Key>()(_d2Key));
//        hash_combine(h, LinearSolver::_solverId);

//        sprintf(tmp, "kt_d2_%zx.bin", h);
//        return tmp;

    }

    //////////////////////////////////////////////////////////////////////////
    bool Transformer::d2Load()
    {
        std::cerr<<"load d2: "<<d2FileName()<<"...";

        std::ifstream in(d2FileName().c_str(), std::ios::in|std::ios::binary);
        if(!in)
        {
            std::cerr<<"no file"<<std::endl;
            return false;
        }

        std::size_t rows, cols;

        in.read((char *)&rows, sizeof(rows));
        in.read((char *)&cols, sizeof(cols));

        _linearSolverKernT.resize(rows, cols);
        in.read( (char *) _linearSolverKernT.data(), _linearSolverKernT.rows()*_linearSolverKernT.cols()*sizeof(typename Matrix::Scalar) );
        if(!in)
        {
            std::cerr<<"bad file"<<std::endl;
            return false;
        }

        _linearSolver.reset(new LinearSolver());
        _linearSolver->load(in);

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
    bool Transformer::d2Save()
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

        std::size_t rows, cols;

        rows = _linearSolverKernT.rows();
        out.write((char *)&rows, sizeof(rows));

        cols = _linearSolverKernT.cols();
        out.write((char *)&cols, sizeof(cols));

        out.write((char*) _linearSolverKernT.data(), _linearSolverKernT.rows()*_linearSolverKernT.cols()*sizeof(typename Matrix::Scalar) );

        _linearSolver->save(out);

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
}}
