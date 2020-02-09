/*
 * This file file.c is part of Cells.
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

// line input from file
char *fgets_uni (char *str, int len, FILE *fptr)
{
	int ch, nextch;
	int i = 0, eol = FALSE;
	char *ret;
	
	ch = fgetc (fptr);
	if (feof (fptr))
	{
		return (NULL);
	}
	while (! feof (fptr) || i == len - 2)
	{
		switch (ch)
		{
			case '\r':
				/* check for '\r\n\' */
				
				nextch = fgetc (fptr);
				if (! feof (fptr))
				{
					if (nextch != '\n')
					{
						ungetc (nextch, fptr);
					}
				}
				str[i] = '\n';
				i++; 
				eol = TRUE;
				break;
				
			case '\n':
				/* check for '\n\r\' */
				
				nextch = fgetc (fptr);
				if (! feof (fptr))
				{
					if (nextch != '\r')
					{
						ungetc (nextch, fptr);
					}
				}
				str[i] = '\n';
				i++; 
				eol = TRUE;
				break;
				
			default:
				str[i] = ch;
				i++;
				
				break;
		}
		
		if (eol)
		{
			break;
		}
		
		ch = fgetc (fptr);
	}
	
	if (feof (fptr))
	{
		//        str[i] = '\n';
		//        i++;
		str[i] = '\0';
	}
	else
	{
		str[i] = '\0';
	}
	
	ret = str;
	return (ret);
}

void str_cut_newline (U1 *str)
{
	S8 str_len;
	
	str_len = strlen_safe ((const char *) str, MAXLINELEN - 1);
	str[str_len - 1] = '\0';
}


S2 fann_save_cells (struct cell *cells, U1 *filename, S8 start_cell, S8 end_cell)
{
	S8 i, l;
	S8 n;
	
	FILE *fptr;
	
	// do sane check:
	if (cells == NULL)
	{
		// error: not allocated memory
		printf ("fann_save_cells: ERROR: cells structure not allocated!\n");
		return (1);
	}
	
	fptr = fopen ((const char *) filename, "w");
	if (fptr == NULL)
	{
		printf ("fann_save_cells: error opening file: %s\n", filename);
		return (1);
	}
	
	// save header
	if (fprintf (fptr, "%s\n", "cells V0.1-save") < 0)
	{
		printf ("fann_save_cells: error saving header to file: %s\n", filename);
		fclose (fptr);
		return (1);
	}
	
	// save number of cells
	if (fprintf (fptr, "cells = %lli\n", end_cell + 1 - start_cell) < 0)
	{
		printf ("fann_save_cells: error saving nunber of cells to file: %s\n", filename);
		fclose (fptr);
		return (1);
	}
	
	// save cell structure
	for (i = start_cell; i <= end_cell; i++)
	{
		// save cell number
		if (fprintf (fptr, "cell = %lli\n", start_cell) < 0)
		{
			printf ("fann_save_cells: error saving cells number to file: %s\n", filename);
			fclose (fptr);
			return (1);
		}
		
		// save neurons max number
		if (fprintf (fptr, "neurons = %lli\n", cells[i].neurons_max) < 0)
		{
			printf ("fann_save_cells: error saving neurons number to file: %s\n", filename);
			fclose (fptr);
			return (1);
		}
		
		for (n = 0; n < cells[i].neurons_max; n++)
		{
			// save type
			if (fprintf (fptr, "type = %i\n", cells[i].neurons[n].type) < 0)
			{
				printf ("fann_save_cells: error saving fann type to file: %s\n", filename);
				fclose (fptr);
				return (1);
			}
			
			// save inputs
			if (fprintf (fptr, "inputs = %lli\n", cells[i].neurons[n].inputs) < 0)
			{
				printf ("fann_save_cells: error saving fann inputs to file: %s\n", filename);
				fclose (fptr);
				return (1);
			}
			
			// save outputs
			if (fprintf (fptr, "outputs = %lli\n", cells[i].neurons[n].outputs) < 0)
			{
				printf ("fann_save_cells: error saving fann outputs to file: %s\n", filename);
				fclose (fptr);
				return (1);
			}
			
			// save links_max
			if (fprintf (fptr, "links_max = %lli\n", cells[i].neurons[n].links_max) < 0)
			{
				printf ("fann_save_cells: error saving links_max to file: %s\n", filename);
				fclose (fptr);
				return (1);
			}
			
			// save layer
			if (fprintf (fptr, "layer = %lli\n", cells[i].neurons[n].layer) < 0)
			{
				printf ("fann_save_cells: error saving fann layer to file: %s\n", filename);
				fclose (fptr);
				return (1);
			}
			
			// save fann name
			if (fprintf (fptr, "fann_name = %s\n", cells[i].neurons[n].fann_name) < 0)
			{
				printf ("fann_save_cells: error saving fann name to file: %s\n", filename);
				fclose (fptr);
				return (1);
			}
			
			// save links 
			if (cells[i].neurons[n].links_max > 0)
   			{
				if (fprintf (fptr, "links_start\n") < 0)
				{
					printf ("fann_save_cells: error saving links header to file: %s\n", filename);
					fclose (fptr);
					return (1);
				}
			}
			
			for (l = 0; l < cells[i].neurons[n].links_max; l++)
			{
				// save link node
				if (fprintf (fptr, "link_node = %lli\n", cells[i].neurons[n].links[l].node) < 0)
				{
					printf ("fann_save_cells: error saving node link to file: %s\n", filename);
					fclose (fptr);
					return (1);
				}
				
				// save node input 
				if (fprintf (fptr, "link_node_input = %lli\n", cells[i].neurons[n].links[l].node_input) < 0)
				{
					printf ("fann_save_cells: error saving node link input to file: %s\n", filename);
					fclose (fptr);
					return (1);
				}
				
				// save node output 
				if (fprintf (fptr, "link_node_output = %lli\n", cells[i].neurons[n].links[l].node_output) < 0)
				{
					printf ("fann_save_cells: error saving node link output to file: %s\n", filename);
					fclose (fptr);
					return (1);
				}
			}
			
			if (cells[i].neurons[n].links_max > 0)
			{
				// save links end
				if (fprintf (fptr, "links_end\n") < 0)
				{
					printf ("fann_save_cells: error saving links header end to file: %s\n", filename);
					fclose (fptr);
					return (1);
				}
			}
			
			// save node end
			if (fprintf (fptr, "node_end\n") < 0)
			{
				printf ("fann_save_cells: error saving node end to file: %s\n", filename);
				fclose (fptr);
				return (1);
			}
		}
		
	}
	
	if (fprintf (fptr, "EOF\n") < 0)
	{
		printf ("fann_save_cells: error saving EOF end to file: %s\n", filename);
		return (1);
	}
	
	fclose (fptr);
	return (0);
}

