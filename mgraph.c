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

