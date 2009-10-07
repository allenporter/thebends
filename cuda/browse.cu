// Tries to dump memory in some far off location just to see whats there.
// Author: Allen Porter <allen@thebends.org>

#include <cuda.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

static const char* out_filename = "OUT";

__global__ void Browse(unsigned long* base, unsigned long* out) {
  int i = threadIdx.x;
  unsigned long* b = (unsigned long*)0x20000000LL;
  out[i] = [i];
}

int main(int argc, char**argv)
{
  // Read 4 mb of data
  int N = 1024 * 1024;
  size_t size = N * sizeof(long);

  // A base address
  unsigned long* d_base;
  cudaMalloc((void**)&d_base, 1);

  unsigned long* d_out;
  cudaMalloc((void**)&d_out, size);

  printf("Invoking kernel\n");

  // Invoke kernel
  Browse<<<1, N>>>(d_base, d_out);

  cudaThreadSynchronize();

  unsigned long* h_out = (unsigned long*)malloc(size);
  printf("Default values\n");
  memset(h_out, 'A', size);

  printf("Copying results\n");
  cudaMemcpy(h_out, d_out, size, cudaMemcpyDeviceToHost);

  printf("Writing to ./%s\n", out_filename);
  FILE* f = fopen(out_filename, "w");
  if (!f) {
    perror("fopen");
    return 1;
  }
  size_t result = fwrite(h_out, N, sizeof(long), f);
  if (result <= 0) {
    perror("fwrite");
    return 1;
  }
  fclose(f);
  return 0;
}
