/*
    Módulo pedestre.

    Cria pedestres e administra o conjunto de pedestres, além de conter as funções de movimentação.

    Daniel Gonçalves, 2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include"../headers/global_declarations.h"
#include"../headers/pedestre.h"

// Equação reduzida da reta. Usada para determinar a existência de movimentação em X.
typedef struct reta{
    double m; // coeficiente angular
    double n; // coeficiente linear (onde corta o eixo y)
}reta;

typedef struct lista_ped_conflito{
    int qtd;
    int ped[8]; // considerando a vizinhança em uso (moore), 8 é a quantidade máxima de conflitos possível
}lista_ped_conflito;

typedef struct celula{
    int loc_lin, loc_col;
    double valor;
}celula;

typedef struct lista_vizin_valida{
    int qtd;
    celula vet[8]; // 8 é a quantidade de células na vizinhança desconsiderando a célula do pedestre
}lista_vizin_valida;


Pedestre criar_pedestre(int loc_linha, int loc_coluna);
lista_vizin_valida *determinar_vizin_valida(int loc_lin, int loc_col);
void adicionar_na_lista_vizin_valida(lista_vizin_valida *vizinhos,int lin, int col, double valor);
int resolver_movimento_em_x(Pedestre um, Pedestre dois);
int intersecao(reta r_um, reta r_dois, Pedestre p);

/**
 * ## criar_pedestre
 * 
 * #### Entrada
 * Linha e coluna indicando a localização do pedestre, respectivamente.
 * 
 * #### Descrição
 * Aloca uma estrutura para o novo pedestre e a preenche.
 * 
 * #### Saída
 * Pedestre, ou NULL.
*/
Pedestre criar_pedestre(int loc_linha, int loc_coluna)
{
    Pedestre novo = malloc(sizeof(struct pedestre));
    if(novo != NULL)
    {
        novo->loc_lin = novo->origin_lin = loc_linha;
        novo->loc_col = novo->origin_col = loc_coluna;
        novo->mov_lin = -1;
        novo->mov_col = -1;
        novo->estado = MOVENDO;
    }

    return novo;
}

/**
 * ## adicionar_pedestre_conjunto
 * 
 * #### Entrada
 * Linha e coluna, indicando a localização do pedestre, respectivamente.
 * 
 * #### Descrição
 * Cria um novo pedestre baseando-se na localização passada e o adiciona no conjunto de pedestres.
 * 
 * #### Saída
 * 1, em sucesso, 0, em fracasso
*/
int adicionar_pedestre_conjunto(int loc_lin, int loc_col)
{
    Pedestre novo = criar_pedestre(loc_lin, loc_col);
    if(novo == NULL)
    {
        fprintf(stderr, "Falha na criação do pedestre em (%d,%d).\n", loc_lin, loc_col);
        return 0;
    }

    pedestres.n_ped += 1;
    pedestres.vet = realloc(pedestres.vet, sizeof(struct pedestre) * pedestres.n_ped);
    if(pedestres.vet == NULL)
    {
        fprintf(stderr,"Falha na realocação do vetor de pedestres.\n");
        return 0;
    }

    novo->id = pedestres.n_ped;
    pedestres.vet[pedestres.n_ped - 1] = novo;

    return 1;
}

/**
 * ## panico
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Determina, com prababiliade de PANICO, se cada pedestre irá entrar em pânico e ficará parado 
 * #### Saída
 * Quantidade de pedestres que entraram em pânico
*/
int panico()
{
    int qtd = 0;
    for(int i = 0; i < pedestres.n_ped; i++)
    {
        if((rand() % 101) / 100 < PANICO)
        {
            pedestres.vet[i]->estado = PARADO;
            qtd++;
        }
    }

    return qtd;
}


/**
 * ## determinar_movimento
 * 
 * #### Entrada:
 * Nenhuma
 * 
 * #### Descrição:
 * Determina a célula para a qual cada pedestre PRETENDE se movimentar
 * 
 * #### Saída:
 * Nenhuma
*/
void determinar_movimento()
{
    for(int p = 0; p < pedestres.n_ped; p++)
    {
        Pedestre atual = pedestres.vet[p];

        if(atual->estado == SAIU || atual->estado == PARADO)
            continue;

        lista_vizin_valida *vizinhos = determinar_vizin_valida(atual->loc_lin, atual->loc_col);

        if(vizinhos->qtd == 0) // não existem células vizinhas válidas para movimentação
            atual->estado = PARADO;
        else
        {
            // trata células com melhor valor iguais
            int iguais = 1;

            for(int i = 1; i < vizinhos->qtd; i++)
            {   
                if(vizinhos->vet[i].valor != vizinhos->vet[0].valor)
                    break;

                iguais++;
            }

            int sorted_cell = rand() % iguais;

            // altera a intenção de movimentação
            atual->mov_lin = vizinhos->vet[sorted_cell].loc_lin;
            atual->mov_col = vizinhos->vet[sorted_cell].loc_col;
        }

        free(vizinhos);
    }
}

