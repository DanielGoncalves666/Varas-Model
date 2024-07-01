#!/bin/bash

# Prints the provided text in the given color.
# $1 Sequence code of the chosen color.
# $2 The string to be printed.
print_in_color()
{
    echo -e "$1$2\033[0m"
}

clear
print_in_color "\033[0;32m" "Varas Experiments Script!"

dir_name="varas_fig_3"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 3 Experiment."
./varas.sh -m5 -l16 -c20 -avaras_section_3.txt -o$dir_name/varas_fig_3.txt -O1 -p50 --allow-x-movement

dir_name="varas_fig_4"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 4 Experiment."
./varas.sh -m5 -l16 -c20 -avaras_section_3.txt -o$dir_name/varas_fig_4.txt -O1 -p50 --diagonal=1 --allow-x-movement

dir_name="varas_fig_5"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 5 Experiment."
./varas.sh -m5 -l16 -c20 -avaras_section_3.txt -o$dir_name/varas_fig_5.txt -O1 -p50 --diagonal=2 --allow-x-movement

dir_name="varas_fig_6"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 6 Experiment."
for num_ped in 50 100 150 200; do
    ./varas.sh -m5 -l16 -c20 -avaras_door_width.txt -o$dir_name/varas_fig_6_${num_ped}ped.txt -O2 -p$num_ped -s20 --allow-x-movement
done

dir_name="varas_fig_7"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 7 Experiment."
for num_ped in 50 100 150 200; do
    ./varas.sh -m5 -l16 -c20 -avaras_door_width.txt -o$dir_name/varas_fig_7_${num_ped}ped.txt -O2 -p$num_ped -s20 --varas-fig7 --allow-x-movement
done

dir_name="varas_fig_12"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 12 Experiment."
./varas.sh -m3 -evaras_classroom_with_obstacles.txt -avaras_optimal_location.txt -o$dir_name/varas_fig_12_with_obstacles.txt -O2 -s10  --allow-x-movement
./varas.sh -m3 -evaras_classroom_without_obstacles.txt -avaras_optimal_location.txt -o$dir_name/varas_fig_12_without_obstacles.txt -O2 -s10  --allow-x-movement

dir_name="varas_fig_13"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 13 Experiment."
./varas.sh -m3 -evaras_classroom_with_obstacles.txt -avaras_door_width.txt -o$dir_name/varas_fig_13_with_obstacles.txt -O2 -s10  --allow-x-movement
./varas.sh -m3 -evaras_classroom_without_obstacles.txt -avaras_door_width.txt -o$dir_name/varas_fig_13_without_obstacles.txt -O2 -s10  --allow-x-movement

dir_name="varas_fig_14"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 14 Experiment."
./varas.sh -m3 -evaras_classroom_with_obstacles.txt -avaras_double_doors.txt -o$dir_name/varas_fig_14.txt -O2 -s10  --allow-x-movement

dir_name="varas_fig_15"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 15 Experiment."
./varas.sh -m3 -evaras_classroom_with_obstacles.txt -avaras_optimal_location-door_combination.txt -o$dir_name/varas_fig_15.txt -O2 -s10  --allow-x-movement

dir_name="varas_fig_17a"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 17a Experiment."
./varas.sh -m3 -evaras_classroom_2_with_obstacles.txt -avaras_optimal_location_2.txt -o$dir_name/varas_fig_17a_with_obstacles.txt -O2 -s10  --allow-x-movement
./varas.sh -m3 -evaras_classroom_2_without_obstacles.txt -avaras_optimal_location_2.txt -o$dir_name/varas_fig_17a_without_obstacles.txt -O2 -s10  --allow-x-movement

dir_name="varas_fig_17b"
mkdir -p output/$dir_name
print_in_color "\033[0;34m" "Varas figure 17b Experiment."
./varas.sh -m3 -evaras_classroom_2_with_obstacles.txt -avaras_double_doors.txt -o$dir_name/varas_fig_17b.txt -O2 -s10  --allow-x-movement