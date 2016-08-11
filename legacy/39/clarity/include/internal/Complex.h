/* 
 * Clarity is Copyright 2008 Center for Integrated Systems for Microscopy, 
 * Copyright 2008 University of North Carolina at Chapel Hill.
 *
 * Clarity is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Public License as published by the Free Software 
 * Foundation; either version 2 of the License, or (at your option) any 
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA. You can also find 
 * the GPL on the GNU web site (http://www.gnu.org/copyleft/gpl.html).
 *
 * File name: Complex.h
 * Author: Cory Quammen <cquammen@cs.unc.edu>
 */


#ifndef __Complex_h_
#define __Complex_h_

/**
 * Returns conjugate of a complex number.
 * 
 * @param c1     Ccomplex number to conjugate.
 * @param result Return parameter storing the conjugate.
 *               Can be the same as c1.
 */
inline void ComplexConjugate(double* c1, double* result) {
   result[0] =  c1[0];
   result[1] = -c1[1];
}


/**
 * Adds two complex numbers.
 * 
 * @param c1     First complex number.
 * @param c2     Second complex number. Can be the same as the first.
 * @param result Storage for result of addition.
 *               Can be the same as c1, c2, or both.
 */
inline void ComplexAdd(double* c1, double* c2, double* result) {
   result[0] = c1[0] + c2[0];
   result[1] = c1[1] + c2[1];
}


/**
 * Multiplies two complex numbers.
 * 
 * @param c1     First complex number.
 * @param c2     Second complex number.
 * @param result Storage for result of multiplication.
 *               Can be the same as c1, c2, or both.
 */
inline void ComplexMultiply(double* c1, double* c2, double* result) {
   // (a + bi) * (c + di) = (ac - bd) + i(ad + bc)
   double a = c1[0];
   double b = c1[1];
   double c = c2[0];
   double d = c2[1];
   result[0] = a*c - b*d;
   result[1] = a*d + b*c;
}


/**
 * Multiplies a complex number with a real number.
 * 
 * @param c      Complex number to multiply.
 * @param real   Real multiplier.
 * @param result Storage for result of multiplication.
 */
inline void ComplexMultiply(double* c, double real, double* result) {
   result[0] = c[0]*real;
   result[1] = c[1]*real;
}


/**
 * Multiplies two complex numbers and scales the result by a real number.
 * 
 * @param c1     First complex number.
 * @param c2     Second complex number.
 * @param scale  Real scalar multiplier.
 * @param result Storage for result of the operation.
 */
inline void ComplexMultiplyAndScale(double* c1, double* c2, double scale, double* result) {
   double a = c1[0];
   double b = c1[1];
   double c = c2[0];
   double d = c2[1];
   result[0] = scale*(a*c - b*d);
   result[1] = scale*(a*d + b*c);
}


/**
 * Computes squared magnitude of a complex number, storing real result as a
 * complex number (zero imaginary component).
 * 
 * @param c		 Complex number whose magnitude should be taken.
 * @param result Complex number storing real result.
 */
inline void ComplexMagnitudeSquared(double* c, double* result) {
  // a^2 + b^2
  double a = c[0];
  double b = c[1];
  result[0] = a*a + b*b;
  result[1] = 0.0f;
}


/**
 * Computes squared magnitude of a complex number, returning real result.
 * 
 * @param c Complex number whose magnitude should be taken.
 * @return Squared magnitude.
 */
inline double ComplexMagnitudeSquared(double* c) {
  double a = c[0];
  double b = c[1];
  return ((a*a) + (b*b));
}


/**
 * Computes inverse of complex number.
 * 
 * @param c      Complex number whose inverse should be computed.
 * @param result Storage for the inverted number.
 */
inline void ComplexInverse(double* c, double* result) {
  double a = c[0];
  double b = c[1];
  double mag = a*a + b*b;
  result[0] = a / mag;
  result[1] = -b / mag;
}


#endif // __Complex_h_
