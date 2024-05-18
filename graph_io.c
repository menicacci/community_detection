#include "graph_io.h"

void print_node(graph* g, int node_id, int show_neighbours, int show_bc) {
	vertex *v;	
	neighbour *n;
	edge *e;

	v = &g->vertices[node_id];
	printf("Node: [%d]\tColor: [%d]\tID Tree: [%d]\tDegree: [%d]\t", v->id, v->color, v->id_tree, v->degree);
	if (show_neighbours) {
		char c = show_bc ? '\n' : '\t';
		printf("%cNeighbours:%c", c, c);

		n = v->first_neighbour;
		while (n != NULL)
		{
			e = &g->edges[n->edge_id];

			printf("%d [E: %d, F: %d]\t", n->id, e->is_edge, e->is_frond);
			if (show_bc)
			{
				printf("BC ID: [%d]\n",  e->color);
			}

			n = n->next;
		}

		if (show_bc)
		{
			print_list(v->bc_ids);
		}
		else
		{
			printf("\n");
		}
	}
	printf("\n");
}


void print_graph(graph *g, int show_not_colored, int show_neighbours, int show_bc)
{
	if (g == NULL || g->vertices == NULL || g->num_vertices == 0)
	{
		printf("Empty graph\n");
	}
	
	printf("\n");
	for (int i = 1; i <= g->num_vertices; i++)
	{
		if (!show_not_colored || !g->vertices[i].color) {
			print_node(g, i, show_neighbours, show_bc);
		}
	}
}


void print_k_bc_stats(graph* g, int num_bc) {
	int num_cores = color_graph_rec(g, 0);
	printf("# of Cores: %d\n\n", num_cores);

	int *cores_size = (int *)calloc(num_cores, sizeof(int));
	int core_id;
	for (int i = 1; i <= g->num_vertices; i++) {
		core_id = g->vertices[i].color;

		if (core_id <= num_cores) {
			cores_size[core_id - 1]++;
		}
	}

	int *bcs_size = (int *)calloc(num_bc, sizeof(int));
	int *bc_to_core = (int *)calloc(num_bc, sizeof(int));
	vertex* v;
	Node* n;
	for (int i = 1; i <= g->num_vertices; i++) {
		v = &g->vertices[i];
		core_id = v->color;

		if (core_id <= num_cores && v->bc_ids->size > 0) {
			n = v->bc_ids->head;

			while (n != NULL) {
				bcs_size[n->item - 1]++;
				bc_to_core[n->item - 1] = core_id;

				n = n->next;
			}
		}
	}

	printf("Cores Info:\n");
	for (int i = 0; i < num_cores; i++) {
		printf("Core ID: %d\tSize: [%d]\n", i + 1, cores_size[i]);
	}

	printf("\n\nCommunities:\n");
	for (int i = 0; i < num_bc; i++) {
		printf("Com. ID: %d\tSize: [%d]\tCore ID: %d\n", i + 1, bcs_size[i], bc_to_core[i]);
	}

	free(bcs_size);
	free(bc_to_core);
	free(cores_size);
}


void write_graph_communities(graph* g, char* filename) {
	FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

	vertex *v;
	neighbour* n;

	for(int i = 1; i <= g->num_vertices; i++) {
		v = &g->vertices[i];
		if (!v->color) {
			n = v->first_neighbour;

			while (n != NULL) {
				if (!g->vertices[n->id].color && i < n->id) {
					fprintf(file, "%d\t%d\t%d\n", i, n->id, g->edges[n->edge_id].color);
				}

				n = n->next;
			}
		}
	}
	fclose(file);
}
