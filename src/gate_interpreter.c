#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "svcore.h"
#include "util.h"

int main(int argc, char* argv[]) {
    FILE* file;
    char gate[10];
    StateVector sv;
    int n=0, q1, q2, readout;

    if (argc > 1) {
        file = fopen(argv[1], "r");
    } else {
        file = stdin;
    }
    fscanf(file, "qubits %d", &n);
    new_sv(&sv, n, 0);
    printf("Initialized state vector for %d qubit system\n", n);
    while (1) {
        fscanf(file, "%s", gate);
        if (strcmp(gate, "exit") == 0) {
            break;
        } else if (strcmp(gate, "x") == 0) {
            fscanf(file, "%d", &q1);
            x(sv, q1);
        } else if (strcmp(gate, "h") == 0) {
            fscanf(file, "%d", &q1);
            h(sv, q1);
        } else if (strcmp(gate, "cnot") == 0) {
            fscanf(file, "%d %d", &q1, &q2);
            cnot(sv, q1, q2);
        } else if (strcmp(gate, "measure") == 0) {
            fscanf(file, "%d", &q1);
            readout = measure(sv, q1);
            printf("%d\n", readout);
        } else if (strcmp(gate, "write") == 0) {
            fscanf(file, "%d %d", &q1, &readout);
            write(sv, q1, readout);
        } else if (strcmp(gate, "state") == 0) {
            print_state(sv);
        } else if (strcmp(gate, "probs") == 0) {
            print_probs(sv);
        } else {
            printf("%s is not a valid gate\n", gate);
        }
    }
    if (file != stdin) fclose(file);
    del_sv(sv);
}
