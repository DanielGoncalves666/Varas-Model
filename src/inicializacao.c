/*
    Módulo inicializacao

    Cria e carrega a estrutura do ambiente especificado, suas saídas e pedestres (se for o caso).
    Alternativamente, pode criar um ambiente retangular do zero, e inserir saídas e pedestres de forma manual.

    Daniel Gonçalves, 2023.
*/

#include<stdio.h>
#include<stdlib.h>

#include"../headers/global_declarations.h"
#include"../headers/inicializacao.h"
#include"../headers/saida.h"
#include"../headers/pedestre.h"

const char *path = "ambientes/";

int **alocar_matriz_int(int num_lin, int num_col);

/**
 * ## carregar_ambiente
 * 
 * #### Entrada
 * Nome do arquivo que contém o ambiente (com a extensão).
 * Inteiro, indicando o modo de carregamento.
 * #### Descrição 
 * Carrega o ambiente armazenado no arquivo passado, considerando o modo requisitado:
 *               {modo} == 0: sala estrutural
 *               {modo} == outro valor: sala estrutural com portas e pedestres
 * #### Saída
 * 1 em sucesso, 0 em falha 
*/
int carregar_ambiente(char *nome, int modo)
{
    char complete_path[100] = "";
    sprintf(complete_path,"%s%s",path,nome);

    FILE *arquivo = fopen(complete_path, "r");
    if(arquivo == NULL)
    {
        fprintf(stderr,"Não foi possível abrir o arquivo %s.\n",nome);
        return 0;
    }

    if( fscanf(arquivo,"%d %d", &num_lin_grid, &num_col_grid) != 2)
    {
        fprintf(stderr, "Dimensões do mapa não encontradas no arquivo.\n");
        return 0;
    }

    grid_esqueleto.mat = alocar_matriz_int(num_lin_grid, num_col_grid);
    grid_pedestres.mat = alocar_matriz_int(num_lin_grid, num_col_grid);
    if(grid_esqueleto.mat == NULL || grid_pedestres.mat == NULL)
    {
        fprintf(stderr,"Falha na alocação das grids com dimensões %d x %d.\n", num_lin_grid, num_col_grid);
        return 0;
    }

    char lido = '\0';
    fscanf(arquivo,"%c",&lido);// elimina o '\n' depois das dimensões
    for(int i = 0; i < num_lin_grid; i++)
    {
        int h = 0;
        for(; h <= num_col_grid; h++)
        {
            int retorno = fscanf(arquivo,"%c",&lido);
            if(retorno == EOF)
                break;

            if(h == num_col_grid && lido != '\n') // nessa posição espera-se uma quebra de linha.
            {
                fprintf(stderr,"Linha %d possui mais colunas que a dimensão passada.\n", i);
                return 0;
            }

            switch(lido)
            {
                case '#':
                    grid_esqueleto.mat[i][h] = VALOR_PAREDE;
                    break;
                case '_':
                    if(modo)
                    {
                        if( !adicionar_saida_conjunto(i,h))
                            return 0;
                     
                        grid_esqueleto.mat[i][h] = VALOR_SAIDA;
                    }
                    else
                        grid_esqueleto.mat[i][h] = VALOR_PAREDE;
                        // na existência de saídas no meio do ambiente uma parede seria colocada no lugar.

                    break;
                case '.':
                    grid_esqueleto.mat[i][h] = 0;
                    break;
                case 'p':
                case 'P':
                    if(modo)
                    {
                        if( !adicionar_pedestre_conjunto(i,h))
                            return 0;
                        grid_pedestres.mat[i][h] = pedestres.vet[pedestres.n_ped - 1]->id;
                    }
                    else
                        grid_esqueleto.mat[i][h] = 0;

                    break;
                case '\n':
                    goto fora;
                default:
                    fprintf(stderr,"Símbolo desconhecido no desenho do ambiente.");
                    return 0;
            }
        }

        fora:
        if( h < num_col_grid)
        {
            fprintf(stderr,"Linha %d possui menos colunas que a dimensão passada.\n", i);
            return 0;
        }
    }

    return 1;
}

/**
 * ## gerar_ambiente
 * 
 * #### Entrada
 * Quantidade de linhas e colunas, respectivamente.
 * #### Descrição
 * Gera um ambiente retangular de tamanho LINHAS x COLUNAS com paredes nas bordas.
 * #### Saída
 * 1, em sucesso, 0, em falha
*/
int gerar_ambiente(int linhas, int colunas)
{
    grid_esqueleto.mat = alocar_matriz_int(linhas,colunas);
    if(grid_esqueleto.mat == NULL)
        return 0;

    num_lin_grid = linhas;
    num_col_grid = colunas;

    for(int i = 0; i < num_lin_grid; i++)
    {
        for(int h = 0; h < num_col_grid; h++)
        {
            if(i != 0 || i != num_lin_grid - 1 || h != 0 || h != num_col_grid)
                grid_esqueleto.mat[i][h] = 0;
            else
                grid_esqueleto.mat[i][h] = VALOR_PAREDE;
        }
    }

    return 1;
}

/**
 * ## inserir_saidas
 * 
 * #### Entrada
 * Quantidade de saídas.
 * Dois vetores, indicando a linha e coluna das saídas, respectivamente.
 * #### Descrição
 * Insere saídas nas coordenadas específicas pelos vetores.
 * #### Saída
 * Quantidade de portas inseridas.
*/
int inserir_saidas(int qtd, int *vet_lin, int *vet_col)
{
    if(qtd <= 0 || vet_lin == NULL || vet_col == NULL)
        return 0;

    for(int q = 0; q < qtd; q++)
    {
        int i = vet_lin[q];
        int h = vet_col[q];
        if(grid_esqueleto.mat[i][h] == VALOR_SAIDA)
        {
            qtd--;
            continue;  
        }

        if( !adicionar_saida_conjunto(i,h) ) 
            return 0;

        grid_esqueleto.mat[i][h] = VALOR_SAIDA;
    }

    return qtd;
}

/**
 * ## inserir_pedestres
 * 
 * #### Entrada
 * Quantidade de pedestres.
 * Dois vetores, indicando a linha e coluna iniciais dos pedestres, respectivamente.
 * #### Descrição
 * Insere pedestres nas coordenadas específicas pelos vetores.
 * #### Saída
 * Quantidade de pedestres inseridos.
*/
int inserir_pedestres(int qtd, int *vet_lin, int *vet_col)
{
    if(qtd <= 0 || vet_lin == NULL || vet_col == NULL)
        return 0;

    for(int q = 0; q < qtd; q++)
    {
        int i = vet_lin[q];
        int h = vet_col[q];

        if(grid_pedestres.mat[i][h] != 0)
        {
            qtd--;
            continue;
        }

        if( !adicionar_pedestre_conjunto(i,h))
            return 0;

        grid_pedestres.mat[i][h] = pedestres.vet[pedestres.n_ped - 1]->id;
    }

    return qtd;
}
