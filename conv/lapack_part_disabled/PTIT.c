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

/* Subroutine */ int ptitr_0_(int n__, U_fp ak, doublereal *u0, doublereal *
	alim, integer *n1, integer *n2, doublereal *z__, doublereal *dl, 
	doublereal *hh, doublereal *c1, doublereal *alpha, doublereal *an, 
	doublereal *u, integer *nu, integer *imax, integer *ierr)
{
    /* System generated locals */
    integer u0_dim1, u0_offset, z_dim1, z_offset;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal t1, t2;
    static integer ip, iq;
    static doublereal om;
    static integer nw1, nw2, nai, nar, nui, nzi, nqu, nur, nmax;
    static doublereal epro;
    extern /* Subroutine */ int pticr1_(doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *), pticr2_(doublereal *, 
	    doublereal *, integer *), ptikr3_(U_fp, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *,
	     integer *, integer *, integer *);

/* *   working array mapping */
/* * name       length    what */
/* *  ARE       N1*N2    real part of kernel Fourier image */
/* *  AMIN      N1*N2    imaginary part of kernel Fourier image */
/* *  URE       N1*N2    real part of right side Fourier image */
/* *  UIM       N1*N2    imaginary part of right side F. image */
/* *  ZIM       N1*N2    imaginary part of solition */
/* * W1 ˆ W2    N1 ˆ N2  stbilizer */
    /* Parameter adjustments */
    if (alim) {
	--alim;
	}
    if (u0) {
	u0_dim1 = *n1;
	u0_offset = 1 + u0_dim1;
	u0 -= u0_offset;
	}
    if (z__) {
	z_dim1 = *n1;
	z_offset = 1 + z_dim1;
	z__ -= z_offset;
	}
    if (u) {
	--u;
	}

    /* Function Body */
    switch(n__) {
	case 1: goto L_ptitre;
	}

    ip = 1;
/* *  IP - start/continue mode flag */
/* * start mode IP=+1 */
/* * reenter mode through PTIKRE IP=-1 */
    epro = 0.f;
    if (*c1 > 1.f) {
	epro = (*c1 - 1.f) * *dl;
    }
/* * EPRO - discrepancy equation accuracy */
/* * if EPRO=0, the only extremal with the specified ALFA */
L100:
/* * calculating solution support */
    alim[1] = alim[5] - (alim[9] + alim[10]) * .5f;
    alim[2] = alim[6] - (alim[9] + alim[10]) * .5f;
    alim[3] = alim[7] - (alim[11] + alim[12]) * .5f;
    alim[4] = alim[8] - (alim[11] + alim[12]) * .5f;
/* * T1, T2 - periods */
    t1 = alim[6] - alim[5];
    t2 = alim[8] - alim[7];
/* * arrays starts formimg */
    nar = 1;
    nqu = *n1 * *n2;
    nai = nar + nqu;
    nw1 = nai + nqu;
    nw2 = nw1 + *n1;
    nzi = nw2 + *n2;
    nur = nzi + nqu;
    nui = nur + nqu;
    nmax = nui + nqu;
/* * check the length of working array */
    if (nmax - 1 > *nu) {
	goto L64;
    }
    if (ip == -1) {
	goto L101;
    }
/* * create right-hand side */
    pticr1_(&u0[u0_offset], &u0[u0_offset], &u[nur], &nqu, &c_b5);
    pticr2_(&u[nui], &c_b5, &nqu);
L101:
/* * solving the equation */
    d__1 = sqrt(*dl);
    d__2 = sqrt(*hh);
    ptikr3_((U_fp)ak, &u[nar], &u[nai], &z__[z_offset], &u[nzi], &u[nur], &u[
	    nui], &u[nw1], &u[nw2], n1, n2, alpha, &alim[1], an, &om, &t1, &
	    t2, &d__1, &d__2, &ip, &epro, &iq, imax, ierr);
    goto L999;
L64:
/* * working array is too short */
    *ierr = 64;
    goto L999;
/* * PTITRE entry for the reentering to solve */
/* * equation with the same kernel and right-hand side */

L_ptitre:
    ip = -1;
    goto L100;
L999:
    return 0;
} /* ptitr_ */

