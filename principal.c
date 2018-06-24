#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "campo.h"
#include <stdbool.h>
#include <float.h>
#include <math.h>

#define MAXSTR 512
#define MAXINT 16
#define MAXD 4
#define MAXACOES 100

#define new_max(x,y) ((x) >= (y)) ? (x) : (y)
#define new_min(x,y) ((x) <= (y)) ? (x) : (y)

movimento def_movimento(char campo[], int tam, char lado_meu);
int gerar_movimentos(char campo[], int tam, int jogador, int bola, movimento movimentos[], char lado_meu);
bool pode_vencer(char campo[], int jogador, int tam);
double minimax(char campo[], int jogador, double alpha, double beta, int depth, int tam, char lado_meu, char jogador_vez);
double estimativa(char campo[], int tam, int bola, int jogador, char lado_meu);
int aplica_mov(char campo[], char resultado[], int tam, char rl, char rm, int rf, int rs, int *rb);
int get_bola(char campo[], int tam);
double sigmoide(double x);
char * transform_e(char campo[], char alvo[], int i_bola, int tam);
void summing_e(char campo[], int bola, int tam, int sum[]);
int pl(int bsum, int psum);
char * transform_d(char campo[],  char alvo[], int i_bola, int tam);
void summing_d(char campo[], int bola, int tam, int sum[]);
char * array_to_str(char * str, int *array, unsigned int n);




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
	printf("Player conectato\n");
	while(1) {
		campo_recebe(buf);
	    printf("Recebendo: %s\n", buf);
	    sscanf(strtok(buf, " \n"), "%c", &lado_meu);
		sscanf(strtok(NULL, " \n"), "%d", &tam_campo);
		sscanf(strtok(NULL, " \n"), "%s", campo);
		movimento mov = def_movimento(campo, tam_campo, lado_meu);
		printf("Movimento definido: %s\n", mov.mov);
		sprintf(buf, "%s\n", mov.mov);
		campo_envia(buf);
	}
}

movimento def_movimento(char campo[], int tam, char lado_meu){
	movimento mov_to_do;
	printf("Definindo movimento para %s\n", campo);
	double aux = -INFINITY;
	movimento acoes[MAXACOES];
	int bola = get_bola(campo, tam);
	int qt_movimentos_possiveis= gerar_movimentos(campo, tam, 1, bola, acoes, lado_meu);
	for(int i = 0;i<qt_movimentos_possiveis;i++){
		double v = minimax(campo, 1, -INFINITY, INFINITY, MAXD, tam, lado_meu, lado_meu);
		if(v>=aux){
			aux = v;
			mov_to_do = acoes[i];
		}
	}
	return mov_to_do;
}

double minimax(char campo[], int jogador, double alpha, double beta, int depth, int tam, char lado_meu, char jogador_vez){
	movimento acoes[MAXACOES];
	int bola = get_bola(campo, tam);
	char prox_lado_a_jogar='e';
	if(jogador_vez =='e')
			prox_lado_a_jogar = 'd';
	if(depth==0){
		double x = estimativa(campo, tam, bola, jogador, lado_meu);
		return x;
	}

	double v = 0;
	if(jogador>0){

		v = -INFINITY;
		int qt_movimentos_possiveis= gerar_movimentos(campo, tam, jogador, bola, acoes, jogador_vez);
		for(int i = 0;i<qt_movimentos_possiveis;i++){
			char result[tam];
			movimento mov = acoes[i];
			aplica_mov(campo, result, tam, jogador_vez, mov.tipo, mov.posicao, mov.qt_saltos, mov.saltos);
			int depth_aux = depth-1;
			double mm = minimax(result, -jogador, alpha, beta, depth_aux, tam, lado_meu, prox_lado_a_jogar);
			v = new_max(v, mm);
			alpha = new_max(v, alpha);
			if(beta<=alpha){
				break;
			}
		}
		return v;
	}else{
		v = INFINITY;
		int qt_movimentos_possiveis= gerar_movimentos(campo, tam, jogador, bola, acoes, jogador_vez);
		for(int i = 0;i<qt_movimentos_possiveis;i++){
			char result[tam];
			movimento mov = acoes[i];
			aplica_mov(campo, result, tam, jogador_vez, mov.tipo, mov.posicao, mov.qt_saltos, mov.saltos);
			int depth_aux = depth-1;
			double mm = minimax(result, -jogador, alpha, beta, depth_aux, tam, lado_meu, prox_lado_a_jogar);
			v = new_min(v, mm);
			beta = new_min(v, beta);
			if(beta<=alpha){
				break;
			}
		}
		return v;
	}
}



