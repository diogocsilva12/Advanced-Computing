#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
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
// Multiplicação de matrizes paralelizada com OpenMP (com B transposta)
// Paralelizamos pelos blocos (ii,jj); cada thread acumula localmente e
// escreve em regiões distintas de C, evitando data races.
// ------------------------------------------------------------
void matrixMultBlocked() {
    const int N = sizeMatrix;

    // Paraleliza pelos blocos de linhas e colunas
    #pragma omp parallel for collapse(2) schedule(static)
    for (int ii = 0; ii < N; ii += TILE_SIZE) {
        for (int jj = 0; jj < N; jj += TILE_SIZE) {
            for (int kk = 0; kk < N; kk += TILE_SIZE) {
                const int i_max = std::min(ii + TILE_SIZE, N);
                const int j_max = std::min(jj + TILE_SIZE, N);
                const int k_max = std::min(kk + TILE_SIZE, N);

                for (int i = ii; i < i_max; ++i) {
                    for (int j = jj; j < j_max; ++j) {
                        double sum = C[i * N + j];
                        for (int k = kk; k < k_max; ++k) {
                            sum += A[i * N + k] * B_T[j * N + k];
                        }
                        C[i * N + j] = sum;
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

    if (numThreads > 0) omp_set_num_threads(numThreads);
    // fprintf(stderr, "OMP using %d threads (max=%d)\n", omp_get_max_threads(), omp_get_max_threads());
    matrixMultBlocked();

    printf("C[center,5] = %f\n", C[(sizeMatrix / 2) * sizeMatrix + 5]);

    free(A);
    free(B);
    free(B_T);
    free(C);

    return 0;
}
