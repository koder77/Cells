/*
* This file main.c is part of Cells.
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

#include <floatfann.h>


typedef unsigned char           U1;		/* UBYTE   */
typedef int16_t                 S2;     /* INT     */
typedef uint16_t                U2;     /* UINT    */
typedef int32_t                 S4;     /* LONGINT */

typedef long long               S8;     /* 64 bit long */
typedef double                  F8;     /* DOUBLE */


#define INTEGER 0
#define FLOAT 1
#define ANN 2

// fann anns
#define ANNOPEN 1              // state flags
#define ANNCLOSED 0

struct link
{
	S8 node;
	S8 node_input;
	S8 node_output;
};

struct neuron
{
	U1 type;
	S8 inputs;
	S8 outputs;
	F8 inputsf;
	F8 outputsf;
	S8 *inputs_node;
	S8 *outputs_node;
	F8 *inputs_nodef;
	F8 *outputs_nodef;
	S8 links_max;
	struct link *links;
	F8 strength;
	struct fann *ann;			// fann neural network
	U1 fann_state;
	S8 layer;
};

struct cell
{
	S8 neurons_max;
	struct neuron *neurons;
};


S2 alloc_neurons_equal (struct cell *cells, S8 max_cells, S8 neurons)
{
	S8 i;
	
	for (i = 0; i < max_cells; i++)
	{
		cells[i].neurons_max = neurons;
		cells[i].neurons = (struct neuron *) calloc (neurons, sizeof (struct neuron));
		if (cells[i].neurons == NULL)
		{
			printf ("ERROR: can't allocate %lli neurons in cell %lli!\n", neurons, i);
			free (cells);
			exit (1);
		}
	}
	return (0);
}

S2 alloc_neurons (struct cell *cells, S8 cell, S8 neurons)
{
	cells[cell].neurons_max = neurons;
	cells[cell].neurons = (struct neuron *) calloc (neurons, sizeof (struct neuron));
	if (cells[cell].neurons == NULL)
	{
		printf ("ERROR: can't allocate %lli neurons in cell %lli!\n", neurons, cell);
		free (cells);
		exit (1);
	}
	return (0);
}

void dealloc_neurons (struct cell *cells, S8 max_cells)
{
	S8 i;
	S8 n;
	
	for (i = 0; i < max_cells; i++)
	{
		for (n = 0; n < cells[i].neurons_max; n++)
		{
			if (cells[i].neurons[n].inputs_node) free (cells[i].neurons[n].inputs_node);
			if (cells[i].neurons[n].outputs_node) free (cells[i].neurons[n].outputs_node);
			if (cells[i].neurons[n].inputs_nodef) free (cells[i].neurons[n].inputs_nodef);
			if (cells[i].neurons[n].outputs_nodef) free (cells[i].neurons[n].outputs_nodef);
			if (cells[i].neurons[n].links) free (cells[i].neurons[n].links);
			if (cells[i].neurons[n].fann_state == ANNOPEN) fann_destroy (cells[i].neurons[n].ann);
		}
		free (cells[i].neurons);
	}
}

S2 fann_read_ann (struct cell *cells, S8 cell, S8 node, U1 *filename, S8 inputs, S8 outputs, F8 *inputs_node, F8 *outputs_node, F8 strength, S8 layer)
{
	S8 n;
	
	printf ("fann_read_ann: cell: %lli, node: %lli\n", cell, node);
	cells[cell].neurons[node].type = ANN;
	cells[cell].neurons[node].ann = (struct fann *) fann_create_from_file ((const char*) filename);
	cells[cell].neurons[node].fann_state = ANNOPEN;
	cells[cell].neurons[node].layer = layer;
	cells[cell].neurons[node].inputs = inputs;
	cells[cell].neurons[node].outputs = outputs;
	cells[cell].neurons[node].links_max = 0;
	cells[cell].neurons[node].links = NULL;
	
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
		printf ("run ann: cell: %lli, node: %lli, output %lli: %lf\n", cell, node, i, cells[cell].neurons[node].outputs_nodef[i]);
	}
	
	free (input_f);
	
	return (0);
}

S2 alloc_node_links (struct cell *cells, S8 cell, S8 node, S8 links)
{
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
	if (cells[cell].neurons[node].links == NULL)
	{
		printf ("set_node_links: error: no links allocated: cell: %lli, node: %lli!\n", cell, node);
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

S2 fann_run_ann_go_links (struct cell *cells, S8 max_cells, S8 max_layer)
{
	S8 i, j;
	S8 n;
	S8 linked_neuron, node_input, node_output;
	S8 layer;
	
	// fann_type *links_input_f;
	// fann_type *links_output_f;
	
	for (i = 0; i < max_cells; i++)
	{
		for (layer = 0; layer <= max_layer; layer++)
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


int main (int ac, char *av[])
{
	struct cell *cells;
	
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
	
	cells = (struct cell *) calloc (max_cells, sizeof (struct cell));
	if (cells == NULL)
	{
		printf ("ERROR: can't allocate %lli cells!\n", max_cells);
		exit (1);
	}
	
	alloc_neurons_equal (cells, max_cells, cell_neurons);
	
	// fann_read_ann (struct cell *cells, S8 cell, S8 node, U1 *filename, S8 inputs, S8 outputs, F8 *inputs_node, F8 *outputs_node, F8 strength, S8 layer))
	
	// read 3 ANNs into cell/nodes
	fann_read_ann (cells, 0, 0, (U1 *) "fann/xor/xor_float.net", 2, 1, node_xor_inputsf, node_xor_outputsf, 1.0, 0);
	fann_read_ann (cells, 0, 1, (U1 *) "fann/or/or_float.net", 2, 1, node_or_inputsf, node_or_outputsf, 1.0, 0);
	fann_read_ann (cells, 0, 2, (U1 *) "fann/and/and_float.net", 2, 1, node_and_inputsf, node_and_outputsf, 1.0, 1);
	
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
	fann_run_ann_go_links (cells, max_cells, max_layers);
	
	dealloc_neurons (cells, max_cells);
	free (cells);
	
	exit (0);
}
