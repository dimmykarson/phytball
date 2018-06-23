#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "campo.h"
#include <stdbool.h>
#include <float.h>
#include <math.h>

#define MAXSTR 512
#define MAXINT 16
#define MAXD 8

#define new_max(x,y) ((x) >= (y)) ? (x) : (y)
#define new_min(x,y) ((x) <= (y)) ? (x) : (y)


char * def_movimento(char campo[], int tam);
bool pode_vencer(char campo[], int jogador, int tam);
double minimax(char * campo, int jogador, double alpha, double beta, int depth, int tam);
int gerar_movimentos(char* campo, int tam, int jogador, int bola, char** movimentos);
double estimativa(char campo[], int tam, int bola, int jogador);
void parse_mov(char *buf, char *rl, char *rm, int *rf, int *rs, int *rb);
int aplica_mov(char *buf, char *campo, int tam, char rl, char rm, int rf, int rs, int *rb);
int get_bola(char * campo, int tam);

double sigmoide(double x);


int main(int argc, char **argv) {
	char buf[MAXSTR];
	char campo[MAXSTR]; 
	char lado_meu; //'e' se seu lado é o esquerdo
	char lado_adv; //'d' se seu lado é o esquerdo
	char mov_adv; // ultima jogada do oponente f ou o ou n
	int tam_campo;//tamanho do campo
	int pos_filo;
	int pos_bola[MAXINT];
	int num_saltos;
	int i;



	campo_conecta(argc, argv);
	printf("Player conectado\n");
	
	while(1) {
	    campo_recebe(buf);
	    printf("%s", buf);

		sscanf(strtok(buf, " \n"), "%c", &lado_meu);
		sscanf(strtok(NULL, " \n"), "%d", &tam_campo);
		sscanf(strtok(NULL, " \n"), "%s", campo);
		sscanf(strtok(NULL, " \n"), "%c", &lado_adv);
		sscanf(strtok(NULL, " \n"), "%c", &mov_adv);
		

		//sprintf(buf, "%c %c %d\n", mov->player, mov->tipo, 1);

		char* mov = def_movimento(campo, tam_campo);
		char rl;
  		char rm;
  		int rf;
  		int rs;
  		int rb[MAXINT];
		parse_mov(mov, &rl, &rm, &rf, &rs, rb);



	    campo_envia(buf);
  	}
}

/**
 jogador pode vencer com saltos?
**/
char * def_movimento(char campo[], int tam){
	char * mov_to_do;
	double aux = -INFINITY;
	char ** acoes;
	int bola = get_bola(campo, tam);
	int qt_movimentos_possiveis= gerar_movimentos(campo, tam, 1, bola, acoes);
	for(int i = 0;i<qt_movimentos_possiveis;i++){
		char * result;
		char * mov;
		strcpy(mov, acoes[i]);
		char rl;
  		char rm;
  		int rf;
  		int rs;
  		int rb[MAXINT];
		parse_mov(mov, &rl, &rm, &rf, &rs, rb);
		aplica_mov(campo, result, tam, rl, rm, rf, rs, rb);
		strcpy(mov, acoes[i]);
		double v = minimax(result, -1, -INFINITY, INFINITY, 0, tam);
		if(v>=aux){
			aux = v;
			strcpy(mov_to_do, acoes[i]);
		}
	}
	return mov_to_do;
}

int get_bola(char * campo, int tam){
	for(int i = 0;i<tam;i++){
		if(campo[i]=='o'){
			return i;
		}
	}
	return -1;
}


double minimax(char * campo, int jogador, double alpha, double beta, int depth, int tam){
	int bola = get_bola(campo, tam);
	bool vence_em_1 = pode_vencer(campo, jogador, tam);
	if(depth==MAXD && !vence_em_1){
		return estimativa( campo,  tam,  bola, jogador);
	}
	if(vence_em_1){
		return jogador;
	}
	double v = 0;
	if(jogador>0)
		v = -INFINITY;
	else
		v = INFINITY;
	char ** acoes;
	int qt_movimentos_possiveis= gerar_movimentos(campo, tam, jogador, bola, acoes);
	for(int i = 0;i<qt_movimentos_possiveis;i++){
		char * result;
		char * mov;
		strcpy(mov, acoes[i]);
		char rl;
  		char rm;
  		int rf;
  		int rs;
  		int rb[MAXINT];
		parse_mov(mov, &rl, &rm, &rf, &rs, rb);
		strcpy(mov, acoes[i]);
		if(!aplica_mov(campo, result, tam, rl, rm, rf, rs, rb))
			return v;
		if(jogador > 0){
			v = new_max(v, minimax(result,-jogador, alpha, beta, ++depth, tam));
			if(v>=beta){
				return v;
			}
			alpha = new_max(alpha, v);
		}else{
			v = new_min(v, minimax(result,-jogador, alpha, beta, ++depth, tam));
			if(v<=alpha){
				return v;
			}
			beta = new_min(beta, v);
		}
	}
	return v;
}

