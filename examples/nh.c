#include <stdio.h>
#include <stdlib.h>
#include "../src/svcore.h"

int main(int argc, char* argv[]) {
    int i, n;
    StateVector sv;
    if (argc < 2) {
        printf("Please supply the number of qubits to use as the first argument\n");
        return 1;
    }
    n = atoi(argv[1]);
    new_sv(&sv, n, 0);
    for (i = 0; i < n; ++i) {
        x(sv, i);
    }
    del_sv(sv);
    return 0;
}