S2 get_number (U1 *buf, S8 *number)
{
	S8 pos, i, j, str_len;
	U1 numstr[MAXLINELEN];
	
	str_len = strlen_safe ((const char *) buf, MAXLINELEN - 1);
	
	pos = searchstr (buf, (U1 *) "=", 0, 0, 1);
	if (pos > -1)
	{
		j = 0;
		for (i = pos + 1; i < str_len; i++)
		{
			numstr[j] = buf[i];
			j++;
		}
		numstr[j] = '\0';
		*number = atoi ((const char *) numstr);
		return (0);
	}
	number = 0;
	return (1);
}

S2 get_string (U1 *buf, U1 *string)
{
	S8 pos, i, j, str_len;
	
	str_len = strlen_safe ((const char *) buf, MAXLINELEN - 1);
	
	pos = searchstr (buf, (U1 *) "=", 0, 0, 1);
	if (pos > -1)
	{
		j = 0;
		for (i = pos + 2; i < str_len; i++)
		{
			string[j] = buf[i];
			j++;
		}
		string[j] = '\0';
		return (0);
	}
	string = NULL;
	return (1);
}

struct cell *fann_load_cells (U1 *filename)
{
	/* The ANNs must be loaded in fann_read_ann() from their filenames as set
	 * in this cells structure!
	 *
	 */ 
	
	struct cell *cells;
	S8 l;
	S8 n;
	S8 val;
	S8 max_cells;
	S8 curr_cell;
	U1 buf[MAXLINELEN];
	U1 fann_name[MAXLINELEN];
	U1 file_eof = 0;
	
	FILE *fptr;
	
	U1 link_node = 0;
	U1 link_node_input = 0;
	U1 link_node_output = 0;
	U1 link_found_all = 0;
	
