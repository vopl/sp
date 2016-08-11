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

������������ ������������  �������  N  ����������  F(x)  �  ��������������
�����-������������� ������ (LBFGS-�����), ����������������� �� �������������
����������� ������.

������������ ����� ������������� �������, �������� �  ��������  ������,  �
�������������� ���������� � M ���������� ����� ���������  (������ N),  ���
���������  �������  ���������  ����� ����������� ������ � �������� �������
N^2 �� �������� ������� O(N*M).

������ ������������ ����������� �� ������ ��������, �����������  �  ������
D.C. Liu � J. Nocedal (1989).

�������� ������������

� ����  �����  ������  ������������  ����������  ��������  ������������  �
������������� ��� ���������� �������� ������� � ���������. ��������  �����
������ �������� ���:
1. ���������� ������������ MinLBFGS � ��������� ���������� ������ (�������,
   ���������   ���������   �  �.�.).  ���������  ���������  �����������  �
   ����������  �  ��������  ������  ��  ����� ��������� State, �����  ����
   ���������� ������������ ������������.
2. ����������  ������������  MinLBFGSIteration.  ���� ������������ �������
   True, �� �������� �������� ������. ������� ������ � ����� � ������ �����
   ���� �������� ��� ������ ������������ MinLBFGSResults.
3. �  ����  ������  ������������  ���������  �������� ������� � �������� �
   ����� State.X (array [0..N-1]),  ��������� �� �  State.F  (������������
   ����)  � State.G (array [0..N-1]),  �����  ����  ��������  ������������
   MinLBFGSIteration.

������� ���������:
    N       -   ����������� ������, N>1
    M       -   ����� ��������� � BFGS-����� ���������� �������������
                ��������. �������������   ��������:  3 <= M <= 7.  �������
                �������� �� �������� �������� ���������� �������� ����������,
                ������� - �� �������� �������� �������� ������� � ��������
                ����������, ���� �������� � ������� ��������������.
                1<=M<=N.
    X       -   ��������� ����������� � �������, array[0..N-1]
    EpsG    -   ������������� �����, ������������ �������� ������ ��������
                ������������ ����������  ������, ���� �����������  �������
                ||G|| < EpsG, ��� ||.|| ����������  ���������  �����,  G -
                ��������, X - ������� ����������� � ��������.
    EpsF    -   ������������� �����, ������������ �������� ������ ��������
                ������������ ���������� ������, ���� ��  k+1-��   ��������
                ����������� �������
                |F(k+1)-F(k)| <= EpsF*max{|F(k)|, |F(k+1)|, 1}
    EpsX    -   ������������� �����, ������������ �������� ������ ��������
                ������������ ���������� ������, ���� ��  k+1-��   ��������
                ����������� ������� |X(k+1)-X(k)| <= EpsX
    MaxIts  -   ������������ ����� �������� ���������.
                ���� MaxIts=0, �� ����� �������� �� ����������.
    Flags   -   �������������� ��������� ���������:
                * 0     ��� �������������� ��������
                * 1     �� �������� ������ (������������ ��� ������� �����
                        ����� � ������ � ���� �� ���������� N � M;  ������
                        ���������� ������ ��� ������ ������, ��� ���������
                        ������� ����� �������� ��������������, �����������
                        �� � ���������, ���� ���������� � ��������  �����
                        ������������������ ��������� State). ���� ���������
                        State ���� ���������������� � �������������� ������
                        �������� N/M, �� ������������� �����  �����  �����
                        �������� � ���� � ������ ������������.

�������� ���������:
    State   -   ���������, ����������� ��������� ��������� ����� ����������
                �������� � �������������� ��� ��������  ������������.

��. �����
    ������������ MinLBFGSIteration, MinLBFGSResult

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
���� �������� L-BFGS ���������.

���������� ����� ������������� ��������� ������������� MinLBFGS. ������
������������� - ��. HTML-������������.

������� ���������:
    State   -   ���������,   �����������   ���������    ���������    �����
                ����������  ��������   �   ��������������   ���   ��������
                ������������. ��� ��������� ������  ����  ����������������
                ������������� MinLBFGS.

���� ������������ ���������� False,  �����������  �������  ��������.
���� ������������ ������� True,  ��  ���������  ��������� �������� �������
State.F � ��������  ���������  State.G[0..N-1]  �  �����  State.X[0..N-1],
����� ���� �������� ������� ������������.


  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
bool minlbfgsiteration(lbfgsstate& state);


/*************************************************************************
��������� ����������� ������ L-BFGS ���������.

���������� ����� ����, ��� MinLBFGSIteration ������ False.

������� ���������:
    State   -   ��������� ��������� (�������������� MinLBFGSIteration).

�������� ���������:
    X       -   array[0..N-1], ������� ������.
    Rep     -   ����� � ������:
                * Rep.TerminationType �������� ������� ���������� ���������:
                    * -2    ������ ���������� ������������ ����������
                            ������ ���������. X �������� ��������� �����
                            �� ���������.
                    * -1    ������� �������� ���������
                    *  1    ������������� ���������� ������� �� �����������
                            EpsF.
                    *  2    ��������� �������� ����������� �� �����������
                            EpsX.
                    *  4    ����� ��������� �� ����������� EpsG
                    *  5    ��������� ������������ ����� �������� MaxIts
                * Rep.IterationsCount �������� ����� �������� ���������
                * NFEV �������� ����� ������� �� ��������� ���  ����������
                  �������� �������.

  -- ALGLIB --
     Copyright 14.11.2007 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresults(const lbfgsstate& state,
     ap::real_1d_array& x,
     lbfgsreport& rep);


#endif

