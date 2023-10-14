/*
    Modelo de evacuação de pedestres proposto por Varas em seu artigo de 2007.

    Daniel Gonçalves, 2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<argp.h>
#include<unistd.h>

#include"../headers/global_declarations.h"
#include"../headers/inicializacao.h"
#include"../headers/saida.h"
#include"../headers/pedestre.h"

struct command_line {
    char nome_arquivo_entrada[51];
    char nome_arquivo_saida[51];
    char nome_arquivo_auxiliar[51];
    int output_type;
    int output_to_file;
    int input_method;
};

const char *path_saidas = "saidas/";
const char *path_output = "output/";

const char * argp_program_version = "Varas Original, sem movimentação em X ou através de obstáculos.";

const char doc[] = "Varas - Simula uma evacuação de pedestres por meio do modelo de (Varas,2007)."
"\v"
"O arquivo passado por --auxiliary-file deve conter, em cada uma de suas linhas, as localizações das saídas para um único conjunto de simulações.\n"
"Um número indeterminado de portas é aceito para um dado conjunto de simulações, com o conteúdo do arquivo devendo seguir o seguinte padrão:\n"
"LIN_PORTA_1 COL_PORTA_1, LIN_PORTA_2 COL_PORTA_2 ...\n"
"\tDepois do último par deve vir um ponto final.\n"
"OBS: Saídas repetidas são aceitas.\n"
"\n"
"--output-type indica quais e como os dados gerados pela simulação devem ser enviados para a saída. As seguintes opções são possíveis:\n"
"\t 1 - Impressão visual do ambiente.\n"
"\t 2 - Quantidade de passos de tempo até o fim das simulações.\n"
"\t 3 - Impressão de mapas de calor das células do ambiente.\n"
"A opção 1 é a padrão.\n"
"\n"
"--input-method indica como o ambiente informado em --input-file deve ser carregado ou se o ambiente deve ser gerado.\n"
"\tAmbiente carregado de um arquivo:\n"
"\t\t1 - Apenas a estrutura do ambiente (portas substituídas por paredes).\n"
"\t\t2 - Apenas a estrutura do ambiente (portas incluídas).\n"
"\t\t3 - Estrutura, portas e pedestres.\n"
"\tAmbiente criado automaticamente:\n"
"\t\t4 - Ambiente será criado considerando quantidade de linhas e colunas passadas pelas opções --lin e --col,"
"com portas informadas via --auxiliary-file.\n"
"O método 3 é o padrão.\n"
"Para os métodos 1 e 4, --auxiliary-file é obrigatório.\n"
"Para o método 4, --lin e --col são obrigatórios.\n"
"\n"
"O restante das opções (--simu, --ped, --seed), são sempre opcionais.\n"
"--input-file tem valor padrão de \"sala_padrao.txt\".\n"
"--simu e --ped tem valor padrão de 1.\n"
"--seed tem valor padrão de 0.\n"
"\n"
"Opções desnecessárias para determinados modos são ignoradas.\n";

static struct argp_option options[] = {
    {"\nArquivos:\n",0,0,OPTION_DOC,0,1},
    {"input-file", 'i', "INPUT-FILE", 0, "INPUT-FILE é o nome do arquivo que contém o ambiente pre-definido a ser carregado.",2},
    {"output-file", 'o', "OUTPUT-FILE", OPTION_ARG_OPTIONAL, "Indica se a saída deve ser armazenada em um arquivo, com o nome do arquivo sendo opcionalmente passado."},
    {"auxiliary-file", 'a', "ARQUIVO_AUXILIAR",0, "Contém informações auxiliares para a realização das simulações. Ex: localização das saídas."},

    {"\nModos de operação:\n",0,0,OPTION_DOC,0,3},    
    {"input-method", 'm', "METHOD",0, "Indica como tratar INPUT_FILE.",4},
    {"output-type", 'O', "TYPE", 0, "Indica o tipo de saída que deve ser gerada pelas simulações."},
    
    {"\nDimensões do ambiente:\n",0,0,OPTION_DOC,0,5},
    {"lin", 'l', "LINHAS", 0, "LINHAS indica a quantidade de linhas que o ambiente criado deve ter.",6},
    {"col", 'c', "COLUNAS", 0, "COLUNAS indica a quantidade de colunas que o ambiente criado deve ter."},

    {"\nVariáveis de simulação:\n",0,0,OPTION_DOC,0,7},
    {"ped", 'p', "PEDESTRES", 0, "Número de pedestres a serem inseridos no ambiente de forma aleatória.",8},
    {"simu", 's', "SIMULACOES", 0, "Número de simulações a serem realizadas por conjunto de saídas."},
    {"seed", 'e', "SEED", 0, "Semente inicial para geração de números pseudo-aleatórios."},
    {"\nOutros:\n",0,0,OPTION_DOC,0,9},
    {0}
};


error_t parser_function(int key, char *arg, struct argp_state *state);
int abrir_arquivo_auxiliar(struct command_line commands, FILE **arquivo_auxiliar);
int abri_arquivo_output(struct command_line commands, FILE **arquivo_saida);
void imprimir_mapa_calor(FILE *arquivo_saida);

static struct argp argp = {options,&parser_function, NULL, doc};

int main(int argc, char **argv){
    struct command_line commands = {"sala_padrao.txt","","",1,0,3};

    if( argp_parse(&argp, argc, argv,0,0,&commands) != 0)
        return 0;

    FILE *arquivo_auxiliar = NULL;
    if(!abrir_arquivo_auxiliar(commands, &arquivo_auxiliar))
        return 0;
    
    FILE *arquivo_saida = NULL;
    if(!abri_arquivo_output(commands, &arquivo_saida))
    {
        if(arquivo_auxiliar != NULL)
            fclose(arquivo_auxiliar);
        return 0;
    }

    if(commands.input_method == 4 && !alocar_grids())
        return 0;

    if(commands.input_method != 4)
    {
        if( !carregar_ambiente(commands.nome_arquivo_entrada, commands.input_method))
            return 0;
    }
    else
    {
        if( !gerar_ambiente())
            return 0;
    }

    do
    {
        if(commands.input_method == 1 || commands.input_method == 4)
        {
            if( !extrair_saidas(arquivo_auxiliar))
                break; // caso ocorra algum erro, ou se todos os conjuntos de saídas já tiverem sido processados.
        }

        int seed = original_seed;
        for(int i = 0; i < numero_simulacoes; i++, seed++)
        {
            srand(seed);
            if( !determinar_piso_geral())
                return 0;

            //imprimir_piso(saidas.combined_field);

            if(commands.input_method != 3)
            {
                if( !inserir_pedestres_aleatoriamente(numero_pedestres))
                    return 0;
            }
            
            if(commands.output_type == 1)
            {
                fprintf(arquivo_saida,"Simulação %d\n",i);
                if(!commands.output_to_file)
                    printf("\e[1;1H\e[2J");
                imprimir_grid_pedestres(arquivo_saida);
            }

            int num_passos_tempo = 0;
            while(!sala_vazia())
            {
                determinar_movimento();
                //printf("\t Passo %d:\n", num_passos_tempo);
                //fprintf(arquivo_saida, "\t Passo %d:\n", num_passos_tempo);
                panico();
                varredura_movimento_em_x();
                resolver_conflitos_movimento();
                confirmar_movimentacao();
                atualizar_grid_pedestres();
                resetar_estado_pedestres();
                
                if(commands.output_type == 1)
                {
                    if(!commands.output_to_file)
                    {
                        sleep(1);
                        printf("\e[1;1H\e[2J");
                    }
                    imprimir_grid_pedestres(arquivo_saida);
                }

                num_passos_tempo++;
            }

            if(commands.input_method == 3)
                reiniciar_pedestre();
            else
                desalocar_pedestres();

            if(commands.output_type == 2) // passos e tempo
                fprintf(arquivo_saida,"%d ", num_passos_tempo);
        }

        if(commands.input_method == 1 || commands.input_method == 4)
            desalocar_saidas();

        if(commands.output_type == 2)
            fprintf(arquivo_saida, "\n\n");

        if(commands.output_type == 3) // mapa de calor
            imprimir_mapa_calor(arquivo_saida);        

        // esses modos apresentam apenas um conjunto de saídas (as saídas do próprio ambiente)
        if(commands.input_method == 2 || commands.input_method == 3)
            break;

    }while(1);

    if(arquivo_auxiliar != NULL)
        fclose(arquivo_auxiliar);

    if(arquivo_saida != NULL && arquivo_saida != stdout)
        fclose(arquivo_saida);

    return 0;
}

error_t parser_function(int key, char *arg, struct argp_state *state)
{
    struct command_line *commands = state->input;

    switch(key)
    {
        case 'o':
            if(arg != NULL)
                strcpy(commands->nome_arquivo_saida, arg);

            commands->output_to_file = 1;
            break;
        case 'O':
            commands->output_type = atoi(arg);
            if(commands->output_type <= 0 || commands->output_type > 3)
            {
                fprintf(stderr, "Nenhuma forma de saída corresponde ao output-type passado.\n");
                return EIO;
            }

            break;
        case 'i':
            strcpy(commands->nome_arquivo_entrada, arg);
            break;
        case 'm':
            commands->input_method = atoi(arg);
            if(commands->input_method <= 0 || commands->input_method > 4)
            {
                fprintf(stderr, "Nenhum tratamento corresponde ao input_method passado.\n");
                return EIO;
            }

            break;
        case 'a':
            strcpy(commands->nome_arquivo_auxiliar, arg);
            break;
        case 'l':
            num_lin_grid = atoi(arg);
            if(num_lin_grid <= 0)
            {
                fprintf(stderr,"Número de linhas inválido.\n");
                return EIO;
            }
            break;
        case 'c':
            num_col_grid = atoi(arg);
            if(num_col_grid <= 0)
            {
                fprintf(stderr,"Número de colunas inválido.\n");
                return EIO;
            }
            break;
        case 'p':
            numero_pedestres = atoi(arg);
            if(numero_pedestres <= 0)
            {
                fprintf(stderr, "Número de pedestres inválido.\n");
                return EIO;
            }
            break;
        case 's':
            numero_simulacoes = atoi(arg);
            if(numero_simulacoes <= 0)
            {
                fprintf(stderr, "Número de simulações inválido.\n");
                return EIO;
            }
            break;
        case 'e':
            original_seed = atoi(arg);
            if(original_seed < 0)
            {
                fprintf(stderr, "Seed inválida.\n");
                return EIO;
            }
            break;
        case ARGP_KEY_ARG:
            fprintf(stderr, "Nenhum argumento não-opcional é esperado.\n");
            return EINVAL;
            break;
        case ARGP_KEY_END:
            if( commands->input_method == 1 || commands->input_method == 4) // sem portas
            {
                if( strcmp(commands->nome_arquivo_auxiliar,"") == 0)
                {
                    fprintf(stderr, "Arquivo auxiliar com a localização das saídas é esperado.\n");
                    return EIO;
                }
            }
            else
            {
                if( strcmp(commands->nome_arquivo_auxiliar,"") != 0)
                    strcpy(commands->nome_arquivo_auxiliar,"");
            }

            if(commands->input_method == 4)
            {
                if(num_lin_grid == 0 || num_col_grid == 0)
                {
                    fprintf(stderr, "Dimensões do ambiente não foram informadas.\n");
                    return EIO;
                }
            }

            break;
        default:
            return ARGP_ERR_UNKNOWN;
            break;
    }

    return 0;
}


/**
 * ## abrir_arquivo_auxiliar
 * 
 * #### Entrada
 * Estrutura de comandos, contendo os argumentos de linha de comando
 * Ponteiro para ponteiro de arquivo.
 * #### Descrição
 * Abre o arquivo auxiliar em modo leitura.
 * #### Saída,
 * 1, em sucesso, 0, em falha.
*/
int abrir_arquivo_auxiliar(struct command_line commands, FILE **arquivo_auxiliar)
{
    char complete_path[100] = "";
    
    if( commands.input_method == 1 || commands.input_method == 4)
    {
        sprintf(complete_path,"%s%s",path_saidas,commands.nome_arquivo_auxiliar);

        *arquivo_auxiliar = fopen(complete_path,"r");
        if(*arquivo_auxiliar == NULL)
        {
            fprintf(stderr, "Não foi possível abrir o arquivo auxiliar.\n");
            return 0;
        }
    }

    return 1;
}

