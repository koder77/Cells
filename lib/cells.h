/*
* This file cells.h is part of Cells.
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


#include <floatfann.h>


typedef unsigned char           U1;		/* UBYTE   */
typedef int16_t                 S2;     /* INT     */
typedef uint16_t                U2;     /* UINT    */
typedef int32_t                 S4;     /* LONGINT */

typedef long long               S8;     /* 64 bit long */
typedef double                  F8;     /* DOUBLE */


#define EMPTY 0
#define ANN 1

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
	struct fann *ann;			// fann neural network
	U1 fann_state;
	S8 layer;
};

struct cell
{
	S8 neurons_max;
	struct neuron *neurons;
};

// protos
S2 alloc_neurons_equal (struct cell *cells, S8 max_cells, S8 neurons);
S2 alloc_neurons (struct cell *cells, S8 cell, S8 neurons);
void dealloc_neurons (struct cell *cells, S8 max_cells);
S2 fann_read_ann (struct cell *cells, S8 cell, S8 node, U1 *filename, S8 inputs, S8 outputs, F8 *inputs_node, F8 *outputs_node, S8 layer);
S2 fann_run_ann (struct cell *cells, S8 cell, S8 node);
S2 alloc_node_links (struct cell *cells, S8 cell, S8 node, S8 links);
S2 set_node_link (struct cell *cells, S8 cell, S8 node, S8 link, S8 link_node, S8 input, S8 output);
S2 fann_run_ann_go_links (struct cell *cells, S8 max_cells, S8 max_layer);
F8 fann_get_output (struct cell *cells, S8 cell, S8 node, S8 output);
