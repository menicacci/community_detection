#include <stdio.h>
#include "graph.h"
#include "list.h"


graph *initiate_graph(int num_verteces)
{
	graph *g = (graph *)malloc(sizeof(graph));
	g->v_len = num_verteces;
	g->vertices = (vertex *)malloc(sizeof(vertex) * g->v_len);
	
	for (int i = 0; i < g->v_len; i++)
	{
		vertex *new_vertex = &g->vertices[i];
		new_vertex->id = i + 1;
		new_vertex->id_tree = 0;
		new_vertex->color = 0;
		new_vertex->neighbours = NULL;
		new_vertex->n_len = 0;
		new_vertex->bc_ids = (List *)malloc(sizeof(List));
		initiate_list(new_vertex->bc_ids);
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
		link *new_link = &g->links[i];
		new_link->id_vertex_1 = links[i][0];
		new_link->id_vertex_2 = links[i][1];
		new_link->is_edge = 0;
		new_link->is_frond = 0;
		new_link->color = 0;
		
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


// Resets the color of each node
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


// Extracts the neighbour of a node from its link
int get_neighbour_id(link *l, int id)
{
	return l->id_vertex_1 == id ? l->id_vertex_2 : l->id_vertex_1;
}


// Colors a connected component
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


// Colors all the graph's nodes. Nodes of the same connected component have the same ID
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
		
		printf("\nBiconnected Components IDs:");
		print_list(v->bc_ids);
		
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


/* 
 * Builds a tree graph inside the original graph to calculate biconnected components.
 * Identifies edges and fronds between graph's links.
*/
void build_tree_graph(graph *g, int node_id)
{
	vertex *v = &g->vertices[node_id - 1];
	
	// If the node has already been visited, skip it
	if (v != NULL && !v->color)
	{
		// Set the node as visited
		v->color = 1;
		
		// Initiate the id_tree of the node, this will be helpful going forward
		g->progress++;
		v->id_tree = g->progress;
		
		// Go though all the node's neighbours
		for (int i = 0; i < v->n_len; i++)
		{
			link *l = v->neighbours[i];
			
			// Link already visited? Then skip it
			if (!l->is_edge && !l->is_frond)
			{
				int neighbour_id = get_neighbour_id(l, node_id);
				
				// Has the neighbour been already visited?
				if (!g->vertices[neighbour_id - 1].color)
				{
					// If not, the link is an edge. 
					// The current node will be the neighbour's parent in the tree graph
					l->is_edge = 1;
					// Go forward
					build_tree_graph(g, neighbour_id);
				}
				else
				{
					// Neighbour has already been visited, the the link that connects them its a frond
					l->is_frond = 1;
				}
			}
		}
		
	}
	return;
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


// Colors all the edges of the same connected component
void color_edges(graph *g, int cut_vertex_id, int node_id, int c)
{
	vertex *v = &g->vertices[node_id - 1];
	
	if (v->color == c + 1)
	{
		return;
	}
	
	v->color = c + 1;
	add_tail(v->bc_ids, g->progress);
	
	int neighbour_id;
	link *l;
	for (int i = 0; i < v->n_len; i++)
	{
		l = v->neighbours[i];
		neighbour_id = get_neighbour_id(l, v->id);
		
		if (!l->color)
		{
			l->color = c;
			
			// Don't call this function on the cut-vertex
			if (neighbour_id != cut_vertex_id)
			{
				color_edges(g, cut_vertex_id, neighbour_id, c);
			}
		}
	}
}

// Finds all the biconnected components inside a connected component
int find_biconnected_components(graph *g, int node_id)
{
	vertex *v = &g->vertices[node_id - 1];
	
	if (v != NULL && !v->color)
	{
		v->color = 1;
		
		// A single node is a biconnected component it-self
		if (!v->n_len)
		{
			add_tail(v->bc_ids, g->progress);
			g->progress++;
			return 0;
		}
		
		// This variable will store the lowpoint of each neighbour
		int neighbour_lowpoint = v->id_tree;
		// This variable will store the lowpoint to pass to the node's parent
		int node_lowpoint = v->id_tree;
		
		link *l;
		vertex *neighbour;
		for (int i = 0; i < v->n_len; i++)
		{
			l = v->neighbours[i];
			neighbour = &g->vertices[get_neighbour_id(l, node_id) - 1];
			
			// If the link is an edge, find its lowpoint
			if (l->is_edge) 
			{
				neighbour_lowpoint = find_biconnected_components(g, neighbour->id);
				// If the neighbour_lowpoint is 0, that means that the neighbour was already visited.
				// No further action required
				if (neighbour_lowpoint != 0)
				{
					// If the neighbour lowpoint is lower that the node's id_tree, then you found a biconnected component
					// This means that this node is a cut-vertex
					if (neighbour_lowpoint >= v->id_tree)
					{
						// Add the new biconnected component ID to the node's bc_ids
						add_tail(v->bc_ids, g->progress);
						
						l->color = g->progress;
						// Color all the biconnected component edges from the neighbour
						color_edges(g, v->id, neighbour->id, l->color);
						// Update the ID
						g->progress++;
					}
					// If no new component has been found, update the node's lowpoint if required
					else
					{
						node_lowpoint = node_lowpoint > neighbour_lowpoint ? neighbour_lowpoint : node_lowpoint;
					}
				}
			}
			// If the node it's a frond, update the lowpoint if required.
			// This action can be performed just from a son of the current node
			else if (l->is_frond && v->id_tree > neighbour->id_tree)
			{
				node_lowpoint = node_lowpoint > neighbour->id_tree ? neighbour->id_tree : node_lowpoint;
			}
			else
			{
				continue;
			}
		}
		
		return node_lowpoint;
	}
	return 0;
}


// Calculates all the biconnected components of a graph
int calculate_biconnected_components(graph *g)
{
	clear_colors(g);
	int num_components = color_graph(g);
	int *root_nodes = get_root_nodes_index(g, num_components);
	
	clear_colors(g);
	int num_bc = 0;
	int i;
	if (root_nodes != NULL)
	{
		for (i = 0; i < num_components; i++)
		{
			build_tree_graph(g, root_nodes[i]);
		}
		
		clear_colors(g);
		g->progress = 1;
		for (i = 0; i < num_components; i++)
		{
			find_biconnected_components(g, root_nodes[i]);
		}
		num_bc = g->progress - 1;
		
		clear_colors(g);
		
		free(root_nodes);
	}
	
	return num_bc;
}
