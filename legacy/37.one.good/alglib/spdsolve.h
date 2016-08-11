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
     ap::real_1d_array& x);


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

