#include "graph.h"

#ifndef UTILS_H
#define UTILS_H

void clear_colors(graph*);
void order_node_edges_by_bc(graph*,vertex*,int*);
int *get_root_nodes_index(graph*,int);

int color_graph_rec(graph*);
void color_component_rec(graph*,int,int);
int color_graph_rec(graph*);
void build_tree_graph_rec(graph*,int);

int color_graph_it(graph*);
void build_tree_graph_it(graph*,int);

int calculate_biconnected_components(graph*);

#endif