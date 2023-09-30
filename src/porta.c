#include<stdio.h>
#include<stdlib.h>
#include"porta.h"
#include"global_declarations.h"

/**
 * ## criar_porta
 * 
 * #### Entrada
 * inteiro, linha da porta
 * inteiro, coluna da porta
 * 
 * #### Processo
 * Se a localização for válida cria uma estrutura Porta correspondente à localização passada.
 * 
 * #### Saída
 * NULL, em falha. Porta em sucesso
 * 
*/
Porta criar_porta(int loc_linha, int loc_coluna)
{
    if(loc_linha < 0 || loc_linha >= qtd_linhas_sala || loc_coluna < 0 || loc_coluna >= qtd_colunas_sala)
    {
        fprintf(stderr,"Porta definida fora das dimensões da sala (linha %d, coluna %d)",loc_linha, loc_coluna);
        return NULL;
    }

    Porta nova = malloc(sizeof(struct porta));
    if(nova != NULL)
    {
        nova->loc_linha = loc_linha;
        nova->loc_coluna = loc_coluna;

        nova->piso = alocar_matriz_double(qtd_linhas_sala, qtd_colunas_sala);
    }

    return nova;
}

/**
 * ## adicionar_porta_pilha
 * 
 * #### Entrada
 * Dois inteiros, indicando a linha e coluna da nova porta, respectivamente.
 * 
 * #### Processo
 * Cria uma nova porta e seu correspondente piso com as coordenadas passadas.
 * Em seguida, adiciona a nova porta na pilha de portas.
 * 
 * #### Saída
 * Inteiro, 0 para falha, 1 para sucesso
*/
int adicionar_porta_pilha(int loc_linha, int loc_coluna)
{
    Porta nova = criar_porta(loc_linha, loc_coluna);
    if(nova == NULL)
        return 0;

    portas.n_portas += 1;
    portas.vet_portas = realloc(portas.vet_portas, sizeof(Porta) * portas.n_portas);
    portas.vet_portas[portas.n_portas - 1] = nova;

    return 1;
}


/**
 * ## alocar_matriz_double
 * 
 * #### Entrada
 * inteiro, número de linhas
 * inteiro, número de colunas
 * #### Processo
 * Aloca de forma dinâmica uma matriz de double de dimensão {num_lin} x {num_col}
 * #### Saída
 * Ponteiro para ponteiro, em sucesso. NULL, em fracasso.
*/
double **alocar_matriz_double(int num_lin, int num_col)
{
    if(num_lin <= 0 || num_col <= 0)
    {
        fprintf(stderr,"Dimensões do matriz nulas ou negativas.\n");
        return NULL;
    }

    double **novo = malloc(sizeof(double *) * num_lin);
    if( novo == NULL )
        return NULL;
    
    for(int i = 0; i < num_lin; i++)
        novo[i] = malloc(sizeof(double) * num_col);

    return novo;
}