#include<stdio.h>
#include<stdlib.h>

#include"global_declarations.h"
#include"pedestre.h"

/**
 * ## criar_pedestre
 * 
 * #### Entrada
 * Dois inteiros, indicando a linha e coluna da localização do pedestre, respectivamente
 * 
 * #### Processo
 * Aloca uma estrutura para o novo pedestre e a preenche.
 * 
 * #### Saída
 * Ponteiro para o novo pedestre ou NULL, em fracasso.
*/
Pedestre criar_pedestre(int loc_linha, int loc_coluna)
{
    Pedestre novo = malloc(sizeof(struct pedestre));
    if(novo != NULL)
    {
        novo->loc_linha = loc_linha;
        novo->loc_coluna = loc_coluna;
        novo->mov_linha = -1;
        novo->mov_coluna = -1;
    }

    return novo;
}

/**
 * ## alterar_movimentacao
 * 
 * #### Entrada:
 * Ponteiro para pedestre, indicando a estrutura do pedestre que pretende alterar sua intenção de movimentação.
 * Dois inteiros, indicando a linha e coluna para a qual o pedestre pretende se movimentar, respectivamente.
 * 
 * #### Processo:
 * Muda a célula para a qual o pedestre passado pretende se movimentar
 * 
 * #### Saída:
 * 1, em sucesso, 0, em fracasso 
*/
int alterar_movimentacao(Pedestre p, int mov_linha, int mov_coluna)
{
    if(p == NULL)
        return 0;

    p->mov_linha = mov_linha;
    p->mov_coluna = mov_coluna;

    return 1;
}

/**
 * ## confirmar_movimentacao
 * 
 * #### Entrada
 * Ponteiro para pedestre, indicando a estrutura do pedestre cuja localização atual será alterada.
 * 
 * #### Processo
 * Muda a célula na qual o pedestre está localizado para a célula que ele pretendia se movimentar.
 * 
 * #### Saída
 *  1, em sucesso, 0, em falha.
*/
int confirmar_movimentacao(Pedestre p)
{
    if(p == NULL)
        return 0;


    p->loc_linha = p->mov_linha;
    p->loc_coluna = p->mov_coluna;

    return 1;
}

/**
 * ## adicionar_pedestre_lista
 * 
 * #### Entrada
 * Dois inteiros, indicando a posição da célula em que o pedestre se localiza
 * 
 * #### Processo
 * Cria um novo pedestre baseando-se na localização passada e o adiciona na lista de pedestres.
 * 
 * #### Saída
 * 1, em sucesso, 0, em fracasso
*/
int adicionar_pedestre_lista(int loc_lin, int loc_col)
{
    Pedestre novo = criar_pedestre(loc_lin, loc_col);
    if(novo == NULL)
        return 0;

    lista_ped.tam_lista += 1;
    lista_ped.lista = realloc(lista_ped.lista, sizeof(struct pedestre) * lista_ped.tam_lista);
    lista_ped.lista[lista_ped.tam_lista - 1] = novo;

    return 1;
}