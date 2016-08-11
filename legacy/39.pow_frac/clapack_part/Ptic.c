/*  -- translated by f2c (version 20060506).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Subroutine */ int pticr0_(D_fp ak, doublereal *a, doublereal *x1, 
	doublereal *x2, doublereal *y1, doublereal *y2, integer *n, integer *
	m)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    static doublereal h__;
    static integer i__, j;
    static doublereal x, y;

/* * Gets system of linear equations */
/* * for finite-difference approximation of */
/* * integral equation of the first type */
/* * AK(X,Y) - integral equation kernel function */
/* * X1,X2 - integration limits */
/* * Y1,Y2 - Y-variable range */
/* * N - number of numerical integration points */
/* * M - number of points of Y-grid */
/* * A(M,N) - linear system matrix (output) */
/* * Integration by the second variable */
/* *      of the function AK(X,Y) */
    /* Parameter adjustments */
    a_dim1 = *m;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    h__ = (*x2 - *x1) / (*n - 1.f);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x = *x1 + (*x2 - *x1) / (*n - 1.f) * (i__ - 1.f);
	i__2 = *m;
	for (j = 1; j <= i__2; ++j) {
	    y = *y1 + (*y2 - *y1) / (*m - 1.f) * (j - 1.f);
	    a[j + i__ * a_dim1] = (*ak)(&x, &y) * h__;
	    if (i__ == 1 || i__ == *n) {
		a[j + i__ * a_dim1] /= 2.f;
	    }
/* L2: */
	}
/* L1: */
    }
    return 0;
} /* pticr0_ */

/* Subroutine */ int pticro_(doublereal *a, doublereal *z__, doublereal *g, 
	doublereal *u, doublereal *h__, doublereal *alm, doublereal *al, 
	integer *n, integer *m, doublereal *alf, doublereal *ro, integer *ied)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static integer i__;
    static doublereal h2, gh, ah2;
    extern /* Subroutine */ int pticr3_(doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), pticr6_(doublereal *, 
	    doublereal *, integer *, doublereal *);

/* * one-dimensional minimization */
/* *      of the smoosing functional */
/* *      on half-line Z-AL*H AL>0 */
/* * A(M,N) - linear operator matrix */
/* * Z(N) - start point */
/* * G(N) - gradient at the point Z */
/* * H(N) - descent direction */
/* * ALM - step restriction AL<ALM */
/* * U(M) - working array */
/* * AL - step found */
/* * ALF - regularization parameter */
/* * RO - wight of difference derivative in norm W21 */
/* * IED - return code */
/* * 0 - normal - minimum found, */
/* * 1 - minimum on the restriction found, */
/* * 2 - negative step found */
/* * 3 - zero divide */
    /* Parameter adjustments */
    --h__;
    --g;
    --z__;
    --u;
    a_dim1 = *m;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    h2 = 0.f;
    pticr6_(&g[1], &h__[1], n, &gh);
    if (gh < 0.f) {
	goto L2;
    }
    pticr3_(&a[a_offset], &h__[1], &u[1], n, m);
    pticr6_(&u[1], &u[1], m, &ah2);
    if (*alf == 0.f) {
	goto L3;
    }
/* * regularization paremeter doesn't equal zero */
/* * find norm of the descent direction */
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/* L4: */
/* Computing 2nd power */
	d__1 = h__[i__] - h__[i__ - 1];
/* Computing 2nd power */
	d__2 = h__[i__];
	h2 = h2 + d__1 * d__1 * *ro + d__2 * d__2;
    }
/* Computing 2nd power */
    d__1 = h__[1];
    h2 += d__1 * d__1;
L3:
    if (ah2 + h2 * *alf == 0.f) {
	goto L998;
    }
    *al = gh / (ah2 + h2 * *alf) * .5f;
    if (*al >= *alm) {
	goto L1;
    }
    *ied = 0;
    goto L999;
L1:
/* * minimum on the restriction (bound) found */
    *al = *alm;
    *ied = 1;
    goto L999;
L2:
/* * negative step found, make it be zero */
    *al = 0.f;
    *ied = 2;
    goto L999;
L998:
/* * zero divide, let step be zero */
    *ied = 3;
    *al = 0.f;
    goto L999;
L999:
    return 0;
} /* pticro_ */

/* Subroutine */ int pticr1_(doublereal *b, doublereal *c__, doublereal *a, 
	integer *n, doublereal *p)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/* * array moving */