	fptr = fopen ((const char *) filename, "r");
	if (fptr == NULL)
	{
		printf ("fann_load_cells: error opening file: %s\n", filename);
		return (NULL);
	}

	// read header and check
	if (fgets_uni ((char *) buf, MAXLINELEN, fptr) == NULL)
	{
		printf ("fann_load_cells: error reading header from file: %s\n", filename);
		fclose (fptr);
		return (NULL);
	}
	
	str_cut_newline (buf);
	// printf ("fann_load_cells: '%s'\n", buf);
	
	if (strcmp ((char *) buf, "cells V0.1-save") != 0)
	{
		printf ("fann_load_cells: error wrong header from file: %s\n", filename);
		fclose (fptr);
		return (NULL);
	}
	
	// header ok, try to load cells file:
	// get number of cells and allocate memory:
	
	// read header and check
	if (fgets_uni ((char *) buf, MAXLINELEN, fptr) == NULL)
	{
		printf ("fann_load_cells: error reading cells number from file: %s\n", filename);
		fclose (fptr);
		return (NULL);
	}
	
	str_cut_newline (buf);
	// printf ("fann_load_cells: '%s'\n", buf);
	
	if (searchstr (buf, (U1 *) "cells =", 0, 0, 1) >= 0)
	{
		if (get_number (buf, &val) == 0)
		{
			max_cells = val;
			// printf ("fann_load_cells: allocating %lli cells...\n", max_cells);
			
			cells = (struct cell *) calloc (max_cells, sizeof (struct cell));
			if (cells == NULL)
			{
				printf ("fann_load_cells: ERROR: can't allocate %lli cells!\n", max_cells);
				fclose (fptr);
				return (NULL);
			}
		}
	}
	else
	{
		printf ("fann_load_cells: error: no cells max number found!\n");
		fclose (fptr);
		return (NULL);
	}
	
