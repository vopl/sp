/*************************************************************************
Copyright (c) 2007-2008, Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses

>>> END OF LICENSE >>>
*************************************************************************/

#include <stdafx.h>
#include "lbfgs.h"

static const double ftol = 0.0001;
static const double xtol = 100*ap::machineepsilon;
static const double gtol = 0.9;
static const int maxfev = 20;
static const double stpmin = 1.0E-20;
static const double stpmax = 1.0E20;

static void mcsrch(const int& n,
     ap::real_1d_array& x,
     double& f,
     ap::real_1d_array& g,
     const ap::real_1d_array& s,
     double& stp,
     int& info,
     int& nfev,
     ap::real_1d_array& wa,
     lbfgsstate& state,
     int& stage);
static void mcstep(double& stx,
     double& fx,
     double& dx,
     double& sty,
     double& fy,
     double& dy,
     double& stp,
     const double& fp,
     const double& dp,
     bool& brackt,
     const double& stmin,
     const double& stmax,
     int& info);

/*************************************************************************
        LIMITED MEMORY BFGS METHOD FOR LARGE SCALE OPTIMIZATION

Подпрограмма минимизирует  функцию  N  аргументов  F(x)  с  использованием
квази-Ньютоновского метода (LBFGS-схема), оптимизированного по использованию
оперативной памяти.

Подпрограмма стоит аппроксимацию матрицы, обратной к  Гессиану  фунции,  с
использованием информации о M предыдущих шагах алгоритма  (вместо N),  что
позволяет  снизить  требуемый  объем оперативной памяти с величины порядка
N^2 до величины порядка O(N*M).

Данная подпрограмма разработана на основе описания, изложенного  в  статье
D.C. Liu и J. Nocedal (1989).

ОБРАТНАЯ КОММУНИКАЦИЯ

В ходе  своей  работы  подпрограмма  использует  обратную  коммуникацию  с
пользователем для вычисления значения функции и градиента. Типичная  схема
работы выглядит так:
1. Вызывается подпрограмма MinLBFGS с указанием параметров задачи (размера,
   критериев   остановки   и  т.д.).  Состояние  программы  сохраняется  в
   переданной  в  качестве  одного  из  полей структуры State, после  чего
   управление возвращается пользователю.
2. Вызывается  подпрограмма  MinLBFGSIteration.  Если подпрограмма вернула
   True, то алгоритм завершил работу. Решение задачи и отчет о работе могут
   быть получены при помощи подпрограммы MinLBFGSResults.
3. В  ином  случае  пользователь  вычисляет  значение функции и градиент в
   точке State.X (array [0..N-1]),  сохраняет их в  State.F  (вещественное
   поле)  и State.G (array [0..N-1]),  после  чего  вызывает  подпрограмму
   MinLBFGSIteration.

ВХОДНЫЕ ПАРАМЕТРЫ:
    N       -   Размерность задачи, N>1
    M       -   Число коррекций в BFGS-схеме обновления аппроксимации
                Гессиана. Рекомендуемое   значение:  3 <= M <= 7.  Меньшее
                значение не позволит добиться нормальной скорости сходимости,
                большее - не позволит получить заметный выигрыш в скорости
                сходимости, зато приведет к падению быстродействия.
                1<=M<=N.
    X       -   Начальное приближение к решению, array[0..N-1]
    EpsG    -   Положительное число, определяющее точность поиска минимума
                Подпрограмма прекращает  работу, если выполняется  условие
                ||G|| < EpsG, где ||.|| обозначает  Евклидову  норму,  G -
                градиент, X - текущее приближение к минимуму.
    EpsF    -   Положительное число, определяющее точность поиска минимума
                Подпрограмма прекращает работу, если на  k+1-ой   итерации
                выполняется условие
                |F(k+1)-F(k)| <= EpsF*max{|F(k)|, |F(k+1)|, 1}
    EpsX    -   Положительное число, определяющее точность поиска минимума
                Подпрограмма прекращает работу, если на  k+1-ой   итерации
                выполняется условие |X(k+1)-X(k)| <= EpsX
    MaxIts  -   Максимальное число итераций алгоритма.
                Если MaxIts=0, то число итераций не ограничено.
    Flags   -   дополнительные настройки алгоритма:
                * 0     нет дополнительных настроек
                * 1     не выделять память (используется при решении серии
                        задач с одними и теми же значениями N и M;  память
                        выделяется только для первой задачи, при повторных
                        вызовах можно ускорить быстродействие, отказавшись
                        от её выделения, если передавать в алгоритм  ранее
                        инициализированную структуру State). Если структура
                        State была инициализирована с использованием других
                        значений N/M, то использование этого  флага  может
                        привести к сбою в работе подпрограммы.

ВЫХОДНЫЕ ПАРАМЕТРЫ:
    State   -   структура, сохраняющая состояние алгоритма между повторными
                вызовами и использующаяся для обратной  коммуникации.

СМ. ТАКЖЕ
    подпрограмма MinLBFGSIteration, MinLBFGSResult

  -- ALGLIB --
     Copyright 14.11.2007 by Bochkanov Sergey
*************************************************************************/
void minlbfgs(const int& n,
     const int& m,
     ap::real_1d_array& x,
     const double& epsg,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     int flags,
     lbfgsstate& state)
{
    bool allocatemem;

    ap::ap_error::make_assertion(n>=1, "MinLBFGS: N too small!");
    ap::ap_error::make_assertion(m>=1, "MinLBFGS: M too small!");
    ap::ap_error::make_assertion(m<=n, "MinLBFGS: M too large!");
    ap::ap_error::make_assertion(epsg>=0, "MinLBFGS: negative EpsG!");
    ap::ap_error::make_assertion(epsf>=0, "MinLBFGS: negative EpsF!");
    ap::ap_error::make_assertion(epsx>=0, "MinLBFGS: negative EpsX!");
    ap::ap_error::make_assertion(maxits>=0, "MinLBFGS: negative MaxIts!");
    
    //
    // Initialize
    //
    state.n = n;
    state.m = m;
    state.epsg = epsg;
    state.epsf = epsf;
    state.epsx = epsx;
    state.maxits = maxits;
    state.flags = flags;
    allocatemem = flags%2==0;
    flags = flags/2;
    if( allocatemem )
    {
        state.rho.setbounds(0, m-1);
        state.theta.setbounds(0, m-1);
        state.y.setbounds(0, m-1, 0, n-1);
        state.s.setbounds(0, m-1, 0, n-1);
        state.d.setbounds(0, n-1);
        state.x.setbounds(0, n-1);
        state.g.setbounds(0, n-1);
        state.work.setbounds(0, n-1);
    }
    
    //
    // Initialize Rep structure
    //
    state.xupdated = false;
    
    //
    // Prepare first run
    //
    state.k = 0;
    ap::vmove(&state.x(0), &x(0), ap::vlen(0,n-1));
    state.rstate.ia.setbounds(0, 6);
    state.rstate.ra.setbounds(0, 4);
    state.rstate.stage = -1;
}


