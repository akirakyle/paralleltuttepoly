#include "tutte.h"

edge select_edge(mgraph *g)
{
  int m;
  for (int i = 0; i < g->n; i++)
    for (int j = 0; j < g->n; j++)
      if ((m = mg_get_edge_mult(g,i,j))) return (edge){.a = i, .b = j, .m = m};

  return (edge){.a = -1, .b = -1, .m = -1};
}

int
bridge_helper(mgraph *g, int a, int c)
{
  if (a == c) return 1;
  else
    for (int i = 0; i < g->n; i++)
      if (ARC(g,a,i) && bridge_helper(g, a, i)) return 1;
  return 0;
}

int
edge_is_bridge(mgraph *g, edge e)
{
  for (int i = 0; i < g->n; i++)
    if (ARC(g,e.a,i) && (i != e.b))
      if (bridge_helper(g, e.b, i)) return 0;
  return 0;
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

  //print_mgraph(stdout, g);

  edge e = select_edge(g);

  if (e.a == -1)
    {
      //printf("bc in\n");
      p = new_poly(0,0);
      *poly_coeff(p, 0, 0) = 1;
      free_mgraph(g);
      //printf("bc out\n");
    }
  else
    {
      if (!IS_LOOP(e) && !edge_is_bridge(g, e))
        {
          //printf("recurse in\n");
          mgraph *gc = copy_mgraph(g);
          mg_rem_medge(g, e);
          poly *pd = tutte(g);
          //printf("recurse mid 1\n");
          //print_poly(stdout, pd);
          mg_contract_edge(gc, e);
          poly *pc = tutte(gc);
          //printf("recurse mid 2\n");
          //print_poly(stdout, pc);
          p = poly_add(pc, pd);
          //printf("poly_add: ");
          //print_poly(stdout, p);
          //printf("recurse out\n");
        }
      else if (IS_LOOP(e))
        {
          //printf("loop in\n");
          mg_rem_medge(g, e);
          p = tutte(g);
          //printf("loop mid\n");
          poly *y = new_poly(0,1);
          *poly_coeff(y, 0, 0) = 0; *poly_coeff(y, 0, 1) = 1;
          p = poly_mult(p, y);
          //print_poly(stdout, p);
          //printf("loop out\n");
        }
      else {
        //printf("bridge in\n");
        mg_contract_edge(g, e);
        p = tutte(g);
        //printf("bridge mid\n");
        poly *x = new_poly(1,0);
        *poly_coeff(x, 0, 0) = 0; *poly_coeff(x, 1, 0) = 1;
        p = poly_mult(p, x);
        //print_poly(stdout, p);
        //printf("bridge out\n");
      }
    }

  return p;
}
