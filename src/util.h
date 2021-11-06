#pragma once

#include <stdio.h>
#include "svcore.h"

void print_state(StateVector sv) {
    int i;
    printf("---\n");
    printf("State\n");
    for (i = 0; i < sv.n; ++i) {
        printf("%x: %f + %fi\n", i, creal(sv.states[i]), cimag(sv.states[i]));
    }
    printf("---\n");
}

void print_probs(StateVector sv) {
    int i;
    printf("---\n");
    printf("Probabilities\n");
    for (i = 0; i < sv.n; ++i) {
        printf("%x: %f\n", i, prob(sv.states[i]));
    }
    printf("---\n");
}
