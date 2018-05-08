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
  //if (x > A->x_deg) return 0;
  //if (y > A->y_deg) return 0;
  return &A->c[y + x*(A->y_deg+1)];
}

void
print_poly_term(FILE *fout, poly *p, int x, int y, char *extra)
{
  if (*poly_coeff(p,x,y) == 0) return;
  else if (x == 0 && y == 0)
    fprintf(fout, "%s%ld", extra, *poly_coeff(p, x, y));
  else if (x == 0 && y == 1)
    fprintf(fout, "%s%ld*y", extra, *poly_coeff(p, x, y));
  else if (x == 1 && y == 0)
    fprintf(fout, "%s%ld*x", extra, *poly_coeff(p, x, y));
  else if (x == 1 && y == 1)
    fprintf(fout, "%s%ld*x*y", extra, *poly_coeff(p, x, y));
  else if (x == 0)
    fprintf(fout, "%s%ld*y^%d", extra, *poly_coeff(p, x, y), y);
  else if (y == 0)
    fprintf(fout, "%s%ld*x^%d", extra, *poly_coeff(p, x, y), x);
  else if (x == 1)
    fprintf(fout, "%s%ld*x*y^%d", extra, *poly_coeff(p, x, y), y);
  else if (y == 1)
    fprintf(fout, "%s%ld*x^%d*y", extra, *poly_coeff(p, x, y), x);
  else
    fprintf(fout, "%s%ld*x^%d*y^%d", extra, *poly_coeff(p, x, y), x, y);
}

void
print_poly(FILE *fout, poly *p)
{
  fprintf(fout, "T = ");
  int first_flag = 1;
  for (int x = 0; x <= p->x_deg; x++)
    for (int y = 0; y <= p->y_deg; y++)
      if (first_flag && *poly_coeff(p,x,y) != 0)
        {print_poly_term(fout, p, x, y, ""); first_flag = 0;}
      else
        print_poly_term(fout, p, x, y, " + ");
  fprintf(fout," :\n");
}

poly *
poly_add(poly *A, poly *B)
{
  poly *C = new_poly(MAX(A->x_deg, B->x_deg), MAX(A->y_deg, B->y_deg));
  for (int x = 0; x <= C->x_deg; x++)
    for (int y = 0; y <= C->y_deg; y++)
      {
        long a = 0; long b = 0;
        if ((x <= A->x_deg) && (y <= A->y_deg))
          a = *poly_coeff(A, x, y);
        if ((x <= B->x_deg) && (y <= B->y_deg))
          b = *poly_coeff(B, x, y);
        *poly_coeff(C, x, y) = a + b;
      }
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
          *poly_coeff(C, ax+bx, ay+by) += ( (*poly_coeff(A, ax, ay)) *
                                            (*poly_coeff(B, bx, by)) );
  free_poly(A); free_poly(B);
  return C;
}
