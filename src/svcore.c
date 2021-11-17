#include <complex.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/random.h>
#include "svcore.h"

/***********
 * Helpers *
 ***********/
double prob(double complex amp) {
    return amp * conj(amp);
}

double drand() {
    ssize_t bytes_written;
    unsigned long int random_number;
    unsigned long int ulmax = ~(unsigned long int) 0;
    bytes_written = getrandom(&random_number, sizeof(unsigned long int), 0);
    return ((double) random_number)/((double) ulmax);
}


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/*
 * Interpreting n as a string of 1s and 0s, insert bit (which should
 * be 1 or 0) into that string at position place.
 * 
 * Example:
 *
 * insert(4, 1, 1) -> 10
 *
 * or in binary
 *
 * insert(100, 1, 1) -> 1010
 *
 * Index 0 is the least significant bit, so for insert(4, 1, 1):
 *
 * 100 -> 1010
 * ---    ----
 * 210    3210
 *
 * and for insert(4, 1, 2):
 *
 * 100 -> 1100
 * ---    ----
 * 210    3210
 */
int insert(int n, int bit, int place) {
    int mask = (1 << place) - 1;
    int back = n & mask;
    int front = n & (~mask);
    return back + (bit * (1 << place)) + (front << 1);
}

/******************************
 * Create/destroy StateVector *
 ******************************/
void new_sv(StateVector *sv, int num_qubits, int state) {
    int i;
    sv->n = (1 << num_qubits);
    sv->states = (double complex*)calloc(sv->n, sizeof(double complex));
    #pragma omp parallel for
    for (i = 0; i < sv->n; ++i) {
        sv->states[i] = 0.0;
    }
    if (state < sv->n) sv->states[state] = 1.0;
}

void reset_sv(StateVector sv, int state) {
    int i;
    if (state >= sv.n) return;
    #pragma omp parallel for
    for(i = 0; i < sv.n; ++i) {
        sv.states[i] = 0.0;
    }
    sv.states[state] = 1.0;
}

void del_sv(StateVector sv) {
    free(sv.states);
}


/**********************
 * Single qubit gates *
 **********************/

void one_qubit_gate(StateVector sv, int qubit, double complex matrix[4]) {
    int i, p, q;
    double complex old0, old1;
    #pragma omp parallel for
    for (i = 0; i < (sv.n >> 1); ++i) {
        p = insert(i, 0, qubit);
        q = insert(i, 1, qubit);
        old0 = sv.states[p];
        old1 = sv.states[q];
        sv.states[p] = matrix[0]*old0 + matrix[1]*old1;
        sv.states[q] = matrix[2]*old0 + matrix[3]*old1;
    }
}

void x(StateVector sv, int qubit) {
    int i, p, q;
    double complex temp;
    #pragma omp parallel for
    for (i = 0; i < (sv.n >> 1); ++i) {
        p = insert(i, 0, qubit);
        q = insert(i, 1, qubit);
        temp = sv.states[p];
        sv.states[p] = sv.states[q];
        sv.states[q] = temp;
    }
}

/* This function is to make a performance comparison. Comparing to x we can see
 * if there is a performance degradation due to performing unnecessary 
 * multiplications by 0 that occur in this function due to using one_qubit_gate
 */
void x2(StateVector sv, int qubit) {
    double complex matrix [4] = {0, 1, 1, 0};
    one_qubit_gate(sv, qubit, matrix);
}

void y(StateVector sv, int qubit) {
    double complex matrix [4] = {0, -I, I, 0};
    one_qubit_gate(sv, qubit, matrix);
}

void z(StateVector sv, int qubit) {
    double complex matrix [4] = {1, 0, 0, -1};
    one_qubit_gate(sv, qubit, matrix);
}

void phase(StateVector sv, int qubit, double phi) {
    double complex matrix [4] = {1, 0, 0, cexp(I*phi)};
    one_qubit_gate(sv, qubit, matrix);
}

