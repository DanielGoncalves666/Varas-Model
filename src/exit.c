/* 
   File: saida.c
   Author: Daniel Gonçalves
   Date: 2023-10-15
   Description: This module contains declarations of structures to hold exit information and functions to create/expand exits, add exits to the exits set, and calculate the floor field.
*/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include"../headers/exit.h"
#include"../headers/grid.h"
#include"../headers/cli_processing.h"
#include"../headers/shared_resources.h"

#define DIAGONAL_VALUE 1.5

const double floor_field_rule[][3] = 
    {{DIAGONAL_VALUE, 1.0, DIAGONAL_VALUE},
     {     1.0,       0.0,       1.0     },
     {DIAGONAL_VALUE, 1.0, DIAGONAL_VALUE}};

Exits_Set exits_set = {NULL, NULL, 0};

static Exit create_new_exit(Location exit_coordinates);
static Function_Status calculate_exit_floor_field(Exit s);
static void initialize_exit_floor_field(Exit current_exit);
static bool is_exit_accessible(Exit s);

/**
 * Adds a new exit to the exits set.
 * 
 * @param exit_coordinates New exit coordinates.
 * @return Function_Status: FAILURE (0) or SUCCESS (1).
*/
Function_Status add_new_exit(Location exit_coordinates)
{
    Exit new_exit = create_new_exit(exit_coordinates);
    if(new_exit == NULL)
    {
        fprintf(stderr,"Failure on creating an exit at coordinates (%d,%d).\n",exit_coordinates.lin, exit_coordinates.col);
        return FAILURE;
    }

    exits_set.num_exits += 1;
    exits_set.list = realloc(exits_set.list, sizeof(Exit) * exits_set.num_exits);
    if(exits_set.list == NULL)
    {
        fprintf(stderr, "Failure in the realloc of the exits_set list.\n");
        return FAILURE;
    }    

    exits_set.list[exits_set.num_exits - 1] = new_exit;

    return SUCCESS;
}

/**
 * Expands an existing exit by adding a new cell based on the provided coordinates.
 * 
 * @param original_exit Exit to be expanded.
 * @param new_coordinates Coordinates of the cell to be added to the exit. 
 * @return Function_Status: FAILURE (0) or SUCCESS (1).
*/
Function_Status expand_exit(Exit original_exit, Location new_coordinates)
{
    if(is_within_grid_lines(new_coordinates.lin) && is_within_grid_columns(new_coordinates.col))
    {
        original_exit->width += 1;
        original_exit->coordinates = realloc(original_exit->coordinates, sizeof(Location) * original_exit->width);
        if(original_exit->coordinates == NULL)
            return FAILURE;

        original_exit->coordinates[original_exit->width - 1] = new_coordinates;

        return SUCCESS;
    }

    return FAILURE;
}


/**
 * Merge the floor_fields of all the exits in the exits_set. The result of this merge is stored at exits_set.final_floor_field.
 * 
 * @return Function_Status: FAILURE (0), SUCCESS (1) or INACCESSIBLE_EXIT(2).
*/
Function_Status calculate_final_floor_field()
{
    if(exits_set.num_exits <= 0 || exits_set.list == NULL)
    {
        fprintf(stderr,"The number of exits (%d) is invalid or the exits list is NULL.\n", exits_set.num_exits);
        return FAILURE;
    }

    for(int exit_index = 0; exit_index < exits_set.num_exits; exit_index++)
    {
        Function_Status returned_status = calculate_exit_floor_field(exits_set.list[exit_index]);
        if(returned_status != SUCCESS )
            return returned_status;
    }

    exits_set.final_floor_field = allocate_double_grid(cli_args.global_line_number, cli_args.global_column_number);
    if(exits_set.final_floor_field == NULL)
    {
        fprintf(stderr,"Failure during the allocation of the final_floor_field.\n");
        return FAILURE;
    }

    if( reset_double_grid(exits_set.final_floor_field, cli_args.global_line_number, cli_args.global_column_number) == FAILURE)
        return FAILURE;

    Double_Grid current_exit = exits_set.list[0]->floor_field;
    copy_double_grid(exits_set.final_floor_field, current_exit); // uses the first exit as the base for the merging
    
    for(int exit_index = 1; exit_index < exits_set.num_exits; exit_index++)
    {
        current_exit = exits_set.list[exit_index]->floor_field;
        for(int i = 0; i < cli_args.global_line_number; i++)
        {
            for(int h = 0; h < cli_args.global_column_number; h++)
            {
                if(exits_set.final_floor_field[i][h] > current_exit[i][h])
                    exits_set.final_floor_field[i][h] = current_exit[i][h];
            }
        }
    }

    return SUCCESS;
}

/**
 * Deallocate and reset the structures related to each exit and the exists set.
*/
void deallocate_exits()
{
    for(int exit_index = 0; exit_index < exits_set.num_exits; exit_index++)
    {
        Exit current = exits_set.list[exit_index];

        free(current->coordinates);
        deallocate_grid((void **) current->floor_field, cli_args.global_line_number);
        free(current);
    }

    free(exits_set.list);
    exits_set.list = NULL;

    deallocate_grid((void **) exits_set.final_floor_field, cli_args.global_line_number);
    exits_set.final_floor_field = NULL;

    exits_set.num_exits = 0;
}

/* ---------------- ---------------- ---------------- ---------------- ---------------- */
/* ---------------- ---------------- STATIC FUNCTIONS ---------------- ---------------- */
/* ---------------- ---------------- ---------------- ---------------- ---------------- */

