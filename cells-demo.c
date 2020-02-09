/*
* This file cells-demo.c is part of Cells.
*
* (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2020
*
* Cells is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Cells is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Cells.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>

#include <cells.h>


int main (int ac, char *av[])
{
	struct cell *cells;
	struct cell *load_cells;
	
	S8 max_cells = 1;
	S8 max_layers = 1;
	S8 cell_neurons = 3;
	
	// set input values
	F8 node_xor_inputsf[2] = {0.0, 1.0};
	F8 node_xor_outputsf[1] = {0.0};
	
	F8 node_or_inputsf[2] = {1.0, 0.0};
	F8 node_or_outputsf[1] = {0.0};
	
	F8 node_and_inputsf[2] = {0.0, 0.0};
	F8 node_and_outputsf[1] = {0.0};
	
	F8 output[1] = {0.0};
	
	cells = (struct cell *) calloc (max_cells, sizeof (struct cell));
	if (cells == NULL)
	{
		printf ("ERROR: can't allocate %lli cells!\n", max_cells);
		exit (1);
	}
	
	alloc_neurons_equal (cells, max_cells, cell_neurons);
	
	// fann_read_ann (struct cell *cells, S8 cell, S8 node, U1 *filename, S8 inputs, S8 outputs, F8 *inputs_node, F8 *outputs_node, S8 layer, S8 init)
	
	// read 3 ANNs into cell/nodes
	fann_read_ann (cells, 0, 0, (U1 *) "fann/xor/xor_float.net", 2, 1, node_xor_inputsf, node_xor_outputsf, 0, 1);
	fann_read_ann (cells, 0, 1, (U1 *) "fann/or/or_float.net", 2, 1, node_or_inputsf, node_or_outputsf, 0, 1);
	fann_read_ann (cells, 0, 2, (U1 *) "fann/and/and_float.net", 2, 1, node_and_inputsf, node_and_outputsf, 1, 1);
	
	// allocate memory for links
	if (alloc_node_links (cells, 0, 0, 1) != 0)
	{
		printf ("ERROR: can't allocate memory for links!\n");
		dealloc_neurons (cells, max_cells);
		free (cells);
		
		exit (1);
	}
	
	if (alloc_node_links (cells, 0, 1, 1) != 0)
	{
		printf ("ERROR: can't allocate memory for links!\n");
		dealloc_neurons (cells, max_cells);
		free (cells);
		
		exit (1);
	}
	
	// set_node_link (struct cell *cells, S8 cell, S8 node, S8 link, S8 link_node, S8 input, S8 output)
	// set links: link output of node 0 and node 1 into inputs of node 2 (AND):
	set_node_link (cells, 0, 0, 0, 2, 0, 0);
	set_node_link (cells, 0, 1, 0, 2, 1, 0);
	
	// finally run ANNs on the layers 0 and 1:
	// S2 fann_run_ann_go_links (struct cell *cells, S8 start_cell, S8 end_cell, S8 start_layer, S8 end_layer)
	fann_run_ann_go_links (cells, 0, 0, 0, max_layers);
	
	fann_get_output (cells, 0, 0, 0, output);
	printf ("run ann XOR layer 0: cell: 0, node: 0, output 0: %lf\n", output[0]);
	
	fann_get_output (cells, 0, 1, 0, output);
	printf ("run ann OR layer  0: cell: 0, node: 1, output 0: %lf\n", output[0]);
	
	fann_get_output (cells, 0, 2, 0, output);
	printf ("run ann AND layer 1: cell: 0, node: 2, output 0: %lf\n", output[0]);
	
	printf ("\n\nchanging OR: to 0.0, 0.0...\n");
	node_or_inputsf[0] = 0.0;
	node_or_inputsf[1] = 0.0;
	
	fann_do_update_ann (cells, 0, 1, node_or_inputsf);
	
	// finally rerun ANNs on the layers 0 and 1:
	fann_run_ann_go_links (cells, 0, 0, 0, max_layers);
	
	fann_get_output (cells, 0, 2, 0, output);
	printf ("rerun ann AND layer 1: cell: 0, node: 2, output 0: %lf\n", output[0]);
	
	printf ("saving cells...\n");
	// S2 fann_save_cells (struct cell *cells, U1 *filename, S8 start_cell, S8 end_cell)
	if (fann_save_cells (cells, (U1 *) "cell-demo.cells", 0, 0) != 0)
	{
		printf ("ERROR saving cells!\n");
		dealloc_neurons (cells, max_cells);
		free (cells);
		exit (1);
	}
	printf ("OK!\n");
	
	printf ("loading cells file into 'load_cells' ...\n");
	load_cells = fann_load_cells ((U1 *) "cell-demo.cells");
	if (load_cells == NULL)
	{
		printf ("ERROR: can't load cells file!\n");
		dealloc_neurons (cells, max_cells);
		free (cells);
		exit (1);
	}
	printf ("OK!\n");
	
	printf ("\n\nrunning 'load_cells' ANNs...\n");
	
	// read 3 ANNs into cell/nodes: load_cell
	fann_read_ann (load_cells, 0, 0, (U1 *) "", 0, 0, node_xor_inputsf, node_xor_outputsf, 0, 0);
	fann_read_ann (load_cells, 0, 1, (U1 *) "", 0, 0, node_or_inputsf, node_or_outputsf, 0, 0);
	fann_read_ann (load_cells, 0, 2, (U1 *) "", 0, 0, node_and_inputsf, node_and_outputsf, 1, 0);
	
	// run ANNs in load_cell:
	fann_get_max_layer (load_cells, 0, 0, &max_layers);
	fann_run_ann_go_links (load_cells, 0, 0, 0, max_layers);
	
	fann_get_output (load_cells, 0, 0, 0, output);
	printf ("\nload_cells: run ann XOR layer 0: cell: 0, node: 0, output 0: %lf\n", output[0]);
	
	fann_get_output (load_cells, 0, 1, 0, output);
	printf ("load_cells: run ann OR layer  0: cell: 0, node: 1, output 0: %lf\n", output[0]);
	
	fann_get_output (load_cells, 0, 2, 0, output);
	printf ("load_cells: run ann AND layer 1: cell: 0, node: 2, output 0: %lf\n", output[0]);
	
	dealloc_neurons (cells, max_cells);
	free (cells);
	
	if (load_cells) 
	{
		dealloc_neurons (load_cells, max_cells);
		free (load_cells);
	}
	exit (0);
}