void h(StateVector sv, int qubit) {
    double complex matrix [4];
    double complex invrt2 = 1/SQRT2;
    matrix[0] = invrt2; matrix[1] = invrt2;
    matrix[2] = invrt2; matrix[3] = -invrt2;
    one_qubit_gate(sv, qubit, matrix);
}

void rot(StateVector sv, int qubit, double theta) {
    double complex matrix [4] = {cos(theta), -sin(theta), sin(theta), cos(theta)};
    one_qubit_gate(sv, qubit, matrix);
}

int measure(StateVector sv, int qubit) {
    int i, p, q, result;
    double prob_zero = 0.0;
    double renorm;
    if ((1 << qubit) >= sv.n) return -1;
    #pragma omp parallel for
    for (i = 0; i < (sv.n >> 1); ++i) {
        p = insert(i, 0, qubit);
        prob_zero += sv.states[p] * conj(sv.states[p]);
    }
    if (prob_zero == 0.0) return 1;
    result = drand() > prob_zero;
    if (result == 0) {
        renorm = 1.0/sqrt(prob_zero);
    } else {
        renorm = 1.0/sqrt(1-prob_zero);
    }
    #pragma omp parallel for
    for (i = 0; i < (sv.n >> 1); ++i) {
        p = insert(i, result, qubit);
        q = insert(i, 1-result, qubit);
        sv.states[p] = renorm*sv.states[p];
        sv.states[q] = 0.0;
    }
    return result;
}

void write(StateVector sv, int qubit, int value) {
    int result = measure(sv, qubit);
    if (result < 0) return;
    else if (result != value) x(sv, qubit);
}

/*******************
 * Two qubit gates *
 *******************/

void two_qubit_gate(StateVector sv, int q1, int q2, double complex matrix[16]) {
    int i, j, k, template;
    int p[4];
    int qmin = MIN(q1, q2);
    int qmax = MAX(q1, q2);
    double complex old[4];
    #pragma omp parallel for
    for (i = 0; i < (sv.n >> 2); ++i) {
        template = insert(insert(i, 0, qmin), 0, qmax);
        p[0] = template;
        p[1] = template + (1 << q1);
        p[2] = template + (1 << q2);
        p[3] = template + (1 << q1) + 1 << q2;
        for (j = 0; j < 4; ++j) old[j] = sv.states[p[j]];
        for (j = 0; j < 4; ++j) {
            sv.states[p[j]] = (double complex) 0.;
            for (k = 0; k < 4; ++k) {
                sv.states[p[j]] += matrix[4*j + k] * old[k];
            }
        }
    }
}

void c_one_qubit_gate(StateVector sv, int q1, int q2, double complex matrix[4]) {
    int i, template, p1, p2;
    double complex old1, old2;
    int qmin = MIN(q1, q2);
    int qmax = MAX(q1, q2);
    #pragma omp parallel for
    for (i = 0; i < (sv.n >> 2); ++i) {
        template = insert(insert(i, 0, qmin), 0, qmax);
        p1 = template + (1 << q1);
        p2 = template + (1 << q1) + (1 << q2);
        old1 = sv.states[p1];
        old2 = sv.states[p2];
        sv.states[p1] = matrix[0]*old1 + matrix[1]*old2;
        sv.states[p2] = matrix[2]*old1 + matrix[3]*old2;
    }
}
   

void cx(StateVector sv, int q1, int q2) {
    int i, template, p, q;
    double complex old;
    int qmin = MIN(q1, q2);
    int qmax = MAX(q1, q2);
    #pragma omp parallel for
    for (i = 0; i < (sv.n >> 2); ++i) {
        template = insert(insert(i, 0, qmin), 0, qmax);
        p = template + (1 << q1);
        q = template + (1 << q1) + (1 << q2);
        old = sv.states[p];
        sv.states[p] = sv.states[q];
        sv.states[q] = old;
    }
}
