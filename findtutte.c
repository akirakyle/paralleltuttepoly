#include "tutte.h"

int verbose = 0;

#define print(...) if (verbose) printf(__VA_ARGS__)

#define USAGE "Usage: tutte [-h] [-q] [-p#] [-s#] [infile [outfile]]\n"

#define HELPTEXT                                                        \
  " Computes the Tutte polynomial of a graph.\n\
\n\
    infile  the input graph file in either graph6 or sparse6 format.\n\
\n\
    outfile  the output file for the computed polynomial.\n\
\n\
    -p#  choose which graph in the input file to use.\n\
         The first graph is number 1. (default 1)\n\
    -s#  set the number of elements the head process should\n\
         queue for the workers. (default 12)\n\
    -v  be verbose\n\
    -h  show this help text\n"

int
main(int argc, char *argv[])
{
  char* infile = NULL;
  char* outfile = NULL;
  FILE* fin = NULL;
  FILE* fout = NULL;
  int n,m;
  int codetype;
  boolean digraph;

  int c;
  long position = 1;
  int nsplit = 12;

  while ((c = getopt (argc, argv, "hvp:s:")) != -1)
    switch (c)
      {
      case 'h':
        printf("%s\n%s", USAGE, HELPTEXT);
        return 0;
      case 'q':
        verbose = 1;
        break;
      case 'p':
        position = strtol(optarg,(char **)NULL, 10);
        break;
      case 's':
        nsplit = (int)strtol(optarg,(char **)NULL, 10);
        break;
      case '?':
        printf("%s", USAGE);
        return 1;
      default:
        abort ();
      }
  if (optind < (argc-2))
    {
      fprintf(stderr, "Too many non-option arguments\nUse tutte -h for help\n%s", USAGE);
      return 1;
    }
  if (optind == (argc-2)) {infile = argv[optind]; outfile = argv[optind+1];}
  if (optind == (argc-1)) infile = argv[optind];

  fin = opengraphfile(infile, &codetype, 0, position);
  if (fin == NULL)
    {fprintf(stderr, "error opening file"); exit(1);}
  graph *gg = readgg(fin, NULL, 0, &m, &n, &digraph);
  if (digraph) {fprintf(stderr, "digraphs not supported"); exit(1);}
  nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
  print("n = %d, m = %d\n", n, m);

  mgraph *mg = nautygraph_to_mgraph(gg,n,m);

  tutte_options to = { .nsplit = nsplit };

  double t = currentSeconds();
  poly *p = run_tutte(to, mg);
  t = currentSeconds() - t;

  if (outfile == NULL)
    fout = stdout;
  else
    {
      fout = fopen(outfile,"w");
      if (fout == NULL) {fprintf(stderr,"Can't open %s\n", outfile); exit(1);}
    }

  print_poly(fout, p);
  free_poly(p);
  fprintf(fout, "Time : %.2f\n", t);

  return 0;
}
