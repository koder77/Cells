/*
 * This file cells.c is part of Cells.
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
#include <string.h>
#include <inttypes.h>

#include "cells.h"


S2 alloc_neurons_equal (struct cell *cells, S8 max_cells, S8 neurons)
{
	S8 i, n;
	
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("alloc_neurons_equal: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	for (i = 0; i < max_cells; i++)
	{
		cells[i].neurons_max = neurons;
		cells[i].neurons = (struct neuron *) calloc (neurons, sizeof (struct neuron));
		if (cells[i].neurons == NULL)
		{
			printf ("alloc_nerons_equal: ERROR: can't allocate %lli neurons in cell %lli!\n", neurons, i);
			return (1);
		}
		
		for (n = 0; n < neurons; n++)
		{
			cells[i].neurons[n].type = EMPTY;
			cells[i].neurons[n].ann = NULL;
			cells[i].neurons[n].fann_state = ANNCLOSED;
			cells[i].neurons[n].layer = 0;
			cells[i].neurons[n].inputs = 0;
			cells[i].neurons[n].outputs = 0;
			cells[i].neurons[n].links_max = 0;
			cells[i].neurons[n].links = NULL;
			cells[i].neurons[n].inputs_nodef = NULL;
			cells[i].neurons[n].outputs_nodef = NULL;
		}
	}
	return (0);
}

S2 alloc_neurons (struct cell *cells, S8 cell, S8 neurons)
{
	S8 n;
	
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("alloc_neurons: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	cells[cell].neurons_max = neurons;
	cells[cell].neurons = (struct neuron *) calloc (neurons, sizeof (struct neuron));
	if (cells[cell].neurons == NULL)
	{
		printf ("alloc_neurons: can't allocate %lli neurons in cell %lli!\n", neurons, cell);
		return (1);
	}
	
	for (n = 0; n < neurons; n++)
	{
		cells[cell].neurons[n].type = EMPTY;
		cells[cell].neurons[n].ann = NULL;
		cells[cell].neurons[n].fann_state = ANNCLOSED;
		cells[cell].neurons[n].layer = 0;
		cells[cell].neurons[n].inputs = 0;
		cells[cell].neurons[n].outputs = 0;
		cells[cell].neurons[n].links_max = 0;
		cells[cell].neurons[n].links = NULL;
		cells[cell].neurons[n].inputs_nodef = NULL;
		cells[cell].neurons[n].outputs_nodef = NULL;
	}
	return (0);
}

S2 dealloc_neurons (struct cell *cells, S8 max_cells)
{
	S8 i;
	S8 n;
	
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("dealloc_neurons: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	for (i = 0; i < max_cells; i++)
	{
		for (n = 0; n < cells[i].neurons_max; n++)
		{
			if (cells[i].neurons[n].inputs_nodef) free (cells[i].neurons[n].inputs_nodef);
			if (cells[i].neurons[n].outputs_nodef) free (cells[i].neurons[n].outputs_nodef);
			if (cells[i].neurons[n].links) free (cells[i].neurons[n].links);
			if (cells[i].neurons[n].fann_state == ANNOPEN) fann_destroy (cells[i].neurons[n].ann);
		}
		free (cells[i].neurons);
	}
	return (0);
}

S2 fann_do_update_ann (struct cell *cells, S8 cell, S8 node, F8 *inputs_node)
{
	S8 n;
	
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("fann_do_update_ann: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	// safety check:
	if (node < 0 || node >= cells[cell].neurons_max)
	{
		printf ("fann_do_update_ann: error: node out of range!\n");
		return (1);
	}
	
	for (n = 0; n < cells[cell].neurons[node].inputs; n++)
	{
		cells[cell].neurons[node].inputs_nodef[n] = inputs_node[n];
	}
	
	return (0);
}

S2 fann_read_ann (struct cell *cells, S8 cell, S8 node, U1 *filename, S8 inputs, S8 outputs, F8 *inputs_node, F8 *outputs_node, S8 layer, S8 init)
{
	// do a new ANN init if "init" is set to one!!
	
	S8 n;
	S8 filename_len;
	
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("fann_read_ann: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	// safety check:
	if (node < 0 || node >= cells[cell].neurons_max)
	{
		printf ("fann_read_ann: error: node out of range!\n");
		return (1);
	}
	
	filename_len = strlen_safe ((const char *) filename, MAXFANNNAME - 1);
	if (filename_len + 1 > MAXFANNNAME)
 	{
		printf ("fann_read_ann: error fileaname too long!\n");
		return (1);
	}
	
	if (filename_len > 0)
	{
		strcpy ((char *) cells[cell].neurons[node].fann_name, (const char *) filename);
	}
	
	// printf ("fann_read_ann: ann filename: '%s'\n", cells[cell].neurons[node].fann_name);
	
	// printf ("fann_read_ann: cell: %lli, node: %lli\n", cell, node);
	cells[cell].neurons[node].type = ANN;
	cells[cell].neurons[node].ann = (struct fann *) fann_create_from_file ((const char*) cells[cell].neurons[node].fann_name);
	cells[cell].neurons[node].fann_state = ANNOPEN;
	
	if (init == 1)
 	{
		cells[cell].neurons[node].layer = layer;
		cells[cell].neurons[node].inputs = inputs;
		cells[cell].neurons[node].outputs = outputs;
		cells[cell].neurons[node].links_max = 0;
		cells[cell].neurons[node].links = NULL;
	}
	else
	{
		inputs = cells[cell].neurons[node].inputs;
		outputs = cells[cell].neurons[node].outputs;
	}
	
	// allocate and copy neurons inputs/outputs;
	
	cells[cell].neurons[node].inputs_nodef = calloc (inputs, sizeof (F8));
	if (cells[cell].neurons[node].inputs_nodef == NULL)
	{
		printf ("ERROR: can't allocate inputs nodes!\n");
		return (1);
	}
	for (n = 0; n < inputs; n++)
	{
		cells[cell].neurons[node].inputs_nodef[n] = inputs_node[n];
	}
	
	cells[cell].neurons[node].outputs_nodef = calloc (outputs, sizeof (F8));
	if (cells[cell].neurons[node].outputs_nodef == NULL)
	{
		printf ("ERROR: can't allocate outputs nodes!\n");
		return (1);
	}
	for (n = 0; n < outputs; n++)
	{
		cells[cell].neurons[node].outputs_nodef[n] = outputs_node[n];
	}
	return (0);
}

S2 fann_run_ann (struct cell *cells, S8 cell, S8 node)
{
	S8 i;
	
	fann_type *input_f;
	fann_type *output_f;
	
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("fann_run_ann: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	// safety check:
	if (node < 0 || node >= cells[cell].neurons_max)
	{
		printf ("fann_run_ann: error: node out of range!\n");
		return (1);
	}
	
	// printf ("fann_run_ann: cell: %lli, node: %lli\n", cell, node);
	
	input_f = calloc (cells[cell].neurons[node].inputs, sizeof (fann_type));
	if (input_f == NULL)
	{
		printf ("run ann: out of memory, allocating input!\n");
		return (1);
	}
	
	for (i = 0; i < cells[cell].neurons[node].inputs; i++)
	{
		input_f[i] = cells[cell].neurons[node].inputs_nodef[i];
	}
	
	output_f = fann_run (cells[cell].neurons[node].ann, input_f);
	
	for (i = 0; i < cells[cell].neurons[node].outputs; i++)
	{
		cells[cell].neurons[node].outputs_nodef[i] = output_f[i];
	}
	
	for (i = 0; i < cells[cell].neurons[node].outputs; i++)
	{
		// printf ("run ann: cell: %lli, node: %lli, output %lli: %lf\n", cell, node, i, cells[cell].neurons[node].outputs_nodef[i]);
	}
	
	free (input_f);
	
	return (0);
}

S2 fann_get_output (struct cell *cells, S8 cell, S8 node, S8 output, F8 *return_value)
{
	// safety check:
	
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("fann_get_output: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	if (node < 0 || node >= cells[cell].neurons_max)
	{
		printf ("fann_get_output: error: node out of range!\n");
		return (1);
	}
	
	if (output >= cells[cell].neurons[node].outputs)
	{
		printf ("fann_get_output: error: output out of range!\n");
		return (1);
	}
	
	return_value[0] = cells[cell].neurons[node].outputs_nodef[output];
	return (0);
}

S2 alloc_node_links (struct cell *cells, S8 cell, S8 node, S8 links)
{
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("alloc_node_links: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	cells[cell].neurons[node].links = calloc (links, sizeof (struct link));
	if (cells[cell].neurons[node].links == NULL)
	{
		printf ("alloc_node_links: out of memory, allocating links!\n");
		return (1);
	}
	cells[cell].neurons[node].links_max = links;
	return (0);
}

S2 set_node_link (struct cell *cells, S8 cell, S8 node, S8 link, S8 link_node, S8 input, S8 output)
{
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("set_node_link: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	// safety check:
	if (node < 0 || node >= cells[cell].neurons_max)
	{
		printf ("set_node_link: error: node out of range!\n");
		return (1);
	}
	
	if (cells[cell].neurons[node].links == NULL)
	{
		printf ("set_node_links: error: no links allocated: cell: %lli, node: %lli!\n", cell, node);
		return (1);
	}
	
	if (link >= cells[cell].neurons[node].links_max || link < 0)
	{
		printf ("set_node_link: error: link overflow!\n");
		return (1);
	}
	
	// inputs/outputs sense check:
	if (input >= cells[cell].neurons[link_node].inputs || input < 0)
	{
		printf ("set_node_link: error: link input overflow!\n");
		return (1);
	}
	
	if (output >= cells[cell].neurons[node].outputs || output < 0)
	{
		printf ("set_node_link: error: link output overflow!\n");
		return (1);
	}
	
	if (link < cells[cell].neurons[node].links_max)
	{
		cells[cell].neurons[node].links[link].node = link_node;
		cells[cell].neurons[node].links[link].node_input = input;
		cells[cell].neurons[node].links[link].node_output = output;
		return (0);
	}
	else
	{
		printf ("set_node_links: error: no links overflow: cell: %lli, node: %lli!\n", cell, node);
		return (1);
	}
}

S2 fann_get_max_layer (struct cell *cells, S8 start_cell, S8 end_cell, S8 *max_layer_ret)
{
	S8 i, n;
	S8 max_layer = 0;
	
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("fann_get_max_layer: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	for (i = start_cell; i <= end_cell; i++)
	{
		for (n = 0; n < cells[i].neurons_max; n++)
		{
			if (cells[i].neurons[n].layer > max_layer)
			{
				max_layer = cells[i].neurons[n].layer;
			}
		}
	}
	max_layer_ret = &max_layer;
	return (0);
}

S2  fann_get_max_nodes (struct cell *cells, S8 cell, S8 *neurons_max_ret)
{
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("fann_get_output: ERROR: cells structure not allocated!\n");
		return (1);
	}
	neurons_max_ret = &cells[cell].neurons_max;
	return (0);
}

S2 fann_run_ann_go_links (struct cell *cells, S8 start_cell, S8 end_cell, S8 start_layer, S8 end_layer)
{
	S8 i, j;
	S8 n;
	S8 linked_neuron, node_input, node_output;
	S8 layer;

	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("fann_run_ann_go_links: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	for (i = start_cell; i <= end_cell; i++)
	{
		for (layer = start_layer; layer <= end_layer; layer++)
		{
			for (n = 0; n < cells[i].neurons_max; n++)
			{
				if (cells[i].neurons[n].layer == layer)
				{
					// cell is in current layer, do run 
				
					if (fann_run_ann (cells, i, n) != 0)
					{
						printf ("run_ann_go_links: error running ANN!\n");
						return (1);
					}
				
					// check for links from this cell
					if (cells[i].neurons[n].links_max > 0)
					{
						for (j = 0; j < cells[i].neurons[n].links_max; j++)
						{
							linked_neuron = cells[i].neurons[n].links[j].node; // node to we are linked
							node_input = cells[i].neurons[n].links[j].node_input; // input of linked node
							node_output = cells[i].neurons[n].links[j].node_output; // output of this node, linked to input of next layer node
						
							cells[i].neurons[linked_neuron].inputs_nodef[node_input] = cells[i].neurons[n].outputs_nodef[node_output];
						}
					}
				}
			}
		}
	}
	return (0);
}
