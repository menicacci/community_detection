#include <stdio.h>
#include "graph.h"
#include "list.h"

// TODO: Implement a dynamic way to load a graph from a file
// TODO: Handle memory allocation failures

int main(int argc, char **argv)
{
	
	graph *g = initiate_graph(10);
	
	int links[14][2] = {
		{1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8},
		{2, 3}, {2, 4},
		{4, 5},
		{6, 7}, {6, 8},
		{7, 8}, {7, 9},
	};
	
	initiate_links(g, links, 14);
	
	int num_bc = calculate_biconnected_components(g);
	
	print_graph(g);
	
	List **bc = (List **)malloc(sizeof(List *) * num_bc);
	initiate_array_list(bc, num_bc);
	
	printf("\n\nNumber of Biconnected Components: %d\n\n", num_bc);
	for (int i = 0; i < g->v_len; i++)
	{
		vertex *v = &g->vertices[i];
		Node *n = v->bc_ids->head;
		while (n != NULL)
		{
			add_tail(bc[n->item - 1], v->id);
			n = n->next;
		}
	}
	
	for (int i = 0; i < num_bc; i++)
	{
		printf("List: %d\n", i + 1);
		print_list(bc[i]);
		free_list(bc[i]);
	}
	
	
	free(bc);
	free_graph(g);
}
