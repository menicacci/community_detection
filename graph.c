#include <stdio.h>
#include "graph.h"


graph *initiate_graph(int num_verteces)
{
	graph *g = (graph *)malloc(sizeof(graph));
	g->v_len = num_verteces;
	g->vertices = (vertex *)malloc(sizeof(vertex) * g->v_len);
	
	for (int i = 0; i < g->v_len; i++)
	{
		vertex *new_vertex = (vertex *)malloc(sizeof(vertex));
		new_vertex->id = i + 1;
		new_vertex->id_tree = 0;
		new_vertex->color = 0;
		new_vertex->neighbours = NULL;
		new_vertex->n_len = 0;
		
		g->vertices[i] = *new_vertex;
	}
	
	g->l_len = 0;
	g->links = NULL;
	
	g->progress = 0;
	return g;
}


void initiate_links(graph *g, int links[][2], int l_len)
{
	
	g->l_len = l_len;
	g->links = (link *)malloc(sizeof(link) * g->l_len);

	int i;
	int *count_links_per_node = (int *)calloc(g->v_len, sizeof(int));
	for (i = 0; i < g->l_len; i++) 
	{
		link *new_link = (link *)malloc(sizeof(link));
		new_link->id_vertex_1 = links[i][0];
		new_link->id_vertex_2 = links[i][1];
		new_link->is_edge = 0;
		new_link->is_frond = 0;
		new_link->color = 0;
		
		g->links[i] = *new_link;
		
		count_links_per_node[new_link->id_vertex_1 - 1]++;
		count_links_per_node[new_link->id_vertex_2 - 1]++;
	}
	
	for (i = 0; i < g->v_len; i++)
	{
		g->vertices[i].neighbours = (link **)malloc(sizeof(link *) * count_links_per_node[i]);
	}
	
	for (i = 0; i < g->l_len; i++)
	{
		link *l = &g->links[i];
		vertex *v1 = &g->vertices[l->id_vertex_1 - 1];
		vertex *v2 = &g->vertices[l->id_vertex_2 - 1];
		
		v1->neighbours[v1->n_len] = l;
		v1->n_len++;
		v2->neighbours[v2->n_len] = l;
		v2->n_len++;
	}
}


void clear_colors(graph *g)
{
	if (g == NULL)
	{
		return;
	}
	
	for (int i = 0; i < g->v_len; i++)
	{
		g->vertices[i].color = 0;
	}
}


int get_neighbour_id(link *l, int id)
{
	return l->id_vertex_1 == id ? l->id_vertex_2 : l->id_vertex_1;
}


