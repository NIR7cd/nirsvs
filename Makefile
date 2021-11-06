CC=gcc --std=c99

.PHONY: clean

svcore.o:
	${CC} -c -O3 src/svcore.c -o svcore.o

nirqgi.o:
	${CC} -c src/gate_interpreter.c -o nirqgi.o

nirqgi: svcore.o nirqgi.o
	${CC} svcore.o -lm nirqgi.o -o nirqgi

clean:
	rm *.o