/* Subroutine */ int ptitr_(U_fp ak, doublereal *u0, doublereal *alim, 
	integer *n1, integer *n2, doublereal *z__, doublereal *dl, doublereal 
	*hh, doublereal *c1, doublereal *alpha, doublereal *an, doublereal *u,
	 integer *nu, integer *imax, integer *ierr)
{
    return ptitr_0_(0, ak, u0, alim, n1, n2, z__, dl, hh, c1, alpha, an, u, 
	    nu, imax, ierr);
    }

/* Subroutine */ int ptitre_(void)
{
    return ptitr_0_(1, (U_fp)0, (doublereal *)0, (doublereal *)0, (integer *)
	    0, (integer *)0, (doublereal *)0, (doublereal *)0, (doublereal *)
	    0, (doublereal *)0, (doublereal *)0, (doublereal *)0, (doublereal 
	    *)0, (integer *)0, (integer *)0, (integer *)0);
    }

/* Subroutine */ int ptikr3_(U_fp ak, doublereal *are, doublereal *aim,
	doublereal *zre, doublereal *zim, doublereal *ure, doublereal *uim, 
	doublereal *w1, doublereal *w2, integer *n1, integer *n2, doublereal *
	alp, doublereal *alim, doublereal *beta, doublereal *ro, doublereal *
	t1, doublereal *t2, doublereal *del, doublereal *hh, integer *ipar, 
	doublereal *epro, integer *iq, integer *imax, integer *ierr)
{
    /* System generated locals */
    integer are_dim1, are_offset, zre_dim1, zre_offset, ure_dim1, ure_offset, 
	    aim_dim1, aim_offset, zim_dim1, zim_offset, uim_dim1, uim_offset, 
	    i__1, i__2;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double sin(doublereal), sqrt(doublereal);

    /* Local variables */
    static integer i__, j, m, n;
    static doublereal p, x, a0, a1, a2, c1, c2, c3, f1, f2, h1, h2, f3, r0, 
	    r1, u2, ba, ab, ha, dq, dr, zz, ssi, arg1, arg2, znor;
    extern /* Subroutine */ int ftftc_(doublereal *, doublereal *, integer *, 
	    integer *, doublereal *);

    /* Parameter adjustments */
    --w1;
    --w2;
    uim_dim1 = *n1;
    uim_offset = 1 + uim_dim1;
    uim -= uim_offset;
    ure_dim1 = *n1;
    ure_offset = 1 + ure_dim1;
    ure -= ure_offset;
    zim_dim1 = *n1;
    zim_offset = 1 + zim_dim1;
    zim -= zim_offset;
    zre_dim1 = *n1;
    zre_offset = 1 + zre_dim1;
    zre -= zre_offset;
    aim_dim1 = *n1;
    aim_offset = 1 + aim_dim1;
    aim -= aim_offset;
    are_dim1 = *n1;
    are_offset = 1 + are_dim1;
    are -= are_offset;
    --alim;

    /* Function Body */
    h1 = *t1 / *n1;
    h2 = *t2 / *n2;
    ha = h1 / *n1 * h2 / *n2;
/* * check if it is the first entry */
    if (*ipar == -1) {
	goto L2;
    }
/* * setting equation kernel */
    i__1 = *n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *n2;
	for (j = 1; j <= i__2; ++j) {
	    arg1 = (i__ - *n1 / 2 - 1) * h1 + (alim[9] + alim[10]) * .5f;
	    arg2 = (j - *n2 / 2 - 1) * h2 + (alim[11] + alim[12]) * .5f;
	    are[i__ + j * are_dim1] = (*ak)(&arg1, &arg2) * h1 * h2;
	    aim[i__ + j * aim_dim1] = 0.f;
	    if (arg1 < alim[9] || arg1 > alim[10] || arg2 < alim[11] || arg2 
		    > alim[12]) {
		are[i__ + j * are_dim1] = 0.f;
	    }
/* Computing 2nd power */
	    d__1 = 2.f / h1 * sin(3.14159265359 / *n1 * (i__ - 1));
	    w1[i__] = d__1 * d__1;
/* Computing 2nd power */
	    d__1 = 2.f / h2 * sin(3.14159265359 / *n2 * (j - 1));
	    w2[j] = d__1 * d__1;
/* L1: */
	}
    }
