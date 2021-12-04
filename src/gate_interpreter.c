#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "svcore.h"
#include "util.h"

int main(int argc, char* argv[]) {
    FILE* file;
    char gate[10];
    StateVector sv;
    int n=0, q1, q2, q3, readout;
    double d1;

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
        } else if (strcmp(gate, "y") == 0) {
            fscanf(file, "%d", &q1);
            y(sv, q1);
        } else if (strcmp(gate, "z") == 0) {
            fscanf(file, "%d", &q1);
            z(sv, q1);
        } else if (strcmp(gate, "phase") == 0) {
            fscanf(file, "%d %lf", &q1, &d1);
            phase(sv, q1, d1*PI/180.);
        } else if (strcmp(gate, "rot") == 0) {
            fscanf(file, "%d %lf", &q1, &d1);
            rot(sv, q1, d1*PI/180.);
        } else if (strcmp(gate, "h") == 0) {
            fscanf(file, "%d", &q1);
            h(sv, q1);
        } else if (strcmp(gate, "cx") == 0) {
            fscanf(file, "%d %d", &q1, &q2);
            cx(sv, q1, q2);
        } else if (strcmp(gate, "cphase") == 0) {
            fscanf(file, "%d %d %lf", &q1, &q2, &d1);
            double complex matrix[4] = {1, 0, 0, cexp(I*d1*PI/180.)};
            c_one_qubit_gate(sv, q1, q2, matrix);
        } else if (strcmp(gate, "swap") == 0) {
            fscanf(file, "%d %d", &q1, &q2);
            swap(sv, q1, q2);
        } else if (strcmp(gate, "cswap") == 0) {
            fscanf(file, "%d %d %d", &q1, &q2, &q3);
            cswap(sv, q1, q2, q3);
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
