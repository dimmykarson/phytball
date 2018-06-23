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
#define MAXACOES 100

#define new_max(x,y) ((x) >= (y)) ? (x) : (y)
#define new_min(x,y) ((x) <= (y)) ? (x) : (y)


movimento def_movimento(char campo[], int tam, char lado_meu);
int gerar_movimentos(char* campo, int tam, int jogador, int bola, movimento movimentos[], char lado_meu);

bool pode_vencer(char campo[], int jogador, int tam);
double minimax(char * campo, int jogador, double alpha, double beta, int depth, int tam, char lado_meu);



double estimativa(char campo[], int tam, int bola, int jogador, char lado_meu);
void parse_mov(char *buf, char *rl, char *rm, int *rf, int *rs, int *rb);
int aplica_mov(char *buf, char *campo, int tam, char rl, char rm, int rf, int rs, int *rb);
int get_bola(char * campo, int tam);

double sigmoide(double x);


char * transform_e(char * campo, int i_bola, int tam);
void summing_e(char * campo, int bola, int tam, int sum[]);
int pl(int bsum, int psum);


char * transform_d(char * campo, int i_bola, int tam);
void summing_d(char * campo, int bola, int tam, int sum[]);


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
		


		movimento mov = def_movimento(campo, tam_campo, lado_meu);



		//sprintf(buf, "%c %c %d\n", mov->player, mov->tipo, 1);


	    campo_envia(buf);
  	}
}

