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

������������   ������������   �����   ��������  �������  �  ��������������
��������� � ��������. ��������  - ����������� ������ ����������-����������
�  ��������������  ���������������   �����������   L-BFGS   ����������   �
���������� ����������������� ����������� L-BFGS ����������  �����  �������
���� �� ������ ����������-����������.

��������������, ��� ������� F ����� ����� ���:

    F = f(x[0], ..., x[n-1])

������ �������������

��. HTML-������������

������� ���������:
    N       -   ����������� ������, N>1
    X       -   ��������� ����������� � �������, array[0..N-1]
    EpsF    -   ������������� �����, ������������ �������� ������ ��������
                ������������ ���������� ������, ���� ��  k+1-��   ��������
                ����������� �������
                |F(k+1)-F(k)| <= EpsF*max{|F(k)|, |F(k+1)|, 1}
    EpsX    -   ������������� �����, ������������ �������� ������ ��������
                ������������ ���������� ������, ���� ��  k+1-��   ��������
                ����������� ������� |X(k+1)-X(k)| <= EpsX*(1+|X(k)|)
    MaxIts  -   ������������ ����� �������� ���������.
                ���� MaxIts=0, �� ����� �������� �� ����������.

�������� ���������:
    State   -   ���������, ����������� ��������� ��������� ����� ����������
                �������� � �������������� ���  ��������  ������������.  ���
                ��������� ������ ������������ � ������������ MinLMIteration

��. �����
    MinLMIteration, MinLMResults
    
���������

������� ������������ EpsF=0, EpsX=0 � MaxIts=0 �������� � ���������������
������ �������� �������� (������ ������ �������� EpsX).

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
������ ������������ MinLMFGH ��� ������, ����� ���������� ������� �������,
�� �������� � �������. ���������������� ��������  ����������-���������� �
�����������. ����� �������� ��. MinLMFGH.

�������������� ������� ����� ���:

F = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])

������ �������������

��. HTML-������������

������� ���������:
    N       -   ����������� ������, N>1
    M       -   ����� ������� f[i], M>1
    X       -   ��������� ����������� � �������, array[0..N-1]
    EpsF    -   �������� �������� (��. MinLMFGH)
    EpsX    -   �������� �������� (��. MinLMFGH)
    MaxIts  -   �������� �������� (��. MinLMFGH)

�������� ���������:
    State   -   ���������, ����������� ��������� ��������� ����� ����������
                �������� � �������������� ���  ��������  ������������.  ���
                ��������� ������ ������������ � ������������ MinLMIteration

��. �����
    MinLMIteration, MinLMResults

���������

������� ������������ EpsF=0, EpsX=0 � MaxIts=0 �������� � ���������������
������ �������� �������� (������ ������ �������� EpsX).

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
������������ ������� ���������  ����������-����������  ���  ���������.  ��
������ ���������� ������������ MinLMFGH, �����  ��������  ��.  �����������
� ���� ������������.

�������������� ������� ����� ���:

F = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])

������ �������������

��. HTML-������������

������� ���������:
    N       -   ����������� ������, N>1
    M       -   ����� ������� f[i], M>1
    X       -   ��������� ����������� � �������, array[0..N-1]
    EpsF    -   �������� �������� (��. MinLMFGH)
    EpsX    -   �������� �������� (��. MinLMFGH)
    MaxIts  -   �������� �������� (��. MinLMFGH)

�������� ���������:
    State   -   ���������, ����������� ��������� ��������� ����� ����������
                �������� � �������������� ���  ��������  ������������.  ���
                ��������� ������ ������������ � ������������ MinLMIteration

��. �����
    MinLMIteration, MinLMResults

���������

������� ������������ EpsF=0, EpsX=0 � MaxIts=0 �������� � ���������������
������ �������� �������� (������ ������ �������� EpsX).

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
���� �������� ���������.

���������� ����� ������������� ��������� ������������� MinLMxxx. ������
������������� - ��. HTML-������������.

������� ���������:
    State   -   ���������,   �����������   ���������    ���������    �����
                ����������  ��������   �   ��������������   ���   ��������
                ������������. ��� ��������� ������  ����  ����������������
                ������������� MinLMxxx.

���� ������������ ���������� False,  �����������  �������  ��������.
���� ������������ ������� True, ��:
* ���� State.NeedF=True,    -   ��������� ��������� �������� ������� F �
                                ����� State.X[0..N-1]
* ���� State.NeedFG=True    -   �������� ������� F � ���������
* ���� State.NeedFiJ=True   -   �������� ������� ������� f[i] � ������� �����
* ���� State.NeedFGH=True   -   �������� ������� F, ���������, ��������

������������ ����� ���� ����������� ���� � ������ ���� �� ���� �����.

���������� ����������:
* �������� �������          -   � LMState.F
* ��������                  -   � LMState.G[0..N-1]
* ������� �����             -   � LMState.J[0..M-1,0..N-1]
* �������                   -   � LMState.H[0..N-1,0..N-1]

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
bool minlmiteration(lmstate& state);


/*************************************************************************
������������ ���������� ��������� ������ ����������������� ���������
����������-����������.

�������� ���������:
    State   -   ���������, ���������������� MinLMIteration.
    X       -   array[0..N-1], ������� ������
    Rep     -   ����� � ������� ������:
                * Rep.TerminationType �������� ������� ���������� ���������:
                    * -1    ������� �������� ���������
                    *  1    ������������� ���������� ������� �� �����������
                            EpsF.
                    *  2    ��������� �������� ����������� �� �����������
                            EpsX.
                    *  4    ����� ��������� �� ����������� EpsG
                    *  5    ��������� ������������ ����� �������� MaxIts
                * Rep.IterationsCount  ��������  �����  �������� ���������
                * Rep.NFunc     - ����� ���������� �������� �������
                * Rep.NJac      - ����� ���������� ������� �����
                * Rep.NGrad     - ����� ���������� ���������
                * Rep.NHess     - ����� ���������� ��������
                * Rep.NCholesky - ����� ���������� ���������� ���������

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresults(const lmstate& state, ap::real_1d_array& x, lmreport& rep);


#endif

