#ifndef _hel_responseModel_h_
#define _hel_responseModel_h_

#include "math.hpp"
#include <cstring>


namespace sp
{
    //////////////////////////////////////////////////////////////////////////
    class ResponseModel
    {
    public:
        ResponseModel();
        virtual ~ResponseModel(){}

        void setup(
            real maxT,
            size_t stepsT_l1,
            size_t stepsT_ge1,
            size_t pow);

        void evalVRaw(real t, real &rr, real &ri, real &ir, real &ii);
//        void evalVRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii);

        void evalVDRaw(
            real tm, real t, real tp,
            real & rr, real & ri, real & ir, real & ii,
            real &drr, real &dri, real &dir, real &dii);
//        void evalDRaw(const TVReal &t, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii);
//        void evalVDRaw(const TVReal &t, TVReal &rr, TVReal &ri, TVReal &ir, TVReal &ii, TVReal &drr, TVReal &dri, TVReal &dir, TVReal &dii);

    public:
        complex eval(real t, real st, const complex &sv);

    public:
        //////////////////////////////////////////////////////////////////////////
        struct SFuncOptParams
        {
            //сетка логарифма периода, должна быть задана всегда
            //длина n
            const real        *_logt;

            //значения отклика, если задано, то будет вычтено из вычесленного (можно не задавать в dlrvmar_der)
            //длина n
            const complex    *_ev;

            //собственно, вычислялка ядра
            ResponseModel    *_rm;
        };
        //аргумент s как набор триплетов количеством m штук
        //p(j*3+0)=st
        //p(j*3+1)=sr
        //p(j*3+2)=si
        static void evalFuncOpt(double *p, double *hx, int m, int n, void *_SFuncOptParams);
        static void jacoFuncOpt(double *p, double *jx, int m, int n, void *_SFuncOptParams);

        int solve_v(real mu,
            size_t size, const real *logt, const complex *ev,//отклик
            complex *sv,//спектр
            size_t itMax,//макс итераций
            TVReal &work);
    };
}

#endif
