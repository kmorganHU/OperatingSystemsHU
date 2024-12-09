#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20

// Matrix declarations
int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

// Result matrices for addition, subtraction, and multiplication
int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

// Structure to store arguments for each thread
typedef struct {
    int start_row;
    int end_row;
} ThreadArgs;

// Function to fill a matrix with random integers between 1 and 10
void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Function to print the contents of a matrix
void printMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Thread function to compute the sum of two matrices for assigned rows
void* computeSum(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    int start_row = thread_args->start_row;
    int end_row = thread_args->end_row;
    
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    
    free(args); // Free allocated memory for thread arguments
    return NULL;
}

// Thread function to compute the difference of two matrices for assigned rows
void* computeDiff(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    int start_row = thread_args->start_row;
    int end_row = thread_args->end_row;
    
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    
    free(args); // Free allocated memory for thread arguments
    return NULL;
}

// Thread function to compute the product of two matrices for assigned rows
void* computeProduct(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    int start_row = thread_args->start_row;
    int end_row = thread_args->end_row;
    
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0; // Initialize result to zero
            for (int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    
    free(args); // Free allocated memory for thread arguments
    return NULL;
}

int main() {
    srand(time(0)); // Seed the random number generator

    // Fill matrices A and B with random values
    fillMatrix(matA);
    fillMatrix(matB);

    // Display the original matrices
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // Thread setup
    pthread_t threads[10];
    int num_threads = 10;
    int rows_per_thread = MAX / num_threads;

    // Compute matrix addition using threads
    for (int i = 0; i < num_threads; i++) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->start_row = i * rows_per_thread;
        args->end_row = (i == num_threads - 1) ? MAX : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, computeSum, (void*)args);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Compute matrix subtraction using threads
    for (int i = 0; i < num_threads; i++) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->start_row = i * rows_per_thread;
        args->end_row = (i == num_threads - 1) ? MAX : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, computeDiff, (void*)args);
    }
    
    // Compute matrix multiplication using threads
    for (int i = 0; i < num_threads; i++) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->start_row = i * rows_per_thread;
        args->end_row = (i == num_threads - 1) ? MAX : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, computeProduct, (void*)args);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Display the results
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}