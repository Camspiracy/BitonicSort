#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>
#include "bitonic.h"

int compare_asc(const void *a, const void *b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

int compare_desc(const void *a, const void *b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fb > fa) - (fb < fa);
}

void bitonic_mpi(float *local_arr, int local_n, int rank, int size) {

    for (int k = 2; k <= size; k <<= 1) {
        for (int j = k >> 1; j > 0; j >>= 1) {

            int partner = rank ^ j;

            float *recv_buf = (float *) malloc(local_n * sizeof(float));
            MPI_Sendrecv(local_arr, local_n, MPI_FLOAT, partner, 0,
                        recv_buf, local_n, MPI_FLOAT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            int ascending = ((rank & k) == 0);


            int total_n = 2 * local_n;
            int *combined = (int *) malloc(total_n * sizeof(int));
            for (int i = 0; i < local_n; i++) {
                combined[i] = local_arr[i];
                combined[i + local_n] = recv_buf[i];
            }


            if (ascending) {
                qsort(combined, total_n, sizeof(int), compare_asc);
            } else {
                qsort(combined, total_n, sizeof(int), compare_desc);
            }


            if ((rank & j) == 0) {
                for (int i = 0; i < local_n; i++) {
                    local_arr[i] = combined[i];
                }
            } else {
                for (int i = 0; i < local_n; i++) {
                    local_arr[i] = combined[i + local_n];
                }
            }

            free(combined);
            free(recv_buf);
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int n = atoi(argv[1]);
    double start, end, s_time, mpi_time;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    float *local_data;

    int local_n = n / size;

    local_data = (float*)malloc(sizeof(float) * n);

    for (int i = 0; i < n; i++) {
        local_data[i] = 1.1f * rand() * 5000 / RAND_MAX;
    }

    if(rank == 0){
        printf("Number of processing elements: %d \n", size);
        printf("Size of list: %d \n", n);
        start = MPI_Wtime();
        seq_bitonic(n, &local_data[0]);
        s_time = MPI_Wtime() - start;
        printf("Serial runtime: %fs\n", s_time);

        for(int i = 0; i < n; i++){
            local_data[i] = rand()%1000-1;
        }
    }

    start = MPI_Wtime();

    // Local sort
    if (rank % 2 == 0)
        qsort(local_data, local_n, sizeof(float), compare_asc);
    else
        qsort(local_data, local_n, sizeof(float), compare_desc);

    // Bitonic merge
    bitonic_mpi(local_data, local_n, rank, size);

    end = MPI_Wtime();
    mpi_time = end-start;

    // Validate
    bool local_valid = validate_sort(local_n, local_data);
    bool global_valid = false;
    MPI_Reduce(&local_valid, &global_valid, 1, MPI_C_BOOL, MPI_LAND, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Validation %s\n", global_valid ? "passed" : "failed");
        printf("MPI runtime: %.6f seconds\n", mpi_time);
        printf("MPI speedup: %lf s\n", s_time/mpi_time);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