/**
 * ## determinar_vizin_valida
 * 
 * #### Entrada
 * Dois inteiros, indicando a posição que o pedestre ocupa
 * #### Descrição
 * Varre a vizinhança da célula passada (que um pedestre está) e determina as células vizinhas em 
 * que o pedestre o pedestre pode se mover.
 * #### Saída
 * Ponteiro para uma lista contendo as células da vizinhança que são válidas para movimentação.
*/
lista_vizin_valida *determinar_vizin_valida(int loc_lin, int loc_col)
{
    double **piso = saidas.combined_field;
    lista_vizin_valida *vizinhos = calloc(1, sizeof(lista_vizin_valida));

    for(int j = -1; j < 2; j++)
    {
        for(int k = -1; k < 2; k++)
        {
            if(j != 0 && k != 0)
            {
                if(! eh_diagonal_valida(loc_lin,loc_col,j,k,piso))
                    continue; // impossível chegar na célula
            }

            if(piso[loc_lin + j][loc_col + k] == VALOR_PAREDE)
                continue; // parede na célula

            if(grid_pedestres.mat[loc_lin + j][loc_col + k] > 0)
                continue; // pedestre na célula (inclui a célula do próprio pedestre)

            adicionar_na_lista_vizin_valida(vizinhos,loc_lin + j,loc_col + k, piso[loc_lin + j][loc_col + k]);
        }
    }

    return vizinhos;
}

/**
 * ## adicionar_na_lista_vizin_valida
 * 
 * #### Entrada
 * Uma lista de vizinhança válida.
 * Dois inteiros, indicando a linha e coluna da célula vizinha.
 * Um double, indicando o valor da célula vizinha.
 * #### Descrição
 * Adiciona, de forma ordenada (por isertion sort) a célula vizinha na lista de vizinhança válida.
 * #### Saída
 * Nenhuma
*/
void adicionar_na_lista_vizin_valida(lista_vizin_valida *vizinhos,int lin, int col, double valor)
{
    celula nova = {lin,col,valor};

    if(vizinhos->qtd == 0)
    {
        vizinhos->vet[0] = nova;
    }
    else
    {
        int i;
        for(i = vizinhos->qtd - 1; i >= 0; i--)
        {
            if(valor < vizinhos->vet[i].valor)
                vizinhos->vet[i + 1] = vizinhos->vet[i];
            else
            {
                vizinhos->vet[i + 1] = nova;
                break;
            }
        }

        if(i < 0)
            vizinhos->vet[0] = nova;
    }

    vizinhos->qtd++;
}


/**
 * ## resolver_conflitos_movimento
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Determina movimentos pretendidos para mesmo célula e decide qual dos pedestres irá se mover, 
 * enquanto os outros permanecem parados.
 * #### Saída
 * Quantidade de células em conflito
*/
int resolver_conflitos_movimento()
{
    int count = 0;
    int **mat_conflitos = alocar_matriz_int(num_lin_grid,num_col_grid); // matriz zerada
    lista_ped_conflito *conflitos = NULL;

    // verifica para onde todos os pedestres querem se mover e determina onde existe conflitos
    for(int i = 0; i < pedestres.n_ped; i++)
    {
        Pedestre atual = pedestres.vet[i];

        if(atual->estado == SAIU || atual->estado == PARADO)
            continue;

        int conteudo = mat_conflitos[atual->mov_lin][atual->mov_col]; 
        if(conteudo > 0) // novo conflito
        {
            conflitos = realloc(conflitos, sizeof(lista_ped_conflito) * (count + 1));
            conflitos->ped[0] = conteudo;
            conflitos->ped[1] = atual->id;
            conflitos->qtd = 2;
            /* já existia um pedestre na célula. Uma lista de pedestres em conflito para aquela célula é criada
               e preenchida com os dois pedestre em conflito.*/

            count++;

            mat_conflitos[atual->mov_lin][atual->mov_col] = count * -1;
            // o número negativo indica que já existe um conflito na célula.
            // Além disso, serve como index para a lista de pedestres em conflito.
            // index = (conteudo * -1) - 1;
        }
        else if(conteudo < 0) // conflito já existente
        {
            int index = (conteudo * -1) - 1;
            conflitos[index].ped[conflitos->qtd] = atual->id;
            conflitos[index].qtd++;
        }
        else
            mat_conflitos[atual->mov_lin][atual->mov_col] = atual->id;
            // o id do pedestre é colocado na célula que ele pretende se movimentar se ela estiver vazia.
    }

    // para cada conflito, determina o pedestre que irá se movimentar
    for(int i = 0; i < count; i++)
    {
        int sorted_num = rand() % conflitos[i].qtd;
        // sorteia um número entre 0 e ( quantidade de ped em conflito)

        for(int h = 0; h < conflitos[i].qtd; h++)
        {
            int index_ped_atual = conflitos[i].ped[h] - 1;

            if(sorted_num == -1)
                pedestres.vet[index_ped_atual]->estado = PARADO;
                continue;

            if(sorted_num < (h + 1))
                sorted_num = -1;
        }
    }

    desalocar_matriz_int(mat_conflitos,num_lin_grid);

    return count;
}

