#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <thread>

#define N 1000
#define nt 4

static int A[N][N], B[N][N], C[N][N]; // BSS/segmento global, não usa stack


void initMatrixZero(int matrix[N][N]) {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            matrix[i][j] = 0;
}

void initMatrixRandom(int matrix[N][N]) {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            matrix[(int)i][(int)j] = rand() % 10;
}

void multiplyMatrices(int id) {
    for(int i = id; i < N; i+= nt)
        for(int j = 0; j < N; j++)
            for(int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
}

int main() {
    initMatrixRandom(A);
    initMatrixRandom(B);
    initMatrixZero(C);


    std::thread tx[nt];
    for(int i=0; i<nt; i++)
        tx[i] = std::thread(multiplyMatrices,i);
        printf("Threads created\n");
    for(int i=0; i<nt; i++)
        tx[i].join();
    printf("End\n");
    return 0;
}