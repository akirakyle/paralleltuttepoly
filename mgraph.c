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

mgraph_data *
new_mgraph_data(int n)
{
  mgraph_data *dd = malloc(sizeof(mgraph_data));
  dd->dfs_tree = (int *)calloc(n*n, sizeof(int));
  dd->num = malloc(n*sizeof(int));
  for (int i = 0; i < n; i++) dd->num[i] = -1;
  dd->dfi = calloc(n, sizeof(int));
  dd->comps = calloc(n, sizeof(int));
  dd->visited = calloc(n, sizeof(int));
  dd->chains = calloc(n, sizeof(int));

  dd->n = n;
  dd->time  = 0;
  dd->ncomps = 0;
  dd->nchains = 0;
  dd->is_tree = true;
  return dd;
}

void
free_mgraph_data(mgraph_data *dd)
{
  free(dd->dfs_tree);
  free(dd->num);
  free(dd->dfi);
  free(dd->comps);
  free(dd->visited);
  free(dd->chains);
  free(dd);
}

void
print_mgraph_data(FILE *fout, mgraph_data *dd)
{
  fprintf(fout, "dfs_tree: \n");
  for (int i = 0; i < dd->n; i++)
    {
      for (int j =0; j < dd->n; j++)
        fprintf(fout, "%3d ",DD_IDX(dd,i,j));
      fprintf(fout, "\n");
    }
  fprintf(fout, "num: ");
  for (int v = 0; v < dd->n; v++)
    fprintf(fout, "%d, ", dd->num[v]);
  fprintf(fout, "\n");
  fprintf(fout, "dfi: ");
  for (int v = 0; v < dd->n; v++)
    fprintf(fout, "%d, ", dd->dfi[v]);
  fprintf(fout, "\n");
  fprintf(fout, "comps: ");
  for (int v = 0; v < dd->n; v++)
    fprintf(fout, "%d, ", dd->comps[v]);
  fprintf(fout, "\n");
  fprintf(fout, "visited: ");
  for (int v = 0; v < dd->n; v++)
    fprintf(fout, "%d, ", dd->visited[v]);
  fprintf(fout, "\n");
  fprintf(fout, "chains: ");
  for (int v = 0; v < dd->n; v++)
    fprintf(fout, "%d, ", dd->chains[v]);
  fprintf(fout, "\nn = %d, ncomps = %d, nchains = %d, is_tree = %d",
          dd->n, dd->ncomps, dd->nchains, dd->is_tree);
  fprintf(fout, "\n\n");
}

void
DFS(mgraph *g, mgraph_data *dd, int v, int p)
{
  //printf("dfs on %d\n", v);
  //print_mgraph_data(dd);
  //printf("\n");

  dd->comps[v] = dd->ncomps;
  dd->dfi[dd->time] = v;
  dd->num[v] = dd->time;
  dd->time++;
  for (int u = 0; u < g->n; u++)
    if (mg_get_edge_mult(g,v,u))
      {
        if (dd->num[u] == -1)
          { // (u,v) is directed tree edge
            DD_IDX(dd,u,v) = 1;
            DFS(g, dd, u, v);
          }
        else if (dd->num[v] > dd->num[u] && u != p)
          { // (u,v) is directed backedge
            dd->is_tree = false;
            DD_IDX(dd,u,v) = 1;
          }
      }
}

void
chain_traverse(mgraph *g, mgraph_data *dd, int v)
{
  //printf("traverse: %d\n", v);
  //if (dd->visited[v] != 0) return;
  //print_mgraph_data(stdout,dd);
  dd->visited[v] = 1;
  dd->chains[v] = dd->nchains;
  for (int u = 0; u < g->n; u++)
    if (dd->visited[u] == 0 && DD_IDX(dd,v,u))// == TREE_EDGE)
      chain_traverse(g, dd, u);
  //if (mg_get_edge_mult(g,v,u) && (dd->num[u] < dd->num[v])) // uv is backedge
}

mgraph_data *
chain_decomp(mgraph *g)
{
  mgraph_data *dd = new_mgraph_data(g->n);

  for (int v = 0; v < g->n; v++)
    if (dd->num[v] == -1)
      {
        DFS(g, dd, v, v);
        dd->ncomps++;
      }

  //print_mgraph_data(stdout, dd);

  for (int i = 0; i < g->n; i++)
    {
      int v = dd->dfi[i];
      for (int u = 0; u < g->n; u++)
        if (dd->visited[u] == 0 && DD_IDX(dd,v,u))// == BACK_EDGE)
          {
            dd->nchains++;
            //printf("calling traverse: %d\n", u);
            chain_traverse(g, dd, u);
          }
    }

  //print_mgraph_data(stdout, dd);

  return dd;
}

bool
medge_is_bridge(mgraph *g, medge e)
{
  if (mg_get_edge_mult(g, e.a, e.b) != 1) return false;
  mgraph_data *dd = chain_decomp(g);

  bool is_bridge = false;
  if (dd->chains[e.a] != dd->chains[e.b] || dd->is_tree) is_bridge = true;
  free_mgraph_data(dd);
  return is_bridge;
}

void
print_medge(FILE *fout, medge e)
{
  fprintf(fout, "%d(%d, %d)", e.m, e.a, e.b);
}
