#include "blaswrap.h"
#include "f2c.h"

/* Subroutine */ int drot_(const integer *n, doublereal *dx, const integer *incx, 
	doublereal *dy, const integer *incy, const doublereal *c__, const doublereal *s)
{
    /* System generated locals */
    integer i__1;
    /* Local variables */
    _THREAD_STATIC_ integer i__, ix, iy;
    _THREAD_STATIC_ doublereal dtemp;
/*  Purpose   
    =======   
       applies a plane rotation.   
       jack dongarra, linpack, 3/11/78.   
       modified 12/3/93, array(1) declarations changed to array(*)   
       Parameter adjustments */
    --dy;
    --dx;
    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }
/*       code for unequal increments or equal increments not equal   
           to 1 */
    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dtemp = *c__ * dx[ix] + *s * dy[iy];
	dy[iy] = *c__ * dy[iy] - *s * dx[ix];
	dx[ix] = dtemp;
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    return 0;
/*       code for both increments equal to 1 */
L20:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dtemp = *c__ * dx[i__] + *s * dy[i__];
	dy[i__] = *c__ * dy[i__] - *s * dx[i__];
	dx[i__] = dtemp;
/* L30: */
    }
    return 0;
} /* drot_ */

