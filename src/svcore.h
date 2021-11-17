#pragma once
#include <complex.h>
#define SQRT2 1.4142135623730951454746218587388284504413604736328125
#define PI 3.141592653589793115997963468544185161590576171875

typedef struct StateVector {
    double complex *states; // state vector
    int n;                  // size of the state vector (2^<num qubits>)
} StateVector;


double prob(double complex amp);
int insert(int n, int bit, int place);
void new_sv(StateVector *sv, int num_qubits, int state);
void reset_sv(StateVector sv, int state);
void del_sv(StateVector sv);
void one_qubit_gate(StateVector sv, int qubit, double complex matrix[4]);
void x(StateVector sv, int qubit);
void x2(StateVector sv, int qubit);
void y(StateVector sv, int qubit);
void z(StateVector sv, int qubit);
void phase(StateVector sv, int qubit, double phi);
void h(StateVector sv, int qubit);
void rot(StateVector sv, int qubit, double theta);
int measure(StateVector sv, int qubit);
void write(StateVector sv, int qubit, int value);
void two_qubit_gate(StateVector sv, int q1, int q2, double complex matrix[16]);
void c_one_qubit_gate(StateVector sv, int q1, int q2, double complex matrix[4]);
void cx(StateVector sv, int q1, int q2);

