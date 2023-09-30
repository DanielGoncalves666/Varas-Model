#ifndef PEDESTRE__H
#define PEDESTRE__H

#include"global_declarations.h"

Pedestre criar_pedestre(int loc_linha, int loc_coluna);
int alterar_movimentacao(Pedestre p, int mov_linha, int mov_coluna);
int confirmar_movimentacao(Pedestre p);
int adicionar_pedestre_lista(int loc_lin, int loc_col);

#endif