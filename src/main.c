#include<stdio.h>

#include"global_declarations.h"
#include"carregamento_salas.h"
#include"porta.h"
#include"pedestre.h"

int main(int argc, char **argv){

    carregar_sala("sala_padrao",1);

    if(determinar_piso(portas.vet_portas[0]))
        imprimir_piso(portas.vet_portas[0]->piso);

    return 0;
}