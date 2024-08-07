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
typedef uint16_t                U2;  	/* UINT */
typedef int32_t                 S4;     /* LONGINT */

typedef long long               S8;     /* 64 bit long */
typedef double                  F8;     /* DOUBLE */


#define EMPTY 0
#define ANN 1

// fann anns
#define ANNOPEN 1              // state flags
#define ANNCLOSED 0

#define MAXFANNNAME 256
#define MAXLINELEN 256

#define TRUE 1
#define FALSE 0


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
	F8 *inputs_nodef;
	F8 *outputs_nodef;
	S8 links_max;
	struct link *links;
	U1 fann_name[MAXFANNNAME];
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
S2 Cells_alloc_neurons_equal (struct cell *cells, S8 max_cells, S8 neurons);
S2 Cells_alloc_neurons (struct cell *cells, S8 cell, S8 neurons);
S2 Cells_dealloc_neurons (struct cell *cells, S8 max_cells);
S2 Cells_fann_read_ann (struct cell *cells, S8 cell, S8 node, U1 *filename, S8 inputs, S8 outputs, F8 *inputs_node, F8 *outputs_node, S8 layer, S8 init);
S2 Cells_fann_run_ann (struct cell *cells, S8 cell, S8 node);
S2 Cells_alloc_node_links (struct cell *cells, S8 cell, S8 node, S8 links);
S2 Cells_set_node_link (struct cell *cells, S8 cell, S8 node, S8 link, S8 link_node, S8 input, S8 output);
S2 Cells_fann_run_ann_go_links (struct cell *cells, S8 start_cell, S8 end_cell, S8 start_layer, S8 end_layer);
S2 Cells_fann_get_output (struct cell *cells, S8 cell, S8 node, S8 output, F8 *return_value);
S2 Cells_fann_do_update_ann (struct cell *cells, S8 cell, S8 node, F8 *inputs_node);
S2 Cells_fann_get_max_layer (struct cell *cells, S8 start_cell, S8 end_cell, S8 *max_layer_ret);
S2 Cells_fann_get_max_nodes (struct cell *cells, S8 cell, S8 *neurons_max_ret);
// file.c:
char *fgets_uni (char *str, int len, FILE *fptr);
S2 Cells_fann_save_cells (struct cell *cells, U1 *filename, S8 start_cell, S8 end_cell);
struct cell *Cells_fann_load_cells (U1 *filename);
// string.c:
size_t strlen_safe (const char *str, S8  maxlen);
S2 searchstr (U1 *str, U1 *srchstr, S2 start, S2 end, U1 case_sens);
void convtabs (U1 *str);
