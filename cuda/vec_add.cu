// Simple vector addition, from the samples provided in the CUDA SDK.
// Author: Allen Porter <allen@thebends.org>

#include <stdlib.h>
#include <stdio.h>
#include <cuda.h>

__global__ void VecAdd(float* A, float* B, float* C) {
  int i = threadIdx.x;
  C[i] = A[i] + B[i];
}

int main(int argc, char**argv)
{
  int N = 10;
  size_t size = N * sizeof(float);

  // Input; Host memory
  float* h_A = (float*)malloc(size);
  float* h_B = (float*)malloc(size);
  for (int i = 0; i < N; i++) {
    h_A[i] = i;
    h_B[i] = i;
  }


  // Device memory
  float* d_A;
  cudaMalloc((void**)&d_A, size);
  float* d_B;
  cudaMalloc((void**)&d_B, size);
  float* d_C;
  cudaMalloc((void**)&d_C, size);

  // Copy from host to device
  cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

  // Invoke kernel
  VecAdd<<<1, N>>>(d_A, d_B, d_C);

  float* h_C = (float*)malloc(size);
  cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

  for (int i = 0; i < N; i++) {
    printf("%0.f ", h_A[i]);
  }
  printf("\n");
  for (int i = 0; i < N; i++) {
    printf("%0.f ", h_B[i]);
  }
  printf("\n");
  for (int i = 0; i < N; i++) {
    printf("%0.f ", h_C[i]);
  }
  printf("\n");

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);
  return 0;
}
