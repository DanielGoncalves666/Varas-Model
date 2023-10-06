#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include"../headers/global_declarations.h"
#include"../headers/inicializacao.h"
#include"../headers/saida.h"
#include"../headers/pedestre.h"

int main(int argc, char **argv){

    if( !carregar_ambiente(argv[1],1))
		return 1;

	srand(0);

    determinar_piso_geral();
    //imprimir_piso(saidas.combined_field);
    printf("\e[1;1H\e[2J");
    imprimir_grid_pedestres();
    sleep(1);

    while(!sala_vazia())
    {
        printf("\e[1;1H\e[2J");
        determinar_movimento();
        panico();
        varredura_movimento_em_x();
        resolver_conflitos_movimento();
        confirmar_movimentacao();
        atualizar_grid_pedestres();
        imprimir_grid_pedestres();
        resetar_pedestres();
        sleep(1);
    }
    

    return 0;
}