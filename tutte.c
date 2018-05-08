#include "tutte.h"

medge select_edge(mgraph *g)
{
  int m;
  for (int i = 0; i < g->n; i++)
    for (int j = 0; j < g->n; j++)
      if ((m = mg_get_edge_mult(g,i,j))) return (medge){.a = i, .b = j, .m = 1};

  return (medge){.a = -1, .b = -1, .m = -1};
}

/*
poly *
reduce_components(mgraph *g, dfs_data *info)
{
  return NULL;
}

poly *
reduce_trees(mgraph *g, dfs_data *info)
{
  return NULL;
}

poly *
reduce_multiears(mgraph *g, dfs_data *info)
{
  return NULL;
}
poly *
reduce_loops(mgraph *g, dfs_data *info)
{
  return NULL;
}
poly *
reduce_multiedges(mgraph *g, dfs_data *info)
{
  return NULL;
}

poly *
reduce_edge_heuristic(mgraph *g, dfs_data *info)
{
  medge e = select_edge(g);
  mgraph *gc = copy_mgraph(g);
  mg_rem_medge(g, e);
  poly *pd = tutte(g);

  mg_contract_medge(gc, e);
  poly *pc = tutte(gc);
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


  poly *p;

  medge e = select_edge(g);

  //print_mgraph(stdout, g);
  //printf("selected : ");
  //print_medge(stdout, e);
  //printf("\n");

  if (e.a == -1)
    {
      //printf("base case\n");
      p = new_poly(0,0);
      *poly_coeff(p, 0, 0) = 1;
      free_mgraph(g);
    }
  else
    {
      if (!medge_is_loop(g, e) && !medge_is_bridge(g, e))
        {
          //printf("recursive\n");
          mgraph *gc = copy_mgraph(g);
          mg_rem_medge(g, e);
          poly *pd = tutte(g);
          mg_contract_medge(gc, e);
          poly *pc = tutte(gc);
          p = poly_add(pc, pd);
        }
      else if (medge_is_loop(g,e))
        {
          //printf("loop\n");
          mg_rem_medge(g, e);
          p = tutte(g);
          poly *y = new_poly(0,1);
          *poly_coeff(y, 0, 0) = 0; *poly_coeff(y, 0, 1) = 1;
          p = poly_mult(p, y);
        }
      else {
        //printf("bridge\n");
        mg_contract_medge(g, e);
        p = tutte(g);
        poly *x = new_poly(1,0);
        *poly_coeff(x, 0, 0) = 0; *poly_coeff(x, 1, 0) = 1;
        p = poly_mult(p, x);
      }
    }

  //print_poly(stdout, p);
  return p;
}
