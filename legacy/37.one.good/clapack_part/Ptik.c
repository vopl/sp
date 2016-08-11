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

/* Table of constant values */

static doublereal c_b5 = 0.;
static integer c__1 = 1;

/* Subroutine */ int ptikr_0_(int n__, D_fp ak, doublereal *u0, doublereal *a,
	 doublereal *b, doublereal *c__, doublereal *d__, doublereal *l1, 
	doublereal *l2, integer *n, doublereal *z__, doublereal *an, 
	doublereal *dl, doublereal *hh, doublereal *c1, integer *imax, 
	doublereal *alpha, doublereal *u, integer *nu, integer *ierr)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal t;
    static integer ip, iq;
    static doublereal om;
    static integer nw, nai, nar, nui, nzi, nur, nmax;
    static doublereal epro;
    extern /* Subroutine */ int pticr1_(doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *), pticr2_(doublereal *, 
	    doublereal *, integer *), ptikr1_(D_fp, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *);

/* *      working array mapping */
/* *  name length    what it is */
/* *  ARE    N    real part of kernel Fourier image */
/* *  AIM    N    imagenary part of kernel Fourier image */
/* *  URE    N    real part of right side Fourier image */
/* *  UIM    N    imagenary part of right side Fourier image */
/* *  ZIM    N    imagenary part of solution */
/* *   W     N    stabilizator */
    /* Parameter adjustments */
    if (u0) {
	--u0;
	}
    if (z__) {
	--z__;
	}
    if (u) {
	--u;
	}

    /* Function Body */
    switch(n__) {
	case 1: goto L_ptikre;
	}

    ip = 1;
/* * IP - mode: */
/* * on the first call IP=+1 */
/* * on the next calls through PTIKRE IP=-1 */
    epro = 0.f;
    if (*c1 > 1.f) {
	epro = (*c1 - 1.f) * *dl;
    }
/* * EPRO - accuracy for discrepancy equation */
/* * if EPRO=0, the only extremal is calculated with the */
/* * fixed regularization parameter ALFA */
L100:
/* * calculating support of solution */
    *a = *c__ - (*l1 + *l2) * .5f;
    *b = *d__ - (*l1 + *l2) * .5f;
/* * T - period */
    t = *d__ - *c__;
/* * getting arrays starts */
    nar = 1;
    nai = nar + *n;
    nw = nai + *n;
    nzi = nw + *n;
    nur = nzi + *n;
    nui = nur + *n;
    nmax = nui + *n;
/* * check if the working array size if sufficient */
    if (nmax - 1 > *nu) {
	goto L64;
    }
    if (ip == -1) {
	goto L101;
    }
/* * setting right side */
    pticr1_(&u0[1], &u0[1], &u[nur], n, &c_b5);
    pticr2_(&u[nui], &c_b5, n);
L101:
/* * solving equation */
    d__1 = sqrt(*dl);
    d__2 = sqrt(*hh);
    ptikr1_((D_fp)ak, &u[nar], &u[nai], &z__[1], &u[nzi], &u[nur], &u[nui], &
	    u[nw], n, alpha, l1, l2, an, &om, &t, &d__1, &d__2, &ip, &epro, &
	    iq, imax, ierr);
    goto L999;
L64:
/* * working array is too short */
    *ierr = 64;
    goto L999;
/* * the entry  PTIKRE is for the repeated solving */
/* * of the same equation and right side */

L_ptikre:
    ip = -1;
    goto L100;
L999:
    return 0;
} /* ptikr_ */

/* Subroutine */ int ptikr_(D_fp ak, doublereal *u0, doublereal *a, 
	doublereal *b, doublereal *c__, doublereal *d__, doublereal *l1, 
	doublereal *l2, integer *n, doublereal *z__, doublereal *an, 
	doublereal *dl, doublereal *hh, doublereal *c1, integer *imax, 
	doublereal *alpha, doublereal *u, integer *nu, integer *ierr)
{
    return ptikr_0_(0, ak, u0, a, b, c__, d__, l1, l2, n, z__, an, dl, hh, c1,
	     imax, alpha, u, nu, ierr);
    }

/* Subroutine */ int ptikre_(void)
{
    return ptikr_0_(1, (D_fp)0, (doublereal *)0, (doublereal *)0, (doublereal 
	    *)0, (doublereal *)0, (doublereal *)0, (doublereal *)0, (
	    doublereal *)0, (integer *)0, (doublereal *)0, (doublereal *)0, (
	    doublereal *)0, (doublereal *)0, (doublereal *)0, (integer *)0, (
	    doublereal *)0, (doublereal *)0, (integer *)0, (integer *)0);
    }

