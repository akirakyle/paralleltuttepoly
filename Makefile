CC=gcc

MPI=-DMPI
MPICC = mpicc

CFLAGS=-g -O3 -Wall -Wno-unused-variable -std=gnu99 #-DDEBUG
#LDFLAGS= -lm
IFLAGS= -Inauty

NAUTYLIB = nauty/nauty.a

CFILES = cycletimer.c linked_list.c mgraph.c poly.c tutte.c findtutte.c
HFILES = cycletimer.h linked_list.h mgraph.h poly.h tutte.h

all: tutte tutte-mpi

tutte: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o tutte $(CFILES) $(NAUTYLIB) $(LDFLAGS) $(IFLAGS)

tutte-mpi: $(CFILES) $(HFILES)
	$(MPICC) $(CFLAGS) $(MPI) -o tutte-mpi $(CFILES) $(NAUTYLIB) $(LDFLAGS) $(IFLAGS)

clean:
	rm -f tutte *.o
	rm -rf *.dSYM

sync:
	rsync -azP --no-perms ~/class/parallel/project/paralleltuttepoly latedays:
#	rsync -azP latedays:paralleltuttepoly/output ~/class/parallel/project/paralleltuttepoly
