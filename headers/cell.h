#ifndef CELL_H
#define CELL_H

#include<stdbool.h>

#include"shared_resources.h"

typedef struct{
    Location coordinates;
    double value;
}Cell;

typedef struct cell_list{
    int num_cells;
    Cell *list;
}cell_list;

Cell find_smallest_cell(Location ped_coordinates, bool unoccupied_only);

#endif