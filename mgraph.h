#ifndef  _MGRAPH_H_    /* only process this file once */
#define  _MGRAPH_H_

/*
  A data structure for handling undirected multigraphs
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "nauty.h"
#include "nausparse.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define BACK_EDGE -1
#define TREE_EDGE 1
#define UNVISITED -1

typedef struct mgraph
{ // an undirected multigraph
  int *g;
  int n;
} mgraph;

typedef struct medge
{
  int a;
  int b;
  int m; //multiplicity
} medge;

typedef struct mgraph_data
{
  int *dfs_tree;
  //depth first index: dfi[i] gives vertex with dfs num (discovered) at time i
  int *dfi;
  int *num;
  int *comps;

  int *visited;
  int *chains;

  int n;
  int time;
  int ncomps;
  int nchains;
  bool is_tree;
} mgraph_data;

#define DD_IDX(dd,i,j) dd->dfs_tree[j + i*dd->n]

#define MG_IDX(mg,i,j) mg->g[j + i*mg->n]
#define IS_LOOP(e) (e.a == e.b)

static inline int mg_get_edge_mult(mgraph *mg, int i, int j)
{
  return MG_IDX(mg,i,j);
}
static inline void mg_add_medge(mgraph *mg, medge e)
{
  if (e.a != e.b)
    {
      MG_IDX(mg,e.a,e.b) += e.m;
      MG_IDX(mg,e.b,e.a) += e.m;
    }
  else
    MG_IDX(mg,e.a,e.b) += e.m;
}
static inline void mg_rem_medge(mgraph *mg, medge e)
{
  if (e.a != e.b)
    {
      MG_IDX(mg,e.a,e.b) -= e.m;
      MG_IDX(mg,e.b,e.a) -= e.m;
    }
  else
    MG_IDX(mg,e.a,e.b) -= e.m;
}
static inline void mg_contract_medge(mgraph *mg, medge e)
{
  int m;
  mg_rem_medge(mg, e);
  for (int j = 0; j < mg->n; j++)
    if ((m = mg_get_edge_mult(mg,e.b,j)))
    {
      mg_rem_medge(mg, (medge){.a = e.b, .b = j, .m = m});
      mg_add_medge(mg, (medge){.a = e.a, .b = j, .m = m});
    }
}

mgraph * empty_mgraph(int n);
mgraph * copy_mgraph(mgraph *g);
mgraph * nautygraph_to_mgraph(graph *gg, int n, int m);
graph * mgraph_to_nauttygraph(mgraph *mg);
sparsegraph * mgraph_to_sparsegraph(mgraph *mg);
void free_mgraph(mgraph *g);
void print_mgraph(FILE *fout, mgraph *mg);

mgraph_data * new_mgraph_data(int n);
void free_mgraph_data(mgraph_data *dd);
void print_dfs_data(mgraph_data *dd);

//void DFS(mgraph *g, mgraph_data *dd, int v);
//dfs_data * strong_comps(mgraph *g);

static inline bool medge_is_loop(mgraph *g, medge e) {return IS_LOOP(e);}
bool medge_is_bridge(mgraph *g, medge e);
void print_medge(FILE *fout, medge e);

#endif /* _MGRAPH_H_  */
