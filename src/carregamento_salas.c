#include<stdio.h>
#include<stdlib.h>
#include"global_declarations.h"
#include"carregamento_salas.h"
#include"porta.h"
#include"pedestre.h"

const char *path = "../salas/";

/**
 * ## carregar_sala
 * 
 * #### Entrada
 * string, indicando o nome do arquivo que contém a sala (sem a extensão)
 * inteiro, indicando o modo de carregamento
 * #### Processo 
 * Carrega a sala passada, considerando o modo requisitado:
 *               {modo} == 1: sala estrutural
 *               {modo} == 2: sala estrutural com portas e pedestres
 * #### Saída
 * 1 em sucesso, 0 em falha 
*/
int carregar_sala(char *nome, int modo)
{
    char complete_path[100] = "";
    sprintf(complete_path,"%s%s.txt",path,nome);

    FILE *arquivo = fopen(complete_path, "r");
    if(arquivo == NULL)
    {
        fprintf(stderr,"Não foi possível abrir o arquivo %s.\n",nome);
        return 0;
    }

    if( fscanf(arquivo,"%d %d", &qtd_linhas_sala, &qtd_colunas_sala) != 2)
    {
        fprintf(stderr, "Dimensões do mapa não encontradas no arquivo.\n");
        return 0;
    }

    base.sala = alocar_matriz_int(qtd_linhas_sala, qtd_colunas_sala);
    if(base.sala == NULL)
    {
        fprintf(stderr,"Falha na alocação da sala base.\n");
        return 0;
    }


    char lido = '\0';
    fscanf(arquivo,"%c",&lido);// elimina o '\n' depois das dimensões
    for(int i = 0; i < qtd_linhas_sala; i++)
    {
        int h = 0;
        for(; h <= qtd_colunas_sala; h++)
        {
            int retorno = fscanf(arquivo,"%c",&lido);
            if(retorno == EOF)
                break;

            if(h == qtd_colunas_sala && lido != '\n') // nessa posição espera-se uma quebra de linha.
            {
                fprintf(stderr,"Linha %d possui mais colunas que o requerido.\n", i);
                return 0;
            }

            switch(lido)
            {
                case '#':
                    base.sala[i][h] = VALOR_PAREDE;
                    break;
                case '_':
                    base.sala[i][h] = VALOR_PORTA;

                    if( !adicionar_porta_pilha(i,h))
                        return 0;
                    break;
                case '.':
                    base.sala[i][h] = 0;
                    break;
                case 'P':
                    base.sala[i][h] = 10;

                    if( !adicionar_pedestre_lista(i,h))
                        return 0;
                    break;
                case '\n':
                    goto fora;
                    break;
                default:
                    fprintf(stderr,"Símbolo desconhecido no desenho da sala.");
                    return 0;
                    break;
            }
        }

        fora:
        if( h < qtd_colunas_sala)
        {
            fprintf(stderr,"Linha %d possui menos colunas que o requerido.\n", i);
            return 0;
        }
    }

    return 1;
}

/**
 * ## alocar_matriz_int
 * 
 * #### Entrada
 * inteiro, número de linhas
 * inteiro, número de colunas
 * #### Processo
 * Aloca de forma dinâmica uma matriz de inteiros de dimensão {num_lin} x {num_col}
 * #### Saída
 * Ponteiro para ponteiro, em sucesso. NULL, em fracasso.
*/
int **alocar_matriz_int(int num_lin, int num_col)
{
    if(num_lin <= 0 || num_col <= 0)
    {
        fprintf(stderr,"Dimensões do matriz nulas ou negativas.\n");
        return NULL;
    }

    int **novo = malloc(sizeof(int *) * num_lin);
    if( novo == NULL )
        return NULL;
    
    for(int i = 0; i < num_lin; i++)
        novo[i] = malloc(sizeof(int) * num_col);

    return novo;
}