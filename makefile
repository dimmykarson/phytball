CFLAGS = -g -Wall -Wno-unused-result -O3 -lm
LDLIBS = -l hiredis -l readline

all: controlador principal exemplo2

controlador: controlador.c

exemplo2: campo.h campo.c exemplo2.c

principal: campo.h campo.c principal.c
	gcc -g -c campo.h
	gcc -g -c campo.c -o campo.o
	gcc -g -c principal.c -o principal.o -lm

clean:
	rm -f controlador exemplo1 exemplo2 principal *.o
