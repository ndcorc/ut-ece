#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MATRIX_FILE "matrix.txt"
#define VECTOR_FILE "vector.txt"
#define OUTFILE "result.txt"

#define DEBUG 0

typedef struct vector {
    int *elements;
    int capacity;
    int size;
} vector;

void int_vector_init(vector *vector) {
    if (vector == NULL) {
        return;
    }
    vector -> elements = (int*) malloc(sizeof(int));
    vector -> capacity = 1;
    vector -> size = 0;
}

int int_vector_add(vector* vector, int element) {
    if(vector->size + 1 == vector->capacity) {
        int *temp = (int*)realloc(vector->elements, vector->capacity*2 * sizeof (int));
        if(temp == NULL) {
            return 0;
        }
        vector -> capacity *= 2;
        vector -> elements = temp;
    }
    
    vector -> elements[vector->size] = element;
    vector -> size += 1;
    return 1;
}

void int_vector_free(vector *vector){
    free(vector->elements);
    free(vector);
}

void print_vector(vector *vector) {
    int i;
    for (i = 0; i < vector -> size; i++) {
        printf("%d ", vector->elements[i]);
    }
}

void print_matrix(int size, vector **matrix) {
    int i;
    for (i = 0; i < size; i++) {
        print_vector(matrix[i]);
        printf("\n");
    }
}

int row_to_process(int i, int rows, int size) {
    size = size - 1;
    int rows_per_process = (int) ceil((double) rows / (double) size);
    int process = i / rows_per_process;
    return process + 1;
}

