#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <algorithm> // para std::min

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
void matrixMult(int threadID) {
    int chunk = sizeMatrix / numThreads;
    int start = threadID * chunk;
    int end   = (threadID == numThreads - 1) ? sizeMatrix : start + chunk;

    for (int i = start; i < end; i++) {               
        for (int k = 0; k < sizeMatrix; k++) {        
            double sum = 0.0;
            for (int j = 0; j < sizeMatrix; j++) {    
                sum += A[i * sizeMatrix + k] * B_T[j * sizeMatrix + k];
            }
            C[i * sizeMatrix + k] = sum;
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
        tx[i] = std::thread(matrixMult, i);

    for (int i = 0; i < numThreads; i++)
        tx[i].join();

    printf("C[center,5] = %f\n", C[(sizeMatrix / 2) * sizeMatrix + 5]);

    free(A);
    free(B);
    free(B_T);
    free(C);

    return 0;
}
