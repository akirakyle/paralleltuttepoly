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

  medge e = select_edge(g);

  poly *p;


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

comp_tree_el *
new_comp_tree_el()//mgraph *g)
{
  comp_tree_el *cte = (comp_tree_el *)malloc(sizeof(comp_tree_el));
  //  cte->g = g;
  //cte->D = NULL;
  //cte->C = NULL;
  cte->pd = NULL;
  cte->pc = NULL;
  return cte;
}

void
free_comp_tree_el(comp_tree_el *cte)
{
  free(cte);
}
void
print_comp_tree_el(FILE *fout, void *ctev)
{
  comp_tree_el *cte = (comp_tree_el *)ctev;
  fprintf(fout, "%p: parent: %p, type: %d, dctype: %d, pd: %p, pc: %p\n",
          cte, cte->parent, cte->type, cte->dctype, cte->pd, cte->pc);
  if (cte->pd != NULL) print_poly(fout, cte->pd);
  if (cte->pc != NULL) print_poly(fout, cte->pc);
}

queued_graph *
new_queued_graph(mgraph *g, comp_tree_el *ctp, del_con_type type)
{
  queued_graph *qg = (queued_graph *)malloc(sizeof(queued_graph));
  qg->g = g;
  qg->parent = ctp;
  qg->dctype = type;
  return qg;
};
void
free_queued_graph(queued_graph *qg)
{
  free(qg);
}
void
print_queued_graph(FILE *fout, void *qgv)
{
  queued_graph *qg = (queued_graph *)qgv;
  fprintf(fout, "%p: g: %p, parent: %p, type: %d\n",
          qg, qg->g, qg->parent, qg->dctype);
  //print_mgraph(fout, qg->g);
}


//queued_poly *
//new_queued_poly(poly *p, comp_tree_el *ctp)
//{
//  queued_poly *qp = (queued_poly *)malloc(sizeof(queued_poly));
//  qp->p = p;
//  qp->parent = ctp;
//  return qp;
//};

//void
//free_queued_poly(queued_poly *qp)
//{
//  free(qp);
//}

poly *
run_tutte(tutte_options to, mgraph *og)
{
  queue *s = stack_new();
  queue *q = queue_new();
  comp_tree_el *root_cte = new_comp_tree_el();
  queue_enq(q, (void *)new_queued_graph(og, root_cte, -1));

  printf("bfs queue:\n");
  while (q->len < to.nsplit && q->len > 0)
    {
      queue_print(stdout, q, print_queued_graph);

      queued_graph *qg = (queued_graph *)queue_deq(q);
      mgraph *g = qg->g;
      medge e = select_edge(g);

      comp_tree_el *cte = new_comp_tree_el();
      cte->parent = qg->parent;
      stack_push(s, (void *)cte);

      if (e.a == -1)
        {
          cte->type = Base;
          cte->dctype = qg->dctype;
          free_mgraph(g);
        }
      else
      if (e.a != -1)
        {
          if (!medge_is_loop(g, e) && !medge_is_bridge(g, e))
            {
              //printf("blahhhh 3\n");
              cte->type = Recursive;
              mgraph *gc = copy_mgraph(g);
              mg_rem_medge(g, e);
              queue_enq(q, (void *)new_queued_graph(g, cte, Deletion));
              mg_contract_medge(gc, e);
              queue_enq(q, (void *)new_queued_graph(gc, cte, Contraction));
            }
          else if (medge_is_loop(g,e))
            {
              //printf("blahhhh 4\n");
              cte->type = Loop;
              mg_rem_medge(g, e);
              queue_enq(q, (void *)new_queued_graph(g, cte, Deletion));
            }
          else {
            //printf("blahhhh 5\n");
            cte->type = Bridge;
            mg_contract_medge(g, e);
            queue_enq(q, (void *)new_queued_graph(g, cte, Contraction));
          }
        }
    }


  printf("dfs tutte:\n");
  while (q->len > 0)
    {
      queue_print(stdout, q, print_queued_graph);
      stack_print(stdout, s, print_comp_tree_el);

      queued_graph *qg = (queued_graph *)queue_deq(q);
      poly *p = tutte(qg->g);
      if (qg->dctype == Deletion) qg->parent->pd = p;
      if (qg->dctype == Contraction) qg->parent->pc = p;
      free_queued_graph(qg);
    }

  printf("poly stuff:\n");
  while (s->len > 0)
    {
      stack_print(stdout, s, print_comp_tree_el);
      comp_tree_el *cte = (comp_tree_el *)stack_pop(s);
      //printf("popped:\n");
      //print_comp_tree_el(stdout, cte);

      printf("c1: %d, c2:%d, c3:%d\n",
             ((cte->type == Recursive) && (cte->pd == NULL || cte->pc == NULL)),
             ((cte->type == Bridge) && (cte->pc == NULL)),
             ((cte->type == Loop) && (cte->pd == NULL)) );

      if ( ((cte->type == Recursive) && (cte->pd == NULL || cte->pc == NULL)) ||
           ((cte->type == Bridge) && (cte->pc == NULL)) ||
           ((cte->type == Loop) && (cte->pd == NULL)) )
        {
          //queue_enq((queue *)s, (void *)cte);
          printf("badd thing happen\n");
        }
      else if (cte->type == Base)
        {
          poly *id = new_poly(0,0);
          *poly_coeff(id, 0, 0) = 1;
          if (cte->dctype == Deletion)
            cte->parent->pd = id;
          if (cte->dctype == Contraction)
            cte->parent->pc = id;
        }
      else if (cte->type == Recursive)
        {
          if (cte->dctype == Deletion)
            cte->parent->pd = poly_add(cte->pd, cte->pc);
          if (cte->dctype == Contraction)
            cte->parent->pc = poly_add(cte->pd, cte->pc);
          free_comp_tree_el(cte);
        }
      else if (cte->type == Loop)
        {
          poly *y = new_poly(0,1);
          *poly_coeff(y, 0, 0) = 0; *poly_coeff(y, 0, 1) = 1;
          cte->parent->pd = poly_mult(cte->pd, y);
          free_comp_tree_el(cte);
        }

      else if (cte->type == Bridge)
        {
          poly *x = new_poly(1,0);
          *poly_coeff(x, 0, 0) = 0; *poly_coeff(x, 1, 0) = 1;
          cte->parent->pc = poly_mult(cte->pc, x);
          free_comp_tree_el(cte);
        }
    }

  if ((root_cte->pd == NULL) && (root_cte->pc == NULL)) fprintf(stderr, "something went wrong\n");
  if ((root_cte->pd != NULL) && (root_cte->pc != NULL)) fprintf(stderr, "something went wrong\n");
  poly *p;
  if (root_cte->pd != NULL) p = root_cte->pd;
  else p = root_cte->pc;

  queue_free(q);
  stack_free(s);
  return p;
}
