CC=gcc

MPI=-DMPI
MPICC = mpicc

CFLAGS=-g -O3 -Wall -std=gnu99 #-DDEBUG
#LDFLAGS= -lm
IFLAGS= -Inauty

CFILES = mgraph.c poly.c tutte.c findtutte.c
HFILES = mgraph.h poly.h tutte.h

NAUTYFILES = nauty/nauty.a # nauty/nausparse.h nauty/nausparse.c

all: tutte tutte-mpi

nauty.a:
	$(MAKE) -C nauty nauty.a

tutte: nauty.a $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o tutte $(CFILES) $(NAUTYFILES) $(LDFLAGS) $(IFLAGS)

tutte-mpi: nauty.a $(CFILES) $(HFILES)
	$(MPICC) $(CFLAGS) $(MPI) -o tutte-mpi $(CFILES) $(NAUTYFILES) $(LDFLAGS) $(IFLAGS)

clean:
	$(MAKE) -C nauty clean
	rm -f tutte *.o

execute: tutte
	./tutte graphs/edge10.g6

sync:
	rsync -azP --no-perms ~/class/parallel/project/nauty26r10 andrew:
	rsync -azP --no-perms ~/class/parallel/project/paralleltuttepoly andrew:
#	rsync -azP --no-perms ~/class/parallel/project/paralleltuttepoly latedays:
