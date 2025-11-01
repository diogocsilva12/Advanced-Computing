#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

// Deixe sizeMatrix como variável (runtime), não macro
int sizeMatrix = 512;
int numThreads = 2;

double *A, *B, *C;

//Aloca espaço na memória para as matrizes A, B e C
void alloc() {
    A = (double *) malloc(sizeMatrix*sizeMatrix*sizeof(double));
    B = (double *) malloc(sizeMatrix*sizeMatrix*sizeof(double));
    C = (double *) malloc(sizeMatrix*sizeMatrix*sizeof(double));
}

//Inicializa matrizes A e B com valores aleatórios e C com zeros
void init() {
    for(int i=0; i<sizeMatrix; i++) {
        for(int j=0; j<sizeMatrix; j++) {
            A[i*sizeMatrix+j] = rand();
            B[i*sizeMatrix+j] = rand();
            C[i*sizeMatrix+j] = 0;
        }
    }
}

// OpenMP-parallel matrix multiply (row partition):
// C[i,j] += sum_k A[i,k] * B[k,j]
// Parallelize outer i-loop; each thread owns distinct rows of C (no data races).
void matrixMult_omp() {
    const int N = sizeMatrix;
    // i-k-j order to improve locality on C and B; hoist A[i,k] into a register
    #pragma omp parallel for num_threads(numThreads)
    for (int i = 0; i < N; ++i) {
        int baseCi = i * N;
        for (int k = 0; k < N; ++k) {
            double aik = A[i * N + k];
            int baseBk = k * N;
            for (int j = 0; j < N; ++j) {
                C[baseCi + j] += aik * B[baseBk + j];
            }
        }
    }
}

int main(int argc, char** argv) {
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

    // Configure OpenMP threads (can also use OMP_NUM_THREADS env var)
    omp_set_dynamic(0); // don't let runtime lower the team size
    if (numThreads > 0) omp_set_num_threads(numThreads);
    // Optional: report thread count
    // #pragma omp parallel
    // {
    //     #pragma omp master
    //     fprintf(stderr, "OpenMP using %d threads (requested=%d)\n", omp_get_num_threads(), numThreads);
    // }

    matrixMult_omp();
    
    printf("%f\n", C[(sizeMatrix/2)*sizeMatrix + 5]);
    return 0;
}