/* * Fourier transformation of the kernel and right side */
    p = 1.f;
    ftftc_(&are[are_offset], &aim[aim_offset], n1, n2, &p);
    p = 1.f;
    ftftc_(&ure[ure_offset], &uim[uim_offset], n1, n2, &p);
L2:
/* * find ALP so PO(ALP) > 0 */
    *iq = 0;
L77:
    ++(*iq);
/* * calculating discrepancies */
    f1 = 0.f;
    f2 = 0.f;
    i__2 = *n1;
    for (m = 1; m <= i__2; ++m) {
	i__1 = *n2;
	for (n = 1; n <= i__1; ++n) {
/* Computing 2nd power */
	    d__1 = ure[m + n * ure_dim1];
/* Computing 2nd power */
	    d__2 = uim[m + n * uim_dim1];
	    u2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	    d__1 = are[m + n * are_dim1];
/* Computing 2nd power */
	    d__2 = aim[m + n * aim_dim1];
	    a2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	    d__1 = w1[m] + w2[n];
	    x = d__1 * d__1 + 1.f;
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
    }
/* * calculating generalized discrepancy */
/* * to check if RO(ALP) > 0 */
    *beta = f1;
/* Computing 2nd power */
    d__1 = *del + *hh * sqrt(f2);
    *ro = f1 - d__1 * d__1;
/* * if EPRO=0 exit the program */
    if (*epro == 0.f) {
	goto L10;
    }
/* * if ALP=0.0 then exit the program */
    if (*alp == 0.f) {
	goto L68;
    }
    if (*ro > 0.f) {
	goto L33;
    }
/* * cannot find initial regularization parameter */
/* * RO(ALP) > 0 */
    if (*iq > *imax) {
	goto L65;
    }
    *alp *= 2.f;
    goto L77;
/* * starting the Newton method */
L33:
    *iq = 0;
L3:
    ++(*iq);
/* * discrepancies calculation */
    f1 = 0.f;
    f2 = 0.f;
    f3 = 0.f;
    i__1 = *n1;
    for (m = 1; m <= i__1; ++m) {
	i__2 = *n2;
	for (n = 1; n <= i__2; ++n) {
/* Computing 2nd power */
	    d__1 = ure[m + n * ure_dim1];
/* Computing 2nd power */
	    d__2 = uim[m + n * uim_dim1];
	    u2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	    d__1 = are[m + n * are_dim1];
/* Computing 2nd power */
	    d__2 = aim[m + n * aim_dim1];
	    a2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	    d__1 = w1[m] + w2[n];
	    x = d__1 * d__1 + 1.f;
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
    }
/* * discrepancy - */
/* * solution norm in W21 -ZNOR, */
/* * generalized discrepancy -RO */
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
/* * accuracy achieved */
    if (abs(*ro) < *epro) {
	goto L10;
    }
/* * switch to the chord method */
    if (*ro < 0.f) {
	goto L61;
    }
/* * calculating new argument */
/* * Newton method formulae */
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
/* * changing interval if R0 < 0 */
L6:
    a0 = *alp;
    r0 = *ro;
L7:
    ++(*iq);
/* * getting new argument by chord method */
    *alp = a0 * a1 * (r0 - r1) / (a0 * r0 - a1 * r1);
/* * discrepancy calculations */
    f1 = 0.f;
    f2 = 0.f;
    i__2 = *n1;
    for (m = 1; m <= i__2; ++m) {
	i__1 = *n2;
	for (n = 1; n <= i__1; ++n) {
/* Computing 2nd power */
	    d__1 = ure[m + n * ure_dim1];
/* Computing 2nd power */
	    d__2 = uim[m + n * uim_dim1];
	    u2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	    d__1 = are[m + n * are_dim1];
/* Computing 2nd power */
	    d__2 = aim[m + n * aim_dim1];
	    a2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	    d__1 = w1[m] + w2[n];
	    x = d__1 * d__1 + 1.f;
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
    }
/* * getting discrepancy, generalized discrepancy */
/* * and solution norm */
    *beta = f1;
    znor = sqrt(f2);
/* Computing 2nd power */
    d__1 = *del + *hh * znor;
    *ro = *beta - d__1 * d__1;
    if (*alp == 0.f) {
	goto L68;
    }
/* * accuracy achieved */
    if (abs(*ro) < *epro) {
	goto L11;
    }
    if (*iq == *imax) {
	goto L67;
    }
    if (*ro < 0.f) {
	goto L6;
    }
/* * changing interval if RO > 0 */
    a1 = *alp;
    r1 = *ro;
    goto L7;
L65:
/* * cannot find ALP so that RO(ALP) be greater zero */
    *ierr = 65;
    goto L999;
L66:
/* * IMAX Newton iterations made */
    *ierr = 66;
    goto L999;
L67:
/* * IMAX chord iterations made */
    *ierr = 67;
    goto L999;
L68:
/* * ALP=0.0 is specified or found */
    *ierr = 68;
    goto L999;
L11:
/* * solution is found by chord method */
    *ierr = 1;
    goto L999;
L10:
/* * solution is found by Newton method */
    *ierr = 0;
L999:
/* * getting solution Fourier image */
    ssi = 1.f;
    i__1 = *n1;
    for (m = 1; m <= i__1; ++m) {
	ssi = -ssi;
	i__2 = *n2;
	for (n = 1; n <= i__2; ++n) {
	    ssi = -ssi;
/* Computing 2nd power */
	    d__1 = are[m + n * are_dim1];
/* Computing 2nd power */
	    d__2 = aim[m + n * aim_dim1];
/* Computing 2nd power */
	    d__3 = w1[m] + w2[n];
	    zz = *n1 * *n2 * (d__1 * d__1 + d__2 * d__2 + *alp * (d__3 * d__3 
		    + 1.f));
	    if (zz != 0.f) {
		goto L111;
	    }
	    zre[m + n * zre_dim1] = 0.f;
	    zim[m + n * zim_dim1] = 0.f;
	    goto L12;
L111:
	    zre[m + n * zre_dim1] = ssi * (are[m + n * are_dim1] * ure[m + n *
		     ure_dim1] + aim[m + n * aim_dim1] * uim[m + n * uim_dim1]
		    ) / zz;
	    zim[m + n * zim_dim1] = ssi * (are[m + n * are_dim1] * uim[m + n *
		     uim_dim1] - aim[m + n * aim_dim1] * ure[m + n * ure_dim1]
		    ) / zz;
L12:
	    ;
	}
    }
/* * inverse Fourier transformation */
    p = -1.f;
    ftftc_(&zre[zre_offset], &zim[zim_offset], n1, n2, &p);
    return 0;
} /* ptikr3_ */

/* Subroutine */ int ftftc_(doublereal *are, doublereal *aim, integer *n1, 
	integer *n2, doublereal *p)
{
    /* System generated locals */
    integer are_dim1, are_offset, aim_dim1, aim_offset, i__1, i__2;

    /* Local variables */
    static integer i__, j;
    extern /* Subroutine */ int ftf1c_(doublereal *, doublereal *, integer *, 
	    integer *, integer *, doublereal *);

/* * Fast two-dimensional Fourier transformation */
/* * if P > 0 - direct transformation */
/* * if P < 0 - inverse transformation */
/* * P - must be called by name */
    /* Parameter adjustments */
    aim_dim1 = *n1;
    aim_offset = 1 + aim_dim1;
    aim -= aim_offset;
    are_dim1 = *n1;
    are_offset = 1 + are_dim1;
    are -= are_offset;

    /* Function Body */
    i__1 = *n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *n1 * *n2;
	ftf1c_(&are[are_offset], &aim[aim_offset], &i__2, &i__, n1, p);
/* L1: */
    }
    i__1 = *n2;
    for (j = 1; j <= i__1; ++j) {
	ftf1c_(&are[j * are_dim1 + 1], &aim[j * aim_dim1 + 1], n1, &c__1, &
		c__1, p);
/* L2: */
    }
    return 0;
} /* ftftc_ */

/* Subroutine */ int ftf1c_(doublereal *xare, doublereal *xaim, integer *n, 
	integer *in, integer *k, doublereal *p)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    double sin(doublereal), cos(doublereal);

    /* Local variables */
    static doublereal a, b, c__;
    static integer i__, j, m;
    static doublereal r__, s, t;
    static integer n1, n2, ii;
    static doublereal co;
    static integer jj, mm;
    static doublereal si;

    /* Parameter adjustments */
    --xaim;
    --xare;

    /* Function Body */
    n1 = *n / 2;
    mm = n1 / 2;
    n2 = n1 + *k;
    j = *in;
    jj = j;
L1:
    j += *k;
    if (j - n1 <= 0) {
	goto L2;
    } else {
	goto L10;
    }
L2:
    ii = jj + n1;
    r__ = xare[j];
    xare[j] = xare[ii];
    xare[ii] = r__;
    r__ = xaim[j];
    xaim[j] = xaim[ii];
    xaim[ii] = r__;
    j += *k;
    m = mm;
L3:
    if (jj - m <= 0) {
	goto L5;
    } else {
	goto L4;
    }
L4:
    jj -= m;
    m /= 2;
    goto L3;
L5:
    jj += m;
    if (jj - j <= 0) {
	goto L1;
    } else {
	goto L6;
    }
L6:
    r__ = xare[j];
    xare[j] = xare[jj];
    xare[jj] = r__;
    r__ = xaim[j];
    xaim[j] = xaim[jj];
    xaim[jj] = r__;
    i__ = j + n2;
    ii = jj + n2;
    r__ = xare[i__];
    xare[i__] = xare[ii];
    xare[ii] = r__;
    r__ = xaim[i__];
    xaim[i__] = xaim[ii];
    xaim[ii] = r__;
    goto L1;
L10:
    i__ = *k;
    t = 3.14159265359f;
    if (*p < 0.) {
	goto L13;
    } else if (*p == 0) {
	goto L17;
    } else {
	goto L11;
    }
L11:
    t = -t;
L13:
    *p = -t;
L14:
    si = 0.f;
    co = 1.f;
    s = sin(t);
    c__ = cos(t);
    t *= .5f;
    ii = i__;
    i__ += i__;
    i__1 = ii;
    i__2 = *k;
    for (m = *in; i__2 < 0 ? m >= i__1 : m <= i__1; m += i__2) {
	i__3 = *n;
	i__4 = i__;
	for (j = m; i__4 < 0 ? j >= i__3 : j <= i__3; j += i__4) {
	    jj = j + ii;
	    a = xare[jj];
	    b = xaim[jj];
	    r__ = a * co - b * si;
	    xare[jj] = xare[j] - r__;
	    xare[j] += r__;
	    r__ = b * co + a * si;
	    xaim[jj] = xaim[j] - r__;
/* L15: */
	    xaim[j] += r__;
	}
	r__ = c__ * co - s * si;
	si = c__ * si + s * co;
/* L16: */
	co = r__;
    }
    if (i__ - *n >= 0) {
	goto L17;
    } else {
	goto L14;
    }
L17:
    return 0;
} /* ftf1c_ */