/*************************************************************************
Одна итерация L-BFGS алгоритма.

Вызывается после инициализации алгоритма подпрограммой MinLBFGS. Пример
использования - см. HTML-документацию.

Входные параметры:
    State   -   структура,   сохраняющая   состояние    алгоритма    между
                повторными  вызовами   и   использующаяся   для   обратной
                коммуникации. Эта структура должна  быть  инициализирована
                подпрограммой MinLBFGS.

Если подпрограмма возвращает False,  итеративный  процесс  завершен.
Если подпрограмма вернула True,  то  требуется  вычислить значение функции
State.F и значение  градиента  State.G[0..N-1]  в  точке  State.X[0..N-1],
после чего повторно вызвать подпрограмму.


  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
bool minlbfgsiteration(lbfgsstate& state)
{
    bool result;
    int n;
    int m;
    int maxits;
    double epsf;
    double epsg;
    double epsx;
    int i;
    int j;
    int ic;
    int mcinfo;
    double v;
    double vv;

    
    //
    // Reverse communication preparations
    // I know it looks ugly, but it works the same way
    // anywhere from C++ to Python.
    //
    // This code initializes locals by:
    // * random values determined during code
    //   generation - on first subroutine call
    // * values from previous call - on subsequent calls
    //
    if( state.rstate.stage>=0 )
    {
        n = state.rstate.ia(0);
        m = state.rstate.ia(1);
        maxits = state.rstate.ia(2);
        i = state.rstate.ia(3);
        j = state.rstate.ia(4);
        ic = state.rstate.ia(5);
        mcinfo = state.rstate.ia(6);
        epsf = state.rstate.ra(0);
        epsg = state.rstate.ra(1);
        epsx = state.rstate.ra(2);
        v = state.rstate.ra(3);
        vv = state.rstate.ra(4);
    }
    else
    {
        n = -983;
        m = -989;
        maxits = -834;
        i = 900;
        j = -287;
        ic = 364;
        mcinfo = 214;
        epsf = -338;
        epsg = -686;
        epsx = 912;
        v = 585;
        vv = 497;
    }
    if( state.rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state.rstate.stage==1 )
    {
        goto lbl_1;
    }
    
    //
    // Routine body
    //
    
    //
    // Unload frequently used variables from State structure
    // (just for typing convinience)
    //
    n = state.n;
    m = state.m;
    epsg = state.epsg;
    epsf = state.epsf;
    epsx = state.epsx;
    maxits = state.maxits;
    state.repterminationtype = 0;
    state.repiterationscount = 0;
    state.repnfev = 0;
    
    //
    // Update info
    //
    state.xupdated = false;
    
    //
    // Calculate F/G
    //
    state.rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state.repnfev = 1;
    
    //
    // Preparations
    //
    state.fold = state.f;
    v = ap::vdotproduct(&state.g(0), &state.g(0), ap::vlen(0,n-1));
    v = sqrt(v);
    if( v==0 )
    {
        state.repterminationtype = 4;
        result = false;
        return result;
    }
    state.stp = 1.0/v;
    ap::vmoveneg(&state.d(0), &state.g(0), ap::vlen(0,n-1));
    
    //
    // Main cycle
    //
lbl_2:
    if( false )
    {
        goto lbl_3;
    }
    
    //
    // Main cycle: prepare to 1-D line search
    //
    state.p = state.k%m;
    state.q = ap::minint(state.k, m-1);
    
    //
    // Store X[k], G[k]
    //
    ap::vmoveneg(&state.s(state.p, 0), &state.x(0), ap::vlen(0,n-1));
    ap::vmoveneg(&state.y(state.p, 0), &state.g(0), ap::vlen(0,n-1));
    
    //
    // Minimize F(x+alpha*d)
    //
    state.mcstage = 0;
    if( state.k!=0 )
    {
        state.stp = 1.0;
    }
    mcsrch(n, state.x, state.f, state.g, state.d, state.stp, mcinfo, state.nfev, state.work, state, state.mcstage);
lbl_4:
    if( state.mcstage==0 )
    {
        goto lbl_5;
    }
    state.rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    mcsrch(n, state.x, state.f, state.g, state.d, state.stp, mcinfo, state.nfev, state.work, state, state.mcstage);
    goto lbl_4;
lbl_5:
    
    //
    // Main cycle: update information and Hessian.
    // Check stopping conditions.
    //
    state.repnfev = state.repnfev+state.nfev;
    state.repiterationscount = state.repiterationscount+1;
    
    //
    // Calculate S[k], Y[k], Rho[k], GammaK
    //
    ap::vadd(&state.s(state.p, 0), &state.x(0), ap::vlen(0,n-1));
    ap::vadd(&state.y(state.p, 0), &state.g(0), ap::vlen(0,n-1));
    
    //
    // Stopping conditions
    //
    if( state.repiterationscount>=maxits&&maxits>0 )
    {
        
        //
        // Too many iterations
        //
        state.repterminationtype = 5;
        result = false;
        return result;
    }
    v = ap::vdotproduct(&state.g(0), &state.g(0), ap::vlen(0,n-1));
    if( sqrt(v)<=epsg )
    {
        
        //
        // Gradient is small enough
        //
        state.repterminationtype = 4;
        result = false;
        return result;
    }
    if( state.fold-state.f<=epsf*ap::maxreal(fabs(state.fold), ap::maxreal(fabs(state.f), 1.0)) )
    {
        
        //
        // F(k+1)-F(k) is small enough
        //
        state.repterminationtype = 1;
        result = false;
        return result;
    }
    v = ap::vdotproduct(&state.s(state.p, 0), &state.s(state.p, 0), ap::vlen(0,n-1));
    if( sqrt(v)<=epsx )
    {
        
        //
        // X(k+1)-X(k) is small enough
        //
        state.repterminationtype = 2;
        result = false;
        return result;
    }
    
    //
    // Calculate Rho[k], GammaK
    //
    v = ap::vdotproduct(&state.y(state.p, 0), &state.s(state.p, 0), ap::vlen(0,n-1));
    vv = ap::vdotproduct(&state.y(state.p, 0), &state.y(state.p, 0), ap::vlen(0,n-1));
    if( v==0||vv==0 )
    {
        
        //
        // Rounding errors make further iterations impossible.
        //
        state.repterminationtype = -2;
        result = false;
        return result;
    }
    state.rho(state.p) = 1/v;
    state.gammak = v/vv;
    
    //
    //  Calculate d(k+1) = H(k+1)*g(k+1)
    //
    //  for I:=K downto K-Q do
    //      V = s(i)^T * work(iteration:I)
    //      theta(i) = V
    //      work(iteration:I+1) = work(iteration:I) - V*Rho(i)*y(i)
    //  work(last iteration) = H0*work(last iteration)
    //  for I:=K-Q to K do
    //      V = y(i)^T*work(iteration:I)
    //      work(iteration:I+1) = work(iteration:I) +(-V+theta(i))*Rho(i)*s(i)
    //
    //  NOW WORK CONTAINS d(k+1)
    //
    ap::vmove(&state.work(0), &state.g(0), ap::vlen(0,n-1));
    for(i = state.k; i >= state.k-state.q; i--)
    {
        ic = i%m;
        v = ap::vdotproduct(&state.s(ic, 0), &state.work(0), ap::vlen(0,n-1));
        state.theta(ic) = v;
        vv = v*state.rho(ic);
        ap::vsub(&state.work(0), &state.y(ic, 0), ap::vlen(0,n-1), vv);
    }
    v = state.gammak;
    ap::vmul(&state.work(0), ap::vlen(0,n-1), v);
    for(i = state.k-state.q; i <= state.k; i++)
    {
        ic = i%m;
        v = ap::vdotproduct(&state.y(ic, 0), &state.work(0), ap::vlen(0,n-1));
        vv = state.rho(ic)*(-v+state.theta(ic));
        ap::vadd(&state.work(0), &state.s(ic, 0), ap::vlen(0,n-1), vv);
    }
    ap::vmoveneg(&state.d(0), &state.work(0), ap::vlen(0,n-1));
    
    //
    // Next step
    //
    state.fold = state.f;
    state.k = state.k+1;
    state.xupdated = true;
    goto lbl_2;
lbl_3:
    result = false;
    return result;
    
    //
    // Saving state
    //
lbl_rcomm:
    result = true;
    state.rstate.ia(0) = n;
    state.rstate.ia(1) = m;
    state.rstate.ia(2) = maxits;
    state.rstate.ia(3) = i;
    state.rstate.ia(4) = j;
    state.rstate.ia(5) = ic;
    state.rstate.ia(6) = mcinfo;
    state.rstate.ra(0) = epsf;
    state.rstate.ra(1) = epsg;
    state.rstate.ra(2) = epsx;
    state.rstate.ra(3) = v;
    state.rstate.ra(4) = vv;
    return result;
}


/*************************************************************************
Получение результатов работы L-BFGS алгоритма.

Вызывается после того, как MinLBFGSIteration вернет False.

Входные параметры:
    State   -   состояние алгоритма (использовалась MinLBFGSIteration).

Выходные параметры:
    X       -   array[0..N-1], решение задачи.
    Rep     -   отчет о работе:
                * Rep.TerminationType содержит причину завершения алгоритма:
                    * -2    ошибки округления препятствуют дальнейшей
                            работе алгоритма. X содержит наилучшую точку
                            из найденных.
                    * -1    указаны неверные параметры
                    *  1    относительное уменьшение функции не превосходит
                            EpsF.
                    *  2    изменение текущего приближения не превосходит
                            EpsX.
                    *  4    норма градиента не превосходит EpsG
                    *  5    превышено максимальное число итераций MaxIts
                * Rep.IterationsCount содержит число итераций алгоритма
                * NFEV содержит число выходов из алгоритма для  вычисления
                  значения функции.

  -- ALGLIB --
     Copyright 14.11.2007 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresults(const lbfgsstate& state,
     ap::real_1d_array& x,
     lbfgsreport& rep)
{

    x.setbounds(0, state.n-1);
    ap::vmove(&x(0), &state.x(0), ap::vlen(0,state.n-1));
    rep.iterationscount = state.repiterationscount;
    rep.nfev = state.repnfev;
    rep.terminationtype = state.repterminationtype;
}


/*************************************************************************
THE  PURPOSE  OF  MCSRCH  IS  TO  FIND A STEP WHICH SATISFIES A SUFFICIENT
DECREASE CONDITION AND A CURVATURE CONDITION.

AT EACH STAGE THE SUBROUTINE  UPDATES  AN  INTERVAL  OF  UNCERTAINTY  WITH
ENDPOINTS  STX  AND  STY.  THE INTERVAL OF UNCERTAINTY IS INITIALLY CHOSEN
SO THAT IT CONTAINS A MINIMIZER OF THE MODIFIED FUNCTION

    F(X+STP*S) - F(X) - FTOL*STP*(GRADF(X)'S).

IF  A STEP  IS OBTAINED FOR  WHICH THE MODIFIED FUNCTION HAS A NONPOSITIVE
FUNCTION  VALUE  AND  NONNEGATIVE  DERIVATIVE,   THEN   THE   INTERVAL  OF
UNCERTAINTY IS CHOSEN SO THAT IT CONTAINS A MINIMIZER OF F(X+STP*S).

THE  ALGORITHM  IS  DESIGNED TO FIND A STEP WHICH SATISFIES THE SUFFICIENT
DECREASE CONDITION

    F(X+STP*S) .LE. F(X) + FTOL*STP*(GRADF(X)'S),

AND THE CURVATURE CONDITION

    ABS(GRADF(X+STP*S)'S)) .LE. GTOL*ABS(GRADF(X)'S).

IF  FTOL  IS  LESS  THAN GTOL AND IF, FOR EXAMPLE, THE FUNCTION IS BOUNDED
BELOW,  THEN  THERE  IS  ALWAYS  A  STEP  WHICH SATISFIES BOTH CONDITIONS.
IF  NO  STEP  CAN BE FOUND  WHICH  SATISFIES  BOTH  CONDITIONS,  THEN  THE
ALGORITHM  USUALLY STOPS  WHEN  ROUNDING ERRORS  PREVENT FURTHER PROGRESS.
IN THIS CASE STP ONLY SATISFIES THE SUFFICIENT DECREASE CONDITION.

PARAMETERS DESCRIPRION

N IS A POSITIVE INTEGER INPUT VARIABLE SET TO THE NUMBER OF VARIABLES.

X IS  AN  ARRAY  OF  LENGTH N. ON INPUT IT MUST CONTAIN THE BASE POINT FOR
THE LINE SEARCH. ON OUTPUT IT CONTAINS X+STP*S.

F IS  A  VARIABLE. ON INPUT IT MUST CONTAIN THE VALUE OF F AT X. ON OUTPUT
IT CONTAINS THE VALUE OF F AT X + STP*S.

G IS AN ARRAY OF LENGTH N. ON INPUT IT MUST CONTAIN THE GRADIENT OF F AT X.
ON OUTPUT IT CONTAINS THE GRADIENT OF F AT X + STP*S.

S IS AN INPUT ARRAY OF LENGTH N WHICH SPECIFIES THE SEARCH DIRECTION.

STP  IS  A NONNEGATIVE VARIABLE. ON INPUT STP CONTAINS AN INITIAL ESTIMATE
OF A SATISFACTORY STEP. ON OUTPUT STP CONTAINS THE FINAL ESTIMATE.

FTOL AND GTOL ARE NONNEGATIVE INPUT VARIABLES. TERMINATION OCCURS WHEN THE
SUFFICIENT DECREASE CONDITION AND THE DIRECTIONAL DERIVATIVE CONDITION ARE
SATISFIED.

XTOL IS A NONNEGATIVE INPUT VARIABLE. TERMINATION OCCURS WHEN THE RELATIVE
WIDTH OF THE INTERVAL OF UNCERTAINTY IS AT MOST XTOL.

STPMIN AND STPMAX ARE NONNEGATIVE INPUT VARIABLES WHICH SPECIFY LOWER  AND
UPPER BOUNDS FOR THE STEP.

MAXFEV IS A POSITIVE INTEGER INPUT VARIABLE. TERMINATION OCCURS WHEN THE
NUMBER OF CALLS TO FCN IS AT LEAST MAXFEV BY THE END OF AN ITERATION.

INFO IS AN INTEGER OUTPUT VARIABLE SET AS FOLLOWS:
    INFO = 0  IMPROPER INPUT PARAMETERS.

    INFO = 1  THE SUFFICIENT DECREASE CONDITION AND THE
              DIRECTIONAL DERIVATIVE CONDITION HOLD.

    INFO = 2  RELATIVE WIDTH OF THE INTERVAL OF UNCERTAINTY
              IS AT MOST XTOL.

    INFO = 3  NUMBER OF CALLS TO FCN HAS REACHED MAXFEV.

    INFO = 4  THE STEP IS AT THE LOWER BOUND STPMIN.

    INFO = 5  THE STEP IS AT THE UPPER BOUND STPMAX.

    INFO = 6  ROUNDING ERRORS PREVENT FURTHER PROGRESS.
              THERE MAY NOT BE A STEP WHICH SATISFIES THE
              SUFFICIENT DECREASE AND CURVATURE CONDITIONS.
              TOLERANCES MAY BE TOO SMALL.

NFEV IS AN INTEGER OUTPUT VARIABLE SET TO THE NUMBER OF CALLS TO FCN.

WA IS A WORK ARRAY OF LENGTH N.

ARGONNE NATIONAL LABORATORY. MINPACK PROJECT. JUNE 1983
JORGE J. MORE', DAVID J. THUENTE
*************************************************************************/
static void mcsrch(const int& n,
     ap::real_1d_array& x,
     double& f,
     ap::real_1d_array& g,
     const ap::real_1d_array& s,
     double& stp,
     int& info,
     int& nfev,
     ap::real_1d_array& wa,
     lbfgsstate& state,
     int& stage)
{
    double v;
    double p5;
    double p66;
    double zero;

    
    //
    // init
    //
    p5 = 0.5;
    p66 = 0.66;
    state.xtrapf = 4.0;
    zero = 0;
    
    //
    // Main cycle
    //
    while(true)
    {
        if( stage==0 )
        {
            
            //
            // NEXT
            //
            stage = 2;
            continue;
        }
        if( stage==2 )
        {
            state.infoc = 1;
            info = 0;
            
            //
            //     CHECK THE INPUT PARAMETERS FOR ERRORS.
            //
            if( n<=0||stp<=0||ftol<0||gtol<zero||xtol<zero||stpmin<zero||stpmax<stpmin||maxfev<=0 )
            {
                stage = 0;
                return;
            }
            
            //
            //     COMPUTE THE INITIAL GRADIENT IN THE SEARCH DIRECTION
            //     AND CHECK THAT S IS A DESCENT DIRECTION.
            //
            v = ap::vdotproduct(&g(0), &s(0), ap::vlen(0,n-1));
            state.dginit = v;
            if( state.dginit>=0 )
            {
                stage = 0;
                return;
            }
            
            //
            //     INITIALIZE LOCAL VARIABLES.
            //
            state.brackt = false;
            state.stage1 = true;
            nfev = 0;
            state.finit = f;
            state.dgtest = ftol*state.dginit;
            state.width = stpmax-stpmin;
            state.width1 = state.width/p5;
            ap::vmove(&wa(0), &x(0), ap::vlen(0,n-1));
            
            //
            //     THE VARIABLES STX, FX, DGX CONTAIN THE VALUES OF THE STEP,
            //     FUNCTION, AND DIRECTIONAL DERIVATIVE AT THE BEST STEP.
            //     THE VARIABLES STY, FY, DGY CONTAIN THE VALUE OF THE STEP,
            //     FUNCTION, AND DERIVATIVE AT THE OTHER ENDPOINT OF
            //     THE INTERVAL OF UNCERTAINTY.
            //     THE VARIABLES STP, F, DG CONTAIN THE VALUES OF THE STEP,
            //     FUNCTION, AND DERIVATIVE AT THE CURRENT STEP.
            //
            state.stx = 0;
            state.fx = state.finit;
            state.dgx = state.dginit;
            state.sty = 0;
            state.fy = state.finit;
            state.dgy = state.dginit;
            
            //
            // NEXT
            //
            stage = 3;
            continue;
        }
        if( stage==3 )
        {
            
            //
            //     START OF ITERATION.
            //
            //     SET THE MINIMUM AND MAXIMUM STEPS TO CORRESPOND
            //     TO THE PRESENT INTERVAL OF UNCERTAINTY.
            //
            if( state.brackt )
            {
                if( state.stx<state.sty )
                {
                    state.stmin = state.stx;
                    state.stmax = state.sty;
                }
                else
                {
                    state.stmin = state.sty;
                    state.stmax = state.stx;
                }
            }
            else
            {
                state.stmin = state.stx;
                state.stmax = stp+state.xtrapf*(stp-state.stx);
            }
            
            //
            //        FORCE THE STEP TO BE WITHIN THE BOUNDS STPMAX AND STPMIN.
            //
            if( stp>stpmax )
            {
                stp = stpmax;
            }
            if( stp<stpmin )
            {
                stp = stpmin;
            }
            
            //
            //        IF AN UNUSUAL TERMINATION IS TO OCCUR THEN LET
            //        STP BE THE LOWEST POINT OBTAINED SO FAR.
            //
            if( state.brackt&&(stp<=state.stmin||stp>=state.stmax)||nfev>=maxfev-1||state.infoc==0||state.brackt&&state.stmax-state.stmin<=xtol*state.stmax )
            {
                stp = state.stx;
            }
            
            //
            //        EVALUATE THE FUNCTION AND GRADIENT AT STP
            //        AND COMPUTE THE DIRECTIONAL DERIVATIVE.
            //
            ap::vmove(&x(0), &wa(0), ap::vlen(0,n-1));
            ap::vadd(&x(0), &s(0), ap::vlen(0,n-1), stp);
            
            //
            // NEXT
            //
            stage = 4;
            return;
        }
        if( stage==4 )
        {
            info = 0;
            nfev = nfev+1;
            v = ap::vdotproduct(&g(0), &s(0), ap::vlen(0,n-1));
            state.dg = v;
            state.ftest1 = state.finit+stp*state.dgtest;
            
            //
            //        TEST FOR CONVERGENCE.
            //
            if( state.brackt&&(stp<=state.stmin||stp>=state.stmax)||state.infoc==0 )
            {
                info = 6;
            }
            if( stp==stpmax&&f<=state.ftest1&&state.dg<=state.dgtest )
            {
                info = 5;
            }
            if( stp==stpmin&&(f>state.ftest1||state.dg>=state.dgtest) )
            {
                info = 4;
            }
            if( nfev>=maxfev )
            {
                info = 3;
            }
            if( state.brackt&&state.stmax-state.stmin<=xtol*state.stmax )
            {
                info = 2;
            }
            if( f<=state.ftest1&&fabs(state.dg)<=-gtol*state.dginit )
            {
                info = 1;
            }
            
            //
            //        CHECK FOR TERMINATION.
            //
            if( info!=0 )
            {
                stage = 0;
                return;
            }
            
            //
            //        IN THE FIRST STAGE WE SEEK A STEP FOR WHICH THE MODIFIED
            //        FUNCTION HAS A NONPOSITIVE VALUE AND NONNEGATIVE DERIVATIVE.
            //
            if( state.stage1&&f<=state.ftest1&&state.dg>=ap::minreal(ftol, gtol)*state.dginit )
            {
                state.stage1 = false;
            }
            
            //
            //        A MODIFIED FUNCTION IS USED TO PREDICT THE STEP ONLY IF
            //        WE HAVE NOT OBTAINED A STEP FOR WHICH THE MODIFIED
            //        FUNCTION HAS A NONPOSITIVE FUNCTION VALUE AND NONNEGATIVE
            //        DERIVATIVE, AND IF A LOWER FUNCTION VALUE HAS BEEN
            //        OBTAINED BUT THE DECREASE IS NOT SUFFICIENT.
            //
            if( state.stage1&&f<=state.fx&&f>state.ftest1 )
            {
                
                //
                //           DEFINE THE MODIFIED FUNCTION AND DERIVATIVE VALUES.
                //
                state.fm = f-stp*state.dgtest;
                state.fxm = state.fx-state.stx*state.dgtest;
                state.fym = state.fy-state.sty*state.dgtest;
                state.dgm = state.dg-state.dgtest;
                state.dgxm = state.dgx-state.dgtest;
                state.dgym = state.dgy-state.dgtest;
                
                //
                //           CALL CSTEP TO UPDATE THE INTERVAL OF UNCERTAINTY
                //           AND TO COMPUTE THE NEW STEP.
                //
                mcstep(state.stx, state.fxm, state.dgxm, state.sty, state.fym, state.dgym, stp, state.fm, state.dgm, state.brackt, state.stmin, state.stmax, state.infoc);
                
                //
                //           RESET THE FUNCTION AND GRADIENT VALUES FOR F.
                //
                state.fx = state.fxm+state.stx*state.dgtest;
                state.fy = state.fym+state.sty*state.dgtest;
                state.dgx = state.dgxm+state.dgtest;
                state.dgy = state.dgym+state.dgtest;
            }
            else
            {
                
                //
                //           CALL MCSTEP TO UPDATE THE INTERVAL OF UNCERTAINTY
                //           AND TO COMPUTE THE NEW STEP.
                //
                mcstep(state.stx, state.fx, state.dgx, state.sty, state.fy, state.dgy, stp, f, state.dg, state.brackt, state.stmin, state.stmax, state.infoc);
            }
            
            //
            //        FORCE A SUFFICIENT DECREASE IN THE SIZE OF THE
            //        INTERVAL OF UNCERTAINTY.
            //
            if( state.brackt )
            {
                if( fabs(state.sty-state.stx)>=p66*state.width1 )
                {
                    stp = state.stx+p5*(state.sty-state.stx);
                }
                state.width1 = state.width;
                state.width = fabs(state.sty-state.stx);
            }
            
            //
            //  NEXT.
            //
            stage = 3;
            continue;
        }
    }
}


