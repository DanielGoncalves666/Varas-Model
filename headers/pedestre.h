#ifndef PEDESTRE__H
#define PEDESTRE__H

int adicionar_pedestre_conjunto(int loc_lin, int loc_col);
void determinar_movimento();
int panico();
int varredura_movimento_em_x();
int resolver_conflitos_movimento();
void confirmar_movimentacao();
void atualizar_grid_pedestres();
int sala_vazia();
void imprimir_grid_pedestres();
void resetar_pedestres();

#endif