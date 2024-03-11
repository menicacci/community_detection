#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "utils.h"

#define V 1000000
#define E 10000000

int main(int argc, char **argv)
{
	if (argc < 2)
    {
        printf("Please provide a string as a command-line argument.\n");
        return 1; // Exit the program with an error code
    }

    char *input_file = argv[1];
	graph *graph_input = set_input(input_file, V, E);
	
	printf("Graph loaded\n");

	// int num_bc = calculate_biconnected_components(graph_input);
	// printf("Nodes: [%d]\tLinks: [%d]\tIterations: [%d]\tBiconnected Components: [%d]\n", graph_input->num_vertices, graph_input->num_edges, graph_input->iterations, num_bc);
	
	// Testing iterative functions
	int num_components = color_graph_it(graph_input);
	int *root_nodes = get_root_nodes_index(graph_input, num_components);
	
	clear_colors(graph_input);
	build_tree_graph_it(graph_input, 1);
	
	color_graph_it(graph_input);
	
	print_graph(graph_input, 0);
	free_graph(graph_input);
	
	return 0;
}
