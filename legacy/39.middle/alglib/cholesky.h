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

#ifndef _cholesky_h
#define _cholesky_h

#include "ap.h"
#include "ialglib.h"

/*************************************************************************
Разложение  Холецкого

Алгоритм осуществляет разложение   Холецкого   симметричной   положительно
определенной  матрицы

Результатом работы алгоритма  является  представление  матрицы  A  в  виде
A = U'*U или A = L*L'.

Входные параметры:
    A   -   верхний или нижний треугольник факторизуемой матрицы.
            Массив с нумерацией элементов [0..N-1, 0..N-1]
    N   -   размер матрицы
    IsUpper-если IsUpper=True, A содержит верхний треугольник симметричной
            матрицы, иначе A содержит нижний треугольник.
    
Выходные параметры:
    A   -   результат факторизации. Если IsUpper=True, в верхнем треугольнике
            находится матрица U, такая, что A = U'*U, а элементы, лежащие
            ниже главной диагонали, не модифицируются.  Аналогично,  если
            IsUpper=False.

Результат:
    Если матрица положительно определена, функция возвращает True.
    Если матрица знаконеопределена, то функция возвращает False. При этом
    факторизация не может быть осуществлена.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992
*************************************************************************/
bool spdmatrixcholesky(ap::real_2d_array& a, int n, bool isupper);


/*************************************************************************
Obsolete 1-based subroutine.
*************************************************************************/
bool choleskydecomposition(ap::real_2d_array& a, int n, bool isupper);


#endif

