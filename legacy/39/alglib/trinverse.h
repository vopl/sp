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
��������� ����������� �������

������������ �������� ��������� ���� ������:
    * �����������������
    * ����������������� � ��������� ����������
    * ����������������
    * ���������������� � ��������� ����������

� ������, ���� ������� ������(�����)�����������, ��  �������,  ��������  �
���, ���� ������(�����)�����������, � �����  ����������  ������  ���������
�������� ������� �������� ����������. ��� ���� �������� ������������� ����
(����) ��������� �� �������� � ���� ������ ���������.

���� ������� � ���������  ����������, �� �������� �  ���  �������  ����  �
���������  ����������.  �  ��������  ����������  ������    ���������������
��������. ��� ���� � ���������� ������ ��������� ������������ ��������  ��
��������.

������� ���������:
    A           -   �������. ������ � ���������� ��������� [0..N-1,0..N-1]
    N           -   ������ �������
    IsUpper     -   True, ���� ������� �����������������
    IsunitTriangular-   True, ���� ������� � ��������� ����������.

�������� ���������:
    A           -   �������, �������� � �������, ���� ������ �� ���������.

���������:
    True, ���� ������� �� ���������
    False, ���� ������� ���������

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