/**
 * ## varredura_movimento_em_x
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Varre a grid de pedestres no tempo atual em busca de pedestres adjascentes uns aos outros.
 * Quando encontrados, envia os pedestres para a função que verifica e lida com movimentações em X.
 * #### Saída
 * Inteiro indicando a quantidade de movimentações em X resolvidas.
 * 
*/
int varredura_movimento_em_x()
{
    int qtd = 0;

    for(int i = 1; i < num_lin_grid - 1; i++) // evita os limites da grid pois pedestres não estarão lá
    {
        for(int h = 1; h < num_col_grid - 1; h++)
        {
            int cel_atual = grid_pedestres.mat[i][h];
            if(cel_atual > 0)
            {
                if(pedestres.vet[cel_atual - 1]->estado != MOVENDO)
                    continue;

                // pedestres acima da célula atual já terão sido verificados

                // o possível pedestre na célula à esquerda da atual já terá sido verificado
                int cel_adj = grid_pedestres.mat[i][h + 1];
                if(cel_adj > 0)
                    qtd += resolver_movimento_em_x(pedestres.vet[cel_atual- 1], pedestres.vet[cel_adj - 1]);

                for(int k = -1; k < 2; k++) // linha inferior
                {
                    int cel_adj = grid_pedestres.mat[i + 1][h + k];
                    if(cel_adj > 0)
                        qtd += resolver_movimento_em_x(pedestres.vet[cel_atual- 1], pedestres.vet[cel_adj - 1]);
                }
            }
        }
    }

    return qtd;
}

/**
 * ## resolver_movimento_em_x
 * 
 * #### Entrada
 * Dois pedestres cujos movimentos serão verificados para determinar se ocorreria uma movimentação em X
 * 
 * #### Descrição
 * Verifica se a movimentação pretendida pelos dois pedestres resulta em movimentação em X.
 * Se sim, determina aleatoriamente qual deles irá se movimentar.
 * 
 * #### Saída
 * 0, para não ocorrência de movimentação em X.
 * 1, para existência de movimento em X, 
 * 2, para falha
*/
int resolver_movimento_em_x(Pedestre um, Pedestre dois)
{
    if(um == NULL || dois == NULL)
        return 2;

    if(um->estado != MOVENDO || dois->estado != MOVENDO)
        return 0;

    /* Criamos duas retas, cada uma contendo os pontos de origem e destino de um dos pedestres.
       Verifica-se então se as retas se cruzam num dado intervalo (se sim, então movimento em X é garantido). */
    reta r_um, r_dois;

    // m = (yf - yi) / (xf - xi)
    r_um.m = (um->mov_lin - um->loc_lin) / (um->mov_col - um->loc_col);
    r_dois.m = (dois->mov_lin - dois->loc_lin) / (dois->mov_col - dois->loc_col);

    // n = yi - m * xi
    r_um.n = um->loc_lin - r_um.m * um->loc_col;
    r_dois.n = dois->loc_lin - r_dois.m * dois->loc_col;

    if(intersecao(r_um,r_dois,um) == 1)
    {
        int sorted_num = rand() % 100;

        if(sorted_num < 50)
            dois->estado = PARADO;
        else
            um->estado = PARADO;

        return 1;
    }

    return 0;
}


