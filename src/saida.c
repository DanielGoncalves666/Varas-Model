/*
    Módulo saida.

    Contém funções dedicadas à criação e administração de saídas, além do cálculo do floor field.
    
    Daniel Gonçalves, 2023.
*/


#include<stdio.h>
#include<stdlib.h>
#include"../headers/global_declarations.h"
#include"../headers/saida.h"

const double regra[3][3] = {{VALOR_DIAGONAL, 1.0, VALOR_DIAGONAL},
                            {     1.0,       0.0,       1.0     },
                            {VALOR_DIAGONAL, 1.0, VALOR_DIAGONAL}};
/* Considerando que a célula central tenha valor X, 'regra' indica o valor a ser somado para
   determinar o valor dos vizinhos. */

Saida criar_saida(int loc_linha, int loc_coluna);
int determinar_piso_saida(Saida s);

/**
 * ## criar_saida
 * 
 * #### Entrada
 * Localização da saída (sua linha e coluna).
 * 
 * #### Descrição
 * Se a localização for válida cria uma estrutura Saida correspondente à localização passada e a inicializa.
 * 
 * #### Saída
 * Estrutura criada ou NULL.
 * 
*/
Saida criar_saida(int loc_linha, int loc_coluna)
{
    if(loc_linha < 0 || loc_linha >= num_lin_grid || loc_coluna < 0 || loc_coluna >= num_col_grid)
        return NULL;
    
    Saida nova = malloc(sizeof(struct saida));
    if(nova != NULL)
    {
        nova->loc_lin = loc_linha;
        nova->loc_col = loc_coluna;

        nova->field = alocar_matriz_double(num_lin_grid, num_col_grid);
    }

    return nova;
}

/**
 * ## adicionar_saida_conjunto
 * 
 * #### Entrada
 * Localização da saída (linha e coluna, respectivamente).
 * 
 * #### Descrição
 * Cria uma nova saida e calcula o piso correspondente. 
 * Adiciona a saida no conjunto.
 * 
 * #### Saída
 * 0, em falha
 * 1, em sucesso
*/
int adicionar_saida_conjunto(int loc_linha, int loc_coluna)
{
    Saida nova = criar_saida(loc_linha, loc_coluna);
    if(nova == NULL)
    {
        fprintf(stderr,"Falha na criação da Saida em (%d,%d).\n",loc_linha, loc_coluna);
        return 0;
    }

    saidas.n_saidas += 1;
    saidas.vet_saidas = realloc(saidas.vet_saidas, sizeof(Saida) * saidas.n_saidas);
    if(saidas.vet_saidas == NULL)
    {
        fprintf(stderr, "Falha na realocação do vetor de saídas.\n");
        return 0;
    }    

    saidas.vet_saidas[saidas.n_saidas - 1] = nova;

    return 1;
}

/**
 * ## desalocar_saidas
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Desaloca as estruturas para as saídas individuais, a matriz da saída combinada e zera a quantidade de saídas
 * #### Saída
 * Nenhuma
*/
void desalocar_saidas()
{
    free(saidas.vet_saidas);
    for(int i = 0; i < num_lin_grid; i++)
        free(saidas.combined_field[i]);
    free(saidas.combined_field);
    saidas.combined_field = NULL;

    saidas.n_saidas = 0;
}

