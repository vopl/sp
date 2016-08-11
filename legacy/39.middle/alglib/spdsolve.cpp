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

#include <stdafx.h>
#include "spdsolve.h"

/*************************************************************************
������� ������� �������� ��������� � �������� �������, �������� �����������
���������.

�������� ������ ������ ������� ��������� � ���������� ��������.

������� ���������:
    A   -   ���������� ��������� ������� ������� (��������� ������
            ������������ SMatrixCholesky).
    B   -   ������ ����� �������. ������ � ���������� ��������� [0..N-1]
    N   -   ����������� �������.
    IsUpper-��������� ����������� ������� A, � ������� �������� ����������
            ���������. ���� IsUpper=True, ��  ����������  ���������  �����
            ��� U'*U, � � ������� ������������ ������� A ��������  �������
            U (��� ���� ������ ����������� �� ������������ �  ��  ��������
            �������������). ����������,   ����  IsUpper=False,  ����������
            ����� ��� L*L', � ���������� ������� L (� ������ ������������).

�������� ���������:
    X   -   ������� �������. ������ � ���������� ��������� [0..N-1]

���������:
    True    -   ���� ������� �� ���������. X �������� �������.
    False   -   ���� ������� ��������� (�� ���������  �����������  �������
                ��������� ������� �������). �  �����  ������ X �� ��������
                �������.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixcholeskysolve(const ap::real_2d_array& a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x)
{
    bool result;
    int i;
    double v;

    ap::ap_error::make_assertion(n>0, "Error: N<=0 in SolveSystemCholesky");
    
    //
    // det(A)=0?
    //
    result = true;
    for(i = 0; i <= n-1; i++)
    {
        if( a(i,i)==0 )
        {
            result = false;
            return result;
        }
    }
    
    //
    // det(A)<>0
    //
    x.setbounds(0, n-1);
    if( isupper )
    {
        
        //
        // A = U'*U, solve U'*y = b first
        //
        b(0) = b(0)/a(0,0);
        for(i = 1; i <= n-1; i++)
        {
            v = ap::vdotproduct(a.getcolumn(i, 0, i-1), b.getvector(0, i-1));
            b(i) = (b(i)-v)/a(i,i);
        }
        
        //
        // Solve U*x = y
        //
        b(n-1) = b(n-1)/a(n-1,n-1);
        for(i = n-2; i >= 0; i--)
        {
            v = ap::vdotproduct(&a(i, i+1), &b(i+1), ap::vlen(i+1,n-1));
            b(i) = (b(i)-v)/a(i,i);
        }
        ap::vmove(&x(0), &b(0), ap::vlen(0,n-1));
    }
    else
    {
        
        //
        // A = L*L', solve L'*y = b first
        //
        b(0) = b(0)/a(0,0);
        for(i = 1; i <= n-1; i++)
        {
            v = ap::vdotproduct(&a(i, 0), &b(0), ap::vlen(0,i-1));
            b(i) = (b(i)-v)/a(i,i);
        }
        
        //
        // Solve L'*x = y
        //
        b(n-1) = b(n-1)/a(n-1,n-1);
        for(i = n-2; i >= 0; i--)
        {
            v = ap::vdotproduct(a.getcolumn(i, i+1, n-1), b.getvector(i+1, n-1));
            b(i) = (b(i)-v)/a(i,i);
        }
        ap::vmove(&x(0), &b(0), ap::vlen(0,n-1));
    }
    return result;
}


/*************************************************************************
������� ������� �������� ��������� �  ������������  ������������  ��������
������� � �������������� ���������� ���������.

��������  ������  ������� �������� ���������, ������� ������� ������������
���������� � ����������� (Symmetric Positive Definite = SPD).

������� ���������:
    A   -   ������� ��� ������ ����������� ������������ ������� �������.
            ������ � ���������� ��������� [0..N-1, 0..N-1]
    B   -   ������ ����� �������. ������ � ���������� ��������� [0..N-1]
    N   -   ����������� �������.
    IsUpper-��������� ����������� ������� A, � ������� �������� �������.

�������� ���������:
    X   -   ������� �������. ������ � ���������� ��������� [0..N-1]

���������:
    True, ���� ������� �� ���������.
    False, ���� ������� ���������. � ����� ������ X �� �������� �������.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixsolve(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x)
{
    bool result;

    result = spdmatrixcholesky(a, n, isupper);
    if( !result )
    {
        return result;
    }
    result = spdmatrixcholeskysolve(a, b, n, isupper, x);
    return result;
}


/*************************************************************************
Obsolete 1-bases subroutine
*************************************************************************/
bool solvesystemcholesky(const ap::real_2d_array& a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x)
{
    bool result;
    int i;
    int im1;
    int ip1;
    double v;

    ap::ap_error::make_assertion(n>0, "Error: N<=0 in SolveSystemCholesky");
    
    //
    // det(A)=0?
    //
    result = true;
    for(i = 1; i <= n; i++)
    {
        if( a(i,i)==0 )
        {
            result = false;
            return result;
        }
    }
    
    //
    // det(A)<>0
    //
    x.setbounds(1, n);
    if( isupper )
    {
        
        //
        // A = U'*U, solve U'*y = b first
        //
        b(1) = b(1)/a(1,1);
        for(i = 2; i <= n; i++)
        {
            im1 = i-1;
            v = ap::vdotproduct(a.getcolumn(i, 1, im1), b.getvector(1, im1));
            b(i) = (b(i)-v)/a(i,i);
        }
        
        //
        // Solve U*x = y
        //
        b(n) = b(n)/a(n,n);
        for(i = n-1; i >= 1; i--)
        {
            ip1 = i+1;
            v = ap::vdotproduct(&a(i, ip1), &b(ip1), ap::vlen(ip1,n));
            b(i) = (b(i)-v)/a(i,i);
        }
        ap::vmove(&x(1), &b(1), ap::vlen(1,n));
    }
    else
    {
        
        //
        // A = L*L', solve L'*y = b first
        //
        b(1) = b(1)/a(1,1);
        for(i = 2; i <= n; i++)
        {
            im1 = i-1;
            v = ap::vdotproduct(&a(i, 1), &b(1), ap::vlen(1,im1));
            b(i) = (b(i)-v)/a(i,i);
        }
        
        //
        // Solve L'*x = y
        //
        b(n) = b(n)/a(n,n);
        for(i = n-1; i >= 1; i--)
        {
            ip1 = i+1;
            v = ap::vdotproduct(a.getcolumn(i, ip1, n), b.getvector(ip1, n));
            b(i) = (b(i)-v)/a(i,i);
        }
        ap::vmove(&x(1), &b(1), ap::vlen(1,n));
    }
    return result;
}


/*************************************************************************
Obsolete 1-bases subroutine
*************************************************************************/
bool solvespdsystem(ap::real_2d_array a,
     ap::real_1d_array b,
     int n,
     bool isupper,
     ap::real_1d_array& x)
{
    bool result;

    result = choleskydecomposition(a, n, isupper);
    if( !result )
    {
        return result;
    }
    result = solvesystemcholesky(a, b, n, isupper, x);
    return result;
}




