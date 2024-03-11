#include <stdio.h>
#include <stdlib.h>

#ifndef GRAPH_H
#define GRAPH_H

typedef struct graph_struct graph;
typedef struct vertex_struct vertex;
typedef struct edge_struct edge;
typedef struct neighbour_struct neighbour;


typedef struct node
{
	int item;
	neighbour *first_neighbour_bc;
	struct node *next;
} Node;


typedef struct
{
	Node *head;
	Node *tail;
	int size;
} List;

typedef struct neighbour_struct
{
    int id;
	int edge_id;
    struct neighbour_struct *next;
} neighbour;

typedef struct vertex_struct
{
    int id;
	int degree;
    neighbour *first_neighbour;
	int id_tree;
	int color;
	short queued;
	List *bc_ids;
} vertex;

typedef struct edge_struct
{
    int coreness;
    vertex *a;
    vertex *b;
	short is_edge;
	short is_frond;
	int color;
} edge;

typedef struct graph_struct
{
    int num_edges;
    int num_vertices;
    int len_v;
    int len_e;
    edge *edges;
    vertex *vertices;
	int progress;
	int iterations;
} graph;


void initiate_list(List*);
void initiate_array_list(List**,int);
void add(List*,int);
void free_list(List*);
void print_list(List*);

graph *set_input(char*,int,int);
void free_graph(graph*);
void print_graph(graph*,int);

#endif