NVCC=/usr/local/cuda/bin/nvcc
CC=g++
CFLAGS=-I/usr/local/cuda/include
LDFLAGS=-L/usr/local/cuda/lib -lcudart

.SUFFIXES: .cu

.cu.o:
	$(NVCC) $(CFLAGS) -c $<

all: vec_add

vec_add: vec_add.o
	$(CC) -o $@ vec_add.o $(LDFLAGS)

#/usr/local/cuda/bin/nvcc  -I/usr/local/cuda/include -c vec_add.cu 
# g++ -o vec_add vec_add.o  -L/usr/local/cuda/lib -lcudart

clean:
	-rm -fr *.o vec_add
