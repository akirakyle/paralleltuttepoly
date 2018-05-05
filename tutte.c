#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

 /* MAXN=0 is defined by nauty.h, which implies dynamic allocation */
#include "nauty.h"
#include "gtools.h"
#include "gutils.h"
#define USAGE "Usage: tutte [-h] [-q] [-p#] [infile [outfile]]\n"

#define HELPTEXT \
" Computes the Tutte polynomial of a graph.\n\
\n\
    infile  the input graph file in either graph6 or sparse6 format.\n\
\n\
    outfile  the output file for the computed polynomial.\n\
\n\
    -p#  choose the which graph in the input file.\n\
         The first graph is number 1. (default 1)\n\
    -v  be verbose\n\
    -h  show this help text\n"

#define HASH_KEY 1

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int verbose = 0;
#define print(...) if (verbose) printf(__VA_ARGS__)

typedef struct mgraph
{ // an undirected multigraph
  int *g;
  int n;
} mgraph;

typedef struct medge
{
  int i;
  int j;
  int m; //multiplicity
} edge;

#define MG_IDX(mg,i,j) mg->g[j + i*mg->n]

extern int mg_get_edge_mult(mgraph *mg, int i, int j);
extern void mg_add_medge(mgraph *mg, edge e);
extern void mg_rem_medge(mgraph *mg, edge e);
extern void mg_contract_edge(mgraph *mg, edge e);

inline int mg_get_edge_mult(mgraph *mg, int i, int j)
{
  return MG_IDX(mg,i,j);
}
inline void mg_add_medge(mgraph *mg, edge e)
{
  MG_IDX(mg,e.i,e.j) += e.m;
  MG_IDX(mg,e.j,e.i) += e.m;
}
inline void mg_rem_medge(mgraph *mg, edge e)
{
  MG_IDX(mg,e.i,e.j) -= e.m;
  MG_IDX(mg,e.j,e.i) -= e.m;
}
inline void mg_contract_edge(mgraph *mg, edge e)
{
  int m;
  for (int j = 0; j < mg->n; j++)
    if ((m = mg_get_edge_mult(mg,e.j,j)))
    {
      mg_rem_medge(mg, (edge){.i = e.i, .j = j, .m = m});
      mg_add_medge(mg, (edge){.i = e.i, .i = j, .m = m});
    }
}


/*
#define MG_ARC(mg,i,j) mg->g[j + i*mg->n]

#define MG_GET_EMULT(mg,i,j) MG_ARC(mg,i,j)

#define MG_ADD_MEDGE(mg,e) do { \
    MG_ARC(mg, e.i, e.j) += e.m; \
    MG_ARC(mg, e.j, e.i) += e.m; \
  } while (0)

#define MG_REM_MEDGE(mg,e) do { \
    MG_ARC(mg, e.i, e.j) -= e.m; \
    MG_ARC(mg, e.j, e.i) -= e.m; \
  } while (0)
*/

mgraph *
empty_mgraph(int n)
{
  mgraph *g = (mgraph *)malloc(sizeof(mgraph));
  g->n = n;
  g->g = (int *)calloc(n*n, sizeof(int));
  return g;
}
mgraph *
copy_mgraph(mgraph *g)
{
  mgraph *cg = empty_mgraph(g->n);
  memcpy(cg->g, g->g, (g->n*g->n) * sizeof(int));
  return g;
}

mgraph *
nautygraph_to_mgraph(graph *gg, int n, int m)
{
  mgraph *mg = empty_mgraph(n);
  for (int i = 0; i < n; i++)
    for (int j =0; j < n; j++)
      if (ISELEMENT(GRAPHROW(gg,i,m),j)) MG_IDX(mg,i,j) = 1;
  DYNFREE(gg,n);
  return mg;
}

graph *
mgraph_to_nauttygraph(mgraph *mg)
{
  return NULL;
}

sparsegraph *
mgraph_to_sparsegraph(mgraph *mg)
{
  return NULL;
}

void
free_mgraph(mgraph *g)
{
  free(g->g);
  free(g);
}

typedef struct mgraph_info
{
  int props;
  mgraph **comps;
  int comps_len;
} mgraph_info;

void
free_mgraph_info(mgraph_info *info)
{
  for (int i = 0; i < info->comps_len; i++)
    free_mgraph(info->comps[i]);
  free(info->comps);
  free(info);
}
typedef struct poly
{ // a polynomial
  long *c;
  int y_deg;
  int x_deg;
} poly;

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
      fprintf(fout, "%ldx^%dy^%d\n", *poly_coeff(p, x, y), x, y);
}

