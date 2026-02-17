
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "bitonic.h"

#define BILLION  1000000000L

void for_bitonic(int n, float *data){
    for (int size = 2; size <= n; size *= 2) {
        for (int stride = size / 2; stride > 0; stride /= 2) {
            #pragma omp parallel for
            for (int i = 0; i < n; i++) {
                int j = i ^ stride;
                if (j > i) {
                    int ascending = ((i & size) == 0);
                    if ((ascending && data[i] > data[j]) || (!ascending && data[i] < data[j])) {
                        float temp = data[i];
                        data[i] = data[j];
                        data[j] = temp;
                    }
                }
            }
        }
    }
}

int main(int argc, char * argv[]) {
    int n = atoi(argv[1]);
    int pe = atoi(argv[2]);
    omp_set_num_threads(pe);
    float *data;
    double start, end, f_time, s_time;

    data = (float*)malloc(sizeof(float) * n);

    for (int i = 0; i < n; i++) {
        data[i] = 1.1f * rand() * 5000 / RAND_MAX;
    }

    start = omp_get_wtime();
    seq_bitonic(n, data);
    end = omp_get_wtime();
    s_time = end - start;

    for (int i = 0; i < n; i++) {
        data[i] = 1.1f * rand() * 5000 / RAND_MAX;
    }

    start = omp_get_wtime();
    for_bitonic(n, data);
    end = omp_get_wtime();
    f_time = end - start;

    printf("Serial runtime: %lf\n", s_time);
    printf("Number of processing elements: %d \n", pe);
    printf("Size of list: %d \n", n);
    if(validate_sort(n, data)){
        printf("Validation passed\n");
    }
    else{
        printf("Validation failed\n");
    }
    
    printf("OMP runtime: %lf s\n", f_time);
    printf("OMP speedup: %lf s\n", s_time/f_time);
    free(data);
    return 0;
}
