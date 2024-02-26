#include <stdio.h>
#include <stdlib.h>
#include "graph.h"


void initiate_list(List *lp)
{
	lp->head = NULL;
	lp->tail = NULL;
}


Node *create_node(int item)
{
	Node * new_node;
	new_node = (Node *) malloc(sizeof(Node));
	
	new_node->item = item;
	new_node->next = NULL;
	
	return new_node;
}


void add(List *lp, int item)
{
	Node *node;
	node = create_node(item);
	
	if(lp->head == NULL)
	{
		lp->head = node;
		lp->tail = node;
	}
	else
	{
		lp->tail->next  = node;
		lp->tail = lp->tail->next;
	}
	
	lp->size++;
}


void print_list(List *lp)
{
	Node *node;
	
	if(lp->head == NULL)
	{
		printf("\nEmpty List");
		return;
	}
	
	node = lp->head;
	
	printf("BC IDs:\n"); 
	neighbour *n;
	while(node != NULL)
	{
		n = node->first_neighbour_bc;
		printf("%d [First Neighbour: %d]\n", node->item, n->id);
		node = node->next;
	}
	printf("\n\n");
}


void free_list(List *list) {
	Node *current = list->head;
	Node *next;
	
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	
	list->head = NULL;
	list->tail = NULL;
}


void initiate_array_list(List **l, int size) 
{
	for (int i = 0; i < size; i++) {
		l[i] = (List *) malloc(sizeof(List));
		initiate_list(l[i]);
	}
}


graph *set_input(char *input_path, int max_num_nodes, int max_num_edges)
{
	FILE *file = fopen(input_path, "r");
	
	if (file != NULL)
	{
		
		graph *new_graph = (graph*)malloc(sizeof(graph));
		vertex *v_array = (vertex*)calloc(max_num_nodes, sizeof(vertex));
		edge *e_array = (edge*)calloc(max_num_edges, sizeof(edge));
		
		if (new_graph == NULL || v_array == NULL || e_array == NULL)
		{
			printf("malloc failed\n");
			exit(1);
		}
		
		new_graph->len_v = max_num_nodes;
		new_graph->vertices = v_array;
		
		new_graph->len_e = max_num_edges;
		new_graph->edges = e_array;
		
		new_graph->progress = 0;
		
		int len_list_edges = 0;
		int len_list_vertex = 0;
		int c = 0;
		char line[1000];
		
		vertex *vertex_1, *vertex_2;       
		
		while (fgets(line, sizeof(line), file))
		{
			int start, end;
			if (sscanf(line, "%d %d", &start, &end) == 2)
			{
				if (start != end)
				{
					c++;
					if (start > len_list_vertex)
					{
						len_list_vertex = start;
					}
					if (end > len_list_vertex)
					{
						len_list_vertex = end;
					}
					
					vertex_1 = &(new_graph->vertices[start]);
					vertex_2 = &(new_graph->vertices[end]);
					
					if (vertex_1->id != 0)
					{
					vertex_1->degree++;
					}
					else
					{
						vertex_1->id = start;
						vertex_1->degree = 1;
					}
					
					if (vertex_2->id != 0)
					{
						vertex_2->degree++;
					}
					else
					{
						vertex_2->id = end;
						vertex_2->degree = 1;
					}
					
					neighbour *neighbour_1 = (neighbour*)malloc(sizeof(neighbour));
					neighbour *neighbour_2 = (neighbour*)malloc(sizeof(neighbour));
					if (neighbour_1 == NULL || neighbour_2 == NULL)
					{
						printf("malloc neighbour failed\n");
					}
					
					neighbour_1->id = vertex_2->id;
					neighbour_1->next = vertex_1->first_neighbour;
					vertex_1->first_neighbour = neighbour_1;
					
					neighbour_2->id = vertex_1->id;
					neighbour_2->next = vertex_2->first_neighbour;
					vertex_2->first_neighbour = neighbour_2;
					
					new_graph->edges[len_list_edges].a = vertex_1;
					new_graph->edges[len_list_edges].b = vertex_2;
					
					neighbour_1->edge_id = len_list_edges;
					neighbour_2->edge_id = len_list_edges;
					
					len_list_edges++;
					
				}
			}
		}
		
		fclose(file);
		new_graph->num_edges = len_list_edges;
		new_graph->num_vertices = len_list_vertex;
		
		new_graph->iterations = 0;
		return new_graph;
	}
	else
	{
		printf("file does not exist\n");
		exit(1);
	}
}


void free_graph(graph *g)
{
	if (g == NULL)
	{
		return;
	}
	
	int i;
	vertex *v;
	neighbour *n;
	neighbour *n_next;
	for (i = 1; i <= g->num_vertices; i++)
	{
		v = &g->vertices[i];
		
		free_list(v->bc_ids);
		n = v->first_neighbour;
		
		while (n != NULL)
		{
			n_next = n->next;
			free(n);
			
			n = n_next;
		}
	}
	
	free(g->edges);
	free(g->vertices);
	free(g);
}


void clear_colors(graph *g)
{
	if (g == NULL)
	{
		return;
	}
	
	for (int i = 1; i <= g->num_vertices; i++)
	{
		g->vertices[i].color = 0;
	}
}


void color_component(graph *g, int id, int color)
{
	vertex *v = &g->vertices[id];
	
	if (!v->color)
	{
		v->color = color;
		neighbour *neighbour = v->first_neighbour;
		
		while (neighbour != NULL)
		{
			color_component(g, neighbour->id, color);
			neighbour = neighbour->next;
		}
	}
}


int color_graph(graph* g)
{
	if (g == NULL || g->vertices == NULL)
	{
		return 0;
	}
	
	clear_colors(g);
	int i, color = 0;
	for (i = 1; i <= g->num_vertices; i++)
	{
		if (!g->vertices[i].color)
		{
			color++;
			color_component(g, i, color);
		}
	}
	
	return color;
}

void print_graph(graph *g)
{
	if (g == NULL || g->vertices == NULL || g->num_vertices == 0)
	{
		printf("Empty graph\n");
	}
	
	neighbour *n;
	edge *e;
	vertex *v;
	for (int i = 1; i <= g->num_vertices; i++)
	{
		v = &g->vertices[i];
		
		printf("Node [%d]\t Degree: %d\nNeighbours:\n", v->id, v->degree);
		n = v->first_neighbour;
		while (n != NULL)
		{
			e = &g->edges[n->edge_id];
			
			printf("%d\t BC ID: [%d]\n", n->id, e->color);
			n = n->next;
		}
		printf("\n");
		print_list(v->bc_ids);
	}
}
