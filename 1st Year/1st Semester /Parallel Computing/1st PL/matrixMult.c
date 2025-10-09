#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

static int A[N][N], B[N][N], C[N][N]; // BSS/segmento global, não usa stack


void initMatrixZero(int matrix[N][N]) {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            matrix[i][j] = 0;
}

void initMatrixRandom(int matrix[N][N]) {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            matrix[(int)i][(int)j] = rand() % 10; // Random values between 0 and 9
}

double multiplyMatrices(int A[N][N], int B[N][N], int C[N][N]) {
    initMatrixRandom(A);
    initMatrixRandom(B);
    initMatrixZero(C);

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            for(int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
    return 0.0;
}

int main() {
    multiplyMatrices(A, B, C);
    printf("Matrix multiplication completed.\n");
    return 0;
}