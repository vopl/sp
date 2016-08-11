/* ec00r.f -- translated by f2c (version 20000817).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int ec00r_c(integer *n, real *dt, real *a, real *y, real *alp,
         real *p, integer *l, real *h__, real *x, real *z__)
{
    /* Initialized data */

    static real sys003 = 6.283185f;

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal);

    /* Local variables */
    extern /* Subroutine */ int ftf1c_c(real *, real *, integer *, integer *,
            integer *, real *);
    static real b, c__;
    static integer i__;
    static real q, r__, w;
    static integer n1, n2, n3, n4, n5;
    static real p1, s1, s2, s3, r1, r2, r3, r4, r5, r6, r7, r8, dw;

    /* Parameter adjustments */
    --z__;
    --x;
    --y;
    --a;
    --h__;

    /* Function Body */
    n1 = *n / 2;
    switch (*l) {
        case 1:  goto L1;
        case 2:  goto L7;
        case 3:  goto L7;
        case 4:  goto L4;
    }
L1:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L2: */
        x[i__] = 0.f;
    }
    q = 1.f;
    ftf1c_c(&a[1], &x[1], n, &c__1, &c__1, &q);
    i__1 = n1;
    for (i__ = 2; i__ <= i__1; ++i__) {
        n2 = n1 + i__;
/* L3: */
        a[n2] = x[i__];
    }
L4:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L5: */
        x[i__] = 0.f;
    }
    q = 1.f;
    ftf1c_c(&y[1], &x[1], n, &c__1, &c__1, &q);
    i__1 = n1;
    for (i__ = 2; i__ <= i__1; ++i__) {
        n2 = n1 + i__;
/* L6: */
        y[n2] = x[i__];
    }
    goto L11;
L7:
    b = 1.f / *n;
    c__ = 1.f / *dt;
    dw = sys003 * b * c__;
    p1 = *p * 2;
    q = *alp * c__ * c__;
    s1 = 0.f;
    s2 = 0.f;
    s3 = 0.f;
    i__1 = n1;
    for (i__ = 2; i__ <= i__1; ++i__) {
        n2 = n1 + i__;
        r1 = a[i__];
        r2 = a[n2];
        r3 = y[i__];
        r4 = y[n2];
        r5 = r1 * r1 + r2 * r2;
        w = dw * (i__ - 1);
        d__1 = (doublereal) w;
        d__2 = (doublereal) p1;
        r__ = (real)pow_dd(&d__1, &d__2);
        r6 = 1.f / (r5 + q * r__);
        r7 = (r1 * r3 + r2 * r4) * c__ * r6;
        r8 = (r1 * r4 - r2 * r3) * c__ * r6;
        x[i__] = r7;
        x[n2] = r8;
        r3 = *dt * (r1 * r7 - r2 * r8) - r3;
        r4 = *dt * (r1 * r8 + r2 * r7) - r4;
        s1 = s1 + r3 * r3 + r4 * r4;
        r3 = r__ * b * (r7 * r7 + r8 * r8);
        s2 += r3;
        r4 = w * (*alp * r6 * r__) * (*alp * r6) * r3;
/* L8: */
        s3 += r4;
    }
    r3 = 0.f;
    if (*p == 0.f) {
        r3 = 1.f;
    }
    x[1] = a[1] * y[1] * c__ / (a[1] * a[1] + q * r3);
    r1 = *dt * a[1] * x[1] - y[1];
    d__1 = (doublereal) (dw * n1);
    d__2 = (doublereal) p1;
    r__ = (real)pow_dd(&d__1, &d__2);
    n3 = *n / 2 + 1;
    r6 = a[n3] * a[n3] + q * r__;
    x[n3] = a[n3] * y[n3] * c__ / r6;
    r2 = *dt * a[n3] * x[n3] - y[n3];
    s1 = (s1 * 2 + r1 * r1 + r2 * r2) * *dt * b;
    s2 = (s2 * 2 + r3 * b * x[1] * x[1] + r__ * b * x[n3] * x[n3]) * *dt;
    h__[1] = (real)sqrt(s1);
    h__[2] = (real)sqrt(s2);
    h__[3] = (real)sqrt((real)(s1 + *alp * s2));
    r5 = *alp / r6 * r__ * x[n3];
    r4 = dw / 2.f * r5 * r5;
    h__[4] = (real)sqrt((real)((s3 * 2 + r4) * c__)) * c__;
    if (*l == 3) {
        goto L11;
    }
    z__[1] = 0.f;
    z__[n3] = 0.f;
    n4 = n1 + 2;
    n5 = *n + 2;
    i__1 = n1;
    for (i__ = 2; i__ <= i__1; ++i__) {
        n2 = n1 + i__;
        z__[i__] = x[n2];
        n3 = n4 - i__;
        x[n2] = x[n3];
        n3 = n5 - i__;
/* L9: */
        z__[n3] = -z__[i__];
    }
    q = -1.f;
    ftf1c_c(&x[1], &z__[1], n, &c__1, &c__1, &q);
    i__1 = n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
        r__ = x[i__] * b;
        n2 = n1 + i__;
        x[i__] = x[n2] * b;
/* L10: */
        x[n2] = r__;
    }
L11:
    return 0;
} /* ec00r_c */