/**
 * ## determinar_piso_saida
 * 
 * #### Entrada
 * Saida cujo piso será calculado
 * 
 * #### Descrição
 * Calcula o campo de piso referente à saida dada como argumento.
 * 
 * #### Saída
 * 0, em falha
 * 1, em sucesso
*/
int determinar_piso_saida(Saida s)
{
    if(s == NULL)
    {
        fprintf(stderr, "NULL como entrada em 'determinar_piso_saida'.\n");
        return 0;
    }

    double **mat = s->field;

    // adiciona as paredes no piso da saida (outras saidas também são consideradas como paredes)
    for(int i = 0; i < num_lin_grid; i++)
    {
        for(int h = 0; h < num_col_grid; h++)
        {
            double conteudo = grid_esqueleto.mat[i][h];
            if(conteudo == VALOR_PAREDE)
                mat[i][h] = VALOR_PAREDE;
            else
                mat[i][h] = 0.0;
        }
    }
    mat[s->loc_lin][s->loc_col] = VALOR_SAIDA; // Adiciona a saida

    double **aux = alocar_matriz_double(num_lin_grid,num_col_grid);
    // matriz para armazenar as alterações para o tempo t + 1 do autômato
    if(aux == NULL)
    {
        fprintf(stderr,"Falha na alocação da matriz auxiliar para cálculo do piso.\n");
        return 0;
    }

    copiar_matriz_double(aux, mat); // copia o piso base para a matriz auxiliar

    int qtd_mudancas;
    do
    {
        qtd_mudancas = 0;
        for(int i = 0; i < num_lin_grid; i++)
        {
            for(int h = 0; h < num_col_grid; h++)
            {
                double atual = mat[i][h]; // valor atual de piso na posição dada

                if(atual == VALOR_PAREDE || atual == 0.0) 
                    continue; // cálculo de piso dos vizinhos ocorre apenas para células com valor de piso já dado

                for(int j = -1; j < 2; j++)
                {
                    if(i + j < 0 || i + j >= num_lin_grid)
                        continue;

                    for(int k = -1; k < 2; k++)
                    {
                        if(h + k < 0 || h + k >= num_col_grid)
                            continue;

                        if(mat[i + j][h + k] == VALOR_PAREDE || mat[i + j][h + k] == VALOR_SAIDA)
                            continue;

                        if(j != 0 && k != 0)
                        {
                            if(! eh_diagonal_valida(i,h,j,k,mat))
                                continue;
                        }

                        double novo_valor = mat[i][h] + regra[1 + j][1 + k];
                        if(aux[i + j][h + k] == 0.0)
                        {    
                            aux[i + j][h + k] = novo_valor;
                            qtd_mudancas++;
                        }
                        else if(novo_valor < aux[i + j][h + k])
                        {
                            aux[i + j][h + k] = novo_valor;
                            qtd_mudancas++;
                        }
                    }
                }
            }
        }
        copiar_matriz_double(mat,aux);
    }
    while(qtd_mudancas != 0);

    return 1;
}


/**
 * ## determinar_piso_geral
 * 
 * #### Entrada
 * Nada
 * 
 * #### Descrição
 * Determina o piso geral do ambiente por meio da fusão dos pisos para cada saída.
 * 
 * #### Saída
 * 1, em sucesso, 0, em falha
 * 
*/
int determinar_piso_geral()
{
    if(saidas.n_saidas <= 0 || saidas.vet_saidas == NULL)
    {
        fprintf(stderr,"O número de saídas (%d) é inválido ou o vetor de saidas é NULL.\n", saidas.n_saidas);
        return 0;
    }

    for(int q = 0; q < saidas.n_saidas; q++)
    {
        if( !determinar_piso_saida(saidas.vet_saidas[q]))
            return 0;
    }

    saidas.combined_field = alocar_matriz_double(num_lin_grid, num_col_grid);
    if(saidas.combined_field == NULL)
    {
        fprintf(stderr,"Falha na alocação da matriz de piso combinado.\n");
        return 0;
    }

    copiar_matriz_double(saidas.combined_field, saidas.vet_saidas[0]->field); // copia o piso da primeira porta
    
    for(int q = 1; q < saidas.n_saidas; q++)
    {
        for(int i = 0; i < num_lin_grid; i++)
        {
            for(int h = 0; h < num_col_grid; h++)
            {
                if(saidas.combined_field[i][h] > saidas.vet_saidas[q]->field[i][h])
                    saidas.combined_field[i][h] = saidas.vet_saidas[q]->field[i][h];
            }
        }
    }

    return 1;
}

/**
 * ## imprimir_piso
 * 
 * #### Entrada
 * Matriz de Double.
 * 
 * #### Descrição
 * Imprime o conteúdo da matriz
 * 
 * #### Saída
 * 1, em sucesso, 0, em falha
*/
int imprimir_piso(double **mat)
{
    if(mat == NULL)
        return 0;

    for(int i=0; i < num_lin_grid; i++){
		for(int h=0; h < num_col_grid; h++){
            if(mat[i][h] >= 1000.0)
                printf("%.0lf\t", mat[i][h]);
            else
			    printf("%5.1lf\t", mat[i][h]);
		}
		printf("\n\n");
	}
    return 1;
}