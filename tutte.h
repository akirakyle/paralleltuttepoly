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

typedef enum{Recursive, Bridge, Loop, Base} comp_tree_el_type;
typedef enum{Deletion, Contraction} del_con_type;

typedef struct comp_tree_el comp_tree_el;
struct comp_tree_el //computation tree element
{
  //  mgraph *g;
  //comp_tree_el *D;
  //comp_tree_el *C;
  poly *pd;
  poly *pc;
  comp_tree_el *parent;
  comp_tree_el_type type;
  del_con_type dctype;
};

typedef struct queued_graph
{
  mgraph *g;
  comp_tree_el *parent;
  del_con_type dctype;
} queued_graph;

poly *run_tutte(tutte_options to, mgraph *g);
poly *tutte(mgraph *g);

#endif /* _TUTTE_H_  */
