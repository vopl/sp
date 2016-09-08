#include <iostream>
#include <eigen3/Eigen/SVD>

#include "sp/config.hpp"
#include "sp/math.hpp"
#include "sp/kernel.hpp"
#include "sp/kernelTabled.hpp"
#include "sp/convolver.hpp"
#include "sp/periodGrid.hpp"
#include "sp/signalConvolver.hpp"

using namespace std;












void inv()
{
    cout.precision(20);
    cout.setf(std::ios::scientific);








    sp::PeriodGrid echoPeriods(sp::g_periodMin, sp::g_periodMax, sp::g_periodSteps, sp::PeriodGridType::frequencyLog);
    std::cerr
          <<"efmin: "<<(1.0/echoPeriods.grid().back())
          <<", efmax: "<<(1.0/echoPeriods.grid().front())
          <<", efcount: "<<echoPeriods.grid().size()
          <<", efstep: "<<(echoPeriods.grid()[1]/echoPeriods.grid()[0])
          <<std::endl;

    std::size_t sps = std::size_t(echoPeriods.grid().size()*0.78+0.5);
    sp::PeriodGrid spectrPeriods = sp::PeriodGrid(echoPeriods.grid()[1], echoPeriods.grid()[sps+1], sps, sp::PeriodGridType::frequencyLog);
    std::cerr
            <<"sfmin: "<<(1.0/spectrPeriods.grid().back())
            <<", sfmax: "<<(1.0/spectrPeriods.grid().front())
            <<", sfcount: "<<spectrPeriods.grid().size()
            <<", sfstep: "<<(spectrPeriods.grid()[1]/spectrPeriods.grid()[0])
            <<std::endl;

    //exit(0);





#define POW 10.0

    std::size_t splp = 200;
    std::size_t cpo = 11;


    sp::KernelTabled kt(POW, splp, cpo);

    using namespace Eigen;
    MatrixXd m = MatrixXd::Zero(echoPeriods.grid().size()*2, spectrPeriods.grid().size()*2);




    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sp::real rr, ri, ir, ii;
    for(size_t i(0); i<echoPeriods.grid().size(); ++i)
    {
        for(std::size_t k(0); k<spectrPeriods.grid().size(); ++k)
        {
            sp::real t = echoPeriods.grid()[i]/spectrPeriods.grid()[k];
            kt.evalKernel(t, rr, ri, ir, ii);

            m(i*2+0, k*2+0) = rr;
            m(i*2+0, k*2+1) = ri;
            m(i*2+1, k*2+0) = ir;
            m(i*2+1, k*2+1) = ii;

        }
    }

    if(0)
    {
        cout<<m<<endl;
        exit(0);
    }

    //cout << "Here is the matrix m:" << endl << m << endl;
    JacobiSVD<MatrixXd, ColPivHouseholderQRPreconditioner> svd(m, ComputeThinU | ComputeThinV);
    cout << "Its singular values are:" << endl << svd.singularValues() << endl;
    exit(0);
//    cout << "Its left singular vectors are the columns of the thin U matrix:" << endl << svd.matrixU() << endl;
//    cout << "Its right singular vectors are the columns of the thin V matrix:" << endl << svd.matrixV() << endl;


    double  pinvtoler=1.e-10; // choose your tolerance wisely!
    auto singularValues_inv=svd.singularValues();
    for ( long i=0; i<svd.cols(); ++i)
    {
       if ( singularValues_inv(i) > pinvtoler)
       {
          singularValues_inv(i)=1.0/singularValues_inv(i);
       }
    }
    MatrixXd pinvmat = (svd.matrixV()*singularValues_inv.asDiagonal()*svd.matrixU().transpose());

    if(0)
    {
        cout<<pinvmat<<endl;
        exit(0);
    }


    sp::TVComplex echo(echoPeriods.grid().size());

    for(size_t i(0); i<echoPeriods.grid().size(); ++i)
    {
        //for(size_t k(10); k<spectrPeriods.grid().size(); k+=10)
        size_t k(spectrPeriods.grid().size()/2);
        {
            echo[i] += kt.eval(
                echoPeriods.grid()[i],
               (spectrPeriods.grid()[k]),//+spectrPeriods.grid()[k])/2,
                sp::complex(1,0));
        }
    }

    sp::TVComplex spectr(spectrPeriods.grid().size());

    for(size_t i(0); i<echoPeriods.grid().size(); ++i)
    {
        sp::complex ev = echo[i];

        for(std::size_t k(0); k<spectrPeriods.grid().size(); ++k)
        {
            sp::real rr,ri,ir,ii;

            rr = pinvmat(k*2+0, i*2+0);
            ri = pinvmat(k*2+1, i*2+0);
            ir = pinvmat(k*2+0, i*2+1);
            ii = pinvmat(k*2+1, i*2+1);

            sp::complex sv(
                        ev.re()*rr - ev.im()*ri,
                        ev.re()*ir + ev.im()*ii);

            spectr[k] += sv;
        }
    }

    for(std::size_t k(0); k<spectrPeriods.grid().size(); ++k)
    {
        cout<<spectr[k].re()<<" "<<spectr[k].im()<<endl;
    }





}
