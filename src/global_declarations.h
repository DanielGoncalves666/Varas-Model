#ifndef GLOBAL_DECLARATIONS
#define GLOBAL_DECLARATIONS

// usada para armazenar o layout base da sala e para acompanhar o posicionamento dos pedestres e outros elementos
typedef struct sala {
    int **mapa; 
} Sala;

struct porta {
    int loc_linha, loc_coluna; // localização da porta
    double **piso; // piso referente à porta especificada
};
typedef struct porta * Porta;

typedef struct pilha_portas{
    double **piso_final; // piso resultante da combinação dos pisos de cada uma das portas
    Porta *vet_portas;// vetor de todas as portas
    int n_portas;// número de portas na pilha
} Pilha_Portas;

struct pedestre {
    int loc_linha, loc_coluna; // localização atual do pedestre
    int mov_linha, mov_coluna; // célula que o pedestre pretende se mover
};
typedef struct pedestre * Pedestre;

typedef struct lista_pedestres {
    int tam_lista;
    Pedestre *lista;
} Lista_pedestres;

#define VALOR_PORTA 1
#define VALOR_PAREDE 1000
#define VALOR_DIAGONAL 1.5

extern int qtd_linhas_sala;
extern int qtd_colunas_sala;
extern int qtd_portas;
extern int total_pedestres;

extern Sala base;
extern Pilha_Portas portas;
extern Lista_pedestres lista_ped;

#endif