/**
 * ## intersecao
 * 
 * #### Entrada
 * Duas retas
 * Pedestre, o qual contendo as coordenadas do segmeno de reta onde a interseção deve ocorrer.
 * 
 * #### Descrição
 * Determina se as duas retas passadas se interceptam no segmento determinado pela origem e destino do pedestre.
 * 
 * #### Saída
 * 0, para não se interceptam
 * 1, para se interceptam no intervalo
 * 2, para se interceptam na ponta do intervalo (pedestres se movem para mesma célula)
*/
int intersecao(reta r_um, reta r_dois, Pedestre p)
{
    if(r_um.m == r_dois.m) // retas paralelas
        return 0;

     /*
        (1): y = m1 * x + n1
        (2): y = m2 * x + n2

        (1') --> (1) * m2/m1
        (1'): m2/m1 * y = m2 * x + m2/m1 * n1

        (1') - (2):
        (m2/m1 - 1) * y = m2/m1 * n1 + n2
            (os coeficientes de x se anulam)

        y = ((m2/m1) * n1 + n2) / (m2/m1 - 1)
        x = (y - n1) / m1   

    */

    double fracao = r_dois.m / r_um.m;
    double intersect_y = (r_um.n * fracao + r_dois.n) / (fracao - 1);
    double intersect_x = (intersect_y - r_um.n) / r_um.m;

    if(intersect_x == p->mov_col && intersect_y == p->mov_lin)
        return 2; // os pedestre se movem para o mesmo local

    if(intersect_x > fmin(p->loc_col, p->mov_col) && intersect_x < fmax(p->loc_col, p->mov_col)
       && intersect_y > fmin(p->loc_lin, p->mov_lin) && intersect_y < fmax(p->loc_lin, p->mov_lin))
        return 1; // movimentação em x
}

/**
 * ## confirmar_movimentacao
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Transforma a intenção de movimentação dos pedestres em estado MOVENDO em atual movimentação.
 * (as coordenadas de destino são copiadas para as coordenadas atuais).
 * #### Saída
 * Nenhuma
*/
void confirmar_movimentacao()
{
    for(int i = 0; i < pedestres.n_ped; i++)
    {
        Pedestre atual = pedestres.vet[i];
        if(atual->estado != MOVENDO)
        {
            atual->loc_lin = atual->mov_lin;
            atual->loc_col = atual->mov_col;

            if(saidas.combined_field[atual->loc_lin][atual->loc_col] == VALOR_SAIDA)
                atual->estado = SAIU;
        }
    }
}

/**
 * ## sala_vazia
 * 
 * #### Entrada
 * Nenhum
 * #### Descrição
 * Verifica se todos os pedestres saíram da sala
 * #### Saída
 * 0, se ainda existem pedestres na sala
 * 1, se todos os pedestres já sairam
*/
int sala_vazia()
{
    for(int i = 0; i < pedestres.n_ped; i++)
    {
        Pedestre atual = pedestres.vet[i];
        if(atual->estado != SAIU)
            return 0;
    }

    return 1;
}

/**
 * ## atualizar_grid_pedestre
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Atualiza a grid de pedestres com as localizações de todos os pedestres.
 * #### Saída
 * Nenhuma
*/
void atualizar_grid_pedestres()
{
    for(int i = 0; i < num_lin_grid; i++)
    {
        for(int h = 0; h < num_col_grid; h++)
            grid_pedestres.mat[i][h] = 0;
    }

    for(int p = 0; p < pedestres.n_ped; p++)
    {
        Pedestre atual = pedestres.vet[p];

        if(atual->estado == SAIU)
            continue;

        grid_pedestres.mat[atual->loc_lin][atual->loc_col] = atual->id;
    }
}

/**
 * ## resetar_pedestres
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Reseta todos os pedestres que estão em estado PARADO para estado MOVENDO.
 * #### Saída
 * Nenhuma
*/
void resetar_pedestres()
{
    for(int p = 0; p < pedestres.n_ped; p++)
    {
        if(pedestres.vet[p]->estado != SAIU)
           pedestres.vet[p]->estado = MOVENDO;

    }
}

/**
 * ## imprimir_grid_pedestres
 * 
 * #### Entrada
 * Nenhuma
 * #### Descrição
 * Imprime agrid dos pedestres.
 * #### Saída
 * Nenhuma
*/
void imprimir_grid_pedestres()
{
	for(int i = 0; i < num_lin_grid; i++){
		for(int h = 0; h < num_col_grid; h++)
		{
			if(saidas.combined_field[i][h] == VALOR_SAIDA)
				printf("🚪");
			else if(saidas.combined_field[i][h] == VALOR_PAREDE)
				printf("🧱"); // imprime parede
			else if(grid_pedestres.mat[i][h] == 0)
				printf("⬛"); // célula vazia, imprime nada
			else
				printf("👤"); //pedestre
		}
		printf("\n");
	}
}