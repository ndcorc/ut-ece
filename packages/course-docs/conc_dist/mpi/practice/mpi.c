/*
    Nolan Corcoran
    ndc466
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

enum {
    REQUEST,
    PRODUCT,
    INDEX,
    ROW
};

void *schedule(void *vsize);

int main(int argc, char **argv) {
    int ignore;
    int world;
    int world_rank;
    int vLen;
    int row;
    int vector[1025];
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &ignore); // init for MPI with threads
    MPI_Comm_size(MPI_COMM_WORLD, &world);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    if (world_rank == 0) {
        FILE *fp = fopen("vector.txt", "r");
        while (fscanf(fp, "%d", &vector[vLen]) != -1) {
            vLen++;
        }
        fclose(fp);
        fp = fopen("matrix.txt", "r");
        fscanf(fp, "%d", &row);
        fclose(fp);
    }
    
    MPI_Bcast(&vLen, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vector, vLen, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&row, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(row <= 0){
        if(world_rank == 0){
            FILE* fpempty = fopen("result.txt", "w");
            printf("empty vector\n");
            fclose(fpempty);
        }
        return 0;
    }
    
    pthread_t scheduler;
    
    if (world_rank == 0) {
        pthread_create(&scheduler, NULL, schedule, &vLen);
    }

    int index = -world;
    MPI_Send(&index, 1, MPI_INT, 0, REQUEST, MPI_COMM_WORLD);
    MPI_Recv(&index, 1, MPI_INT, 0, INDEX, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    if (index == -1) {
        if (world_rank == 0) {
            void *exit;
            pthread_join(scheduler, exit);
        }
        MPI_Finalize();
        return 0;
    }
    while (index != -1) {
        int buf[vLen];
        int res = 0;
        int p;
        MPI_Recv(&buf, vLen, MPI_INT, 0, ROW, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        for (p = 0; p < vLen; p++) {
            res += vector[p] * buf[p];
        }
        MPI_Send(&index, 1, MPI_INT, 0, REQUEST, MPI_COMM_WORLD);
        MPI_Send(&res, 1, MPI_INT, 0, PRODUCT, MPI_COMM_WORLD);
        MPI_Recv(&index, 1, MPI_INT, 0, INDEX, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
    // Finalize the MPI environment. No more MPI calls can be made after this
    if (world_rank == 0) {
        void *exit;
        pthread_join(scheduler, exit);
    }
    MPI_Finalize();
}

void *schedule(void *vsize) {
    FILE *fp;
    int world, vs, row, product, stored, index, i, j, halt;
    vs = *((int *) vsize);
    int *buf = malloc(sizeof(int) * vs);
    int *result = malloc(sizeof(int) * vs);
    stored = 0;
    halt = -1;
    
    MPI_Status status;
    fp = fopen("matrix.txt", "r");
    fscanf(fp, "%d", &row);
    for (j = 0; j < row; j++) {
        i;
        for (i = 0; i < vs; i++) {
            fscanf(fp, "%d", &buf[i]);
        }
        
        MPI_Recv(&index, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST, MPI_COMM_WORLD,
                 &status);
        if (index >= 0) {
            MPI_Recv(&product, 1, MPI_INT, status.MPI_SOURCE, PRODUCT, MPI_COMM_WORLD,
                     &status);
            result[index] = product;
            stored++;
        } else {
            world = -index;
        }
        MPI_Send(&j, 1, MPI_INT, status.MPI_SOURCE, INDEX, MPI_COMM_WORLD);
        MPI_Send(buf, vs, MPI_INT, status.MPI_SOURCE, ROW, MPI_COMM_WORLD);
        
    }
    
    while (stored < row) {
        MPI_Recv(&index, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST, MPI_COMM_WORLD,
                 &status);
        if (index >= 0) {
            MPI_Recv(&product, 1, MPI_INT, status.MPI_SOURCE, PRODUCT, MPI_COMM_WORLD,
                     &status);
            result[index] = product;
            stored++;
        }
    }
    
    
    fclose(fp);
    printf("world size is %d\n", world);
    for (i = 1; i < world; i++) {
        MPI_Send(&halt, 1, MPI_INT, i, INDEX, MPI_COMM_WORLD);
    }
    fp = fopen("result.txt", "w");
    int k;
    for (k = 0; k < (row - 1); k++) {
        fprintf(fp, "%d ", result[k]);
    }
    if (vs > 0) {
        fprintf(fp, "%d", result[row - 1]);
    }
    MPI_Send(&halt, 1, MPI_INT, 0, INDEX, MPI_COMM_WORLD);
    printf("scheduler exiting\n");
    fclose(fp);
    pthread_exit(0);
}

