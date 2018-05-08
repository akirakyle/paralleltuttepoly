#ifndef  _TUTTE_H_    /* only process this file once */
#define  _TUTTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "cycletimer.h"

 /* MAXN=0 is defined by nauty.h, which implies dynamic allocation */
#include "nauty.h"
#include "gtools.h"
#include "gutils.h"

#include "mgraph.h"
#include "poly.h"
#include "linked_list.h"

#define HASH_KEY 1

typedef struct tutte_options
{
  int nsplit;
} tutte_options;

typedef enum{Base, Recurse, Bridge, Loop} comp_tree_el_type;

typedef struct comp_tree_el //computation tree element
{
  poly *pd;
  poly *pc;
  poly **parent_p;
  comp_tree_el_type type;
} comp_tree_el;

typedef struct queued_graph
{
  mgraph *g;
  poly **parent_p;
} queued_graph;

poly *run_tutte(tutte_options to, mgraph *g);
poly *tutte(mgraph *g);

#endif /* _TUTTE_H_  */
