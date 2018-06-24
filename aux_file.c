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
int gol(char campo[], int tam);

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
			if(gol(campo, tam)) {
		      return jogador*2;
		    }
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
			if(gol(campo, tam)) {
		      return jogador*2;
		    }
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
	int pe, pd;
    char campo_e[tam];
    char campo_d[tam];
    strcpy(campo_e, campo);
    strcpy(campo_d, campo);
    if(bola<tam-1){
        int bsum, psum;
        campo_e[bola]=campo_e[bola+1];
        campo_e[bola+1]='o';
        int i;
        int qt_bola = 0;
        int first_p = -1;
        for(i = bola+1;i<tam;i++){
            if(campo_e[i]=='.'){
                qt_bola++;
                if(first_p==-1)
                    first_p=i;
            }
            if(campo_e[i]=='f'){
                if(qt_bola%2!=0 && first_p!=-1)
                    campo_e[first_p]='f';
                qt_bola=0;
                first_p=-1;
            }
        }
        for(int i = tam-1;i>=bola;i--){
            if(i==bola){
                if(campo_e[i]=='.'){
                    bsum++;
                    psum++;
                }
                if(bsum>0 && campo_e[i]=='f'){
                    bsum--;
                }
            }else{
                if(campo_e[i]=='.'){
                    bsum++;
                    psum++;
                }
                if(bsum>0 && campo_e[i]=='f'){
                    bsum--;
                }
                if(psum>0 && campo_e[i]=='f'){
                    psum--;
                }   
            }   
        }
        int kno = 0;
        if(bsum == psum){
            kno = 1;
        }
        pe = floor(bsum/4)+(1-kno);
    }
    if(bola>0){
        int bsum, psum;
        campo_d[bola]=campo_d[bola-1];
        campo_d[bola-1]='o';
        int i;
        int qt_bola = 0;
        int first_p = -1;
        for(i = bola;i>=0;i--){
            if(campo_d[i]=='.'){
                qt_bola++;
                if(first_p==-1){
                    first_p=i;
                }
            }
            if(campo_d[i]=='f'){
                if(qt_bola%2!=0 && first_p!=-1)
                    campo_d[first_p]='f';
                qt_bola=0;
                first_p=-1;
            }
        }
        for(int i = 0;i<=bola-1;i++){
            if(i==bola){
                if(campo_d[i]=='.'){
                    bsum++;
                    psum++;
                }
                if(bsum>0 && campo_d[i]=='f'){
                    bsum--;
                }
            }else{
                if(campo_d[i]=='.'){
                    bsum++;
                    psum++;
                }
                if(bsum>0 && campo_d[i]=='f'){
                    bsum--;
                }
                if(psum>0 && campo_d[i]=='f'){
                    psum--;
                }   
            }   
        }
        int kno = 0;
        if(bsum == psum){
            kno = 1;
        }
        pd = floor(bsum/4)+(1-kno);
    }
    if(pe < pd)
         return jogador;
     if(pe > pd)
         return -jogador;
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
  if (n == 1) r = sprintf(str, "%d", array[0]);
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
	
			char buffer[l_d];
			array_to_str(buffer, saltos_d, l_d);
			sprintf(m_d.mov, "%c o %d %s\n", lado_meu, l_d, buffer);	
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

			sprintf(m_x.mov, "%c o 1 %d\n", lado_meu, saltos_e[0]);	
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
			char buffer[l_e];
			array_to_str(buffer, saltos_e, l_e);
			sprintf(m_d.mov, "%c o %d %s\n", lado_meu, l_e, buffer);
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

			sprintf(m_x.mov, "%c o 1 %d\n", lado_meu, saltos_d[0]);	
			movimentos[qt_movimentos_possiveis]=m_x;
			qt_movimentos_possiveis++;	
		}
	}
	return qt_movimentos_possiveis;
}

int gol(char campo[], int tam) {
  int i;

  for(i = 0; i < tam; i++)
    if(campo[i] == 'o')
      return 0;
  return 1;
}