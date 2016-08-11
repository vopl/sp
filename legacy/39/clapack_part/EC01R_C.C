/* ec01r.f -- translated by f2c (version 20000817).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int ec01r_c(integer *n, real *dt, real *a, real *y, real *p,
        real *eps, integer *l, real *h__, real *x, real *z__)
{
    /* Initialized data */

    static real sys003 = 6.28318530717958647692528676655900576839433879875021164194988918461563281257241799725606965068423413;

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    extern /* Subroutine */ int ftf1c_c(real *, real *, integer *, integer *,
            integer *, real *);
    static real b, c__, d__, e;
    static integer i__, k;
    static real q, r__, s, v, w;
    static integer n1, n2, n3;
    static real p1, r1, r2, r3, r4, r5, s1, s2, dw, alp;

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
        case 3:  goto L4;
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
L7:
    b = 1.f / *n;
    c__ = 1.f / *dt;
    d__ = *dt * *dt;
    s = 0.f;
    i__1 = n1;
    for (i__ = 2; i__ <= i__1; ++i__) {
        n2 = n1 + i__;
        r1 = y[i__];
        r2 = y[n2];
/* L8: */
        s = s + r1 * r1 + r2 * r2;
    }
    n3 = *n / 2 + 1;
    s = (s * 2 + y[1] * y[1] + y[n3] * y[n3]) * *dt * b;
    s = (real)sqrt(s);
    e = 1.f / (*eps * s);
    dw = sys003 * b * c__;
    p1 = *p * 2;
    v = 0.f;
    k = 0;
L9:
    q = v * d__;
    s1 = 0.f;
    s2 = 0.f;
    i__1 = n1;
    for (i__ = 2; i__ <= i__1; ++i__) {
        n2 = n1 + i__;
        r1 = a[i__];
        r2 = a[n2];
        r1 = r1 * r1 + r2 * r2;
        r2 = y[i__];
        r3 = y[n2];
        r2 = r2 * r2 + r3 * r3;
        w = dw * (i__ - 1);
        d__1 = (doublereal) w;
        d__2 = (doublereal) p1;
        r__ = (real)pow_dd(&d__1, &d__2);
        r3 = 1.f / (q * r1 + r__);
        r2 = r2 * (r__ * b) * r__ * r3 * r3;
        s1 += r2;
        r1 = r2 * r1 * r3;
/* L10: */
        s2 += r1;
    }
    d__1 = (doublereal) (dw * n1);
    d__2 = (doublereal) p1;
    r__ = (real)pow_dd(&d__1, &d__2);
    r3 = 1.f / (q * a[n3] * a[n3] + r__);
    r2 = y[n3] * y[n3] * (r__ * b) * r__ * r3 * r3;
    r4 = 0.f;
    if (*p == 0.f) {
        r4 = 1.f / (q * a[1] * a[1] + 1.f);
    }
    r5 = y[1] * y[1] * b * r4 * r4;
    s1 = (real)sqrt((real)((s1 * 2 + r5 + r2) * *dt));
    r1 = s1 * e;
    if (r1 - 1.001f <= 0.f) {
        goto L11;
    } else {
        goto L14;
    }
L11:
    if (v <= 0.f) {
        goto L12;
    } else {
        goto L16;
    }
L12:
    r4 = y[1] * b * c__ / a[1];
    if (*p == 0.f) {
        r4 = 0.f;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L13: */
        x[i__] = r4;
    }
    h__[1] = (real)sqrt((real)(s * s - y[1] * y[1] * *dt * b));
    if (*p == 0.f) {
        h__[1] = s;
    }
    h__[2] = 0.f;
    h__[3] = h__[1];
    h__[4] = 0.f;
    h__[5] = 1e18f;
    h__[6] = 0.f;
    h__[7] = h__[1] / s;
    goto L20;
L14:
    if (*p == 0.f) {
        r5 = r5 * a[1] * a[1] * r4;
    }
    if (*p > 0.f) {
        r5 = 0.f;
    }
    r2 = r2 * a[n3] * a[n3] * r3;
    r4 = 1.f / s1;
    s2 = -(s2 * 2.f * r4 + r5 * r4 + r2 * r4) * (d__ * *dt);
    if (s2 == 0.f) {
        alp = 0.f;
    }
    if (s2 == 0.f) {
        goto L15;
    }
    s2 = s1 / s2 * (1.f - r1);
    v += s2;
    ++k;
    alp = 1.f / v;
    if (alp <= 0.f) {
        goto L15;
    } else {
        goto L9;
    }
L15:
    s1 = 0.f;
L16:
    h__[1] = s1;
    h__[5] = alp;
    h__[6] = (real) k;
    h__[7] = h__[1] / s;
    q = alp * c__ * c__;
    s1 = 0.f;
    s2 = 0.f;
    i__1 = n1;
    for (i__ = 2; i__ <= i__1; ++i__) {
        n2 = n1 + i__;
        r1 = a[i__];
        r2 = a[n2];
        r3 = y[i__];
        r4 = y[n2];
        w = dw * (i__ - 1);
        d__1 = (doublereal) w;
        d__2 = (doublereal) p1;
        r__ = (real)pow_dd(&d__1, &d__2);
        r5 = 1.f / (r1 * r1 + r2 * r2 + q * r__);
        e = (r1 * r3 + r2 * r4) * c__ * r5;
        r2 = (r1 * r4 - r2 * r3) * c__ * r5;
        x[i__] = e;
        z__[i__] = r2;
        r3 = r__ * b * (e * e + r2 * r2);
        s1 += r3;
        r4 = w * (alp * r5 * r__) * (alp * r5) * r3;
/* L17: */
        s2 += r4;
    }
    r3 = 0.f;
    if (*p == 0.f) {
        r3 = 1.f;
    }
    x[1] = a[1] * y[1] * c__ / (a[1] * a[1] + q * r3);
    d__1 = (doublereal) (dw * n1);
    d__2 = (doublereal) p1;
    r__ = (real)pow_dd(&d__1, &d__2);
    r5 = 1.f / (a[n3] * a[n3] + q * r__);
    x[n3] = a[n3] * y[n3] * c__ * r5;
    s1 = (s1 * 2.f + r3 * b * x[1] * x[1] + r__ * b * x[n3] * x[n3]) * *dt;
    h__[2] = (real)sqrt(s1);
    h__[3] = (real)sqrt((real)(h__[1] * h__[1] + alp * s1));
    r5 = alp * r5 * r__ * x[n3];
    r4 = dw / 2.f * r5 * r5;
    h__[4] = (real)sqrt((real)((s2 * 2 + r4) * c__)) * c__;
    z__[1] = 0.f;
    z__[n3] = 0.f;
    n3 = *n + 2;
    i__1 = n1;
    for (i__ = 2; i__ <= i__1; ++i__) {
        n2 = n3 - i__;
        x[n2] = x[i__];
/* L18: */
        z__[n2] = -z__[i__];
    }
    q = -1.f;
    ftf1c_c(&x[1], &z__[1], n, &c__1, &c__1, &q);
    i__1 = n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
        r__ = x[i__] * b;
        n2 = n1 + i__;
        x[i__] = x[n2] * b;
/* L19: */
        x[n2] = r__;
    }
L20:
    return 0;
} /* ec01r_c */
