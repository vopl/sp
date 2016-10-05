#include <iostream>
#include <eigen3/Eigen/SVD>

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




    PeriodGrid echoPeriods(1.0/0.2, 1.0/20000, 20000, PeriodGridType::frequencyLog);
    cerr
          <<"efmin: "<<(1.0/echoPeriods.grid().back())
          <<", efmax: "<<(1.0/echoPeriods.grid().front())
          <<", efcount: "<<echoPeriods.grid().size()
          <<", efstep: "<<(echoPeriods.grid()[1]/echoPeriods.grid()[0])
          <<endl;

    PeriodGrid spectrPeriods = PeriodGrid(1.0/0.2/100, 1.0/20000, 300, PeriodGridType::frequencyLog);
    cerr
            <<"sfmin: "<<(1.0/spectrPeriods.grid().back())
            <<", sfmax: "<<(1.0/spectrPeriods.grid().front())
            <<", sfcount: "<<spectrPeriods.grid().size()
            <<", sfstep: "<<(spectrPeriods.grid()[1]/spectrPeriods.grid()[0])
            <<endl;

    //exit(0);





#define POW 1.0

    size_t splp = 100;
    size_t cpo = 0;


    KernelTabled kt(POW, splp, cpo);

    using namespace Eigen;
    MatrixXd m = MatrixXd::Zero(echoPeriods.grid().size()*2, spectrPeriods.grid().size()*2);



#if 0

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    for(size_t i(0); i<echoPeriods.grid().size(); ++i)
    {
        for(size_t k(0); k<spectrPeriods.grid().size(); ++k)
        {
            sp::real t = echoPeriods.grid()[i]/spectrPeriods.grid()[k];
            sp::real rr, ri, ir, ii;
            kt.evalKernel(t, rr, ri, ir, ii);

            m(i*2+0, k*2+0) = rr;
            m(i*2+0, k*2+1) = ri;
            m(i*2+1, k*2+0) = ir;
            m(i*2+1, k*2+1) = ii;

        }
    }
    write_binary("m", m);

    if(0)
    {
        cout<<m<<endl;
        exit(0);
    }


    //cout << "Here is the matrix m:" << endl << m << endl;
    JacobiSVD<MatrixXd, ColPivHouseholderQRPreconditioner> svd(m, ComputeThinU | ComputeThinV);
    cout << "Its singular values are:" << endl << svd.singularValues() << endl;
//    exit(0);
//    cout << "Its left singular vectors are the columns of the thin U matrix:" << endl << svd.matrixU() << endl;
//    cout << "Its right singular vectors are the columns of the thin V matrix:" << endl << svd.matrixV() << endl;


    double  pinvtoler=1.e-15; // choose your tolerance wisely!
    auto singularValues_inv=svd.singularValues();
    for( long i=0; i<svd.cols(); ++i)
    {
       if( singularValues_inv(i) > pinvtoler)
       {
          singularValues_inv(i)=1.0/singularValues_inv(i);
       }
    }
    MatrixXd pinvmat = (svd.matrixV()*singularValues_inv.asDiagonal()*svd.matrixU().transpose());

    write_binary("inv", pinvmat);
    if(0)
    {
        cout<<pinvmat<<endl;
        exit(0);
    }
    exit(0);

#else
    MatrixXd pinvmat;
    read_binary("inv", pinvmat);

#endif



    TVComplex echo(echoPeriods.grid().size());

    for(size_t i(0); i<echoPeriods.grid().size(); ++i)
    {
        //for(size_t k(10); k<spectrPeriods.grid().size(); k+=10)
        size_t k(spectrPeriods.grid().size()/2);
        {
            echo[i] += kt.eval(
                echoPeriods.grid()[i],
               (spectrPeriods.grid()[k]+spectrPeriods.grid()[k+1])/2,
                sp::complex(1,-1));
        }
    }

    TVComplex spectr(spectrPeriods.grid().size());

    for(size_t i(0); i<echoPeriods.grid().size(); ++i)
    {
        sp::complex ev = echo[i];

        for(size_t k(0); k<spectrPeriods.grid().size(); ++k)
        {
            sp::real rr,ri,ir,ii;

            rr = pinvmat(k*2+0, i*2+0);
            ri = pinvmat(k*2+0, i*2+1);

            ir = pinvmat(k*2+1, i*2+0);
            ii = pinvmat(k*2+1, i*2+1);

            sp::real r1 = ev.re()*rr;
            sp::real r2 = ev.im()*ri;

            sp::real i1 = ev.re()*ir;
            sp::real i2 = ev.im()*ii;

            sp::complex sv(
                        +ev.re()*rr +ev.im()*ri,
                        -ev.re()*ir -ev.im()*ii);

            spectr[k] += sv;
        }
    }

    for(size_t k(0); k<spectrPeriods.grid().size(); ++k)
    {
        cout<<spectr[k].re()<<" "<<spectr[k].im()<<endl;
    }



#endif

}
