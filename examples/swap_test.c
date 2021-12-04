#include <stdio.h>
#include "../src/svcore.h"

#define SHOTS 100

char sum(char* arr, int n) {
    int i;
    char sum = 0;
    for (i = 0; i < n; ++i) {
        sum += arr[i];
    }
    return sum;
}


int main(int argc, char *argv[]) {
    int i;
    char results[SHOTS];
    float avg;
    StateVector sv;
    new_sv(&sv, 3, 0);
    
    // Experiment 1: |00> state
    for (i = 0; i < SHOTS; ++i) {
        h(sv, 2);
        cswap(sv, 2, 0, 1);
        h(sv, 2);
        x(sv, 2);
        results[i] = measure(sv, 2);
        reset_sv(sv, 0);
    }
    avg = (float)sum(results, SHOTS) / (float)SHOTS;
    printf("Average result of swap test for |00> state: %f\n", avg);

    // Experiment 2: |01> state
    for (i = 0; i < SHOTS; ++i) {
        reset_sv(sv, 1);
        h(sv, 2);
        cswap(sv, 2, 0, 1);
        h(sv, 2);
        x(sv, 2);
        results[i] = measure(sv, 2);
    }
    avg = (float)sum(results, SHOTS) / (float)SHOTS;
    printf("Average result of swap test for |01> state: %f\n", avg);

    // Experiment 3: 1/v2|00> + 1/v2|01> state
    for (i = 0; i < SHOTS; ++i) {
        reset_sv(sv, 0);
        h(sv, 1);
        h(sv, 2);
        cswap(sv, 2, 0, 1);
        h(sv, 2);
        x(sv, 2);
        results[i] = measure(sv, 2);
    }
    avg = (float)sum(results, SHOTS) / (float)SHOTS;
    printf("Average result of swap test for 1/sqrt(2)|00> + 1/sqrt(2)|01> state: %f\n", avg);
    del_sv(sv);
}
