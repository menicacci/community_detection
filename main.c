#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "utils.h"
#include "graph_io.h"


#define V 1000000
#define E 10000000


int main(int argc, char **argv)
{

	if (argc < 3) {
        printf("Please provide an input file name and a integer.\n");
        return 1; 
    }

    char *input_file = argv[1];
	char *output_file = argv[2];
	int k = atoi(argv[3]);
	graph *graph_input = set_input(input_file, V, E);
	
	printf("Graph loaded\n");

	int* node_coreness = find_node_coreness(graph_input);

	int max_coreness = find_max_int(node_coreness, graph_input->num_vertices);
	printf("Graph max coreness: %d\n\n", max_coreness);

	int *coreness_values = (int *)calloc(max_coreness + 1, sizeof(int));
	for (int i = 0; i < graph_input->num_vertices; i++) {
		coreness_values[node_coreness[i]]++;
	}

	for (int i = max_coreness - 1; i >= 0; i--) {
		coreness_values[i] += coreness_values[i + 1];
	}

	// for (int i = 0; i < max_coreness + 1; i++) {
	// 	printf("%d\t->\t%d\n", i, coreness_values[i]);
	// }

	int num_bc = calculate_biconnected_components(graph_input, k);
	printf("Nodes: [%d]\tLinks: [%d]\tIterations: [%d]\tBiconnected Components: [%d]\n\n", graph_input->num_vertices, graph_input->num_edges, graph_input->iterations, num_bc);

	printf("# of Nodes with coreness [%d]: %d\n\n", k, coreness_values[k]);

	detect_k_cores(graph_input, k);
	// print_graph(graph_input, 1, 0, 1);

	write_graph_communities(graph_input, output_file);

	print_k_bc_stats(graph_input, num_bc);

	free(node_coreness);
	free(coreness_values);

	free_graph(graph_input);
	
	return 0;
}
