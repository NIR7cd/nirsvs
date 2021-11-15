#include <stdio.h>

#include "../src/util.h"
#include "../src/svcore.h"

#define SHOTS 1000

int main(int argc, char* argv[]) {
    int i, state;
    int qubit_counts[4];
    StateVector sv;

    new_sv(&sv, 2, 0);
    for (i = 0; i < 4; ++i) {
        qubit_counts[i] = 0;
    }
    for (i = 0; i < SHOTS; ++i) {
        h(sv, 0);
        cx(sv, 0, 1);
        state = 0;
        state += measure(sv, 0);
        state += (measure(sv, 1) << 1);
        qubit_counts[state]++;
        reset_sv(sv, 0);
    }
    for (i = 0; i < 4; ++i) {
        printf("%01d%01d: %d\n", (i&2)/2, i&1, qubit_counts[i]);
    }
    del_sv(sv);
}
