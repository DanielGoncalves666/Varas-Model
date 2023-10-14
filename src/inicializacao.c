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

/**
 * ## carregar_ambiente
 * 
 * #### Entrada
 * Nome do arquivo que contém o ambiente (com a extensão).
 * Inteiro, indicando o modo de carregamento.
 * #### Descrição 
 * Carrega o ambiente armazenado no arquivo passado, considerando o modo requisitado:
 *               {modo} == 1: apenas paredes
 *               {modo} == 2: paredes e portas
 *               {modo} == 3: paredes, portas e pedestres
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

    if( !alocar_grids())
        return 0;

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
                    if(modo != 1)
                    {
                        if( !adicionar_saida_conjunto(i,h))
                            return 0;
                     
                        grid_esqueleto.mat[i][h] = VALOR_PAREDE;
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
                    if(modo == 3)
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

    fclose(arquivo);

    return 1;
}

/**
 * ## alocar_grids
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Aloca as matrizes de inteiros das grids.
 * #### Saída
 * 1, em sucesso, 0, em fracasso
*/
int alocar_grids()
{
    grid_esqueleto.mat = alocar_matriz_int(num_lin_grid, num_col_grid);
    grid_pedestres.mat = alocar_matriz_int(num_lin_grid, num_col_grid);
    grid_mapa_calor.mat = alocar_matriz_int(num_lin_grid, num_col_grid);
    if(grid_esqueleto.mat == NULL || grid_pedestres.mat == NULL || grid_mapa_calor.mat == NULL)
    {
        fprintf(stderr,"Falha na alocação das grids com dimensões %d x %d.\n", num_lin_grid, num_col_grid);
        return 0;
    }
}

/**
 * ## gerar_ambiente
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Gera um ambiente retangular de tamanho num_lin_grid x num_col_grid com paredes nas bordas.
 * #### Saída
 * 1, em sucesso, 0, em falha
*/
int gerar_ambiente()
{
    for(int i = 0; i < num_lin_grid; i++)
    {
        for(int h = 0; h < num_col_grid; h++)
        {
            if(i > 0 && i < num_lin_grid - 1 && h > 0 && h < num_col_grid - 1)
                grid_esqueleto.mat[i][h] = 0;
            else
                grid_esqueleto.mat[i][h] = VALOR_PAREDE;
        }
    }

    return 1;
}

/**
 * ## extrair_saidas
 * 
 * #### Entrada
 * Arquivo auxiliar, onde as localizações das portas estão armazenadas. 
 *      Cada linha do arquivo contém as localiações para um conjunto de simulações.
 * 
 * #### Descrição
 * Lê uma única linha do ARQUIVO_AUXILIAR, extrai as saídas e as adiciona no ambiente.
 * 
 * #### Saída
 * Quantidade de saídas extraídas e adicionadas, ou 0, em fracasso.
*/
int extrair_saidas(FILE *arquivo_auxiliar)
{
    int lin = 0;
    int col = 0;

    int qtd = 0;
    char caracter = '\0';

    while(1)
    {
        int retorno = fscanf(arquivo_auxiliar,"%d %d %c ",&lin,&col,&caracter);

        if( retorno == EOF)
            return 0; 

        if( retorno != 3)
        {
            fprintf(stderr, "Falha ao ler o arquivo auxiliar. Verifique se o padrão está sendo seguido.\n");
            return 0;
        }

        qtd++;

        if(caracter == ',')
        {
            if( !adicionar_saida_conjunto(lin,col))
                return 0;
        }
        else if(caracter == '.')
        {
            if( !adicionar_saida_conjunto(lin,col))
                return 0;
            break;
        }
        else
        {
            fprintf(stderr, "Falha ao ler o arquivo auxiliar. Símbolo desconhecido.\n");
            return 0;
        }
    }

    return qtd;
}
