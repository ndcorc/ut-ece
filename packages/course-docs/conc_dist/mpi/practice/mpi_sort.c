#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* the number of data elements in each process */
#define N 10


/* initialize the data to random values based on rank (so they're different) */
void init(int* data, int rank) {
    int i;
    srand(rank);
    for (i = 0; i < N; i++) {
        data[i] = rand( ) % 100;
    }
}

/* print the data to the screen */
void print(int* data, int rank) {
    int i;
    printf("Process %d: ", rank);
    for (i = 0; i < N; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

void printM(int* data, int x) {
    int i;
    for (i = 0; i < x; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

/* comparison function for qsort */
int cmp(const void* ap, const void* bp) {
    int a = * ((const int*) ap);
    int b = * ((const int*) bp);

    if (a < b)  return -1;
    else if (a > b) return 1;
    else return 0;
}

/* find the index of the largest item in an array */
int max_index(int* data) {
    int i, max = data[0], maxi = 0;

    for (i = 1; i < N; i++) {
        if (data[i] > max) {
            max = data[i];
            maxi = i;
        }
    }
    return maxi;
}

/* find the index of the smallest item in an array */
int min_index(int* data) {
    int i, min = data[0], mini = 0;

    for (i = 1; i < N; i++) {
        if (data[i] < min) {
            min = data[i];
            mini = i;
        }
    }
    return mini;
}


/* do the parallel odd/even sort */
void parallel_sort(int* data, int rank, int size) {
    int i;

    /* the array we use for reading from partner */
    int other[N];

    /* we need to apply P phases where P is the number of processes */
    for (i = 0; i < size; i++) {
        /* sort our local array */
        qsort(data, N, sizeof(int), &cmp);

        /* find our partner on this phase */
        int partner;

        /* if it's an even phase */
        if (i % 2 == 0) {
            /* if it's an even process */
            if (rank % 2 == 0) {
                partner = rank + 1;
            } else {
                partner = rank - 1;
            }
        } else {
          /* it's an odd phase - do the opposite */
            if (rank % 2 == 0) {
                partner = rank - 1;
            } else {
                partner = rank + 1;
            }
        }

        /* if the partner is invalid, we should simply move on to the next iteration */
        if (partner < 0 || partner >= size) {
            continue;
        }

        /* do the exchange - even processes send first and odd processes receive first
         * this avoids possible deadlock of two processes working together both sending */
        if (rank % 2 == 0) {
            MPI_Send(data, N, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(other, N, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(other, N, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(data, N, MPI_INT, partner, 0, MPI_COMM_WORLD);
        }

        /* now we need to merge data and other based on if we want smaller or larger ones */
        if (rank < partner) {
            /* keep smaller keys */
            while (1) {
                /* find the smallest one in the other array */
                int mini = min_index(other);

                /* find the largest one in out array */
                int maxi = max_index(data);

                /* if the smallest one in the other array is less than the largest in ours, swap them */
                if (other[mini] < data[maxi]) {
                    int temp = other[mini];
                    other[mini] = data[maxi];
                    data[maxi] = temp;
                } else {
                    /* else stop because the smallest are now in data */
                    break;
                }
            }
        } else {
            /* keep larger keys */
            while (1) {
                /* find the largest one in the other array */
                int maxi = max_index(other);

                /* find the largest one in out array */
                int mini = min_index(data);

                /* if the largest one in the other array is bigger than the smallest in ours, swap them */
                if (other[maxi] > data[mini]) {
                    int temp = other[maxi];
                    other[maxi] = data[mini];
                    data[mini] = temp;
                } else {
                    /* else stop because the largest are now in data */
                    break;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    /* our rank and size */
    int rank, size;

    /* our processes data */
    int data[N];

    /* initialize MPI */
    MPI_Init(&argc, &argv);

    /* get the rank (process id) and size (number of processes) */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* initialize the data */
    printf("rank: %d\n", rank);
    printf("size: %d\n", size);
    init(data, rank);

    /* do the parallel odd/even sort */
    parallel_sort(data, rank, size);

    int * rbuf = (int *)malloc(N*size*sizeof(int));
    MPI_Datatype rtype; 
    MPI_Type_contiguous( N, MPI_INT, &rtype ); 
    MPI_Type_commit( &rtype );

    MPI_Gather(data, N, MPI_INT, rbuf, 1, rtype, 0, MPI_COMM_WORLD);
    /* now print our data */

    print(data, rank);

    if(rank == 0){
        printf("\n\nGathered Data:\n");
        printM(rbuf, N * size);
    }
    /* quit MPI */
    MPI_Finalize( );
    return 0;
}