/* * A(I):=B(I)+P*C(I), I=1,N */
    /* Parameter adjustments */
    --a;
    --c__;
    --b;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L1: */
	a[i__] = b[i__] + *p * c__[i__];
    }
    return 0;
} /* pticr1_ */

/* Subroutine */ int pticr2_(doublereal *a, doublereal *r__, integer *n)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/* * fill real array A of length N */
/* * by the value R */
    /* Parameter adjustments */
    --a;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L1: */
	a[i__] = *r__;
    }
    return 0;
} /* pticr2_ */

/* Subroutine */ int ptici2_(integer *a, integer *r__, integer *n)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/* * fill integer array A of length N */
/* * by the value R */
    /* Parameter adjustments */
    --a;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L1: */
	a[i__] = *r__;
    }
    return 0;
} /* ptici2_ */

/* Subroutine */ int pticr3_(doublereal *a, doublereal *z__, doublereal *u, 
	integer *n, integer *m)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j;
    static doublereal s;

/* * multiply matrix A(M,N) by vector Z(N) */
/* * U(M) - vector-result */
    /* Parameter adjustments */
    --z__;
    --u;
    a_dim1 = *m;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	s = 0.f;
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
/* L2: */
	    s += a[i__ + j * a_dim1] * z__[j];
	}
/* L1: */
	u[i__] = s;
    }
    return 0;
} /* pticr3_ */

/* Subroutine */ int pticr4_(doublereal *g, doublereal *u, doublereal *u0, 
	doublereal *a, integer *n, integer *m)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j;
    static doublereal s;

/* * calculating gradient of discrepancy (norm AZ-U0) */
/* * U=A*Z value of operator at the point Z */
/* * G(N) - result */
    /* Parameter adjustments */
    --g;
    a_dim1 = *m;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --u0;
    --u;

    /* Function Body */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	s = 0.f;
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* L2: */
	    s += a[i__ + j * a_dim1] * (u[i__] - u0[i__]);
	}
/* L1: */
	g[j] = s * 2.f;
    }
    return 0;
} /* pticr4_ */

/* Subroutine */ int pticr5_(doublereal *a, doublereal *b, integer *n, 
	doublereal *s)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    static integer i__;

/* * descrepancy calculation */
/* * S - result */
    /* Parameter adjustments */
    --b;
    --a;

    /* Function Body */
    *s = 0.f;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L1: */
/* Computing 2nd power */
	d__1 = a[i__] - b[i__];
	*s += d__1 * d__1;
    }
    return 0;
} /* pticr5_ */

/* Subroutine */ int pticr6_(doublereal *a, doublereal *b, integer *n, 
	doublereal *s)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/* * scalar product calculation of vectors A and B */
/* * S - result */
    /* Parameter adjustments */
    --b;
    --a;

    /* Function Body */
    *s = 0.f;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L1: */
	*s += a[i__] * b[i__];
    }
    return 0;
} /* pticr6_ */

/* Subroutine */ int pticr7_(doublereal *a, doublereal *b, doublereal *c__, 
	integer *n, doublereal *s)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/* * scalar product of vectors A and B of length N */
/* *      with wights C(I) */
/* * S - result */
    /* Parameter adjustments */
    --c__;
    --b;
    --a;

    /* Function Body */
    *s = 0.f;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L1: */
	*s += a[i__] * b[i__] * c__[i__];
    }
    return 0;
} /* pticr7_ */

/* Subroutine */ int pticr8_(doublereal *g, doublereal *z__, integer *n, 
	doublereal *alf, doublereal *ro)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/* * calculation of stabilizer gradient */
/* *      with wight RO */
/* *      and adding it to vector G(N) */
/* * Z(N) - point to calculate */
/* * ALF - regularization parameter */
    /* Parameter adjustments */
    --z__;
    --g;

    /* Function Body */
    if (*alf == 0.f) {
	goto L999;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	g[i__] += *alf * 2.f * z__[i__];
	if (i__ != 1 && i__ != *n) {
	    g[i__] += *alf * 2.f * (z__[i__] * 2.f - z__[i__ - 1] - z__[i__ + 
		    1]) * *ro;
	}
	if (i__ == 1) {
	    g[i__] += *alf * 2.f * (z__[1] - z__[2]) * *ro;
	}
	if (i__ == *n) {
	    g[i__] += *alf * 2.f * (z__[*n] - z__[*n - 1]) * *ro;
	}
/* L1: */
    }
L999:
    return 0;
} /* pticr8_ */

