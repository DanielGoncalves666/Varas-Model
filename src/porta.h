#ifndef PORTA__H
#define PORTA__H

#include"global_declarations.h"

Porta criar_porta(int loc_linha, int loc_coluna);
int adicionar_porta_pilha(int loc_linha, int loc_coluna);
double **alocar_matriz_double(int num_lin, int num_col);

#endif