/*
    Módulo global_declarations

    Contém declarações e definições de variáveis globais e estruturas que serão usadas por todo o programa.s

    Daniel Gonçalves, 2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include"../headers/global_declarations.h"

int num_lin_grid = 0;
int num_col_grid = 0;

Grid grid_esqueleto = {NULL}; // grid contendo paredes e saidas
Grid grid_pedestres = {NULL}; // grid contendo apenas a localização dos pedestres
Conjunto_saidas saidas = {NULL, NULL, 0};
Conjunto_pedestres pedestres = {NULL,0};

/**
 * ## alocar_matriz_int
 * 
 * #### Entrada
 * Número de linhas e de colunas, respectivamente.
 * #### Descrição
 * Aloca de forma dinâmica uma matriz de inteiros de dimensão NUM_LIN x NUM_COL.
 * #### Saída
 * Matriz de inteiros contendo apenas zeros.
*/
int **alocar_matriz_int(int num_lin, int num_col)
{
    if(num_lin <= 0 || num_col <= 0)
        return NULL;

    int **novo = malloc(sizeof(int *) * num_lin);
    if( novo == NULL )
        return NULL;
    
    for(int i = 0; i < num_lin; i++)
    {
        novo[i] = calloc(num_col, sizeof(int));
        if(novo[i] == NULL)
            return NULL;
    }

    return novo;
}

/**
 * ## desalocar_matriz_int
 * 
 * #### Entrada
 * Matriz a ser desalocada
 * Quantidade de linhas da matriz
 * #### Descrição
 * Desaloca a matriz passadaa
 * #### Nada
*/
void desalocar_matriz_int(int **mat, int lin)
{
    for(int i = 0; i < lin; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

/**
 * ## alocar_matriz_double
 * 
 * #### Entrada
 * Número de linhas e colunas, respectivamente.
 * #### Descrição
 * Aloca de forma dinâmica uma matriz de doubles de dimensão NUM_LIN x NUM_COL
 * #### Saída
 * Matriz de double contendo apenas zeros ou NULL.
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
        novo[i] = calloc(num_col, sizeof(double));

    return novo;
}


/**
 * ## desalocar_matriz_double
 * 
 * #### Entrada
 * Matriz a ser desalocada
 * Quantidade de linhas da matriz
 * #### Descrição
 * Desaloca a matriz passadaa
 * #### Nada
*/
void desalocar_matriz_double(double **mat, int lin)
{
    for(int i = 0; i < lin; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

/**
 * ## copiar_matriz_double
 * 
 * #### Entrada
 * Duas matrizes, sendo a primeira a de destino e a segunda a matriz fonte,
 * 
 * #### Descrição
 *  Copia os dados de SRC para DEST. As matrizes devem ser de mesmo tamanho.
 * 
 * #### Saída
 * 1, em sucesso, 0, em falha
*/
int copiar_matriz_double(double **dest, double **src)
{
    if(dest == NULL || src == NULL)
        return 0;

    for(int i = 0; i < num_lin_grid; i++)
    {
        for(int h = 0; h < num_col_grid; h++)
        {
            dest[i][h] = src[i][h];
        }
    }
}

/**
 * ## eh_diagonal_valida
 * 
 * #### Entrada
 * Dois inteiros, indicando a célula base.
 * Dois inteiros, indicando a célula alvo.
 * Matriz de doubles, indicando o piso.
 * #### Descrição
 * Verifica se a célula na diagonal (alvo) da célula base é acessível.
 * Isso impede a atribuição de valores de pisos e a movimentação de pedestres através de obstáculos.
 * #### Saída
 * 0, se a diagonal for inválida
 * 1, se for válida
*/
int eh_diagonal_valida(int loc_lin, int loc_col, int j, int k, double **mat)
{
    if(j == -1 && k == -1)
    {
        if(loc_lin - 1 >= 0 && mat[loc_lin - 1][loc_col] == VALOR_PAREDE)
        {
            if(loc_col - 1 >= 0 && mat[loc_lin][loc_col - 1] == VALOR_PAREDE)
                return 0;
        }
    }
    else if(j == -1 && k == 1)
    {
        if(loc_lin - 1 >= 0 && mat[loc_lin - 1][loc_col] == VALOR_PAREDE)
        {
            if(loc_col + 1 < num_col_grid && mat[loc_lin][loc_col + 1] == VALOR_PAREDE)
                return 0;
        }
    }
    else if(j == 1 && k == -1)
    {
        if(loc_lin + 1 < num_lin_grid && mat[loc_lin + 1][loc_col] == VALOR_PAREDE)
        {
            if(loc_col - 1 >= 0 && mat[loc_lin][loc_col - 1] == VALOR_PAREDE)
                return 0;
        }
    }
    else if(j == 1 && k == 1)
    {
        if(loc_lin + 1 < num_lin_grid && mat[loc_lin + 1][loc_col] == VALOR_PAREDE)
        {
            if(loc_col + 1 < num_col_grid && mat[loc_lin][loc_col + 1] == VALOR_PAREDE)
                return 0;
        }
    }

    return 1;
}

