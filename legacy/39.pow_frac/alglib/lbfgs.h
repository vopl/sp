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

#ifndef _lbfgs_h
#define _lbfgs_h

#include "ap.h"
#include "ialglib.h"

struct lbfgsstate
{
    int n;
    int m;
    double epsg;
    double epsf;
    double epsx;
    int maxits;
    int flags;
    int nfev;
    int mcstage;
    int k;
    int q;
    int p;
    ap::real_1d_array rho;
    ap::real_2d_array y;
    ap::real_2d_array s;
    ap::real_1d_array theta;
    ap::real_1d_array d;
    double stp;
    ap::real_1d_array work;
    double fold;
    double gammak;
    ap::real_1d_array x;
    double f;
    ap::real_1d_array g;
    bool xupdated;
    ap::rcommstate rstate;
    int repiterationscount;
    int repnfev;
    int repterminationtype;
    bool brackt;
    bool stage1;
    int infoc;
    double dg;
    double dgm;
    double dginit;
    double dgtest;
    double dgx;
    double dgxm;
    double dgy;
    double dgym;
    double finit;
    double ftest1;
    double fm;
    double fx;
    double fxm;
    double fy;
    double fym;
    double stx;
    double sty;
    double stmin;
    double stmax;
    double width;
    double width1;
    double xtrapf;
};


struct lbfgsreport
{
    int iterationscount;
    int nfev;
    int terminationtype;
};




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
     lbfgsstate& state);


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
bool minlbfgsiteration(lbfgsstate& state);


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
     lbfgsreport& rep);


#endif