/**
 * ## abri_arquivo_output
 * 
 * #### Entrada
 * Estrutura de comandos, contendo os argumentos de linha de comando
 * Ponteiro para ponteiro de arquivo.
 * #### Descrição
 * Abre o arquivo de saída em modo leitura, caso algum tenha sido informado.
 * Se nenhum arquivo tiver sido informado, seta a saída para stdout.
 * #### Saída,
 * 1, em sucesso, 0, em falha.
*/
int abri_arquivo_output(struct command_line commands, FILE **arquivo_saida)
{
    char complete_path[300] = "";
    
    time_t timer;
    time(&timer);
    struct tm *agora = gmtime(&timer);
    
    if(commands.output_to_file)
    {
        // se nenhum nome tiver sido passado
        if(strcmp(commands.nome_arquivo_saida, "") == 0)
        {
            char *output_type_name;
            if(commands.output_type == 1)
                output_type_name = "visual";
            else if(commands.output_type == 2)
                output_type_name = "tempo_discreto";
            else
                output_type_name = "mapa_calor";

            sprintf(complete_path,"%s%s-%s-%s-%02d:%02d:%02d-%02d %02d %d.txt", path_output, output_type_name, 
                    commands.nome_arquivo_entrada, commands.nome_arquivo_auxiliar, agora->tm_hour - 3, 
                    agora->tm_min, agora->tm_sec, agora->tm_mday, agora->tm_mon + 1, agora->tm_year + 1900);
        }
        else
            sprintf(complete_path,"%s%s",path_output,commands.nome_arquivo_saida);


        *arquivo_saida = fopen(complete_path,"w");
        if(*arquivo_saida == NULL)
        {
            fprintf(stderr, "Não foi possível criar o arquivo de output.\n");
            return 0;
        }
    }
    else
    {
        *arquivo_saida = stdout;
    }

    return 1;
}

/**
 * ## imprimir_mapa_calor
 * 
 * #### Entrada
 * Arquivo onde será escrita a saída.
 * #### Descrição
 * Imprime a grid do mapa de calor.
 * O valor de cada posição é dividido pelo numero de simulações. Deste modo, o mapa de calor é um mapa de calor médio.
 * #### Saída
 * Nenhuma
*/
void imprimir_mapa_calor(FILE *arquivo_saida)
{
    for(int i = 0; i < num_lin_grid; i++){
		for(int h = 0; h < num_col_grid; h++)
            fprintf(arquivo_saida, "%7.2lf ", (double) grid_mapa_calor.mat[i][h] / (double) numero_simulacoes);

		fprintf(arquivo_saida,"\n");
	}
    fprintf(arquivo_saida,"\n");
}