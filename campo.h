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
  char* mov;
} movimento;

#endif
