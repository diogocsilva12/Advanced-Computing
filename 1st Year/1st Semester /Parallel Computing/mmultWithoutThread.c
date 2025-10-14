#include<stdio.h>
#include<stdlib.h>

#ifndef N
#define N 512
#endif

float *A, *B, *C;

void alloc() {
    A = (float *) malloc(N*N*sizeof(float));
    B = (float *) malloc(N*N*sizeof(float));
    C = (float *) malloc(N*N*sizeof(float));
}

void init() {
    for(int i=0; i<N; i++) {
        for(int j=0; j<N; j++) {
            A[i*N+j] = rand();
            B[i*N+j] = rand();
            C[i*N+j] = 0;
        }
    }
}

void mmult() {
    for(int i=0; i<N; i++) {
        for(int k=0; k<N; k++) {
            for(int j=0; j<N; j++) {
                C[i*N+j] += A[i*N+k] * B[k*N+j];
            }
        }
    }
}

int main() {
    alloc();
    init();
    
    mmult();

    printf("%f\n", C[N/2+5]);
}
