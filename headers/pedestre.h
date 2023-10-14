#ifndef PEDESTRE__H
#define PEDESTRE__H

int adicionar_pedestre_conjunto(int loc_lin, int loc_col);
int inserir_pedestres_aleatoriamente(int qtd);
void desalocar_pedestres();
void determinar_movimento();
int panico();
int varredura_movimento_em_x();
int resolver_conflitos_movimento();
void confirmar_movimentacao();
void atualizar_grid_pedestres();
int sala_vazia();
void resetar_estado_pedestres();
void reiniciar_pedestre();
void imprimir_grid_pedestres(FILE *arquivo_saida);

#endif