#include <stdio.h>
#include "graph.h"

// TODO: Implement a dynamic way to load a graph from a file
// TODO: Handle memory allocation failures

int main(int argc, char **argv)
{
	
	
	graph *g = initiate_graph(28);
	
	int links[38][2] = {
		{1, 2}, {1, 11},
		{2, 6},
		{3, 4}, {3, 5}, {3, 8}, {3, 9},
		{4, 5}, {4, 8}, {4, 9},
		{5, 8}, {5, 9},
		{8, 9},
		{6, 7},
		{7, 11},
		{10, 12}, {10, 13}, {10, 14},
		{12, 13}, {12, 14}, {12, 15}, {12, 16},
		{13, 15},
		{14, 16},
		{15, 16}, {15, 18},
		{16, 17},
		{17, 18},
		{19, 20}, {19, 22}, {19, 24}, {19, 28},
		{20, 25}, {20, 21},
		{21, 22}, {21, 23}, 
		{22, 24},
		{23, 25},
	};
	
	initiate_links(g, links, 38);
	
	int num_bc = calculate_biconnected_components(g);
	
	printf("Number of Biconnected Components: %d\n\n", num_bc);
	print_graph(g);
	free_graph(g);
}
