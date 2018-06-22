CFLAGS = -g -Wall -Wno-unused-result -O3
LDLIBS = -l hiredis -l readline

all: controlador exemplo1 exemplo2 principal

controlador: controlador.c

exemplo1: campo.h campo.c exemplo1.c

exemplo2: campo.h campo.c exemplo2.c

principal: campo.h campo.c principal.c 

clean:
	rm -f controlador exemplo1 exemplo2 principal *.o
