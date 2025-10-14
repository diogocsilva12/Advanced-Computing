#include<stdio.h>
#include<stdlib.h>
#include<thread>

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


void matrixMult(int threadID) {
    for(int i=threadID; i<sizeMatrix; i+=numThreads) {
        for(int k=0; k<sizeMatrix; k++) {
            for(int j=0; j<sizeMatrix; j++) {
                C[i*sizeMatrix+j] += A[i*sizeMatrix+k] * B[k*sizeMatrix+j];
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
    
    std::thread tx[numThreads];
    for(int i=0; i<numThreads; i++)
        tx[i] = std::thread(matrixMult,i);
    for(int i=0; i<numThreads; i++) //espera que todas as threads terminem
        tx[i].join();
    
    printf("%f\n", C[(sizeMatrix/2)*sizeMatrix + 5]);
    return 0;
}