void color_component(graph *g, int id, int color)
{
	vertex *v = &g->vertices[id];
	
	if (!v->color)
	{
		v->color = color;
		for (int i = 0; i < v->n_len; i++)
		{
			color_component(g, get_neighbour_id(v->neighbours[i], v->id) - 1, color);
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
	for (i = 0; i < g->v_len; i++)
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
	if (g == NULL || g->vertices == NULL || g->v_len == 0)
	{
		return;
	}
	
	int i, j;
	printf("Vertices in the graph:\n");
	
	for (i = 0; i < g->v_len; i++)
	{
		vertex *v = &g->vertices[i];
		printf("ID: %d, ID Tree: %d, Color: %d, Neighbours: %d\n", v->id, v->id_tree, v->color, v->n_len);
		
		for (j = 0; j < v->n_len; j++)
		{
			printf("%d  ", get_neighbour_id(v->neighbours[j], i + 1));
		}
		
		printf("\n\n");
	}
	
	printf("Links in the graph:\n");
	for (i = 0; i < g->l_len; i++)
	{
		link *l = &g->links[i];
		printf("Edge: %d, Frond: %d, Color: %d \t-> [%d - %d]\n", l->is_edge, l->is_frond, l->color, l->id_vertex_1, l->id_vertex_2);
	}
}


void free_graph(graph *g)
{
	if (g == NULL)
	{
		return;
	}
	
	int i;
	if (g->vertices != NULL)
	{
		for (i = 0; i < g->v_len; i++)
		{
			vertex *v = &g->vertices[i];
			
			if (v->neighbours != NULL)
			{
				free(v->neighbours);
			}
		}
	}
	
	if (g->links != NULL)
	{
		free(g->links);
	}
	
	free(g);
	return;
}


void build_tree_graph(graph *g, int node_id)
{
	vertex *v = &g->vertices[node_id - 1];
	
	if (v != NULL && !v->color)
	{
		v->color = 1;
		
		g->progress++;
		v->id_tree = g->progress;
		
		for (int i = 0; i < v->n_len; i++)
		{
			link *l = v->neighbours[i];
			if (!l->is_edge && !l->is_frond)
			{
				int neighbour_id = get_neighbour_id(l, node_id);
				if (!g->vertices[neighbour_id - 1].color)
				{
					l->is_edge = 1;
					build_tree_graph(g, neighbour_id);
				}
				else
				{
					l->is_frond = 1;
				}
			}
		}
		
	}
}


// Finds a node's ID for each connected component
int *get_root_nodes_index(graph *g, int num_components)
{
	if (!num_components)
	{
		return NULL;
	}
	
	int *root_nodes = (int *)malloc(sizeof(int) * num_components);
	
	int c = 1;
	for (int i = 0; i < g->v_len; i++)
	{
		if (c == g->vertices[i].color)
		{
			root_nodes[c - 1] = i + 1;
			c++;
		}
	}
	
	return root_nodes;
}


int find_biconnected_components(graph *g, int node_id)
{
	vertex *v = &g->vertices[node_id - 1];
	
	if (v != NULL && !v->color)
	{
		v->color = 1;
		
		if (!v->n_len)
		{
			g->progress++;
			return 0;
		}
		
		
		int min_id_neighbours = 10000;
		int min_id_parents = 10000;
		for (int i = 0; i < v->n_len; i++)
		{
			link *l = v->neighbours[i];
			vertex *neighbour = &g->vertices[get_neighbour_id(l, node_id) - 1];
			
			if (l->is_edge) 
			{
				min_id_neighbours = find_biconnected_components(g, neighbour->id);
				
				if (min_id_neighbours != 0)
				{
					l->color = g->progress;
					
					// Biconnected component found
					if (min_id_neighbours >= v->id_tree)
					{
						g->progress++;
					}
					else
					{
						min_id_parents = min_id_parents > min_id_neighbours ? min_id_neighbours : min_id_parents;
					}
				}
			}
			else if (l->is_frond && v->id_tree > neighbour->id_tree)
			{
				min_id_parents = min_id_parents > neighbour->id_tree ? neighbour->id_tree : min_id_parents;
				l->color = g->progress;
			}
			else
			{
				continue;
			}
		}
		
		return min_id_parents;
	}
	
	return 0;
}


int calculate_biconnected_components(graph *g)
{
	clear_colors(g);
	int num_components = color_graph(g);
	int *root_nodes = get_root_nodes_index(g, num_components);
	clear_colors(g);
	
	int num_bc = 0;
	if (root_nodes != NULL)
	{
		for (int i = 0; i < num_components; i++)
		{
			build_tree_graph(g, root_nodes[i]);
		}
		
		clear_colors(g);
		
		g->progress = 1;
		for (int i = 0; i < num_components; i++)
		{
			find_biconnected_components(g, root_nodes[i]);
		}
		num_bc = g->progress - 1;
		
		free(root_nodes);
		clear_colors(g);
	}
	
	return num_bc;
}


/* 
 * Old logic 
 * Used for testing

typedef struct array {
	int *values;
	int len;
} array;

array *initialize_array(int id)
{
	array *a = (array *)malloc(sizeof(array));
	a->len = 1;
	a->values = (int *)malloc(sizeof(int));
	a->values[0] = id;
	
	return a;
}


void free_array(array *a)
{
	if (a != NULL)
	{
		if(a->values != NULL)
		{
			free(a->values);
		}
		
		free(a);
	}
	
	return;
}
  

array *merge_ordered_arrays(array *a1, array *a2)
{
	array *merged = (array *)malloc(sizeof(array));
	merged->len = a1->len + a2->len;
	merged->values = (int *)malloc(sizeof(int) * merged->len);
	
	int i = 0, j = 0, k = 0;
	// Merge the two sorted arrays
	while (i < a1->len && j < a2->len) 
		{
		if (a1->values[i] < a2->values[j]) 
		{
			merged->values[k++] = a1->values[i++];
		} 
		else if (a1->values[i] > a2->values[j]) 
		{
			merged->values[k++] = a2->values[j++];
		}
		else {
			
			merged->values[k++] = a1->values[i++];
			j++;
		}
	}
	
	while (i < a1->len) {
		merged->values[k++] = a1->values[i++];
	}

	while (j < a2->len) {
		merged->values[k++] = a2->values[j++];
	}
	
	merged->values = (int *)realloc(merged->values, sizeof(int) * k);
	merged->len = k;
	
	return merged;
}

array *find_biconnected_components(graph *g, int node_id, int *id_corresponcence)
{
	vertex *v = &g->vertices[node_id - 1];
	
	if (v != NULL && !v->color)
	{
		v->color = 1;
		
		if (!v->n_len)
		{
			printf("%d\n\n", v->id);
		}
		
		array *v_temp;
		array *v_temp_2;
		array *v_id = initialize_array(v->id_tree);
		array *v_id_for_neighbours = initialize_array(v->id_tree);
		
		array *v_id_neighbour;
		for (int i = 0; i < v->n_len; i++)
		{
			link *l = v->neighbours[i];
			int neighbour_id = get_neighbour_id(l, node_id);
			vertex *neighbour = &g->vertices[neighbour_id - 1];
			
			if (l->is_edge) 
			{
				v_id_neighbour = find_biconnected_components(g, neighbour_id, id_corresponcence);
				
				if (v_id_neighbour != NULL)
				{
					l->color = g->progress;
					
					// Biconnected component found
					if (v_id_neighbour->values[0] >= v->id_tree)
					{
						array *merged = merge_ordered_arrays(v_id, v_id_neighbour);
						
						for (int j = 0; j < merged->len; j++)
						{
							printf("%d ", id_corresponcence[merged->values[j] - 1]);
							
						}
						printf("\n\n");
						
						g->progress++;
						free_array(merged);
					}
					else
					{
						v_temp = v_id_for_neighbours;
						v_id_for_neighbours = merge_ordered_arrays(v_id_for_neighbours, v_id_neighbour);
						
						free_array(v_temp);
					}
					
					free_array(v_id_neighbour);
				}
			}
			else if (l->is_frond && v->id_tree > neighbour->id_tree)
			{
				v_temp = initialize_array(neighbour->id_tree);
				v_temp_2 = v_id_for_neighbours;
				v_id_for_neighbours = merge_ordered_arrays(v_id_for_neighbours, v_temp);
				
				l->color = g->progress;
				
				free_array(v_temp);
				free_array(v_temp_2);
			}
			else
			{
				continue;
			}
		}
		
		return v_id_for_neighbours;
	}
	
	return NULL;
}
*/
