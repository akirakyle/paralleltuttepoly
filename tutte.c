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
  fprintf(fout, "%p: parent_p: %p, pd: %p, pc: %p\n",
          cte, cte->parent_p, cte->pd, cte->pc);
  if (cte->pd != NULL) print_poly(fout, cte->pd);
  if (cte->pc != NULL) print_poly(fout, cte->pc);
}

queued_graph *
new_queued_graph(mgraph *g, poly **p)
{
  queued_graph *qg = (queued_graph *)malloc(sizeof(queued_graph));
  qg->g = g;
  qg->parent_p = p;
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
  fprintf(fout, "%p: g: %p, parent_p: %p\n", qg, qg->g, qg->parent_p);
}

#if MPI
void
worker_tutte()
{
  int process_id;
  MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
  //printf("worker %d started\n", process_id);

  int n;
  poly *p;
  while (1) {
    MPI_Recv(&n,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    if (n < 0) return;
    mgraph *g = empty_mgraph(n);
    MPI_Recv(g->g,n * n,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    poly *p = tutte(g);
    MPI_Send(&p->x_deg,1,MPI_INT,0,3,MPI_COMM_WORLD);
    MPI_Send(&p->y_deg,1,MPI_INT,0,4,MPI_COMM_WORLD);
    MPI_Send(p->c,(p->x_deg+1)*(p->y_deg+1),MPI_INT,0,5,MPI_COMM_WORLD);
  }
}

poly *
master_tutte(tutte_options to, mgraph *og)
{
  queue *s = stack_new();
  queue *q = queue_new();
  poly *p_og = NULL;
  queue_enq(q, (void *)new_queued_graph(og, &p_og));

  //printf("bfs queue:\n");
  while (q->len < to.nsplit && q->len > 0)
    {
      //queue_print(stdout, q, print_queued_graph);
      queued_graph *qg = (queued_graph *)queue_deq(q);
      mgraph *g = qg->g;
      comp_tree_el *cte = new_comp_tree_el();
      cte->parent_p = qg->parent_p;
      stack_push(s, (void *)cte);
      medge e = select_edge(g);

      if (e.a == -1)
        {
          cte->type = Base;
          free_mgraph(g);
        }
      else
        {
          if (!medge_is_loop(g, e) && !medge_is_bridge(g, e))
            {
              cte->type = Recurse;
              mgraph *gc = copy_mgraph(g);
              mg_rem_medge(g, e);
              queue_enq(q, (void *)new_queued_graph(g, &cte->pd));
              mg_contract_medge(gc, e);
              queue_enq(q, (void *)new_queued_graph(gc, &cte->pc));
            }
          else if (medge_is_loop(g,e))
            {
              cte->type = Loop;
              mg_rem_medge(g, e);
              queue_enq(q, (void *)new_queued_graph(g, &cte->pd));
            }
          else {
            cte->type = Bridge;
            mg_contract_medge(g, e);
            queue_enq(q, (void *)new_queued_graph(g, &cte->pc));
          }
        }
    }

  //printf("dfs tutte:\n");
  poly ***mpi_parent_p = (poly ***)malloc(to.nprocess * sizeof(poly **));
  int nsent = 0;
  int nrecv = 0;
  do {
    //queue_print(stdout, q, print_queued_graph);
    //stack_print(stdout, s, print_comp_tree_el);
    //printf("looping with nsent: %d nrecv: %d \n", nsent, nrecv);
    MPI_Status status;
    int x_deg, y_deg;
    int src = nsent+1;
    if ((nsent >= to.nprocess-1 || q-> len == 0) && nsent != nrecv)
      {
        MPI_Recv(&x_deg, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
        src = status.MPI_SOURCE;
        //printf("recieved from %d\n", src);
        MPI_Recv(&y_deg, 1, MPI_INT, src, 4, MPI_COMM_WORLD, &status);
        poly *p = new_poly(x_deg, y_deg);
        MPI_Recv(p->c, (x_deg+1)*(y_deg+1), MPI_INT, src, 5, MPI_COMM_WORLD, &status);
        *mpi_parent_p[src] = p;
        nrecv++;
      }

    if (q->len > 0)
      {
        //printf("sent to %d\n", src);
        queued_graph *qg = (queued_graph *)queue_deq(q);
        int n = qg->g->n;
        MPI_Send(&n,1,MPI_INT,src,0,MPI_COMM_WORLD);
        MPI_Send(qg->g->g,n*n,MPI_INT,src,1,MPI_COMM_WORLD);
        mpi_parent_p[src] = qg->parent_p;
        free_queued_graph(qg);
        nsent++;
      }
  } while (nsent != nrecv);
  queue_free(q);
  int term_val = -1;
  for (int i = 0; i < to.nprocess; i++)
    MPI_Send(&term_val,1,MPI_INT,i,0,MPI_COMM_WORLD);

  //printf("poly stuff:\n");
  while (s->len > 0)
    {
      //stack_print(stdout, s, print_comp_tree_el);
      comp_tree_el *cte = (comp_tree_el *)stack_pop(s);
      switch (cte->type) {
      case Base :    *cte->parent_p = new_poly_id();                     break;
      case Recurse : *cte->parent_p = poly_add(cte->pd, cte->pc);        break;
      case Bridge :  *cte->parent_p = poly_mult(cte->pc, new_poly_x1()); break;
      case Loop :    *cte->parent_p = poly_mult(cte->pd, new_poly_y1()); break;
      }
      free_comp_tree_el(cte);
    }
  stack_free(s);
  return p_og;
}
#endif
