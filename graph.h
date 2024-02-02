
#ifndef GRAPH_H
#define GRAPH_H

typedef struct graph_struct graph;
typedef struct vertex_struct vertex;
typedef struct link_struct link;


typedef struct graph_struct
{
	int v_len;
	vertex *vertices;
	int l_len;
	link *links;
	int progress;
};


typedef struct vertex_struct
{
	int id;
	int id_tree;
	int color;
	int n_len;
	link **neighbours;
};


typedef struct link_struct
{
	int id_vertex_1;
	int id_vertex_2;
	int is_edge;
	int is_frond;
	int color;
};


graph *initiate_graph();
void initiate_links(graph*, int[][2], int);

int color_graph(graph*);
void clear_colors(graph*);

int calculate_biconnected_components(graph*);

void print_graph(graph*);
void free_graph(graph*);

#endif