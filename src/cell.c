/* 
   File: pedestrian.c
   Author: Daniel Gonçalves
   Date: 2024-06-20
   Description: This module defines structures related to a single cell and a function to find the smallest neighbour of a cell.
*/

#include<stdlib.h>
#include<stdbool.h>

#include"../headers/cell.h"
#include"../headers/exit.h"
#include"../headers/grid.h"
#include"../headers/shared_resources.h"

static void sort_cell_list(cell_list neighborhood);

/**
 * Scans the neighborhood of the cell at the given Location and finds the cell with the smallest floor field value.
 * The flag unoccupied_only determines if cells occupied shouldn't or should be considered when determining the smallest cell.
 * Even if the occupied cells are considered, the pedestrian will not move to a occupied cell and instead will remain in the same
 * place.
 * 
 * @param ped_coordinates The coordinates of the pedestrian for which to determine the destination cell.
 * @param unoccupied_only A boolean indicating whether to consider only cells not occupied by a pedestrian (True) or not (False).
 * @return A Cell structure representing the destination cell:
 *         - If the pedestrian can move, the Cell will have valid values.
 *         - If the pedestrian must remain in the same place, the Cell will have -1 values.
 *              - If unoccupied_only is true, then this will happen only when there is not a single empty cell in th neighborhood.
 *              - If unoccupied_only is false, then this will happen when the smallest cell is occupied.
*/
Cell find_smallest_cell(Location ped_coordinates, bool unoccupied_only)
{
    Double_Grid final_floor_field = exits_set.final_floor_field;
    cell_list neighborhood = {0, NULL};
    neighborhood.list = calloc(1, sizeof(Cell) * 8);

    for(int j = -1; j < 2; j++)
    {
        for(int k = -1; k < 2; k++)
        {
            if(j == 0 && k == 0)
                continue; // The Cell in the given coordinates.

            if(is_within_grid_lines(ped_coordinates.lin + j) == false || is_within_grid_columns(ped_coordinates.col + k) == false)
                continue;

            double cell_value = final_floor_field[ped_coordinates.lin + j][ped_coordinates.col + k];

            if(cell_value == WALL_VALUE)
                continue;

            if(j != 0 && k != 0)
            {
                if( is_diagonal_valid(ped_coordinates,(Location){j,k},final_floor_field) == false)
                    continue; // It's impossible to reach the cell.
            }

            if(unoccupied_only && pedestrian_position_grid[ped_coordinates.lin + j][ped_coordinates.col + k] > 0)
                continue; // Pedestrian in the cell.

            Cell neighbor_cell = {{ped_coordinates.lin + j, ped_coordinates.col + k}, cell_value};
            neighborhood.list[neighborhood.num_cells] = neighbor_cell;
            neighborhood.num_cells += 1;
        }
    }
    
    sort_cell_list(neighborhood);

    Cell destination_cell = {{-1,-1},-1};

    if(neighborhood.num_cells > 0)
    {
        int same_value = 1; // Number of cells with the same floor field value.

        for(int neighborhood_index = 1; neighborhood_index < neighborhood.num_cells; neighborhood_index++)
        {   
            if(neighborhood.list[neighborhood_index].value != neighborhood.list[0].value)
                break;

            same_value++;
        }

        int drawn_cell = rand() % same_value;

        if(pedestrian_position_grid[neighborhood.list[drawn_cell].coordinates.lin][neighborhood.list[drawn_cell].coordinates.col] == 0)
            destination_cell = neighborhood.list[drawn_cell]; 
            // Only if the sorted cell is not occupied.
    }

    free(neighborhood.list);

    return destination_cell;
}

/**
 * Sorts the given cell_list in ascending order.
 * 
 * @note Uses Insertion sort.
 * 
 * @param neighborhood A cell_list structure to be sorted.
*/
static void sort_cell_list(cell_list neighborhood)
{
    int h;

    if(neighborhood.num_cells > 1)
    {
        for(int i = 1; i < neighborhood.num_cells; i++)
        {
            Cell current = neighborhood.list[i];
            for(h = i - 1; h >= 0 && current.value < neighborhood.list[h].value; h--)
                neighborhood.list[h + 1] = neighborhood.list[h];
            
            neighborhood.list[h + 1] = current;
        }
    }
}