double estimativa(char campo[], int tam, int bola, int jogador){
	int qt_e = 0;
	int qt_d = 0;
	for(int i = 0;i<tam;i++){
		if(campo[i]=='o'){
			bola = i;
		}else if(campo[i]=='f' && bola<0){
			qt_e++;
		}else if(campo[i]=='f' && bola>=0){
			qt_d++;
		}
	}
	float mult = 1;
	if(jogador>0){
		if(campo[0]=='f')
			mult=1.5;
		mult*=qt_e;
	}else{
		if(campo[tam-1]=='f')
			mult=1.5;
		mult*=qt_d;
	}
	return sigmoide(mult)*jogador;
}

int aplica_mov(char *buf, char *campo, int tam,
	       char rl, char rm, int rf, int rs, int *rb) {

  int i, j, inc, bola;

  strcpy(campo, buf);
  if(rm == 'n')
    return 1;
  if(rm == 'f') {
    if(buf[--rf] == '.') {
      buf[rf] = 'f';
      return 1;
    }
    return 0;
  }
  if(rm == 'o') {
    if(rs < 1)
      return 0;
    for(bola = 0; bola < tam; bola++)
      if(buf[bola] == 'o')
	break;
    for(i = 0; i < rs; i++) {
      inc = (--rb[i] < bola) ? -1 : 1;
      for(j = bola + inc; j != rb[i]; j += inc)
	if(buf[j] != 'f')
	  return 0;
        else
	  buf[j] = '.';
      buf[bola] = '.';
      bola = rb[i];
      if(bola < 0 && rl == 'e')
	return 0;
      if(bola >= tam && rl == 'd')
	return 0;
      if(bola >= 0 && bola < tam)
	buf[bola] = 'o';
    }
    return 1;
  }
  return 0;
}

void parse_mov(char *buf, char *rl, char *rm, int *rf, int *rs, int *rb) {
  int i;
  
  sscanf(strtok(buf, " \n"), "%c", rl);
  sscanf(strtok(NULL, " \n"), "%c", rm);
  if(*rm != 'n') {
    if(*rm == 'f')
      sscanf(strtok(NULL, " \n"), "%d", rf);
    else {
      sscanf(strtok(NULL, " \n"), "%d", rs);
      for(i = 0; i < *rs; i++){
        sscanf(strtok(NULL, " \n"), "%d", &(rb[i]));
      }
    }
  }
}

int gerar_movimentos(char* campo, int tam, int jogador, int bola, char** movimentos){
	int qt_movimentos_possiveis = 0;

	if(jogador>0){
		if(bola==0 && campo[1]=='.'){
			//coloca 'f'
			qt_movimentos_possiveis++;
		}else if(bola==0 && campo[1]=='f'){
			//posso pular ou colocar algum f em alguma posicão
			qt_movimentos_possiveis++;
		}else if(bola!=0){
			for(int i=0;i<bola;i++){
				if(campo[i]=='.'){
					//posso colocar um f aqui
					qt_movimentos_possiveis++;
				}
			}
			int qt_ponto = 0;
			if(campo[bola-1]=='f'){
				for(int i = bola-2;i>=-1;i--){
					if(i==-1){
						//jogador pode vencer nessa jogada
						qt_movimentos_possiveis++;
					}
					if(campo[i]=='.' && qt_ponto<1){
						qt_ponto++;
					}
					if(qt_ponto>1){
						break;
					}
					if(campo[i]=='f'){
						qt_ponto = 0;
						continue;
					}
					//posso saltar para i
					qt_movimentos_possiveis++;
				}
			}
		}
	}else{
		if(bola==tam-1 && campo[tam-1]=='.'){
			//coloca 'f'
			qt_movimentos_possiveis++;
		}else if(bola==tam-1 && campo[1]=='f'){
			//posso pular ou colocar algum f em alguma posicão
			qt_movimentos_possiveis++;
		}else if(bola!=tam-1){
			for(int i=bola;i>=0;i--){
				if(campo[i]=='.'){
					//posso colocar um f aqui
					qt_movimentos_possiveis++;
				}
			}
			int qt_ponto = 0;
			if(campo[bola+1]=='f'){
				for(int i = bola+2;i<=tam+1;i++){
					if(i==tam+1){
						qt_movimentos_possiveis++;
					}
					if(campo[i]=='.' && qt_ponto<1){
						qt_ponto++;
					}
					if(qt_ponto>1){
						break;
					}
					if(campo[i]=='f'){
						qt_ponto = 0;
						continue;
					}
					//posso saltar para i
					qt_movimentos_possiveis++;
				}
			}
		}		
	}
	return qt_movimentos_possiveis;
}

bool pode_vencer(char campo[], int jogador, int tam){

	if(jogador<0){
		int qt_ponto = 0;
		for(int i = tam-1;i>=0;i--){
			if(campo[i]=='.' && qt_ponto<1){
				qt_ponto++;
			}
			if(qt_ponto>1){
				return false;
			}
			if(campo[i]=='f'){
				qt_ponto = 0;
				continue;
			}
			if(campo[i]=='o'){
				return true;
			}
		}
	}else{
		int qt_ponto = 0;
		for(int i = 0;i<tam;i++){
			if(campo[i]=='.' && qt_ponto<1){
				qt_ponto++;
			}
			if(qt_ponto>1){
				return false;
			}
			if(campo[i]=='f'){
				qt_ponto = 0;
				continue;
			}
			if(campo[i]=='o'){
				return true;
			}
		}
	}

	return false;
}

double sigmoide(double x){
	return 1.0/(1+exp(-x));
}