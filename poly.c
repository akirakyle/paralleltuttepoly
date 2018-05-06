#include "poly.h"

poly *
new_poly(int x_deg, int y_deg)
{
  poly *p = (poly *)calloc(1, sizeof(poly));
  p->c = (long *)calloc((x_deg+1)*(y_deg+1), sizeof(long));
  p->x_deg = x_deg;
  p->y_deg = y_deg;
  return p;
}

void
free_poly(poly *p)
{
  free(p->c);
  free(p);
}

long *
poly_coeff(poly *A, int x, int y)
{
  if (x > A->x_deg) return 0;
  if (y > A->y_deg) return 0;
  return &A->c[y + x*A->y_deg];
}

void
print_poly(FILE *fout, poly *p)
{
  for (int x = 0; x <= p->x_deg; x++)
    for (int y = 0; y <= p->y_deg; y++)
      if (x != p->x_deg || y != p->y_deg)
        fprintf(fout, "%ldx^%dy^%d + ", *poly_coeff(p, x, y), x, y);
  fprintf(fout, "%ldx^%dy^%d\n",
          *poly_coeff(p, p->x_deg, p->y_deg), p->x_deg, p->y_deg);
}

poly *
poly_add(poly *A, poly *B)
{
  poly *C = new_poly(MAX(A->x_deg, B->x_deg), MAX(A->y_deg, B->y_deg));
  for (int x = 0; x <= MIN(A->x_deg, B->x_deg); x++)
    for (int y = 0; y <= MIN(A->y_deg, B->y_deg); y++)
      *poly_coeff(C, x, y) = *poly_coeff(A, x, y) + *poly_coeff(B, x, y);
  free_poly(A); free_poly(B);
  return C;
}

poly *
poly_mult(poly *A, poly *B)
{
  poly *C = new_poly((A->x_deg + B->x_deg), (A->y_deg + B->y_deg));

  for (int ax = 0; ax <= A->x_deg; ax++)
    for (int ay = 0; ay <= A->y_deg; ay++)
      for (int bx = 0; bx <= B->x_deg; bx++)
        for (int by = 0; by <= B->y_deg; by++)
          *poly_coeff(C, ax+bx, ay+by) = ( (*poly_coeff(A, ax, ay)) *
                                           (*poly_coeff(B, bx, by)) );
  free_poly(A); free_poly(B);
  return C;
}