double estimativa(char campo[], int tam, int bola, int jogador, char lado_meu){
	int sum_e_p = 0;
	int sum_e_f = 0;
	int qt_e_p_seq = 0;
	int qt_e_p_group_seq=0;
	int i_bola = -1;
	int sum_d_p = 0;
	int sum_d_f = 0;
	int qt_d_p_seq = 0;
	int qt_d_p_group_seq=0;
	bool check_bola = false;
	for(int i = 0;i<tam;i++){
		if(campo[i]=='o'){
			i_bola = i;
			check_bola=true;
			continue;
		}
		if(!check_bola){
			if(campo[i]=='.'){
				sum_e_p++;
				qt_e_p_seq++;
			}
			if(campo[i]=='f'){
				sum_e_f++;
				if(qt_e_p_seq>1){
					qt_e_p_group_seq++;
				}
				qt_e_p_seq=0;
			}
		}else{
			if(campo[i]=='.'){
				sum_d_p++;
				qt_d_p_seq++;
			}
			if(campo[i]=='f'){
				sum_d_f++;
				if(qt_d_p_seq>1){
					qt_d_p_group_seq++;
				}
				qt_d_p_seq=0;
			}
		}
		
	}
	
	return 0;
}

//Obtem index da bola
int get_bola(char campo[], int tam){
	for(int i = 0;i<tam;i++){
		if(campo[i]=='o'){
			return i;
		}
	}
	return -1;
}

char *array_to_str(char * str, int *array, unsigned int n) {
  int r;
  if (n == 0) return 0;
  if (n == 1) r = sprintf(str, "%d\0", array[0]);
  else        r = sprintf(str, "%d ", array[0]);
  array_to_str(str + r, array + 1, n - 1); 
  return str;
}

int aplica_mov(char buf[], char campo[], int tam,
	       char rl, char rm, int rf, int rs, int *rb) {

  int i, j, inc, bola;
  strcpy(campo, buf);
  if(rm == 'n')
    return 1;
  if(rm == 'f') {
    if(campo[--rf] == '.') {
      campo[rf] = 'f';
      return 1;
    }
    return 0;
  }
  if(rm == 'o') {
    if(rs < 1)
      return 0;
    for(bola = 0; bola < tam; bola++)
      if(campo[bola] == 'o')
		break;
    for(i = 0; i < rs; i++) {
      inc = (--rb[i] < bola) ? -1 : 1;
      for(j = bola + inc; j != rb[i]; j += inc)
			if(campo[j] != 'f')
	  			return 0;
       		else
	  			campo[j] = '.';
      campo[bola] = '.';
      bola = rb[i];
      if(bola < 0 && rl == 'e')
		return 0;
      if(bola >= tam && rl == 'd')
		return 0;
      if(bola >= 0 && bola < tam)
		campo[bola] = 'o';
    }
    return 1;
  }
  return 0;
}



