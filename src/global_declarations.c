#include<stdlib.h>
#include"pedestre.h"
#include"global_declarations.h"

const double VALOR_PORTA = 1;
const double VALOR_PAREDE = 1000;

int qtd_linhas_sala = 0;
int qtd_colunas_sala = 0;
int qtd_portas = 0;
int total_pedestres = 0;

Sala base; // preenchida à partir da sala carregada do arquivo
Pilha_Portas portas = {NULL, NULL, 0};
Lista_pedestres lista_ped = {0,NULL};