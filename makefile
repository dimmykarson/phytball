CFLAGS = -g -Wall -Wno-unused-result -O3 -lm
LDLIBS = -l hiredis -l readline
LIBS = -lm -w

all: controlador aux principal exemplo2

controlador: controlador.c

exemplo2: campo.h campo.c exemplo2.c

principal: campo.h campo.c principal.c
	gcc -o $@ $^ $(LDLIBS) $(LIBS) 

aux: campo.h campo.c aux_file.c
	gcc -o $@ $^ $(LDLIBS) $(LIBS) 
	


clean:
	rm -f controlador exemplo1 exemplo2 principal *.o
