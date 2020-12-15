/*  
    Nolan Corcoran
    ndc466
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int init_matrix(int** matrix, int numRows, FILE *fp) {
    char line[255];
    int numCols = 0;
    int i = 0;
    int j = 0;
    char* elements;
    if (fgets(line, sizeof(line), fp) != NULL) {
        elements = strtok(line, " ");
        while (elements != NULL) {
            printf("element[%d] = %s\n", numCols, elements);
            numCols += 1;
            elements = strtok(NULL, " ");
        }
    }
    printf("numCols: %d\n", numCols);
    for (i = 0; i < numRows; i++) {
        matrix[i] = (int*) malloc(sizeof(int)*numCols);
    }

    i = 0;
    j = 0;
    rewind(fp);
    fgets(line, sizeof(line), fp); // skip first line
    while (fgets(line, sizeof(line), fp) != NULL) {
        elements = strtok(line, " ");
        while (elements != NULL) {
            matrix[i][j] = atoi(elements);
            printf("matrix[%d][%d] = %d\n", i, j, matrix[i][j]);
            j += 1;
            elements = strtok(NULL, " ");
        }
        i += 1;
        j = 0;
    }
    fclose(fp);
    return numCols;
}

void init_vector(int* vector, int numCols, FILE *fp) {
    char line[255];
    if((fp = fopen("vector.txt", "r")) == 0) {
        printf("%s cannot be found\n", "vector.txt");
        exit(-1);
    }
    char* elements;
    while (fgets(line, sizeof(line), fp) != NULL) {
        elements = strtok(line, " ");
        int j = 0;
        while (elements != NULL) {
            vector[j] = atoi(elements);
            j += 1;
            elements = strtok(NULL, " ");
        }
    }
    fclose(fp);
}

int main(int argc, char** argv) {
    int size, rank, numRows, numCols, process, source, i, j, temp;
    int** matrix;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    printf("rank: %d\n", rank);
    printf("size: %d\n", size);
    
    if (rank == 0) {
        
        FILE *fp;
        if((fp = fopen("matrix.txt", "r")) == 0) {
            printf("%s cannot be found\n", "matrix.txt");
            exit(-1);
        }
        char line[255];
        // get numRows of input matrix
        fgets(line, sizeof(line), fp);
        char* first = strtok(line, " ");
        numRows = atoi(first);
        printf("numRows = %d\n", numRows);

        // initialize/instantiate input matrix
        matrix = (int**) malloc(sizeof(int*)*numRows);
        numCols = init_matrix(matrix, numRows, fp);
        
        // initialize/instantiate input vector && send size to other processes
        int* vector = (int*) malloc(sizeof(int)*numCols);
        init_vector(vector, numCols, fp);
        FILE *out = fopen("result.txt", "w");
        
        if (size == 1) {
            int result[numRows];
            for (i = 0; i < numRows; i++) {
                int* row = matrix[i];
                int sum = 0;
                for (j = 0; j < numCols; j++) {
                    sum += (row[j] * vector[j]);
                }
                result[i] = sum;
            }
            // Write result to file
            for (i = 0; i < numRows; i++) {
                if (i == numRows - 1) fprintf(out, "%d", result[i]);
                else fprintf(out, "%d ", result[i]);
            }
            fclose(out);
        } else {        
            for (i = 1; i < size; i++) {    // Send size to other processes
                MPI_Send(&numCols, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&numRows, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                MPI_Send(vector, numCols, MPI_INT, i, 2, MPI_COMM_WORLD);
            }
            // Send row by row matrix data to each process
            temp = size;
            for (i = 0; i < numRows; i++) {
                //int process = row_to_process(i, numRows, size);
                temp -= 1;
                process = (i / ((int) ceil((double) numRows / (double) temp))) + 1;
                MPI_Send(matrix[i], numCols, MPI_INT, process, 100*(i+1), MPI_COMM_WORLD);
            }
            
            // Get result
            int result[numRows];
            temp = size;
            for (i = 0; i < numRows; i++) {
                temp -= 1;
                source = (i / ((int) ceil((double) numRows / (double) temp))) + 1;
                MPI_Recv(&result[i], 1, MPI_INT, source, (i*10), MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            
            if (out == NULL) {
                printf("Can't open file %s to write\n", "result.txt");
            }
            for (i = 0; i < numRows; i++) {
                if (i == numRows - 1) fprintf(out, "%d", result[i]);
                else fprintf(out, "%d ", result[i]);
            }
            fclose(out);
            
            // deallocate matrix and vector data
            free(vector);
            for (i = 0; i < numRows; i++) {
                free(matrix[i]);
            }
        }
        
    } else {
        // receive # columns, # rows, && the input vector
        MPI_Recv(&numCols, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&numRows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int vector[numCols];
        MPI_Recv(vector, numCols, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // receive matrix
        temp = size;
        for (i = 0; i < numRows; i++) {
            temp -= 1;
            process = (i / ((int) ceil((double) numRows / (double) temp))) + 1;
            if (rank == process) {
                int buf[numCols];
                MPI_Recv(buf, numCols, MPI_INT, 0, (100*(i+1)), MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // Calculate sum and send result
                int sum = 0;
                for (j = 0; j < numCols; j++) {
                    sum += (buf[j] * vector[j]);
                }
                MPI_Send(&sum, 1, MPI_INT, 0, (i*10), MPI_COMM_WORLD);
            }
        }
    }
    
    MPI_Finalize();
}