/* Subroutine */ int ptikr1_(D_fp ak, doublereal *are, doublereal *aim, 
	doublereal *zre, doublereal *zim, doublereal *ure, doublereal *uim, 
	doublereal *w, integer *n, doublereal *alp, doublereal *l1, 
	doublereal *l2, doublereal *beta, doublereal *ro, doublereal *a, 
	doublereal *del, doublereal *hh, integer *ipar, doublereal *epro, 
	integer *iq, integer *imax, integer *ierr)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sin(doublereal), sqrt(doublereal);

    /* Local variables */
    static doublereal h__;
    static integer i__, m;
    static doublereal p, x, a0, a1, a2, c1, c2, c3, f1, f2, f3, r0, r1, u2, 
	    ba, ab, ha, dq, dr, zz, arg, ssi, znor;
    extern /* Subroutine */ int ftf1c_(doublereal *, doublereal *, integer *, 
	    integer *, integer *, doublereal *);

    /* Parameter adjustments */
    --w;
    --uim;
    --ure;
    --zim;
    --zre;
    --aim;
    --are;

    /* Function Body */
    h__ = *a / *n;
    ha = h__ / *n;
/* * checking if it is the first call */
    if (*ipar == -1) {
	goto L2;
    }
/* * getting equation kernel */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	arg = (i__ - *n / 2 - 1) * h__ + (*l1 + *l2) * .5f;
	are[i__] = (*ak)(&arg) * h__;
	aim[i__] = 0.f;
	if (arg < *l1 || arg > *l2) {
	    are[i__] = 0.f;
	}
/* Computing 2nd power */
	d__1 = 2.f / h__ * sin(3.14159265358 / *n * (i__ - 1));
	w[i__] = d__1 * d__1;
/* L1: */
    }
    p = 1.f;
/* * Fourier transformation of the kernel and right side */
    ftf1c_(&are[1], &aim[1], n, &c__1, &c__1, &p);
    ftf1c_(&ure[1], &uim[1], n, &c__1, &c__1, &p);
L2:
/* * getting ALP so, that RO(ALP) > 0 */
/* * IQ - iteration number */
    *iq = 0;
L77:
    ++(*iq);
/* * discrepancy calculation */
    f1 = 0.f;
    f2 = 0.f;
    i__1 = *n;
    for (m = 1; m <= i__1; ++m) {
/* Computing 2nd power */
	d__1 = ure[m];
/* Computing 2nd power */
	d__2 = uim[m];
	u2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	d__1 = are[m];
/* Computing 2nd power */
	d__2 = aim[m];
	a2 = d__1 * d__1 + d__2 * d__2;
	x = w[m] + 1.f;
	if (a2 == 0.f) {
	    goto L42;
	}
	ba = x / (a2 + *alp * x);
	ab = 1.f - *alp * ba;
/* Computing 2nd power */
	d__1 = ba * *alp;
	c1 = ha * u2 * (d__1 * d__1);
	c2 = ha * u2 * ab * ba;
	f1 += c1;
	f2 += c2;
	goto L44;
L42:
	f1 += ha * u2;
L44:
	;
    }
/* * calculating generalized discrepancy */
/* * to check if RO(ALP)>0 */
    *beta = f1;
/* Computing 2nd power */
    d__1 = *del + *hh * sqrt(f2);
    *ro = f1 - d__1 * d__1;
/* * if EPRO=0 is set then finish */
    if (*epro == 0.f) {
	goto L10;
    }
/* * if ALP=0.0 is set then finish */
    if (*alp == 0.f) {
	goto L68;
    }
    if (*ro > 0.f) {
	goto L33;
    }
/* * go to solution calculation if there is no */
/* * parameter ALP with RO(ALP) > 0 */
    if (*iq > *imax) {
	goto L65;
    }
    *alp *= 2.f;
    goto L77;
/* * starting Newton method calculations */
L33:
    *iq = 0;
L3:
    ++(*iq);
/* * discrepancy calculations */
    f1 = 0.f;
    f2 = 0.f;
    f3 = 0.f;
    i__1 = *n;
    for (m = 1; m <= i__1; ++m) {
/* Computing 2nd power */
	d__1 = ure[m];
/* Computing 2nd power */
	d__2 = uim[m];
	u2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	d__1 = are[m];
/* Computing 2nd power */
	d__2 = aim[m];
	a2 = d__1 * d__1 + d__2 * d__2;
	x = w[m] + 1.f;
	if (a2 == 0.f) {
	    goto L41;
	}
	ba = x / (a2 + *alp * x);
	ab = 1.f - *alp * ba;
/* Computing 2nd power */
	d__1 = ba * *alp;
	c1 = ha * u2 * (d__1 * d__1);
	c2 = ha * u2 * ab * ba;
	c3 = c1 * 2.f * ab;
	f1 += c1;
	f2 += c2;
	f3 += c3;
	goto L4;
L41:
	f1 += ha * u2;
L4:
	;
    }
