P1=bitonic
P2=bitonic_omp
P3=bitonic_mpi
UTILS=bitonic_utils.cpp

OMPFLAG=-fopenmp
MPICC=mpic++
CC=g++
CFLAGS = -g -Wall

all: $(P1) $(P2) $(P3)

$(P1): $(P1).cpp $(UTILS)
	$(CC) $(CFLAGS) $(OMPFLAG) $(P1).cpp $(UTILS) -o $(P1)

$(P2): $(P2).cpp $(UTILS)
	$(CC) $(CFLAGS) $(OMPFLAG) $(P2).cpp $(UTILS) -o $(P2)

$(P3): $(P3).cpp $(UTILS)
	$(MPICC) $(CFLAGS) $(P3).cpp $(UTILS) -o $(P3)

clean:
	rm -vf $(P1) $(P2) $(P3)
