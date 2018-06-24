#ifndef __CAMPO_H__
#define __CAMPO_H__
#include <stdbool.h>

void campo_conecta(int, char**);
void campo_envia(char*);
void campo_recebe(char*);
typedef struct {
  char player;
  char tipo;
  int posicao;
  int qt_saltos;
  int * saltos;
  bool nula;
  double h;
  bool terminal;
  char* campo;
  char mov[128];
} movimento;

movimento def_movimento(char campo[], int tam, char lado_meu);
int gerar_movimentos(char* campo, int tam, int jogador, int bola, movimento movimentos[], char lado_meu);
bool pode_vencer(char campo[], int jogador, int tam);
double minimax(char * campo, int jogador, double alpha, double beta, int depth, int tam, char lado_meu, char jogador_vez);
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
char * array_to_str(char * str, int *array, unsigned int n);


#define typename(x) _Generic((x),                                                 \
        _Bool: "_Bool",                  unsigned char: "unsigned char",          \
         char: "char",                     signed char: "signed char",            \
    short int: "short int",         unsigned short int: "unsigned short int",     \
          int: "int",                     unsigned int: "unsigned int",           \
     long int: "long int",           unsigned long int: "unsigned long int",      \
long long int: "long long int", unsigned long long int: "unsigned long long int", \
        float: "float",                         double: "double",                 \
  long double: "long double",                   char *: "pointer to char",        \
       void *: "pointer to void",                int *: "pointer to int",         \
      default: "other")


#endif
