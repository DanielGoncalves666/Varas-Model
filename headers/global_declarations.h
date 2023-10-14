#ifndef GLOBAL_DECLARATIONS
#define GLOBAL_DECLARATIONS

#define VALOR_SAIDA 1
#define VALOR_PAREDE 1000
#define VALOR_DIAGONAL 1.5
#define PANICO 0.05

extern int num_lin_grid;
extern int num_col_grid;
extern int numero_simulacoes;
extern int original_seed;
extern int numero_pedestres;

enum Status {SAIU, PARADO, MOVENDO};

// armazena a grid que contém os estados das células
typedef struct grid {
    int **mat; 
} Grid;

// typedef int ** Grid;

struct saida {
    int loc_lin, loc_col; // localização da saida na grid
    double **field; // piso referente à saida especificada
};
typedef struct saida * Saida;

typedef struct conjunto_portas{
    double **combined_field; // piso resultante da combinação dos pisos de cada uma das saidas
    Saida *vet_saidas;// vetor de todas as saidas
    int n_saidas;// número de saidas
} Conjunto_saidas;

struct pedestre {
    int id;
    enum Status estado;
    int origin_lin, origin_col; // localização inicial do pedestre (não se altera)
    int loc_lin, loc_col; // localização atual do pedestre
    int mov_lin, mov_col; // célula que o pedestre pretende se mover
};
typedef struct pedestre * Pedestre;

typedef struct conjunto_pedestres {
    Pedestre *vet;
    int n_ped;
} Conjunto_pedestres;

extern Grid grid_esqueleto;
extern Grid grid_pedestres;
extern Grid grid_mapa_calor;
extern Conjunto_saidas saidas;
extern Conjunto_pedestres pedestres;

int **alocar_matriz_int(int num_lin, int num_col);
void desalocar_matriz_int(int **mat, int lin);
double **alocar_matriz_double(int num_lin, int num_col);
void desalocar_matriz_double(double **mat, int lin);
int copiar_matriz_double(double **dest, double **src);
int eh_diagonal_valida(int loc_lin, int loc_col, int j, int k, double **mat);
int zerar_matriz_inteiros(int **mat, int num_lin, int num_col);

#endif