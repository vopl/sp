#include "sp/conv/kernelTabled.hpp"
#include "sp/math.hpp"

#include <iostream>
#include "Eigen/SVD"
#include "Eigen/Dense"



namespace sp { namespace conv
{
    namespace
    {
        using real4deconv = double;

        //////////////////////////////////////////////////////////////////////////
        struct Params
        {
            //сетка периода, должна быть задана всегда
            //отклика - длина n/2
            const real      *_et;

            //спектра - длина m/2
            const real      *_st;

            //значения отклика, если задано
            //длина n/2
            const complex   *_ev;

            KernelTabled    *_kernelTabled;
        };

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        void evalFunc(real4deconv *p, real4deconv *hx, int m, int n, void *vparams)
        {
            Params *params = reinterpret_cast<Params *>(vparams);

            for(int i(0); i<n/2; i++)
            {
                real et = params->_et[i];
                Summator<real> re;
                Summator<real> im;

                for(int j(0); j<m/2; j++)
                {
                    real st = params->_st[j];

                    real rr, ri, ir, ii;
                    params->_kernelTabled->evalKernel(et/st, rr, ri, ir, ii);

                    //assert(std::isfinite(rr) && std::isfinite(ri));
                    //assert(std::isfinite(ir) && std::isfinite(ii));

                    real sr = p[j*2+0];
                    real si = p[j*2+1];

                    re += sr*rr - si*ri;
                    im += sr*ir - si*ii;
                }

                //assert(std::isfinite(re) && std::isfinite(im));

                re += -params->_ev[i].re();
                im += -params->_ev[i].im();

                hx[i*2+0] = real4deconv(re.v());
                hx[i*2+1] = real4deconv(im.v());
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void evalJaco(real4deconv *jx, int m, int n, void *vparams)
        {
            Params *params = reinterpret_cast<Params *>(vparams);

            for(int i(0); i<n/2; i++)
            {
                real et = params->_et[i];

                for(int j(0); j<m/2; j++)
                {
                    real st = params->_st[j];

                    real rr,  ri,  ir,  ii;
                    params->_kernelTabled->evalKernel(et/st, rr, ri, ir, ii);

                    jx[(i*2+0)+(j*2+0)*n] = real4deconv(rr);
                    jx[(i*2+0)+(j*2+1)*n] = real4deconv(-ri);

                    jx[(i*2+1)+(j*2+0)*n] = real4deconv(ir);
                    jx[(i*2+1)+(j*2+1)*n] = real4deconv(-ii);
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void KernelTabled::deconvolve2(
            size_t esize, const real *et, const complex *ev, //отклик
            size_t ssize, const real *st,       complex *sv, //спектр
            size_t &iters, //макс итераций
            real initialMu,
            real &error0,
            real &error1)
    {
        Params params;
        params._et = et;
        params._ev = ev;
        params._st = st;
        params._kernelTabled = this;

        using Matrix = Eigen::MatrixXd;

        //build jacTJac
        Matrix jac(esize*2, ssize*2);
        evalJaco(jac.data(), ssize*2, esize*2, &params);
        Matrix jacTJac = jac.transpose() * jac;

        //build jacTJacInv
        auto AX_EQ_B_solver = jacTJac.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);

        Matrix jacTDiagE(ssize*2, 1);

        //for(std::size_t iter(0); iter<iters; ++iter)
        {

            //fill jacTe
            {
                std::size_t i=0;
                for(std::size_t l=0; l<ssize*2; ++l)
                {
                    jacTDiagE(l,0) = jac(i, l) * (&ev->re())[i];
                }

                for(; i<esize*2; ++i)
                {
                    for(std::size_t l=0; l<ssize*2; ++l)
                    {
                        jacTDiagE(l,0) += jac(i, l) * (&ev->re())[i];
                    }
                }
            }

            //solve AX_EQ_B, dp=x
            Matrix Dp = AX_EQ_B_solver.solve(jacTDiagE);

            //update p
            for(std::size_t i=0; i<ssize*2; ++i)
            {
                (&sv->re())[i] += Dp(i, 0);
            }
        }
    }

}}
