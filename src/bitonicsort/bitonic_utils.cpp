
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "bitonic.h"

#define BILLION  1000000000L

void seq_bitonic(int n, float *data) {
    for (int size = 2; size <= n; size *= 2) {
        for (int stride = size / 2; stride > 0; stride /= 2) {
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

int floatcompare(const void *p1, const void *p2){
    float i = *((float *)p1);
    float j = *((float *)p2);
    if (i > j) return 1;
    if (i < j) return -1;
    return 0;
}

bool validate_sort(int n, float *data) {
    for (int i = 0; i < n - 1; i++) {
        if (floatcompare(&data[i], &data[i + 1]) > 0) {
            return false;
        }
    }
    return true;
}


