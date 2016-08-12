#ifndef _hel_responseModel_h_
#define _hel_responseModel_h_

#include "sp/types.hpp"
#include "sp/complex.hpp"
#include <cstring>

namespace sp
{
    using Complex = complex;

    //////////////////////////////////////////////////////////////////////////
    class ResponseModelLegacy
    {
    public:
        struct VDS
        {
            real _rr;
            real _ri;
            real _ir;
            real _ii;
            real _drr;
            real _dri;
            real _dir;
            real _dii;
        };
        typedef std::vector<VDS> TVVDS;
        //////////////////////////////////////////////////////////////////////////
        //таблицы для t<1
        real    _l1_tStep;
        TVReal    _l1_t;//ti=_le1_t[i]
        TVVDS    _l1_vds;

        //таблицы для t>=1
        real    _ge1_loglogtStep;
        TVReal    _ge1_loglogt;//ti=exp(exp(_g1_loglogt)-1);
        TVVDS    _ge1_vds;


        size_t    _pow;

    public:
        ResponseModelLegacy();
        virtual ~ResponseModelLegacy(){}

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

        //////////////////////////////////////////////////////////////////////////
        void evalVRaw_tabled(
            real logt, 
            real &rr, real &ri, real &ir, real &ii);
        void evalVDRaw_tabled(
            real logt, 
            real &rr, real &ri, real &ir, real &ii,
            real &drr, real &dri, real &dir, real &dii);

    public:
        Complex eval(real t, real st, const Complex &sv);
        Complex eval_tabled(real t, real st, const Complex &sv);

    public:
        //////////////////////////////////////////////////////////////////////////
        struct SFuncOptParams
        {
            //сетка логарифма периода, должна быть задана всегда
            //длина n
            const real        *_elogt;

            //значения отклика, если задано, то будет вычтено из вычесленного (можно не задавать в dlrvmar_der)
            //длина n
            const Complex    *_ev;

            //логарифм значения периода спектра. Если задано то период спектра не оптимизируется
            //длина m
            const real        *_slogt;

            //собственно, вычислялка ядра
            ResponseModelLegacy    *_rm;
        };
        //аргумент s как набор триплетов количеством m штук
        //p(j*3+0)=st
        //p(j*3+1)=sr
        //p(j*3+2)=si
        static void evalFuncOpt(double *p, double *hx, int m, int n, void *_SFuncOptParams);
        static void jacoFuncOpt(double *p, double *jx, int m, int n, void *_SFuncOptParams);

        int solve_tv(
            size_t eSize, const real *elogt, const Complex *ev,//отклик
            size_t sSize,       real *slogt,       Complex *sv,//спектр
            size_t itMax,//макс итераций
            TVReal &work);
            
        int solve_v(
            size_t eSize, const real *elogt, const Complex *ev,//отклик
            size_t sSize, const real *slogt,       Complex *sv,//спектр
            size_t itMax,//макс итераций
            TVReal &work);

        void dump(const char *fname);
    };

}

#endif