void calculate_single_process() {
    // Open matrix file
    FILE *fp;
    if((fp = fopen(MATRIX_FILE, "r")) == 0) {
        printf("%s cannot be found\n", MATRIX_FILE);
        exit(-1);
    }
    
    char line[2048];
    char* token;
    int index = 0;
    
    // Read number of rows in matrix
    fgets(line, sizeof(line), fp);
    char* first = strtok(line, " ");
    int numRows = atoi(first);
    
    // Create empty matrix
    vector* matrix[numRows];
    
    // Read in matrix
    while (fgets(line, sizeof(line), fp) != NULL) {
        vector *a = (vector*) malloc(sizeof(vector));
        int_vector_init(a);
        
        token = strtok(line, " ");
        while (token != NULL) {
            int num = atoi(token);
            int_vector_add(a, num);
            
            token = strtok(NULL, " ");
        }
        matrix[index] = a;
        index++;
    }
    fclose(fp);
    
    // Open vector file
    if((fp = fopen(VECTOR_FILE, "r")) == 0) {
        printf("%s cannot be found\n", VECTOR_FILE);
        exit(-1);
    }
    
    // Create empty vector
    vector *vec = (vector*) malloc(sizeof(vector));
    int_vector_init(vec);
    
    // Read vector
    while (fgets(line, sizeof(line), fp) != NULL) {
        token = strtok(line, " ");
        while (token != NULL) {
            int num = atoi(token);
            int_vector_add(vec, num);
            
            token = strtok(NULL, " ");
        }
    }
    fclose(fp);
    // Calculate result vector
    int result[numRows];
    int i;
    for (i = 0; i < numRows; i++) {
        vector* row = matrix[i];
        int sum = 0;
        int j;
        for (j = 0; j < vec->size; j++) {
            sum += (row->elements[j] * vec->elements[j]);
        }
        
        result[i] = sum;
    }
    // Write result to file
    FILE *outFp = fopen(OUTFILE, "w");
    for (i = 0; i < numRows; i++) {
        if (i == numRows - 1) {
            fprintf(outFp, "%d", result[i]);
        } else {
            fprintf(outFp, "%d ", result[i]);
        }
    }
    fclose(outFp);
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
    
    MPI_Init(NULL, NULL);
    
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    // Special case for 1 process
    if (world_size == 1) {
        calculate_single_process();
        return 0;
    }
    
    if (world_rank == 0) {
        // Open matrix file
        FILE *fp;
        if((fp = fopen(MATRIX_FILE, "r")) == 0) {
            printf("%s cannot be found\n", MATRIX_FILE);
            exit(-1);
        }
        
        char line[255];
        char* token;
        int index = 0;
        
        // Read number of rows in matrix
        fgets(line, sizeof(line), fp);
        char* first = strtok(line, " ");
        int numRows = atoi(first);
        
        // Create empty matrix
        vector* matrix[numRows];
        
        // Read in matrix
        while (fgets(line, sizeof(line), fp) != NULL) {
            vector *a = (vector*) malloc(sizeof(vector));
            int_vector_init(a);
            
            token = strtok(line, " ");
            while (token != NULL) {
                int num = atoi(token);
                int_vector_add(a, num);
                
                token = strtok(NULL, " ");
            }
            
            matrix[index] = a;
            index++;
        }
        
        // Print matrix
        if (DEBUG) {
            printf("Built Matrix:\n");
            print_matrix(numRows, matrix);
            printf("\n");
        }
        
        fclose(fp);
        
        // Open vector file
        if((fp = fopen(VECTOR_FILE, "r")) == 0) {
            printf("%s cannot be found\n", VECTOR_FILE);
            exit(-1);
        }
        
        // Create empty vector
        vector *vec = (vector*) malloc(sizeof(vector));
        int_vector_init(vec);
        
        // Read vector
        while (fgets(line, sizeof(line), fp) != NULL) {
            token = strtok(line, " ");
            while (token != NULL) {
                int num = atoi(token);
                int_vector_add(vec, num);
                
                token = strtok(NULL, " ");
            }
        }
        
        // Print vector
        if (DEBUG) {
            printf("Build Vector:\n");
            print_vector(vec);
            printf("\n");
        }
        
        fclose(fp);
        
        // Send size to other processes
        int i;
        for (i = 1; i < world_size; i++) {
            MPI_Send(&vec->size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        
        // Send vector to other processes
        for (i = 1; i < world_size; i++) {
            MPI_Send(vec->elements, vec->size, MPI_INT, i, 99, MPI_COMM_WORLD);
        }
        
        // Send number of rows to other processes
        for (i = 1; i < world_size; i++) {
            MPI_Send(&numRows, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        }
        
        // Send matrix rows to specific processes
        for (i = 0; i < numRows; i++) {
            int process = row_to_process(i, numRows, world_size);
            MPI_Send(matrix[i]->elements, vec->size, MPI_INT, process, 100*(i+1), MPI_COMM_WORLD);
        }
        
        // Define result vector
        int result[numRows];
        
        // Receive result
        for (i = 0; i < numRows; i++) {
            int source = row_to_process(i, numRows, world_size);
            MPI_Recv(&result[i], 1, MPI_INT, source, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        if (DEBUG) {
            printf("[Master] Received result: ");
            for (i = 0; i < numRows; i++) {
                printf("%d ", result[i]);
            }
            printf("\n");
        }
        
        // Write result to file
        FILE *outFp = fopen(OUTFILE, "w");
        if (outFp == NULL) {
            printf("Can't open file %s to write\n", OUTFILE);
        }
        
        for (i = 0; i < numRows; i++) {
            if (i == numRows - 1) {
                fprintf(outFp, "%d", result[i]);
            } else {
                fprintf(outFp, "%d ", result[i]);
            }
        }
        
        fclose(outFp);
        
    } else {
        int size;
        
        // Receive size
        MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (DEBUG) {
            printf("[Process %d] Size: %d\n", world_rank, size);
        }
        
        int vec[size];
        
        // Receive vector
        MPI_Recv(vec, size, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (DEBUG) {
            printf("[Process %d] Vector: ", world_rank);
            int i;
            for (i = 0; i < size; i++) {
                printf("%d ", vec[i]);
            }
            printf("\n");
        }
        
        int rows;
        
        // Receive number of rows
        MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Receive matrix
        int i;
        for (i = 0; i < rows; i++) {
            int process = row_to_process(i, rows, world_size);
            if (world_rank == process) {
                int buf[size];
                MPI_Recv(buf, size, MPI_INT, 0, (100*(i+1)), MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                // Calculate sum and send result
                int sum = 0;
                int j;
                for (j = 0; j < size; j++) {
                    sum += (buf[j] * vec[j]);
                }
                MPI_Send(&sum, 1, MPI_INT, 0, i, MPI_COMM_WORLD);
            }
        }
    }
    
    // Finalize the MPI environment.
    MPI_Finalize();
}
