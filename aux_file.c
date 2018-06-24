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
void transform_e(char campo[], char alvo[], int i_bola, int tam);
void summing_e(char campo[], int bola, int tam, int sum[]);
int pl(int bsum, int psum);
void transform_d(char campo[],  char alvo[], int i_bola, int tam);
void summing_d(char campo[], int bola, int tam, int sum[]);
char * array_to_str(char * str, int *array, unsigned int n);


int main(int argc, char **argv) {
	char buf[MAXSTR];
	char lado_meu; 
	char lado_adv; 
	char * campo; 
	int tam_campo; 
	campo = "f....fo....ff";
  	tam_campo = 13;
  	lado_meu = 'e';
  	movimento mov = def_movimento(campo, tam_campo, lado_meu);
  	printf("Fazendo movimento: %s\n", mov.mov);

	printf("<<<<<<<<<<<FIM>>>>>>>>>>>\n");
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
	printf("Estimando...\n");
	int i_bola = get_bola(campo, tam);

	char campo_e[tam];
	transform_e(campo, campo_e, i_bola, tam);
	printf("\nFim transform_e ex");
	int i_bola_e = i_bola+1;
	printf("\nBola nova");
	int sum_e[2];
	summing_e(campo_e, i_bola_e, tam, sum_e);
	int p_e = pl(sum_e[0], sum_e[1]);

	char campo_d[tam];
	transform_d(campo, campo_d, i_bola, tam);
	int i_bola_d = i_bola-1;
	int sum_d[2];
	summing_d(campo_d, i_bola_d, tam, sum_d);
	int p_d = pl(sum_d[0], sum_d[1]);

	if(p_e < p_d)
		return 1;
	if(p_e > p_d)
		return -1;
	printf("\nFIM Estimando...\n");
	return 0;
}


void transform_e(char campo[], char campo_target[], int i_bola, int tam){
	printf("t1\n");
	strcpy(campo_target, campo);
	printf("t2\n");
	int i_sub = i_bola+1;
	campo_target[i_bola] = campo_target[i_sub];
	printf("t3\n");
	campo_target[i_sub] = 'o';
	int qt_impar = 0;
	int max_impar = -1;
	printf("=i_sub=%d\n", i_sub);
	printf("=tam=%d\n", tam);

	for(int i = i_sub;i<tam;i++){
		printf("%d.", i);
		if(campo_target[i]=='.'){
			qt_impar++;
			max_impar=i;
		}else{
			if(qt_impar%2!=0){
				campo_target[max_impar]='f';
				qt_impar = 0;
				max_impar = -1;	
			}
		}
		if(i==tam-1){
			if(qt_impar%2!=0){
				campo_target[max_impar]='f';
				qt_impar = 0;
				max_impar = -1;	
			}	
		}
		printf(".");
	}
	printf("\nFIM>>transform_e");
}

void summing_e(char campo[], int bola, int tam, int sum[]){
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
}




int pl(int bsum, int psum){
	int p_e = 0;
	int kno = 0;
	if(bsum == psum){
		kno=1;
	}
	p_e = floor(bsum/4)+(1-kno);

	return p_e;
}




//Estimativa para d
void summing_d(char campo[], int bola, int tam, int sum[]){
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

void transform_d(char campo[], char pl[], int i_bola, int tam){
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