/*************************************************************************
Copyright (c) 2009, Sergey Bochkanov (ALGLIB project).

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

#ifndef _minlm_h
#define _minlm_h

#include "ap.h"
#include "ialglib.h"

#include "blas.h"
#include "trinverse.h"
#include "cholesky.h"
#include "spdsolve.h"
#include "lbfgs.h"


struct lmstate
{
    bool wrongparams;
    int n;
    int m;
    double epsf;
    double epsx;
    int maxits;
    int flags;
    int usermode;
    ap::real_1d_array x;
    double f;
    ap::real_1d_array fi;
    ap::real_2d_array j;
    ap::real_2d_array h;
    ap::real_1d_array g;
    bool needf;
    bool needfg;
    bool needfgh;
    bool needfij;
    bool xupdated;
    lbfgsstate internalstate;
    lbfgsreport internalrep;
    ap::real_1d_array xprec;
    ap::real_1d_array xbase;
    ap::real_1d_array xdir;
    ap::real_1d_array gbase;
    ap::real_2d_array rawmodel;
    ap::real_2d_array model;
    ap::real_1d_array work;
    ap::rcommstate rstate;
    int repiterationscount;
    int repterminationtype;
    int repnfunc;
    int repnjac;
    int repngrad;
    int repnhess;
    int repncholesky;
};


struct lmreport
{
    int iterationscount;
    int terminationtype;
    int nfunc;
    int njac;
    int ngrad;
    int nhess;
    int ncholesky;
};




/*************************************************************************
    LEVENBERG-MARQUARDT-LIKE METHOD FOR NON-LINEAR OPTIMIZATION

Подпрограмма   осуществляет   поиск   минимума  функции  с  использованием
градиента и гессиана. Алгоритм  - модификация метода Левенберга-Марквардта
с  использованием  предварительной   оптимизации   L-BFGS   алгоритмом   и
внутренней предобусловленной оптимизации L-BFGS алгоритмом  после  каждого
шага по методу Левенберга-Марквардта.

Предполагается, что функция F имеет общий вид:

    F = f(x[0], ..., x[n-1])

ПРИМЕР ИСПОЛЬЗОВАНИЯ

см. HTML-документацию

ВХОДНЫЕ ПАРАМЕТРЫ:
    N       -   Размерность задачи, N>1
    X       -   Начальное приближение к решению, array[0..N-1]
    EpsF    -   Положительное число, определяющее точность поиска минимума
                Подпрограмма прекращает работу, если на  k+1-ой   итерации
                выполняется условие
                |F(k+1)-F(k)| <= EpsF*max{|F(k)|, |F(k+1)|, 1}
    EpsX    -   Положительное число, определяющее точность поиска минимума
                Подпрограмма прекращает работу, если на  k+1-ой   итерации
                выполняется условие |X(k+1)-X(k)| <= EpsX*(1+|X(k)|)
    MaxIts  -   Максимальное число итераций алгоритма.
                Если MaxIts=0, то число итераций не ограничено.

ВЫХОДНЫЕ ПАРАМЕТРЫ:
    State   -   структура, сохраняющая состояние алгоритма между повторными
                вызовами и использующаяся для  обратной  коммуникации.  Эта
                структура должна передаваться в подпрограмму MinLMIteration

СМ. ТАКЖЕ
    MinLMIteration, MinLMResults
    
ЗАМЕЧАНИЕ

Задание одновременно EpsF=0, EpsX=0 и MaxIts=0 приведет к автоматическому
выбору критерия останова (выбору малого значения EpsX).

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmfgh(const int& n,
     const ap::real_1d_array& x,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     lmstate& state);


/*************************************************************************
Аналог подпрограммы MinLMFGH для случая, когда неизвестен гессиан функции,
но известен её Якобиан. Модифицированный алгоритм  Левенберга-Марквардта с
улучшениями. Более подробно см. MinLMFGH.

Минимизируемая функция имеет вид:

F = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])

ПРИМЕР ИСПОЛЬЗОВАНИЯ

см. HTML-документацию

ВХОДНЫЕ ПАРАМЕТРЫ:
    N       -   Размерность задачи, N>1
    M       -   число функций f[i], M>1
    X       -   Начальное приближение к решению, array[0..N-1]
    EpsF    -   критерий останова (см. MinLMFGH)
    EpsX    -   критерий останова (см. MinLMFGH)
    MaxIts  -   критерий останова (см. MinLMFGH)

ВЫХОДНЫЕ ПАРАМЕТРЫ:
    State   -   структура, сохраняющая состояние алгоритма между повторными
                вызовами и использующаяся для  обратной  коммуникации.  Эта
                структура должна передаваться в подпрограмму MinLMIteration

СМ. ТАКЖЕ
    MinLMIteration, MinLMResults

ЗАМЕЧАНИЕ

Задание одновременно EpsF=0, EpsX=0 и MaxIts=0 приведет к автоматическому
выбору критерия останова (выбору малого значения EpsX).

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmfgj(const int& n,
     const int& m,
     const ap::real_1d_array& x,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     lmstate& state);


/*************************************************************************
Классический вариант алгоритма  Левенберга-Марквардта  без  улучшений.  Во
многом аналогичен подпрограмме MinLMFGH, более  подробно  см.  комментарий
к этой подпрограмме.

Минимизируемая функция имеет вид:

F = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])

ПРИМЕР ИСПОЛЬЗОВАНИЯ

см. HTML-документацию

ВХОДНЫЕ ПАРАМЕТРЫ:
    N       -   Размерность задачи, N>1
    M       -   число функций f[i], M>1
    X       -   Начальное приближение к решению, array[0..N-1]
    EpsF    -   критерий останова (см. MinLMFGH)
    EpsX    -   критерий останова (см. MinLMFGH)
    MaxIts  -   критерий останова (см. MinLMFGH)

ВЫХОДНЫЕ ПАРАМЕТРЫ:
    State   -   структура, сохраняющая состояние алгоритма между повторными
                вызовами и использующаяся для  обратной  коммуникации.  Эта
                структура должна передаваться в подпрограмму MinLMIteration

СМ. ТАКЖЕ
    MinLMIteration, MinLMResults

ЗАМЕЧАНИЕ

Задание одновременно EpsF=0, EpsX=0 и MaxIts=0 приведет к автоматическому
выбору критерия останова (выбору малого значения EpsX).

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmfj(const int& n,
     const int& m,
     const ap::real_1d_array& x,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     lmstate& state);


/*************************************************************************
Одна итерация алгоритма.

Вызывается после инициализации алгоритма подпрограммой MinLMxxx. Пример
использования - см. HTML-документацию.

Входные параметры:
    State   -   структура,   сохраняющая   состояние    алгоритма    между
                повторными  вызовами   и   использующаяся   для   обратной
                коммуникации. Эта структура должна  быть  инициализирована
                подпрограммой MinLMxxx.

Если подпрограмма возвращает False,  итеративный  процесс  завершен.
Если подпрограмма вернула True, то:
* если State.NeedF=True,    -   требуется вычислить значение функции F в
                                точке State.X[0..N-1]
* если State.NeedFG=True    -   значение функции F и градиента
* если State.NeedFiJ=True   -   значение вектора функций f[i] и матрицы Якоби
* если State.NeedFGH=True   -   значение функции F, градиента, гессиана

Одновременно может быть установлено одно и только одно из этих полей.

Результаты помещаются:
* значение функции          -   в LMState.F
* градиент                  -   в LMState.G[0..N-1]
* матрица Якоби             -   в LMState.J[0..M-1,0..N-1]
* гессиан                   -   в LMState.H[0..N-1,0..N-1]

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
bool minlmiteration(lmstate& state);


/*************************************************************************
Подпрограмма возвращает результат работы модифицированного алгоритма
Левенберга-Марквардта.

Выходные параметры:
    State   -   структура, использовавшаяся MinLMIteration.
    X       -   array[0..N-1], решение задачи
    Rep     -   отчет о решении задачи:
                * Rep.TerminationType содержит причину завершения алгоритма:
                    * -1    указаны неверные параметры
                    *  1    относительное уменьшение функции не превосходит
                            EpsF.
                    *  2    изменение текущего приближения не превосходит
                            EpsX.
                    *  4    норма градиента не превосходит EpsG
                    *  5    превышено максимальное число итераций MaxIts
                * Rep.IterationsCount  содержит  число  итераций алгоритма
                * Rep.NFunc     - число вычислений значения функции
                * Rep.NJac      - число вычислений матрицы Якоби
                * Rep.NGrad     - число вычислений градиента
                * Rep.NHess     - число вычислений гессиана
                * Rep.NCholesky - число вычислений разложения Холецкого

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresults(const lmstate& state, ap::real_1d_array& x, lmreport& rep);


#endif

