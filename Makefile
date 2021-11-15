CC=gcc --std=c99
#OMP=-fopenmp
OMP=

.PHONY: clean

svcore.o:
	${CC} -c -O3 ${OMP} src/svcore.c -o svcore.o

nirqgi.o:
	${CC} -c src/gate_interpreter.c -o nirqgi.o

nirqgi: svcore.o nirqgi.o
	${CC} svcore.o -lm ${OMP} nirqgi.o -o nirqgi

bell_experiment: svcore.o 
	${CC} svcore.o -lm ${OMP} -O3 examples/bell_experiment.c -o bell_experiment

nh: svcore.o
	${CC} svcore.o -lm -O3 ${OMP} examples/nh.c -o nh

clean:
	rm *.o
