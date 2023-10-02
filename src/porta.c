#include<stdio.h>
#include<stdlib.h>
#include"porta.h"
#include"global_declarations.h"


const double regra[3][3] = {{VALOR_DIAGONAL, 1.0, VALOR_DIAGONAL},
                            {     1.0,       0.0,       1.0     },
                            {VALOR_DIAGONAL, 1.0, VALOR_DIAGONAL}};

int determinar_piso_porta(Porta p);

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
 * ## determinar_piso_porta
 * 
 * #### Entrada
 * Tipo Porta, indicando a estrutura da porta cujo piso será calculado
 * 
 * #### Processo
 * Calcula o campo de piso referente à porta dada como argumento.
 * 
 * #### Saída
 * 1, em sucesso, 0, em falha
*/
int determinar_piso_porta(Porta p)
{
    if(p == NULL)
        return 0;

    // adiciona as paredes no piso da porta (outras portas também são consideradas como paredes)
    for(int i = 0; i < qtd_linhas_sala; i++)
    {
        for(int h = 0; h < qtd_colunas_sala; h++)
        {
            int val = base.mapa[i][h];
            if(val == VALOR_PAREDE || val == VALOR_PORTA)
                p->piso[i][h] = VALOR_PAREDE;
            else
                p->piso[i][h] = 0;
        }
    }
    p->piso[p->loc_linha][p->loc_coluna] = VALOR_PORTA; // Adiciona a porta

    double **mat = p->piso;
    double **aux = alocar_matriz_double(qtd_linhas_sala,qtd_colunas_sala);
    // matriz para armazenar as alterações para o tempo t + 1
    if(aux == NULL)
        return 0;

    copiar_matriz(aux, mat); // copia o piso base para a matriz auxiliar

    int qtd_mudancas;
    do{
        qtd_mudancas = 0;
        for(int i = 0; i < qtd_linhas_sala; i++)
        {
            for(int h = 0; h < qtd_colunas_sala; h++)
            {
                double atual = mat[i][h];

                if(atual == VALOR_PAREDE || atual == 0.0)
                    continue;

                for(int j = -1; j < 2; j++)
                {
                    if(i + j < 0 || i + j >= qtd_linhas_sala)
                        continue;

                    for(int k = -1; k < 2; k++)
                    {
                        if(h + k < 0 || h + k >= qtd_colunas_sala)
                            continue;

                        if(mat[i + j][h + k] == VALOR_PAREDE || mat[i + j][h + k] == VALOR_PORTA)
                            continue;

                        double novo_valor = mat[i][h] + regra[1 + j][1 + k];
                        if(mat[i + j][h + k] == 0.0)
                        {
                            if(aux[i + j][h + k] == 0.0)
                                aux[i + j][h + k] = novo_valor;
                            else if(novo_valor < aux[i + j][h + k])
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
        copiar_matriz(mat,aux);

    }while(qtd_mudancas != 0);

    return 1;
}

/**
 * ## determinar_piso_geral
 * 
 * #### Entrada
 * Nada
 * 
 * #### Processo
 * Determina o piso geral da sala por meio da fusão dos pisos para cada porta.
 * 
 * #### Saída
 * 1, em sucesso, 0, em falha
 * 
*/
int determinar_piso_geral()
{
    if(portas.n_portas == 0 || portas.vet_portas == NULL)
        return 0;

    for(int q = 0; q < portas.n_portas; q++)
        if(!determinar_piso_porta(portas.vet_portas[q]))
            return 0;

    portas.piso_final = alocar_matriz_double(qtd_linhas_sala, qtd_colunas_sala);
    if(portas.piso_final == NULL)
        return 0;
    copiar_matriz(portas.piso_final, portas.vet_portas[0]->piso);
    
    for(int q = 1; q < portas.n_portas; q++)
    {
        for(int i = 0; i < qtd_linhas_sala; i++)
        {
            for(int h = 0; h < qtd_colunas_sala; h++)
            {
                if(portas.piso_final[i][h] > portas.vet_portas[q]->piso[i][h])
                    portas.piso_final[i][h] = portas.vet_portas[q]->piso[i][h];
            }
        }
    }

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
        novo[i] = calloc(num_col, sizeof(double));

    return novo;
}

/**
 * ## copiar_matriz
 * 
 * #### Entrada
 * Dois ponteiros para ponteiro de double.
 *      O primeiro indica a matriz de destino.
 *      O segundo a matriz fonte.
 * 
 * #### Processo
 *  Copia os dados de 'src' para 'dest'. As matrizes devem ser de mesmo tamanho.
 * 
 * #### Saída
 * 1, em sucesso, 0, em falha
*/
int copiar_matriz(double **dest, double **src)
{
    if(dest == NULL || src == NULL)
        return 0;

    for(int i = 0; i < qtd_linhas_sala; i++)
    {
        for(int h = 0; h < qtd_colunas_sala; h++)
        {
            dest[i][h] = src[i][h];
        }
    }
}

/**
 * ## imprimir_piso
 * 
 * #### Entrada
 * Ponteiro para ponteiro de double (matriz).
 * 
 * #### Processo
 * Imprime o conteúdo da matriz
 * 
 * #### Saída
 * 1, em sucesso, 0, em falha
*/
int imprimir_piso(double **mat)
{
    if(mat == NULL)
        return 0;

    for(int i=0; i < qtd_linhas_sala; i++){
		for(int h=0; h < qtd_colunas_sala; h++){
			printf("%.1lf\t", mat[i][h]);
		}
		printf("\n\n\n");
	}
    return 1;
}