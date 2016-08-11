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

#include <stdafx.h>
#include "cholesky.h"

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
bool spdmatrixcholesky(ap::real_2d_array& a, int n, bool isupper)
{
    bool result;
    int i;
    int j;
    double ajj;
    double v;

    
    //
    //     Test the input parameters.
    //
    ap::ap_error::make_assertion(n>=0, "Error in SMatrixCholesky: incorrect function arguments");
    
    //
    //     Quick return if possible
    //
    result = true;
    if( n<=0 )
    {
        return result;
    }
    if( isupper )
    {
        
        //
        // Compute the Cholesky factorization A = U'*U.
        //
        for(j = 0; j <= n-1; j++)
        {
            
            //
            // Compute U(J,J) and test for non-positive-definiteness.
            //
            v = ap::vdotproduct(a.getcolumn(j, 0, j-1), a.getcolumn(j, 0, j-1));
            ajj = a(j,j)-v;
            if( ajj<=0 )
            {
                result = false;
                return result;
            }
            ajj = sqrt(ajj);
            a(j,j) = ajj;
            
            //
            // Compute elements J+1:N of row J.
            //
            if( j<n-1 )
            {
                for(i = 0; i <= j-1; i++)
                {
                    v = a(i,j);
                    ap::vsub(&a(j, j+1), &a(i, j+1), ap::vlen(j+1,n-1), v);
                }
                v = 1/ajj;
                ap::vmul(&a(j, j+1), ap::vlen(j+1,n-1), v);
            }
        }
    }
    else
    {
        
        //
        // Compute the Cholesky factorization A = L*L'.
        //
        for(j = 0; j <= n-1; j++)
        {
            
            //
            // Compute L(J,J) and test for non-positive-definiteness.
            //
            v = ap::vdotproduct(&a(j, 0), &a(j, 0), ap::vlen(0,j-1));
            ajj = a(j,j)-v;
            if( ajj<=0 )
            {
                result = false;
                return result;
            }
            ajj = sqrt(ajj);
            a(j,j) = ajj;
            
            //
            // Compute elements J+1:N of column J.
            //
            if( j<n-1 )
            {
                for(i = j+1; i <= n-1; i++)
                {
                    v = ap::vdotproduct(&a(i, 0), &a(j, 0), ap::vlen(0,j-1));
                    a(i,j) = a(i,j)-v;
                }
                v = 1/ajj;
                ap::vmul(a.getcolumn(j, j+1, n-1), v);
            }
        }
    }
    return result;
}


/*************************************************************************
Obsolete 1-based subroutine.
*************************************************************************/
bool choleskydecomposition(ap::real_2d_array& a, int n, bool isupper)
{
    bool result;
    int i;
    int j;
    double ajj;
    double v;
    int jm1;
    int jp1;

    
    //
    //     Test the input parameters.
    //
    ap::ap_error::make_assertion(n>=0, "Error in CholeskyDecomposition: incorrect function arguments");
    
    //
    //     Quick return if possible
    //
    result = true;
    if( n==0 )
    {
        return result;
    }
    if( isupper )
    {
        
        //
        // Compute the Cholesky factorization A = U'*U.
        //
        for(j = 1; j <= n; j++)
        {
            
            //
            // Compute U(J,J) and test for non-positive-definiteness.
            //
            jm1 = j-1;
            v = ap::vdotproduct(a.getcolumn(j, 1, jm1), a.getcolumn(j, 1, jm1));
            ajj = a(j,j)-v;
            if( ajj<=0 )
            {
                result = false;
                return result;
            }
            ajj = sqrt(ajj);
            a(j,j) = ajj;
            
            //
            // Compute elements J+1:N of row J.
            //
            if( j<n )
            {
                for(i = j+1; i <= n; i++)
                {
                    jm1 = j-1;
                    v = ap::vdotproduct(a.getcolumn(i, 1, jm1), a.getcolumn(j, 1, jm1));
                    a(j,i) = a(j,i)-v;
                }
                v = 1/ajj;
                jp1 = j+1;
                ap::vmul(&a(j, jp1), ap::vlen(jp1,n), v);
            }
        }
    }
    else
    {
        
        //
        // Compute the Cholesky factorization A = L*L'.
        //
        for(j = 1; j <= n; j++)
        {
            
            //
            // Compute L(J,J) and test for non-positive-definiteness.
            //
            jm1 = j-1;
            v = ap::vdotproduct(&a(j, 1), &a(j, 1), ap::vlen(1,jm1));
            ajj = a(j,j)-v;
            if( ajj<=0 )
            {
                result = false;
                return result;
            }
            ajj = sqrt(ajj);
            a(j,j) = ajj;
            
            //
            // Compute elements J+1:N of column J.
            //
            if( j<n )
            {
                for(i = j+1; i <= n; i++)
                {
                    jm1 = j-1;
                    v = ap::vdotproduct(&a(i, 1), &a(j, 1), ap::vlen(1,jm1));
                    a(i,j) = a(i,j)-v;
                }
                v = 1/ajj;
                jp1 = j+1;
                ap::vmul(a.getcolumn(j, jp1, n), v);
            }
        }
    }
    return result;
}




