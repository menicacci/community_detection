#include "utils.h"

#ifndef IO_H
#define IO_H

void print_graph(graph*,int,int,int);
void print_node(graph*,int,int,int);

void print_k_bc_stats(graph*, int);

void write_graph_communities(graph*,char*);

#endif