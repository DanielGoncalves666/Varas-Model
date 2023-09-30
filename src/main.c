#include<stdio.h>

#include"global_declarations.h"
#include"carregamento_salas.h"


int main(int argc, char **argv){

    carregar_sala("sala_padrao",1);
    printf("%d %d\n", portas.n_portas, lista_ped.tam_lista);

    return 0;
}