/*************************************************************************
     THE PURPOSE OF MCSTEP IS TO COMPUTE A SAFEGUARDED STEP FOR
     A LINESEARCH AND TO UPDATE AN INTERVAL OF UNCERTAINTY FOR
     A MINIMIZER OF THE FUNCTION.

     THE PARAMETER STX CONTAINS THE STEP WITH THE LEAST FUNCTION
     VALUE. THE PARAMETER STP CONTAINS THE CURRENT STEP. IT IS
     ASSUMED THAT THE DERIVATIVE AT STX IS NEGATIVE IN THE
     DIRECTION OF THE STEP. IF BRACKT IS SET TRUE THEN A
     MINIMIZER HAS BEEN BRACKETED IN AN INTERVAL OF UNCERTAINTY
     WITH ENDPOINTS STX AND STY.

     THE SUBROUTINE STATEMENT IS

       SUBROUTINE MCSTEP(STX,FX,DX,STY,FY,DY,STP,FP,DP,BRACKT,
                        STPMIN,STPMAX,INFO)

     WHERE

       STX, FX, AND DX ARE VARIABLES WHICH SPECIFY THE STEP,
         THE FUNCTION, AND THE DERIVATIVE AT THE BEST STEP OBTAINED
         SO FAR. THE DERIVATIVE MUST BE NEGATIVE IN THE DIRECTION
         OF THE STEP, THAT IS, DX AND STP-STX MUST HAVE OPPOSITE
         SIGNS. ON OUTPUT THESE PARAMETERS ARE UPDATED APPROPRIATELY.

       STY, FY, AND DY ARE VARIABLES WHICH SPECIFY THE STEP,
         THE FUNCTION, AND THE DERIVATIVE AT THE OTHER ENDPOINT OF
         THE INTERVAL OF UNCERTAINTY. ON OUTPUT THESE PARAMETERS ARE
         UPDATED APPROPRIATELY.

       STP, FP, AND DP ARE VARIABLES WHICH SPECIFY THE STEP,
         THE FUNCTION, AND THE DERIVATIVE AT THE CURRENT STEP.
         IF BRACKT IS SET TRUE THEN ON INPUT STP MUST BE
         BETWEEN STX AND STY. ON OUTPUT STP IS SET TO THE NEW STEP.

       BRACKT IS A LOGICAL VARIABLE WHICH SPECIFIES IF A MINIMIZER
         HAS BEEN BRACKETED. IF THE MINIMIZER HAS NOT BEEN BRACKETED
         THEN ON INPUT BRACKT MUST BE SET FALSE. IF THE MINIMIZER
         IS BRACKETED THEN ON OUTPUT BRACKT IS SET TRUE.

       STPMIN AND STPMAX ARE INPUT VARIABLES WHICH SPECIFY LOWER
         AND UPPER BOUNDS FOR THE STEP.

       INFO IS AN INTEGER OUTPUT VARIABLE SET AS FOLLOWS:
         IF INFO = 1,2,3,4,5, THEN THE STEP HAS BEEN COMPUTED
         ACCORDING TO ONE OF THE FIVE CASES BELOW. OTHERWISE
         INFO = 0, AND THIS INDICATES IMPROPER INPUT PARAMETERS.

     SUBPROGRAMS CALLED

       FORTRAN-SUPPLIED ... ABS,MAX,MIN,SQRT

     ARGONNE NATIONAL LABORATORY. MINPACK PROJECT. JUNE 1983
     JORGE J. MORE', DAVID J. THUENTE
*************************************************************************/
static void mcstep(double& stx,
     double& fx,
     double& dx,
     double& sty,
     double& fy,
     double& dy,
     double& stp,
     const double& fp,
     const double& dp,
     bool& brackt,
     const double& stmin,
     const double& stmax,
     int& info)
{
    bool bound;
    double gamma;
    double p;
    double q;
    double r;
    double s;
    double sgnd;
    double stpc;
    double stpf;
    double stpq;
    double theta;

    info = 0;
    
    //
    //     CHECK THE INPUT PARAMETERS FOR ERRORS.
    //
    if( brackt&&(stp<=ap::minreal(stx, sty)||stp>=ap::maxreal(stx, sty))||dx*(stp-stx)>=0||stmax<stmin )
    {
        return;
    }
    
    //
    //     DETERMINE IF THE DERIVATIVES HAVE OPPOSITE SIGN.
    //
    sgnd = dp*(dx/fabs(dx));
    
    //
    //     FIRST CASE. A HIGHER FUNCTION VALUE.
    //     THE MINIMUM IS BRACKETED. IF THE CUBIC STEP IS CLOSER
    //     TO STX THAN THE QUADRATIC STEP, THE CUBIC STEP IS TAKEN,
    //     ELSE THE AVERAGE OF THE CUBIC AND QUADRATIC STEPS IS TAKEN.
    //
    if( fp>fx )
    {
        info = 1;
        bound = true;
        theta = 3*(fx-fp)/(stp-stx)+dx+dp;
        s = ap::maxreal(fabs(theta), ap::maxreal(fabs(dx), fabs(dp)));
        gamma = s*sqrt(ap::sqr(theta/s)-dx/s*(dp/s));
        if( stp<stx )
        {
            gamma = -gamma;
        }
        p = gamma-dx+theta;
        q = gamma-dx+gamma+dp;
        r = p/q;
        stpc = stx+r*(stp-stx);
        stpq = stx+dx/((fx-fp)/(stp-stx)+dx)/2*(stp-stx);
        if( fabs(stpc-stx)<fabs(stpq-stx) )
        {
            stpf = stpc;
        }
        else
        {
            stpf = stpc+(stpq-stpc)/2;
        }
        brackt = true;
    }
    else
    {
        if( sgnd<0 )
        {
            
            //
            //     SECOND CASE. A LOWER FUNCTION VALUE AND DERIVATIVES OF
            //     OPPOSITE SIGN. THE MINIMUM IS BRACKETED. IF THE CUBIC
            //     STEP IS CLOSER TO STX THAN THE QUADRATIC (SECANT) STEP,
            //     THE CUBIC STEP IS TAKEN, ELSE THE QUADRATIC STEP IS TAKEN.
            //
            info = 2;
            bound = false;
            theta = 3*(fx-fp)/(stp-stx)+dx+dp;
            s = ap::maxreal(fabs(theta), ap::maxreal(fabs(dx), fabs(dp)));
            gamma = s*sqrt(ap::sqr(theta/s)-dx/s*(dp/s));
            if( stp>stx )
            {
                gamma = -gamma;
            }
            p = gamma-dp+theta;
            q = gamma-dp+gamma+dx;
            r = p/q;
            stpc = stp+r*(stx-stp);
            stpq = stp+dp/(dp-dx)*(stx-stp);
            if( fabs(stpc-stp)>fabs(stpq-stp) )
            {
                stpf = stpc;
            }
            else
            {
                stpf = stpq;
            }
            brackt = true;
        }
        else
        {
            if( fabs(dp)<fabs(dx) )
            {
                
                //
                //     THIRD CASE. A LOWER FUNCTION VALUE, DERIVATIVES OF THE
                //     SAME SIGN, AND THE MAGNITUDE OF THE DERIVATIVE DECREASES.
                //     THE CUBIC STEP IS ONLY USED IF THE CUBIC TENDS TO INFINITY
                //     IN THE DIRECTION OF THE STEP OR IF THE MINIMUM OF THE CUBIC
                //     IS BEYOND STP. OTHERWISE THE CUBIC STEP IS DEFINED TO BE
                //     EITHER STPMIN OR STPMAX. THE QUADRATIC (SECANT) STEP IS ALSO
                //     COMPUTED AND IF THE MINIMUM IS BRACKETED THEN THE THE STEP
                //     CLOSEST TO STX IS TAKEN, ELSE THE STEP FARTHEST AWAY IS TAKEN.
                //
                info = 3;
                bound = true;
                theta = 3*(fx-fp)/(stp-stx)+dx+dp;
                s = ap::maxreal(fabs(theta), ap::maxreal(fabs(dx), fabs(dp)));
                
                //
                //        THE CASE GAMMA = 0 ONLY ARISES IF THE CUBIC DOES NOT TEND
                //        TO INFINITY IN THE DIRECTION OF THE STEP.
                //
                gamma = s*sqrt(ap::maxreal(double(0), ap::sqr(theta/s)-dx/s*(dp/s)));
                if( stp>stx )
                {
                    gamma = -gamma;
                }
                p = gamma-dp+theta;
                q = gamma+(dx-dp)+gamma;
                r = p/q;
                if( r<0&&gamma!=0 )
                {
                    stpc = stp+r*(stx-stp);
                }
                else
                {
                    if( stp>stx )
                    {
                        stpc = stmax;
                    }
                    else
                    {
                        stpc = stmin;
                    }
                }
                stpq = stp+dp/(dp-dx)*(stx-stp);
                if( brackt )
                {
                    if( fabs(stp-stpc)<fabs(stp-stpq) )
                    {
                        stpf = stpc;
                    }
                    else
                    {
                        stpf = stpq;
                    }
                }
                else
                {
                    if( fabs(stp-stpc)>fabs(stp-stpq) )
                    {
                        stpf = stpc;
                    }
                    else
                    {
                        stpf = stpq;
                    }
                }
            }
            else
            {
                
                //
                //     FOURTH CASE. A LOWER FUNCTION VALUE, DERIVATIVES OF THE
                //     SAME SIGN, AND THE MAGNITUDE OF THE DERIVATIVE DOES
                //     NOT DECREASE. IF THE MINIMUM IS NOT BRACKETED, THE STEP
                //     IS EITHER STPMIN OR STPMAX, ELSE THE CUBIC STEP IS TAKEN.
                //
                info = 4;
                bound = false;
                if( brackt )
                {
                    theta = 3*(fp-fy)/(sty-stp)+dy+dp;
                    s = ap::maxreal(fabs(theta), ap::maxreal(fabs(dy), fabs(dp)));
                    gamma = s*sqrt(ap::sqr(theta/s)-dy/s*(dp/s));
                    if( stp>sty )
                    {
                        gamma = -gamma;
                    }
                    p = gamma-dp+theta;
                    q = gamma-dp+gamma+dy;
                    r = p/q;
                    stpc = stp+r*(sty-stp);
                    stpf = stpc;
                }
                else
                {
                    if( stp>stx )
                    {
                        stpf = stmax;
                    }
                    else
                    {
                        stpf = stmin;
                    }
                }
            }
        }
    }
    
    //
    //     UPDATE THE INTERVAL OF UNCERTAINTY. THIS UPDATE DOES NOT
    //     DEPEND ON THE NEW STEP OR THE CASE ANALYSIS ABOVE.
    //
    if( fp>fx )
    {
        sty = stp;
        fy = fp;
        dy = dp;
    }
    else
    {
        if( sgnd<0.0 )
        {
            sty = stx;
            fy = fx;
            dy = dx;
        }
        stx = stp;
        fx = fp;
        dx = dp;
    }
    
    //
    //     COMPUTE THE NEW STEP AND SAFEGUARD IT.
    //
    stpf = ap::minreal(stmax, stpf);
    stpf = ap::maxreal(stmin, stpf);
    stp = stpf;
    if( brackt&&bound )
    {
        if( sty>stx )
        {
            stp = ap::minreal(stx+0.66*(sty-stx), stp);
        }
        else
        {
            stp = ap::maxreal(stx+0.66*(sty-stx), stp);
        }
    }
}




