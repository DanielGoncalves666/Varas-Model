# Implementação do Modelo de Varas para evacuação de pedestres

## Compilação e Execução
Para compilar e executar: `./varas.sh`, seguido dos argumentos necessários.

## Manual de Uso

```bash
Usage: teste [OPTION...]
Varas - Simula uma evacuação de pedestres por meio do modelo de
(Varas,2007).

  
Arquivos:

  -a, --auxiliary-file=ARQUIVO_AUXILIAR
                             Contém informações auxiliares para a
                             realização das simulações. Ex: localização
                             das saídas.
  -i, --input-file=INPUT-FILE   INPUT-FILE é o nome do arquivo que contém o
                             ambiente pre-definido a ser carregado.
  -o, --output-file=OUTPUT-FILE   OUTPUT-FILE é o nome do arquivo que deve
                             conter a saída do programa.
  
Modos de operação:

  -m, --input-method=METHOD  Indica como tratar INPUT_FILE.
  -O, --output-type=TYPE     Indica o tipo de saída que deve ser gerada pelas
                             simulações.
  
Dimensões do ambiente:

  -c, --col=COLUNAS          COLUNAS indica a quantidade de colunas que o
                             ambiente criado deve ter.
  -l, --lin=LINHAS           LINHAS indica a quantidade de linhas que o
                             ambiente criado deve ter.
  
Variáveis de simulação:

  -e, --seed=SEED            Semente inicial para geração de números
                             pseudo-aleatórios.
  -p, --ped=PEDESTRES        Número de pedestres a serem inseridos no ambiente
                             de forma aleatória.
  -s, --simu=SIMULACOES      Número de simulações a serem realizadas.
  
Outros:

  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

O arquivo passado por --auxiliary-file deve conter, em cada uma de suas linhas,
as localizações das saídas para um único conjunto de simulações.
Um número indeterminado de portas é aceito para um dado conjunto de
simulações, com o conteúdo do arquivo devendo seguir o seguinte padrão:
LIN_PORTA_1 COL_PORTA_1, LIN_PORTA_2 COL_PORTA_2 ...
        Depois do último par deve vir um ponto final.
OBS: Saídas repetidas são aceitas.

--output-type indica quais e como os dados gerados pela simulação devem ser
enviados para a saída. As seguintes opções são possíveis:
         1 - Impressão visual do ambiente.
         2 - Quantidade de passos de tempo até o fim das simulações.
         3 - Impressão de mapas de calor das células do ambiente.
A opção 1 é a padrão.

--input-method indica como o ambiente informado em --input-file deve ser
carregado ou se o ambiente deve ser gerado.
        Ambiente carregado de um arquivo:
                1 - Apenas a estrutura do ambiente (portas substituídas por paredes).
                2 - Apenas a estrutura do ambiente (portas incluídas).
                3 - Estrutura, portas e pedestres.
        Ambiente criado automaticamente:
                4 - Ambiente será criado considerando quantidade de linhas e colunas
passadas pelas opções --lin e --col,com portas informadas via
--auxiliary-file.
O método 3 é a padrão.
Para os métodos 1 e 4, --auxiliary-file é obrigatório.
Para o método 4, --lin e --col são obrigatórios.

O restante das opções (--simu, --ped, --seed), são sempre opcionais.
--ambiente tem valor padrão de "sala_padrao.txt
--simu e --ped tem valor padrão de 1.
--seed tem valor padrão de 0.

Opções desnecessárias para determinados modos são ignoradas.

```