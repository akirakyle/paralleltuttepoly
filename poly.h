#ifndef  _POLY_H_    /* only process this file once */
#define  _POLY_H_

/*
  A data structure for handling polynomials
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct poly
{
  long *c;
  int y_deg;
  int x_deg;
} poly;

poly * new_poly(int x_deg, int y_deg);
void free_poly(poly *p);
void print_poly(FILE *fout, poly *p);

long * poly_coeff(poly *A, int x, int y);
poly * poly_add(poly *A, poly *B);
poly * poly_mult(poly *A, poly *B);

#endif /* _POLY_H_  */
