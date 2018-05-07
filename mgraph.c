#include "mgraph.h"

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
  return cg;
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
  graph * p = (graph *)mg;
  return p;
}

sparsegraph *
mgraph_to_sparsegraph(mgraph *mg)
{
  sparsegraph * p = (sparsegraph *)mg;
  return p;
}

void
free_mgraph(mgraph *g)
{
  free(g->g);
  free(g);
}

void
print_mgraph(FILE *fout, mgraph *mg)
{
  for (int i = 0; i < mg->n; i++)
    {
      for (int j =0; j < mg->n; j++)
        fprintf(fout, "%3d ",MG_IDX(mg,i,j));
      fprintf(fout, "\n");
    }

}


/*
void
DFS(mgraph *g, dfs_data *dd, int v)
{
  int *num = dd->num;
  int *low = dd->low;
  int *stack = dd->stack;
  bool *on_stack = dd->on_stack;
  int *comps = dd->comps;

  printf("dfs on %d\n", v);
  print_dfs_data(dd);
  printf("\n");

  if (num[v] != 0) return;
  num[v] = low[v] = ++(dd->count);
  stack[dd->sp++] = v;
  on_stack[v] = true;
  for (int u = 0; u < g->n; u++) if (mg_get_edge_mult(g,v,u)) DFS(g, dd, u);
  for (int u = 0; u < g->n; u++) if (on_stack[u]) low[v] = MIN(low[v], low[u]);
  if (num[v] == low[v])
    {
      while (true)
        {
          int x = stack[--(dd->sp)];
          on_stack[x] = false;
          comps[x] = dd->comp;
          if (x == v) break;
        }
      dd->comp++;
    }
}

dfs_data *
strong_comps(mgraph *g)
{
  dfs_data *dd = new_dfs_data(g->n);
  for (int x = 0; x < g->n; x++) DFS(g, dd, x);
  return dd;
}


bool
medge_is_bridge(mgraph *g, medge e)
{
dfs_data *dd = new_dfs_data(g->n);
for (int x = 0; x < g->n; x++) DFS(g, dd, x);

print_dfs_data(dd);

bool is_bridge = false;
if ((dd->low[e.a] > dd->num[e.b]) || (dd->low[e.b] > dd->num[e.a]))
is_bridge = true;
free_dfs_data(dd);
return is_bridge;
}

*/

dfs_data *
new_dfs_data(int n)
{
  dfs_data *dd = malloc(sizeof(dfs_data));
  dd->num = calloc(n, sizeof(int)); //dfs number
  dd->dfi = calloc(n, sizeof(int));
  dd->comps = calloc(n, sizeof(int));
  dd->visited = calloc(n, sizeof(int));
  dd->chains = calloc(n, sizeof(int));

  dd->n = n;
  dd->time  = 0;
  dd->ncomps = 0;
  dd->nchains = 0;
  return dd;
}

void
free_dfs_data(dfs_data *dd)
{
  free(dd->num);
  free(dd->dfi);
  free(dd->comps);
  free(dd->visited);
  free(dd->chains);
  free(dd);
}

void
DFS(mgraph *g, dfs_data *dd, int v)
{
  //printf("dfs on %d\n", v);
  //print_dfs_data(dd);
  //printf("\n");

  if (dd->num[v] != 0) return;
  dd->num[v] = ++(dd->time);
  dd->dfi[dd->time] = v;
  dd->comps[v] = dd->ncomps;
  for (int u = 0; u < g->n; u++) if (mg_get_edge_mult(g,v,u)) DFS(g, dd, u);
}

void
chain_decomp_helper(mgraph *g, dfs_data *dd, int v)
{
  if (dd->visited[v] != 0) return;
  dd->chains[v] = dd->nchains;
  for (int u = 0; u < g->n; u++)
    if (mg_get_edge_mult(g,v,u) && (dd->num[u] < dd->num[v])) // uv is backedge
      chain_decomp_helper(g, dd, u);
}

dfs_data *
chain_decomp(mgraph *g)
{
  dfs_data *dd = new_dfs_data(g->n);

  for (int v = 0; v < g->n; v++)
    {
      if (dd->num[v] == 0) dd->ncomps++;
      DFS(g, dd, v);
    }

  print_dfs_data(dd);

  for (int i = 0; i < g->n; i++)
    {
      int v = dd->dfi[i];
      if (dd->visited[v] == 0) dd->nchains++;
      chain_decomp_helper(g, dd, v);
    }
  return dd;
}

bool
medge_is_bridge(mgraph *g, medge e)
{
  dfs_data *dd = chain_decomp(g);

  bool is_bridge = false;
  if (dd->chains[e.a] != dd->chains[e.b]) is_bridge = true;
  free_dfs_data(dd);
  return is_bridge;
}

void
print_dfs_data(dfs_data *dd)
{
  printf("num: ");
  for (int v = 0; v < dd->n; v++)
    printf("%d, ", dd->num[v]);
  printf("\n");
  /*
  printf("low: ");
  for (int v = 0; v < dd->n; v++)
    printf("%d, ", dd->low[v]);
  printf("\n");
  printf("stack: ");
  for (int v = 0; v < dd->n; v++)
    printf("%d, ", dd->stack[v]);
  printf("\n");
  printf("on_stack: ");
  for (int v = 0; v < dd->n; v++)
    printf("%d, ", dd->on_stack[v]);
  printf("\n");
  printf("comps: ");
  for (int v = 0; v < dd->n; v++)
    printf("%d, ", dd->comps[v]);
  */
  printf("\n");
}


void
print_medge(FILE *fout, medge e)
{
  fprintf(fout, "%d(%d, %d)", e.m, e.a, e.b);
}
