#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "campo.h"

#define MAXSTR 512
#define MAXINT 16


int main(int argc, char **argv) {
	char buf[MAXSTR];
	char campo[MAXSTR]; 
	char lado_meu;
	char lado_adv;
	char mov_adv;
	int tam_campo;
	int pos_filo;
	int pos_bola[MAXINT];
	int num_saltos;
	int i;

	campo_conecta(argc, argv);
	printf("Player conectado\n");
	
	while(1) {
	    campo_recebe(buf);
	    
		sscanf(strtok(buf, " \n"), "%c", &lado_meu);
		sscanf(strtok(NULL, " \n"), "%d", &tam_campo);
		sscanf(strtok(NULL, " \n"), "%s", campo);
		sscanf(strtok(NULL, " \n"), "%c", &lado_adv);
		sscanf(strtok(NULL, " \n"), "%c", &mov_adv);

		printf("%c %d %s\n", lado_meu, tam_campo, campo);
  		printf("%c %c", lado_adv, mov_adv);


	    campo_envia(buf);
  	}


}