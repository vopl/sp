#include <iostream>
#include "Eigen/SVD"

#include <fstream>

#include "sp/math.hpp"
#include "sp/conv/kernelTabled.hpp"
#include "sp/conv/periodGrid.hpp"
#include "sp/conv/signalConvolver.hpp"

using namespace std;
using namespace sp;
using namespace sp::conv;






namespace
{
    template<class Matrix>
    void write_binary(const char* filename, const Matrix& matrix){
        std::ofstream out(filename,ios::out | ios::binary | ios::trunc);
        typename Matrix::Index rows=matrix.rows(), cols=matrix.cols();
        out.write((char*) (&rows), sizeof(typename Matrix::Index));
        out.write((char*) (&cols), sizeof(typename Matrix::Index));
        out.write((char*) matrix.data(), rows*cols*sizeof(typename Matrix::Scalar) );
        out.close();
    }

    template<class Matrix>
    void read_binary(const char* filename, Matrix& matrix){
        std::ifstream in(filename,ios::in | std::ios::binary);
        typename Matrix::Index rows=0, cols=0;
        in.read((char*) (&rows),sizeof(typename Matrix::Index));
        in.read((char*) (&cols),sizeof(typename Matrix::Index));
        matrix.resize(rows, cols);
        in.read( (char *) matrix.data() , rows*cols*sizeof(typename Matrix::Scalar) );
        in.close();
    }
} // Eigen::





void inv()
{
    cout.precision(20);
    cout.setf(ios::scientific);

/*
 *     попробовать уменьшить шаг спектра

    отсмотреть r1, r2, i1, i2
*/

#if 1


    sp::real minT = 1.0/20000;
    sp::real maxT = 1.0/0.2;


    PeriodGrid echoPeriodsGrid(
        minT,
        maxT,
        20000,
        PeriodGridType::frequencyLog);

    TVReal echoPeriods = echoPeriodsGrid.grid();

    cerr
        <<"efmin: "<<(sp::real(1)/echoPeriods.back())
        <<", efmax: "<<(sp::real(1)/echoPeriods.front())
        <<", efcount: "<<echoPeriods.size()
        <<", efstep: "<<echoPeriods[1]/echoPeriods[0]
        <<endl;

    TVReal spectrPeriods;
    {

        TVReal::const_iterator sfBegin = std::lower_bound(echoPeriods.begin(), echoPeriods.end(), echoPeriods.front());
        TVReal::const_iterator sfEnd = std::lower_bound(echoPeriods.begin(), echoPeriods.end(), echoPeriods.back()/100);


        std::size_t sfCountMult = 10;

        spectrPeriods.clear();
        spectrPeriods.resize(std::size_t(sp::real(sfEnd-sfBegin)/sfCountMult+0.5));
        auto sfIter = sfBegin;
        for(std::size_t idx(0); idx<spectrPeriods.size(); ++idx)
        {
            spectrPeriods[idx] = *sfIter;
            sfIter += std::ptrdiff_t(sfCountMult);
        }

    }

    cerr
        <<"sfmin: "<<(sp::real(1)/spectrPeriods.back())
        <<", sfmax: "<<(sp::real(1)/spectrPeriods.front())
        <<", sfcount: "<<spectrPeriods.size()
        <<", sfstep: "<<spectrPeriods[1]/spectrPeriods[0]
        <<endl;

    //exit(0);





#define POW 1.0

    size_t splp = 1000;

    KernelTabled kt(POW, splp);

    using namespace Eigen;


    using EMatrix = MatrixXd;

#if 1

    EMatrix m = EMatrix::Zero(echoPeriods.size()*2, spectrPeriods.size()*2);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    for(size_t i(0); i<echoPeriods.size(); ++i)
    {
        for(size_t k(0); k<spectrPeriods.size(); ++k)
        {
            sp::real t = echoPeriods[i]/spectrPeriods[k];
            sp::real rr, ri, ir, ii;
            kt.evalKernel(t, rr, ri, ir, ii);

            m(i*2+0, k*2+0) = rr;
            m(i*2+0, k*2+1) = ri;
            m(i*2+1, k*2+0) = ir;
            m(i*2+1, k*2+1) = ii;

        }
    }
    kt.flush();
    write_binary("m", m);

    if(0)
    {
        cout<<m<<endl;
        exit(0);
    }


    //cout << "Here is the matrix m:" << endl << m << endl;
    JacobiSVD<EMatrix, ColPivHouseholderQRPreconditioner> svd(m, ComputeThinU | ComputeThinV);
    //cout << "Its singular values are:" << endl << svd.singularValues() << endl;
//    exit(0);
//    cout << "Its left singular vectors are the columns of the thin U matrix:" << endl << svd.matrixU() << endl;
//    cout << "Its right singular vectors are the columns of the thin V matrix:" << endl << svd.matrixV() << endl;


    double  pinvtoler=1.e-20; // choose your tolerance wisely!
    auto singularValues_inv=svd.singularValues();
    for( long i=0; i<svd.cols(); ++i)
    {
       if( /*i < 50*/ singularValues_inv(i) > pinvtoler)
       {
          singularValues_inv(i)=1.0/singularValues_inv(i);
       }
       else
       {
           singularValues_inv(i) = 0;
       }
    }

    write_binary("svdMatrixV", svd.matrixV());
    write_binary("svdValues", singularValues_inv);
    write_binary("svdMatrixU", svd.matrixU());

    EMatrix pinvmat = (svd.matrixV()*singularValues_inv.asDiagonal()*svd.matrixU().transpose());

    write_binary("inv", pinvmat);
    if(0)
    {
        cout<<pinvmat<<endl;
        exit(0);
    }
    //exit(0);

#else
    EMatrix pinvmat;
    read_binary("inv", pinvmat);

#endif



    TVComplex echo(echoPeriods.size());

    for(size_t i(0); i<echoPeriods.size(); ++i)
    {
        for(size_t k(20); k<spectrPeriods.size(); k+=20)
        //size_t k(spectrPeriods.size()/2);
        {
            echo[i] += kt.eval(
                echoPeriods[i],
               (spectrPeriods[k]+spectrPeriods[k+1])/2,
                sp::complex(1,0));
        }
    }
    kt.flush();

    TVComplex spectr(spectrPeriods.size());

    //fetch spectr tail
    for(size_t i(0); i<echoPeriods.size(); ++i)
    {
        sp::complex ev = echo[i];

        for(size_t k(0); k<spectrPeriods.size(); ++k)
        {
            sp::real rr,ri,ir,ii;

            rr = pinvmat(k*2+0, i*2+0);
            ri = pinvmat(k*2+0, i*2+1);

            ir = pinvmat(k*2+1, i*2+0);
            ii = pinvmat(k*2+1, i*2+1);

            sp::complex sv(
                        +ev.re()*rr +ev.im()*ri,
                        -ev.re()*ir -ev.im()*ii);

            spectr[k] += sv;
        }
    }

    //dump spectr
    {
        std::ofstream out("sp");
        for(size_t i(0); i<spectr.size(); ++i)
        {
            out<<spectr[i].re()<<" "<<spectr[i].im()<<endl;
        }
    }


#endif

}