/* * calculation discrepancy -BETA, */
/* * solution norm in W21 -ZNOR, */
/* * generalized discrepancy - RO */
/* * and its derivative - DR */
    *beta = f1;
    znor = sqrt(f2);
/* Computing 2nd power */
    d__1 = *del + *hh * znor;
    *ro = *beta - d__1 * d__1;
    if (*alp == 0.f) {
	goto L68;
    }
    dr = -f3 * *alp - (*del + *hh * znor) * *hh * f3 / znor;
/* * calculate solution if accuracy acheaved */
    if (abs(*ro) < *epro) {
	goto L10;
    }
/* * starting chord method if generalized discrepancy */
/* * is not convex */
    if (*ro < 0.f) {
	goto L61;
    }
/* * getting new argument by Newton method */
    dq = -(*ro) / dr;
    a1 = *alp;
    r1 = *ro;
    *alp = 1.f / (1.f / *alp + dq);
    if (*iq >= *imax) {
	goto L66;
    }
    goto L3;
/* * chord method */
L61:
/* * change interval if RO<0 */
L6:
    a0 = *alp;
    r0 = *ro;
L7:
    ++(*iq);
/* * getting new argument by chord method */
    *alp = a0 * a1 * (r0 - r1) / (a0 * r0 - a1 * r1);
/* * calculation discrepancies */
/* L8: */
    f1 = 0.f;
    f2 = 0.f;
    i__1 = *n;
    for (m = 1; m <= i__1; ++m) {
/* Computing 2nd power */
	d__1 = ure[m];
/* Computing 2nd power */
	d__2 = uim[m];
	u2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	d__1 = are[m];
/* Computing 2nd power */
	d__2 = aim[m];
	a2 = d__1 * d__1 + d__2 * d__2;
	x = w[m] + 1.f;
	if (a2 == 0.f) {
	    goto L91;
	}
	ba = x / (a2 + *alp * x);
	ab = 1.f - *alp * ba;
/* Computing 2nd power */
	d__1 = ba * *alp;
	c1 = ha * u2 * (d__1 * d__1);
	c2 = ha * u2 * ab * ba;
	f1 += c1;
	f2 += c2;
	goto L9;
L91:
	f1 += ha * u2;
L9:
	;
    }
/* * generalized discrepancy and solution norm */
    *beta = f1;
    znor = sqrt(f2);
/* Computing 2nd power */
    d__1 = *del + *hh * znor;
    *ro = *beta - d__1 * d__1;
    if (*alp == 0.f) {
	goto L68;
    }
/* * calculation solution if accuracy achieved */
    if (abs(*ro) < *epro) {
	goto L11;
    }
    if (*iq == *imax) {
	goto L67;
    }
    if (*ro < 0.f) {
	goto L6;
    }
/* * change interval if RO>0 */
    a1 = *alp;
    r1 = *ro;
    goto L7;
L65:
/* * RO(ALP) <= 0 for all regularization parameters */
    *ierr = 65;
    goto L999;
L66:
/* * IMAX iterations by Newton method performed */
    *ierr = 66;
    goto L999;
L67:
/* * IMAX iterations by chord method performed */
    *ierr = 67;
    goto L999;
L68:
/* * ALP=0.0 is set or found */
    *ierr = 68;
    goto L999;
L11:
/* * solution is found by means of chord method */
    *ierr = 1;
    goto L999;
L10:
/* * solution is found by Newton method */
    *ierr = 0;
L999:
/* * calculation Fourier image of solution */
    ssi = -1.f;
    i__1 = *n;
    for (m = 1; m <= i__1; ++m) {
	ssi = -ssi;
/* Computing 2nd power */
	d__1 = are[m];
/* Computing 2nd power */
	d__2 = aim[m];
	zz = *n * (d__1 * d__1 + d__2 * d__2 + *alp * (w[m] + 1.f));
	if (zz != 0.f) {
	    goto L111;
	}
	zre[m] = 0.f;
	zim[m] = 0.f;
	goto L12;
L111:
	zre[m] = ssi * (are[m] * ure[m] + aim[m] * uim[m]) / zz;
	zim[m] = ssi * (are[m] * uim[m] - aim[m] * ure[m]) / zz;
L12:
	;
    }
/* * inverse Fourier transformation of solution */
    p = -1.f;
    ftf1c_(&zre[1], &zim[1], n, &c__1, &c__1, &p);
    return 0;
} /* ptikr1_ */

