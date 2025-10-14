#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <algorithm> // para std::min


#define TILE_SIZE 32
int sizeMatrix = 512;
int numThreads = 2;

double *A, *B, *B_T, *C;

// ------------------------------------------------------------
// Aloca espaço na memória para A, B, B_T (transposta) e C
// Esta versão usa a matrix transposta B_T para melhorar locality, o que permite uma pesquisa contigua na memória
// ------------------------------------------------------------
void alloc() {
    A   = (double *) malloc(sizeMatrix * sizeMatrix * sizeof(double));
    B   = (double *) malloc(sizeMatrix * sizeMatrix * sizeof(double));
    B_T = (double *) malloc(sizeMatrix * sizeMatrix * sizeof(double));
    C   = (double *) malloc(sizeMatrix * sizeMatrix * sizeof(double));
}

// ------------------------------------------------------------
// Inicializa A e B com valores aleatórios, e C com zeros
// ------------------------------------------------------------
void init() {
    for (int i = 0; i < sizeMatrix; i++) {
        for (int j = 0; j < sizeMatrix; j++) {
            A[i * sizeMatrix + j] = rand() / (double) RAND_MAX;
            B[i * sizeMatrix + j] = rand() / (double) RAND_MAX;
            C[i * sizeMatrix + j] = 0.0;
        }
    }
}

// ------------------------------------------------------------
// Transpõe a matriz B para melhorar *spatial locality*
// Agora B_T[j*sizeMatrix + i] = B[i*sizeMatrix + j]
// Assim, B_T é percorrida por linhas (contíguo em memória)
// ------------------------------------------------------------
void transposeB() {
    for (int i = 0; i < sizeMatrix; i++) {
        for (int j = 0; j < sizeMatrix; j++) {
            B_T[j * sizeMatrix + i] = B[i * sizeMatrix + j];
        }
    }
}

// ------------------------------------------------------------
// Multiplicação de matrizes paralelizada (com B transposta)
// Cada thread processa um conjunto contíguo de linhas de A
// ------------------------------------------------------------
void matrixMultBlocked(int threadID) {
    int chunk = sizeMatrix / numThreads;
    int start = threadID * chunk;
    int end   = (threadID == numThreads - 1) ? sizeMatrix : start + chunk;

    for (int ii = start; ii < end; ii += TILE_SIZE) {
        for (int     jj = 0; jj < sizeMatrix; jj += TILE_SIZE) {
            for (int kk = 0; kk < sizeMatrix; kk += TILE_SIZE) {
                int i_max = std::min(ii + TILE_SIZE, end);
                int j_max = std::min(jj + TILE_SIZE, sizeMatrix);
                int k_max = std::min(kk + TILE_SIZE, sizeMatrix);

                for (int i = ii; i < i_max; i++) {
                    for (int j = jj; j < j_max; j++) {
                        double sum = C[i * sizeMatrix + j];
                        for (int k = kk; k < k_max; k++) {
                            sum += A[i * sizeMatrix + k] * B_T[j * sizeMatrix + k];
                        }
                        C[i * sizeMatrix + j] = sum;
                    }
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        sizeMatrix = std::atoi(argv[1]);
        if (sizeMatrix <= 0) {
            fprintf(stderr, "Invalid matrix size: %s\n", argv[1]);
            return 1;
        }
    }

    if (argc >= 3) {
        numThreads = std::atoi(argv[2]);
        if (numThreads <= 0) {
            fprintf(stderr, "Invalid thread count: %s\n", argv[2]);
            return 1;
        }
    }

    alloc();
    init();
    transposeB();

    std::thread tx[numThreads];
    for (int i = 0; i < numThreads; i++)
        tx[i] = std::thread(matrixMultBlocked, i);

    for (int i = 0; i < numThreads; i++)
        tx[i].join();

    printf("C[center,5] = %f\n", C[(sizeMatrix / 2) * sizeMatrix + 5]);

    free(A);
    free(B);
    free(B_T);
    free(C);

    return 0;
}
