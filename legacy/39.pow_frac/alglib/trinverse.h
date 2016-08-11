/*************************************************************************
Copyright (c) 1992-2007 The University of Tennessee.  All rights reserved.

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from FORTRAN to
      pseudocode.

See subroutines comments for additional copyrights.

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

#ifndef _trinverse_h
#define _trinverse_h

#include "ap.h"
#include "ialglib.h"

/*************************************************************************
Обращение треугольной матрицы

Подпрограмма обращает следующие типы матриц:
    * верхнетреугольные
    * верхнетреугольные с единичной диагональю
    * нижнетреугольные
    * нижнетреугольные с единичной диагональю

В случае, если матрица верхне(нижне)треугольная, то  матрица,  обратная  к
ней, тоже верхне(нижне)треугольная, и после  завершения  работы  алгоритма
обратная матрица замещает переданную. При этом элементы расположенные ниже
(выше) диагонали не меняются в ходе работы алгоритма.

Если матрица с единичной  диагональю, то обратная к  ней  матрица  тоже  с
единичной  диагональю.  В  алгоритм  передаются  только    внедиагональные
элементы. При этом в результате работы алгоритма диагональные элементы  не
меняются.

Входные параметры:
    A           -   матрица. Массив с нумерацией элементов [0..N-1,0..N-1]
    N           -   размер матрицы
    IsUpper     -   True, если матрица верхнетреугольная
    IsunitTriangular-   True, если матрица с единичной диагональю.

Выходные параметры:
    A           -   матрица, обратная к входной, если задача не вырождена.

Результат:
    True, если матрица не вырождена
    False, если матрица вырождена

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992
*************************************************************************/
bool rmatrixtrinverse(ap::real_2d_array& a,
     int n,
     bool isupper,
     bool isunittriangular);


/*************************************************************************
Obsolete 1-based subroutine.
See RMatrixTRInverse for 0-based replacement.
*************************************************************************/
bool invtriangular(ap::real_2d_array& a,
     int n,
     bool isupper,
     bool isunittriangular);


#endif