/**
 * Creates a new exit structure based on the provided Location.
 * 
 * @param exit_coordinates New exit coordinates.
 * @return A NULL pointer, on error, or a Exit structure if the new exit is successfully created.
*/
static Exit create_new_exit(Location exit_coordinates)
{
    if(is_within_grid_lines(exit_coordinates.lin) && is_within_grid_columns(exit_coordinates.col))
    {
        Exit new_exit = malloc(sizeof(struct exit));
        if(new_exit != NULL)
        {
            new_exit->coordinates = malloc(sizeof(Location));
            if(new_exit->coordinates == NULL)
                return NULL;
            
            new_exit->coordinates[0] = exit_coordinates;
            new_exit->width = 1;

            new_exit->floor_field = allocate_double_grid(cli_args.global_line_number, cli_args.global_column_number);
        }

        return new_exit;
    }

    return NULL;
}

/**
 * Calculates the floor field for the given exit.
 * 
 * @param current_exit Exit for which the floor field will be calculated.
 * @return Function_Status: FAILURE (0), SUCCESS (1) or INACCESSIBLE_EXIT(2).
*/
static Function_Status calculate_exit_floor_field(Exit current_exit)
{
    if(current_exit == NULL)
    {
        fprintf(stderr, "A Null pointer was received in 'calculate_exit_floor_field' instead of a valid Exit.\n");
        return FAILURE;
    }

    initialize_exit_floor_field(current_exit);

    if(is_exit_accessible(current_exit) == false)
        return INACCESSIBLE_EXIT;

    Double_Grid floor_field = current_exit->floor_field;
    Double_Grid auxiliary_grid = allocate_double_grid(cli_args.global_line_number,cli_args.global_column_number);
    // stores the chances for the timestep t + 1
    
    if(auxiliary_grid == NULL)
        return FAILURE;

    copy_double_grid(auxiliary_grid, floor_field); // copies the base structure of the floor field

    bool has_changed;
    do
    {
        has_changed = false;
        for(int i = 0; i < cli_args.global_line_number; i++)
        {
            for(int h = 0; h < cli_args.global_column_number; h++)
            {
                double current_cell_value = floor_field[i][h];

                if(current_cell_value == WALL_VALUE || current_cell_value == 0.0) // floor field calculations occur only on cells with values
                    continue;

                for(int j = -1; j < 2; j++)
                {
                    if(is_within_grid_lines(i + j) == false)
                        continue;
                    
                    for(int k = -1; k < 2; k++)
                    {
                        if(is_within_grid_columns(h + k) == false)
                            continue;

                        if(floor_field[i + j][h + k] == WALL_VALUE || floor_field[i + j][h + k] == EXIT_VALUE)
                            continue;

                        if(j != 0 && k != 0)
                        {
                            if( is_diagonal_valid((Location){i,h},(Location){j,k},floor_field) == false)
                                continue;
                        }

                        double adjacent_cell_value = current_cell_value + floor_field_rule[1 + j][1 + k];
                        if(auxiliary_grid[i + j][h + k] == 0.0)
                        {    
                            auxiliary_grid[i + j][h + k] = adjacent_cell_value;
                            has_changed = true;
                        }
                        else if(adjacent_cell_value < auxiliary_grid[i + j][h + k])
                        {
                            auxiliary_grid[i + j][h + k] = adjacent_cell_value;
                            has_changed = true;
                        }
                    }
                }
            }
        }
        copy_double_grid(floor_field,auxiliary_grid); 
        // make sure floor_field now holds t + 1 timestep, allowing auxiliary_grid to hold t + 2 timestep.
    }
    while(has_changed);

    deallocate_grid((void **) auxiliary_grid, cli_args.global_line_number);

    return SUCCESS;
}

/**
 * Copies the structure (obstacles and walls) from the environment_only_grid to the floor field grid 
 * for the provided exit. Additionally, adds the exit cells to it.
 * 
 * @param current_exit The exit for which the floor field will be initialized.
*/
static void initialize_exit_floor_field(Exit current_exit)
{
    // Add walls and obstacles do the floor field. 
    for(int i = 0; i < cli_args.global_line_number; i++)
    {
        for(int h = 0; h < cli_args.global_column_number; h++)
        {
            double cell_value = environment_only_grid[i][h];
            if(cell_value == WALL_VALUE)
                current_exit->floor_field[i][h] = WALL_VALUE;
            else
                current_exit->floor_field[i][h] = 0.0;
        }
    }

    // Add the exit cells to the floor field
    for(int i = 0; i < current_exit->width; i++)
    {
        Location exit_cell = current_exit->coordinates[i];

        current_exit->floor_field[exit_cell.lin][exit_cell.col] = EXIT_VALUE;
    }
}

/**
 * Verify if the given exit is accessible.
 * 
 * @note A exit is accessible if there is, at least, one adjacent empty cell in the vertical or horizontal directions. 
 * 
 * @param current_exit The exit that will be verified.
 * @return bool, where True indicates tha the given exit is accessible, or False otherwise.
*/
static bool is_exit_accessible(Exit current_exit)
{
    if(current_exit == NULL)
        return false;

    for(int exit_cell_index = 0; exit_cell_index < current_exit->width; exit_cell_index++)
    {
        Location c = current_exit->coordinates[exit_cell_index];

        for(int j = -1; j < 2; j++)
        {
            if(! is_within_grid_lines(c.lin + j))
                continue;

            for(int k = -1; k < 2; k++)
            {
                if(! is_within_grid_columns(c.col + k))
                    continue;

                if(current_exit->floor_field[c.lin + j][c.col + k] == WALL_VALUE || current_exit->floor_field[c.lin + j][c.col + k] == EXIT_VALUE)
                    continue;

                if(j != 0 && k != 0)
                    continue; // diagonals

                return true;
            }
        }
    }

    return false;
}