poly *
poly_add(poly *A, poly *B)
{
  poly *C = new_poly(MAX(A->x_deg, B->x_deg), MAX(A->y_deg, B->y_deg));
  for (int x = 0; x <= C->x_deg; x++)
    for (int y = 0; y <= C->y_deg; y++)
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
/*
  poly * tutte(mgraph *g);

  mgraph_info *
  search_mgraph(mgraph *g)
  {


  }
*/

edge select_edge(mgraph *g)
{
  int m;
  for (int i = 0; i < g->n; i++)
    for (int j = 0; j < g->n; j++)
      if ((m = mg_get_edge_mult(g,i,j))) return (edge){.i = i, .j = j, .m = m};

  fprintf(stderr, "select_edge called on empty graph\n");
  return (edge){.i = -1, .j = -1, .m = -1};
}

boolean
edge_is_loop(mgraph *g, edge e)
{
  if (e.i == e.j) return TRUE;
  return FALSE;
}

int
bridge_helper(mgraph *g, int a, int c)
{
/*
  if (a == c) return 1;
  else
    for (int i = 0; i < g->n; i++)
      if (ARC(g,a,i) && bridge_helper(g, a, i)) return 1;
*/
  return 0;
}

int
edge_is_bridge(mgraph *g, edge e)
{
/*
  for (int i = 0; i < g->n; i++)
    if (ARC(g,e.a,i) && (i != e.b))
      if (bridge_helper(g, e.b, i)) return 0;
*/
  return 1;
}

/*
poly *
reduce_components(mgraph *g, mgraph_info *info)
{
  return NULL;
}

poly *
reduce_trees(mgraph *g, mgraph_info *info)
{
  return NULL;
}

poly *
reduce_multiears(mgraph *g, mgraph_info *info)
{
  return NULL;
}
poly *
reduce_loops(mgraph *g, mgraph_info *info)
{
  return NULL;
}
poly *
reduce_multiedges(mgraph *g, mgraph_info *info)
{
  return NULL;
}

poly *
reduce_edge_heuristic(mgraph *g, mgraph_info *info)
{
  edge e = select_edge(g);
  poly *pd = tutte(delete_edge(g, e));
  poly *pc = tutte(contract_edge(g, e));
  return poly_add(pc, pd);
}
*/


poly *
tutte(mgraph *g)
{

  // if hashgraph(g) in cache: return cache[key]
  //fcanonise(g, m, n, h, NULL, FALSE);
  //long hash = hashgraph(g, m, n, HASH_KEY);
  //char* graph_str = ntog6(g, m, n);
  //printf("%s->%ld\n\n", graph_strg, hashg);
  //else

  poly *p;
  /*
  mgraph_info *info = search_mgraph(g);

  if      (info->props & NOT_CONNECTED) p = reduce_components(g, info);
  else if (info->props & NOT_CONNECTED) p = reduce_trees(g, info);
  else if (info->props & NOT_CONNECTED) p = reduce_multiears(g, info);
  else if (info->props & NOT_CONNECTED) p = reduce_loops(g, info);
  else if (info->props & NOT_CONNECTED) p = reduce_multiedges(g, info);
  else                                  p = reduce_edge_heuristic(g, info);

  // cache[hashgraph(g)] = p
  free_mgraph_info(info);
  */

  edge e = select_edge(g);
  if (g->n == 0)
    {
      p = new_poly(0,0);
      *poly_coeff(p, 0, 0) = 1;
      free_mgraph(g);
    }
  else
    {
      if (edge_is_loop(g, e))
        {
          mg_rem_medge(g, e);
          p = tutte(g);
          poly *y = new_poly(0,1);
          *poly_coeff(y, 0, 0) = 0; *poly_coeff(y, 0, 1) = 1;
          p = poly_mult(p, y);
        }
      else if (edge_is_bridge(g, e))
        {
          mg_contract_edge(g, e);
          p = tutte(g);
          poly *x = new_poly(1,0);
          *poly_coeff(x, 0, 0) = 0; *poly_coeff(x, 1, 0) = 1;
          p = poly_mult(p, x);
        }
      else {
        mgraph *gc = copy_mgraph(g);
        mg_rem_medge(g, e);
        poly *pd = tutte(g);
        mg_contract_edge(gc, e);
        poly *pc = tutte(gc);
        p = poly_add(pc, pd);
      }
    }

  return p;
}
int
main(int argc, char *argv[])
{
  char* infile = NULL;
  char* outfile = NULL;
  FILE* fin = NULL;
  FILE* fout = NULL;
  int n,m;
  int codetype;
  boolean digraph;

  int c;
  long position = 1;

  while ((c = getopt (argc, argv, "hvp:")) != -1)
    switch (c)
      {
      case 'h':
        printf("%s\n%s", USAGE, HELPTEXT);
        return 0;
      case 'q':
        verbose = 1;
        break;
      case 'p':
        position = strtol(optarg,(char **)NULL, 10);
        break;
      case '?':
        printf("%s", USAGE);
        return 1;
      default:
        abort ();
      }
  if (optind < (argc-2))
    {
      fprintf(stderr, "Too many non-option arguments\nUse tutte -h for help\n%s", USAGE);
      return 1;
    }
  if (optind == (argc-2)) {infile = argv[optind]; outfile = argv[optind+1];}
  if (optind == (argc-1)) infile = argv[optind];

  fin = opengraphfile(infile, &codetype, 0, position);
  if (fin == NULL)
    {fprintf(stderr, "error opening file"); exit(1);}
  graph *gg = readgg(fin, NULL, 0, &m, &n, &digraph);
  if (digraph) {fprintf(stderr, "digraphs not supported"); exit(1);}
  nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
  print("n = %d, m = %d\n", n, m);

  mgraph *mg = nautygraph_to_mgraph(gg,n,m);
  poly *p = tutte(mg);

  if (outfile == NULL)
    {
      fout = fopen(outfile,"w");
      if (fout == NULL) {fprintf(stderr,"Can't open %s\n", outfile); exit(1);}
    }
  else fout = stdout;

  print_poly(fout, p);
  free_poly(p);

  return 0;
}
