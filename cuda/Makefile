NVCC=/usr/local/cuda/bin/nvcc
CC=g++
CFLAGS=-I/usr/local/cuda/include
LDFLAGS=-L/usr/local/cuda/lib -lcudart

.SUFFIXES: .cu

.cu.o:
	$(NVCC) $(CFLAGS) -c $<

all: vec_add browse

vec_add: vec_add.o 
	$(CC) -o $@ vec_add.o $(LDFLAGS)

browse: browse.o
	$(CC) -o $@ browse.o $(LDFLAGS)

clean:
	-rm -fr *.o vec_add browse