int vetor_saltos_para_d(int saltos[], char campo[], int bola, int tam){
	if(campo[bola+1]!='.'){
		int qt_pontos = 0;
		int index = 0;
		for(int i = bola+1;i<=tam;i++){
			if(i==tam && qt_pontos==0){
				saltos[index]=i+1;
				index++;
				continue;
			}
			if(campo[i]=='f'){
				qt_pontos=0;
				continue;
			}else if(campo[i]=='.' && qt_pontos==0){
				qt_pontos++;
				saltos[index]=i+1;
				index++;
			}else if(campo[i]=='.' && qt_pontos!=0){
				break;
			}
		}
		return index;
	}
	return 0;
}


int vetor_saltos_para_e(int saltos[], char campo[], int bola, int tam){
		if(campo[bola-1]!='.'){
			int qt_pontos = 0;
			int index = 0;
			for(int i = bola-1;i>=-1;i--){
				if(i==-1 && qt_pontos==0){
					saltos[index]=0;
					index++;
					continue;
				}
				if(campo[i]=='f'){
					qt_pontos=0;
					continue;
				}else if(campo[i]=='.' && qt_pontos==0){
					qt_pontos++;
					saltos[index]=i+1;
					index++;
				}else if(campo[i]=='.' && qt_pontos!=0){
					break;
				}
			}
			return index;	
		}
	return 0;
}




int gerar_movimentos(char campo[], int tam, int jogador, int bola, movimento movimentos[], char lado_meu){
	int qt_movimentos_possiveis = 0;
	for(int i = 0;i<tam;i++){
		if(campo[i]=='.'){
			movimento mov;
			mov.tipo='f';
			mov.posicao=i+1;
			sprintf(mov.mov, "%c f %d\n", lado_meu, mov.posicao);
			movimentos[qt_movimentos_possiveis]=mov;
			qt_movimentos_possiveis++;
		}
	}
	int saltos_d[tam-bola];
	int l_d = vetor_saltos_para_d(saltos_d, campo, bola, tam);
	int saltos_e[bola];
	int l_e = vetor_saltos_para_e(saltos_e, campo, bola, tam);

	if(lado_meu=='e'){
		if(l_d>0){
			movimento m_d;
			m_d.tipo ='o';
			m_d.player = lado_meu;
			m_d.saltos = saltos_d;
			m_d.qt_saltos = l_d;
	
			char buffer[l_d*2];
			array_to_str(buffer, saltos_d, l_d);
			sprintf(m_d.mov, "%c o %s\n", lado_meu, buffer);	
			movimentos[qt_movimentos_possiveis]=m_d;
			qt_movimentos_possiveis++;		
		}
		
		if(l_e>0){
			movimento m_x;
			m_x.tipo ='o';
			m_x.player = lado_meu;
			m_x.posicao = saltos_e[0];
			m_x.saltos = saltos_e;
			m_x.qt_saltos = 1;

			sprintf(m_x.mov, "%c o %d\n", lado_meu, saltos_e[0]);	
			movimentos[qt_movimentos_possiveis]=m_x;
			qt_movimentos_possiveis++;	
		}
	}else{
		if(l_e>0){
			movimento m_d;
			m_d.tipo ='o';
			m_d.player = lado_meu;
			m_d.saltos = saltos_e;
			m_d.qt_saltos = l_e;
			char buffer[l_e*2];
			array_to_str(buffer, saltos_e, l_e);
			sprintf(m_d.mov, "%c o %s\n", lado_meu, buffer);
			movimentos[qt_movimentos_possiveis]=m_d;
			qt_movimentos_possiveis++;	
		}

		if(l_d>0){
			movimento m_x;
			m_x.tipo ='o';
			m_x.player = lado_meu;
			m_x.posicao = saltos_d[0];
			m_x.saltos = saltos_d;
			m_x.qt_saltos = 1;

			sprintf(m_x.mov, "%c o %d\n", lado_meu, saltos_d[0]);	
			movimentos[qt_movimentos_possiveis]=m_x;
			qt_movimentos_possiveis++;	
		}
	}
	return qt_movimentos_possiveis;
}
