#ifndef  _TUTTE_H_    /* only process this file once */
#define  _TUTTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

 /* MAXN=0 is defined by nauty.h, which implies dynamic allocation */
#include "nauty.h"
#include "gtools.h"
#include "gutils.h"

#include "mgraph.h"
#include "poly.h"

#define HASH_KEY 1

poly * tutte(mgraph *g);

#endif /* _TUTTE_H_  */