/**
 jogador pode vencer com saltos?
**/
movimento def_movimento(char campo[], int tam, char lado_meu){
	movimento mov_to_do;
	double aux = -INFINITY;
	movimento acoes[MAXACOES];
	int bola = get_bola(campo, tam);
	int qt_movimentos_possiveis= gerar_movimentos(campo, tam, 1, bola, acoes, lado_meu);
	for(int i = 0;i<qt_movimentos_possiveis;i++){
		char * result;
		movimento mov = acoes[i];
		char rl;
  		char rm;
  		int rf;
  		int rs;
  		int rb[MAXINT];
		parse_mov(mov.mov, &rl, &rm, &rf, &rs, rb);
		aplica_mov(mov.mov, result, tam, rl, rm, rf, rs, rb);
		char lado = 'e';
		if(lado_meu=='e')
			lado = 'd';
		double v = minimax(result, -1, -INFINITY, INFINITY, 0, tam, lado);
		if(v>=aux){
			aux = v;
			mov_to_do = acoes[i];
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


double minimax(char * campo, int jogador, double alpha, double beta, int depth, int tam, char lado_meu){
	int bola = get_bola(campo, tam);
	bool vence_em_1 = pode_vencer(campo, jogador, tam);
	if(depth==MAXD && !vence_em_1){
		return estimativa(campo,  tam,  bola, jogador, lado_meu);
	}
	if(vence_em_1){
		return jogador;
	}
	double v = 0;
	if(jogador>0)
		v = -INFINITY;
	else
		v = INFINITY;
	movimento acoes[MAXACOES];
	char lado = 'e';
	if(lado_meu=='e')
			lado = 'd';
	int qt_movimentos_possiveis= gerar_movimentos(campo, tam, jogador, bola, acoes, lado_meu);
	for(int i = 0;i<qt_movimentos_possiveis;i++){
		char * result;
		movimento mov = acoes[i];
		char rl;
  		char rm;
  		int rf;
  		int rs;
  		int rb[MAXINT];
		parse_mov(mov.mov, &rl, &rm, &rf, &rs, rb);
		if(!aplica_mov(mov.mov, result, tam, rl, rm, rf, rs, rb))
			return v;
		if(jogador > 0){
			v = new_max(v, minimax(result,-jogador, alpha, beta, ++depth, tam, lado));
			if(v>=beta){
				return v;
			}
			alpha = new_max(alpha, v);
		}else{
			v = new_min(v, minimax(result,-jogador, alpha, beta, ++depth, tam, lado));
			if(v<=alpha){
				return v;
			}
			beta = new_min(beta, v);
		}
	}
	return v;
}

double estimativa(char campo[], int tam, int bola, int jogador, char lado_meu){
	int i_bola = get_bola(campo, tam);
	if(lado_meu=='e'){
		char * campo_e = transform_e(campo, i_bola, tam);
		int i_bola_e = get_bola(campo_e, tam);
		int sum[2];
		summing_e(campo_e, i_bola, tam, sum);
		int p_e = pl(sum[0], sum[1]);
		return jogador*p_e;
	}else{
        char * campo_d = transform_d(campo, i_bola, tam);
		int i_bola_d = get_bola(campo_d, tam);
		int sum[2];
		summing_d(campo_d, i_bola, tam, sum);
		int p_e = pl(sum[0], sum[1]);
		return jogador*p_e;
	}
	return 0.0;
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

void vetor_saltos_para_d(int saltos[], char * campo, int bola, int tam){
	if(campo[bola+1]!='.'){
		int qt_pontos = 0;
		int saltos[tam-bola];
		int index = 0;
		for(int i = bola+1;i<tam;i++){
			if(campo[i]=='f'){
				qt_pontos=0;
				continue;
			}else if(campo[i]=='.' && qt_pontos==0){
				qt_pontos++;
				//alcançavel
				saltos[index]=i;
				index++;
			}else if(campo[i]=='.' && qt_pontos!=0){
				break;
			}
		}
	}
}


void vetor_saltos_para_e(int saltos[], char * campo, int bola, int tam){
		if(campo[bola-1]!='.'){
			int qt_pontos = 0;
			int saltos[tam-bola];
			int index = 0;
			for(int i = bola-1;i>=0;i--){
				if(campo[i]=='f'){
					qt_pontos=0;
					continue;
				}else if(campo[i]=='.' && qt_pontos==0){
					qt_pontos++;
					//alcançavel
					saltos[index]=i;
					index++;
				}else if(campo[i]=='.' && qt_pontos!=0){
					break;
				}
			}	
		}
}


int gerar_movimentos(char* campo, int tam, int jogador, int bola, movimento movimentos[], char lado_meu){
	int qt_movimentos_possiveis = 0;
	for(int i = 0;i<tam;i++){
		if(campo[i]=='.'){
			movimento mov;
			mov.tipo='f';
			mov.posicao=i;
			movimentos[qt_movimentos_possiveis]=mov;
			qt_movimentos_possiveis++;
		}
	}
	int saltos_d[tam-bola];
	vetor_saltos_para_d(saltos_d, campo, bola, tam);
	int l_d = sizeof(saltos_d);
	int saltos_e[tam-bola];
	vetor_saltos_para_e(saltos_e, campo, bola, tam);
	int l_e = sizeof(saltos_e);

	if(lado_meu=='e'){
		movimento m_d;
		m_d.tipo ='o';
		m_d.player = lado_meu;
		m_d.saltos = saltos_d;
		m_d.qt_saltos = l_d;
		movimentos[qt_movimentos_possiveis]=m_d;
		qt_movimentos_possiveis++;	
		if(l_e>0){
			movimento m_x;
			m_x.tipo ='o';
			m_x.player = lado_meu;
			m_x.posicao = saltos_e[0];
			movimentos[qt_movimentos_possiveis]=m_x;
			qt_movimentos_possiveis++;	
		}
	}else{
		movimento m_d;
		m_d.tipo ='o';
		m_d.player = lado_meu;
		m_d.saltos = saltos_e;
		m_d.qt_saltos = l_e;
		movimentos[qt_movimentos_possiveis]=m_d;
		qt_movimentos_possiveis++;	
		if(l_d>0){
			movimento m_x;
			m_x.tipo ='o';
			m_x.player = lado_meu;
			m_x.posicao = saltos_d[0];
			movimentos[qt_movimentos_possiveis]=m_x;
			qt_movimentos_possiveis++;	
		}
	}
	return qt_movimentos_possiveis;
}

bool pode_vencer(char campo[], int jogador, int tam){
	
	return false;
}

double sigmoide(double x){
	return 1.0/(1+exp(-x));
}


void summing_e(char * campo, int bola, int tam, int sum[]){
	int bsum = 0;
	int psum = 0;
	for(int i = tam-1;i>=bola;i--){
		if(i==bola){
			if(campo[i]=='.'){
				bsum++;
				psum++;
			}
			if(bsum>0 && campo[i]=='f'){
				bsum--;
			}
		}else{
			if(campo[i]=='.'){
				bsum++;
				psum++;
			}
			if(bsum>0 && campo[i]=='f'){
				bsum--;
			}
			if(psum>0 && campo[i]=='f'){
				psum--;
			}	
		}
		
	}
	sum[0] = bsum;
	sum[1] = psum;
	printf("Somou\n");
}

char * transform_e(char * campo, int i_bola, int tam){
	char * pl;
	strcpy(pl, campo);
	int i_sub = i_bola+1;
	pl[i_bola] = pl[i_sub];
	pl[i_sub] = 'o';
	int qt_impar = 0;
	int max_impar = -1;
	for(int i = i_sub;i<tam;i++){
		if(pl[i]=='.'){
			qt_impar++;
			max_impar=i;
		}else{
			if(qt_impar%2!=0){
				pl[max_impar]='f';
				qt_impar = 0;
				max_impar = -1;	
			}
		}
		if(i==tam-1){
			if(qt_impar%2!=0){
				pl[max_impar]='f';
				qt_impar = 0;
				max_impar = -1;	
			}	
		}
	}
	return pl;
}



int pl(int bsum, int psum){
	printf("4 \n");
	int p_e = 0;
	int kno = 0;
	if(bsum == psum){
		kno=1;
	}
	printf("4 \n");

	p_e = floor(bsum/4)+(1-kno);

	return p_e;
}




//Estimativa para d
void summing_d(char * campo, int bola, int tam, int sum[]){
	int bsum = 0;
	int psum = 0;
	for(int i = 0;i<=bola;i++){
		if(i==bola){
			if(campo[i]=='.'){
				bsum++;
				psum++;
			}
			if(bsum>0 && campo[i]=='f'){
				bsum--;
			}
		}else{
			if(campo[i]=='.'){
				bsum++;
				psum++;
			}
			if(bsum>0 && campo[i]=='f'){
				bsum--;
			}
			if(psum>0 && campo[i]=='f'){
				psum--;
			}	
		}
		
	}
	sum[0] = bsum;
	sum[1] = psum;
}

char * transform_d(char * campo, int i_bola, int tam){
	char * pl;
	strcpy(pl, campo);
	int i_sub = i_bola-1;
	pl[i_bola] = pl[i_sub];
	pl[i_sub] = 'o';
	int qt_impar = 0;
	int max_impar = -1;
	for(int i = i_sub;i>=0;i--){
		if(pl[i]=='.'){
			qt_impar++;
			max_impar=i;
		}else{
			if(qt_impar%2!=0){
				pl[max_impar]='f';
				qt_impar = 0;
				max_impar = -1;	
			}
		}
		if(i==0){
			if(qt_impar%2!=0){
				pl[max_impar]='f';
				qt_impar = 0;
				max_impar = -1;	
			}	
		}
	}
	return pl;
}