	while (file_eof == 0)
	{
		if (fgets_uni ((char *) buf, MAXLINELEN, fptr) == NULL)
		{
			printf ("fann_load_cells: error reading data from file: %s\n", filename);
			fclose (fptr);
			return (NULL);
		}
		
		str_cut_newline (buf);
		
		// printf ("ANNs load-line: '%s'\n", buf);
		
		if (searchstr (buf, (U1 *) "EOF", 0, 0, 1) >= 0)
		{
			// printf ("fann_load_cells: got EOF line, done reading! %s\n", filename);
			file_eof = 1;
		}
		
		if (searchstr (buf, (U1 *) "cell =", 0, 0, 1) >= 0)
		{
			if (get_number (buf, &val) == 0)
			{
				curr_cell = val;
			}
			else
			{
				printf ("fann_load_cells: error 'cell' parsing file: %s\n", filename);
				fclose (fptr);
				return (NULL);
			}
		}
	
		if (searchstr (buf, (U1 *) "neurons =", 0, 0, 1) >= 0)
		{
			if (get_number (buf, &val) == 0)
			{
				cells[curr_cell].neurons_max = val;
				
				cells[curr_cell].neurons = (struct neuron *) calloc (val, sizeof (struct neuron));
				if (cells[curr_cell].neurons == NULL)
				{
					printf ("fann_load_ann: ERROR: can't allocate %lli neurons in cell %lli!\n", val, curr_cell);
					fclose (fptr);
					return (NULL);
				}
				n = 0;
			}
			else
			{
				printf ("fann_load_cells: error 'neurons' parsing file: %s\n", filename);
				fclose (fptr);
				return (NULL);
			}
		}
		
		if (searchstr (buf, (U1 *) "fann_name =", 0, 0, 1) >= 0)
		{
			if (get_string (buf, fann_name) == 0)
			{
				strcpy ((char *) cells[curr_cell].neurons[n].fann_name, (const char *) fann_name);
				// printf ("fann_name: '%s'\n", fann_name);
			}
			else
			{
				printf ("fann_load_cells: error 'fann_name' parsing file: %s\n", filename);
				fclose (fptr);
				return (NULL);
			}
		}
		
		if (searchstr (buf, (U1 *) "type =", 0, 0, 1) >= 0)
		{
			if (get_number (buf, &val) == 0)
			{
				cells[curr_cell].neurons[n].type = val;
			}
			else
			{
				printf ("fann_load_cells: error 'type' parsing file: %s\n", filename);
				fclose (fptr);
				return (NULL);
			}
		}
		
		if (searchstr (buf, (U1 *) "inputs =", 0, 0, 1) >= 0)
		{
			if (get_number (buf, &val) == 0)
			{
				cells[curr_cell].neurons[n].inputs = val;
			}
			else
			{
				printf ("fann_load_cells: error 'inputs' parsing file: %s\n", filename);
				fclose (fptr);
				return (NULL);
			}
		}
		
		if (searchstr (buf, (U1 *) "outputs =", 0, 0, 1) >= 0)
		{
			if (get_number (buf, &val) == 0)
			{
				cells[curr_cell].neurons[n].outputs = val;
			}
			else
			{
				printf ("fann_load_cells: error 'outputs' parsing file: %s\n", filename);
				fclose (fptr);
				return (NULL);
			}
		}
		
		if (searchstr (buf, (U1 *) "links_max =", 0, 0, 1) >= 0)
		{
			if (get_number (buf, &val) == 0)
			{
				cells[curr_cell].neurons[n].links_max = val;
				
				cells[curr_cell].neurons[n].links = calloc (val, sizeof (struct link));
				if (cells[curr_cell].neurons[n].links == NULL)
				{
					printf ("fann_load_cells: out of memory, allocating links!\n");
					fclose (fptr);
					return (NULL);
				}
			}
			else
			{
				printf ("fann_load_cells: error 'links_max' parsing file: %s\n", filename);
				fclose (fptr);
				return (NULL);
			}
		}
		
		if (searchstr (buf, (U1 *) "layer =", 0, 0, 1) >= 0)
		{
			if (get_number (buf, &val) == 0)
			{
				cells[curr_cell].neurons[n].layer = val;
			}
			else
			{
				printf ("fann_load_cells: error 'layer' parsing file: %s\n", filename);
				fclose (fptr);
				return (NULL);
			}
		}
		
		if (searchstr (buf, (U1 *) "links_start", 0, 0, 1) >= 0)
		{
			for (l = 0; l < cells[curr_cell].neurons[n].links_max; l++)
			{
				// set found variables all to zero:
				link_node = 0;
				link_node_input = 0;
				link_node_output = 0;
				link_found_all = 0;
					
				while (link_found_all == 0)
				{
					if (fgets_uni ((char *) buf, MAXLINELEN, fptr) == NULL)
					{
						printf ("fann_load_cells: error reading data from file: %s\n", filename);
						fclose (fptr);
						return (NULL);
					}
						
					// printf ("buf: '%s'\n", buf);
						
					if (searchstr (buf, (U1 *) "link_node =", 0, 0, 1) >= 0)
					{
						if (get_number (buf, &val) == 0)
						{
							cells[curr_cell].neurons[n].links[l].node = val;
							link_node = 1;
						}
						else
						{
							printf ("fann_load_cells: error 'link_node' parsing file: %s\n", filename);
							fclose (fptr);
							return (NULL);
						}
					}
						
					if (searchstr (buf, (U1 *) "link_node_input =", 0, 0, 1) >= 0)
					{
						if (get_number (buf, &val) == 0)
						{
							cells[curr_cell].neurons[n].links[l].node_input = val;
							link_node_input = 1;
						}
						else
						{
							printf ("fann_load_cells: error 'link_node_input' parsing file: %s\n", filename);
							fclose (fptr);
							return (NULL);
						}
					}
					
					if (searchstr (buf, (U1 *) "link_node_output =", 0, 0, 1) >= 0)
					{
						if (get_number (buf, &val) == 0)
						{
							cells[curr_cell].neurons[n].links[l].node_output = val;
							link_node_output = 1;
						}
						else
						{
							printf ("fann_load_cells: error 'link_node_output' parsing file: %s\n", filename);
							fclose (fptr);
							return (NULL);
						}
					}
						
					if (link_node == 1 && link_node_input == 1 && link_node_output == 1)
					{
						link_found_all = 1;
					}
				}
			}
		}
		
		if (searchstr (buf, (U1 *) "node_end", 0, 0, 1) >= 0)
		{
			// increase node:
			n++;
		}
	}
	
	fclose (fptr);
	return (cells);
}
