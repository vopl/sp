/*************************************************************************
This file is a part of ALGLIB project.

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

#ifndef _spdsolve_h
#define _spdsolve_h

#include "ap.h"
#include "ialglib.h"

#include "cholesky.h"


/*************************************************************************
Решение системы линейных уравнений с матрицей системы, заданной разложением
Холецкого.

Алгоритм решает только системы уравнений с квадратной матрицей.

Входные параметры:
    A   -   разложение Холецкого матрицы системы (результат работы
            подпрограммы SMatrixCholesky).
    B   -   правая часть системы. Массив с нумерацией элементов [0..N-1]
    N   -   размерность системы.
    IsUpper-указывает треугольник матрицы A, в котором хранится разложение
            Холецкого. Если IsUpper=True, то  разложение  Холецкого  имеет
            вид U'*U, и в верхнем треугольнике матрицы A хранится  матрица
            U (при этом нижний треугольник не используется и  не  меняется
            подпрограммой). Аналогично,   если  IsUpper=False,  разложение
            имеет вид L*L', и передается матрица L (в нижнем треугольнике).

Выходные параметры:
    X   -   решение системы. Массив с нумерацией элементов [0..N-1]

Результат:
    True    -   если система не вырождена. X содержит решение.
    False   -   если система вырождена (на диагонали  треугольной  матрицы
                находится нулевой элемент). В  таком  случае X не содержит
                решение.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixcholeskysolve(const ap::real_2d_array& a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x);


/*************************************************************************
Решение системы линейных уравнений с  положительно  определенной  матрицей
системы с использованием разложения Холецкого.

Алгоритм  решает  систему линейных уравнений, матрица которой положительно
определена и симметрична (Symmetric Positive Definite = SPD).

Входные параметры:
    A   -   верхний или нижний треугольник симметричной матрицы системы.
            Массив с нумерацией элементов [0..N-1, 0..N-1]
    B   -   правая часть системы. Массив с нумерацией элементов [0..N-1]
    N   -   размерность системы.
    IsUpper-указывает треугольник матрицы A, в котором хранится матрица.

Выходные параметры:
    X   -   решение системы. Массив с нумерацией элементов [0..N-1]

Результат:
    True, если система не вырождена.
    False, если система вырождена. В таком случае X не содержит решение.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixsolve(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x);


/*************************************************************************
Obsolete 1-bases subroutine
*************************************************************************/
bool solvesystemcholesky(const ap::real_2d_array& a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x);


/*************************************************************************
Obsolete 1-bases subroutine
*************************************************************************/
bool solvespdsystem(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x);


#endif

