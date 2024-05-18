#include "utils.h"

void clear_colors(graph *g)
{
	if (g == NULL)
	{
		return;
	}
	
	for (int i = 1; i <= g->num_vertices; i++)
	{
		g->vertices[i].color = 0;
		g->vertices[i].queued = 0;
	}
}


/*
 * bc_ind works as a support structure to perform the bucket sort algorithm efficiently
 * 
*/
void order_node_edges_by_bc(graph *g, vertex *v, int *bc_ind)
{
	// Reset the indexes of the node's biconnected components
	Node *bc_id = v->bc_ids->head;
	while (bc_id != NULL)
	{
		bc_ind[bc_id->item - 1] = 0;
		bc_id = bc_id->next;
	}
	
	// Count the number of neighbours for each BC, at the same time, it counts the size of the neighbourhood
	int neighbourhood_size = 0;
	neighbour *n = v->first_neighbour;
	while (n != NULL)
	{
		// TEMP
		g->iterations++;
		
		bc_ind[g->edges[n->edge_id].color - 1]++;
		neighbourhood_size++;
		n = n->next;
	}
	
	// Update the indexes' structure, now it will contain the first index where a neighbour is going to be stored
	int value = 0;
	int temp;
	bc_id = v->bc_ids->head;
	while (bc_id != NULL)
	{
		temp = bc_ind[bc_id->item - 1];
		bc_ind[bc_id->item - 1] = value;
		value += temp;
		
		bc_id = bc_id->next;
	}
	
	// Support structure to order the neighbours
	neighbour **sorted_neighbourhood = (neighbour **)malloc(sizeof(neighbour *) * neighbourhood_size);
	
	int neighbour_bc;
	n = v->first_neighbour;
	while (n != NULL)
	{
		// TEMP
		g->iterations++;
		
		// Check the color of the edge to see the neighbour's biconnected component
		neighbour_bc = g->edges[n->edge_id].color;
		
		sorted_neighbourhood[bc_ind[neighbour_bc - 1]] = n;
		bc_ind[neighbour_bc - 1]++;
		
		n = n->next;
	}
	
	// Set, for each node's biconnected component ID the pointer to the first neighbour that belongs to it
	bc_id = v->bc_ids->head;
	v->first_neighbour = sorted_neighbourhood[0];
	for (int i = 0; i < neighbourhood_size - 1; i++)
	{
		sorted_neighbourhood[i]->next = sorted_neighbourhood[i + 1];
		
		if (bc_id != NULL && bc_id->item == g->edges[sorted_neighbourhood[i]->edge_id].color)
		{
			bc_id->first_neighbour_bc = sorted_neighbourhood[i];
			bc_id = bc_id->next;
		}
	}
	
	if (bc_id != NULL)
	{
		bc_id->first_neighbour_bc = sorted_neighbourhood[neighbourhood_size - 1];
	}
	sorted_neighbourhood[neighbourhood_size - 1]->next = NULL;
	
	
	free(sorted_neighbourhood);
}


int *get_root_nodes_index(graph *g, int num_components)
{
	if (!num_components)
	{
		return NULL;
	}
	
	int *root_nodes = (int *)malloc(sizeof(int) * num_components);
	
	int c = 1;
	for (int i = 1; i <= g->num_vertices; i++)
	{
		if (c == g->vertices[i].color)
		{
			root_nodes[c - 1] = i;
			c++;
		}
	}
	
	return root_nodes;
}


void color_component_rec(graph *g, int id, int color)
{
	vertex *v = &g->vertices[id];
	
	if (!v->color)
	{
		v->color = color;
		neighbour *neighbour = v->first_neighbour;
		
		while (neighbour != NULL)
		{
			color_component_rec(g, neighbour->id, color);
			neighbour = neighbour->next;
		}
	}
}


int color_graph_rec(graph* g, int clear)
{
	if (g == NULL || g->vertices == NULL)
	{
		return 0;
	}
	
	if (clear) {
		clear_colors(g);
	}
	
	int i, color = 0;
	for (i = 1; i <= g->num_vertices; i++)
	{
		if (!g->vertices[i].color)
		{
			color++;
			color_component_rec(g, i, color);
		}
	}
	
	return color;
}


void exclude_edges(graph* g) {
	vertex *v;
	neighbour *neighbour;
	for (int i = 1; i <= g->num_vertices; i++) {
		v = &g->vertices[i];

		if (!v->color) {
			neighbour = v->first_neighbour;

			while (neighbour != NULL) {
				if (g->vertices[neighbour->id].color) {
					g->edges[neighbour->edge_id].color = 1;
				}
				
				neighbour = neighbour->next;
			}
		}
	}
}


void build_tree_graph_rec(graph *g, int node_id)
{
	vertex *v = &g->vertices[node_id];
	
	// If the vertex has not been visited, go on
	if (v != NULL && !v->color)
	{
		v->color = 1;
		
		// Set the node's tree ID, this will be unique among the graph
		v->id_tree = ++g->progress;
		
		// Go through all the node's neighbours
		neighbour *neighbour = v->first_neighbour;
		edge *link;
		while (neighbour != NULL)
		{
			link = &g->edges[neighbour->edge_id];
			
			// Check if the edge has already been marked
			if (!link->is_edge && !link->is_frond && !link->color)
			{
				// If the neighbour has not already been visited, then the link is an edge
				if (!g->vertices[neighbour->id].color)
				{
					link->is_edge = 1;
					build_tree_graph_rec(g, neighbour->id);
				}
				// Otherwise, the link brings to an already visited node, so it's a frond
				else
				{
					link->is_frond = 1;
				}
			}
			neighbour = neighbour->next;
		}
	}
	return;
}


void color_edges_rec(graph *g, int cut_vertex_id, int node_id)
{
	int col = g->progress;
	vertex *v = &g->vertices[node_id];
	
	if (v->color == col + 1)
	{
		return;
	}
	
	// Color the edge with the bc component ID
	v->color = col + 1;
	add(v->bc_ids, g->progress);
	
	neighbour *neighbour = v->first_neighbour;
	edge *l;
	while (neighbour != NULL)
	{
		// TEMP
		g->iterations++;
		
		l = &g->edges[neighbour->edge_id];
		if (!l->color)
		{
			l->color = col;
			
			// Don't exit from the bc component, so don't iterate through the cut-vertex (is the only way out)
			if (neighbour->id != cut_vertex_id)
			{
				color_edges_rec(g, cut_vertex_id, neighbour->id);
			}
		}
		
		neighbour = neighbour->next;
	}
	
}


// Finds all the biconnected components inside a connected component
int find_biconnected_components_rec(graph *g, int node_id)
{
	vertex *v = &g->vertices[node_id];
	
	if (v != NULL && !v->color)
	{
		v->color = 1;
		
		// Single node case, not useful in this structure
		if (v->first_neighbour == NULL)
		{
			add(v->bc_ids, g->progress);
			g->progress++;
			return 0;
		}
		
		// This variable will store the lowpoint of each neighbour
		int neighbour_lowpoint = v->id_tree;
		// This variable will store the lowpoint to pass to the node's parent
		int node_lowpoint = v->id_tree;
		
		int neighbour_id_tree;
		edge *l;
		neighbour *n = v->first_neighbour;
		while (n != NULL)
		{
			// TEMP
			g->iterations++;
			
			l = &g->edges[n->edge_id];
			neighbour_id_tree = g->vertices[n->id].id_tree;
			
			// If the link is an edge and brings to a son of the node (in the tree graph), go forward
			if (l->is_edge && v->id_tree < neighbour_id_tree) 
			{
				neighbour_lowpoint = find_biconnected_components_rec(g, n->id);
				// If the neighbour lowpoint is greater of equal the node's tree ID, then you found a biconnected component
				if (neighbour_lowpoint >= v->id_tree)
				{
					add(v->bc_ids, g->progress);
					/* 
					 * Color th edges of the biconnected component, here we are iterating from the neighbour ID, not from the node itself.
					 * So the cost will be in the order of the bc component's links.
					*/
					color_edges_rec(g, v->id, n->id);
					g->progress++;
				}
				else
				{
					// Update the lowpoint
					node_lowpoint = node_lowpoint > neighbour_lowpoint ? neighbour_lowpoint : node_lowpoint;
				}
			}
			// Update the lowpoint
			else if (l->is_frond && v->id_tree > neighbour_id_tree)
			{
				node_lowpoint = node_lowpoint > neighbour_id_tree ? neighbour_id_tree : node_lowpoint;
			}
			
			n = n->next;
		}
		
		return node_lowpoint;
	}
	return 0;
}


int calculate_biconnected_components(graph *g, int k_core)
{
	clear_colors(g);
	
	if (k_core) {
		detect_k_cores(g, k_core);
	}

	int i;
	vertex *v;
	for (i = 1; i <= g->num_vertices; i++)
	{
		v = &g->vertices[i];
		// For each node, initiate the list where the biconnected component IDs are going to be stored
		v->bc_ids = (List *)malloc(sizeof(List));
		initiate_list(v->bc_ids);
	}
	
	// You need to count the number of connected components and store a node ID for each of them
	int num_components = color_graph_rec(g, !k_core);
	int *root_nodes = get_root_nodes_index(g, num_components);
	
	clear_colors(g);
	if (k_core) {
		detect_k_cores(g, k_core);
		exclude_edges(g);
	}

	int num_bc = 0;
	if (root_nodes != NULL)
	{
		// For each connected component, build a tree (inside its structure)
		for (i = 0; i < num_components; i++)
		{
			build_tree_graph_rec(g, root_nodes[i]);
		}
		
		clear_colors(g);
		if (k_core) {
			detect_k_cores(g, k_core);
		}
		g->progress = 1;
		
		// For each connected component, find all the biconnected components
		for (i = 0; i < num_components; i++)
		{
			find_biconnected_components_rec(g, root_nodes[i]);
		}
		num_bc = g->progress - 1;
		
		if (!k_core) {
			// Order node's edges based on biconnected component
			int *bc_ind = (int *)calloc(num_bc, sizeof(int));
			List *l;
			for (i = 1; i <= g->num_vertices; i++)
			{
				v = &g->vertices[i];
				l = v->bc_ids;

				// If a node has only one biconnected component, there's no need to sort the edges
				if (l->size <= 1)
				{
					l->head->first_neighbour_bc = v->first_neighbour;
				}
				else
				{
					order_node_edges_by_bc(g, v, bc_ind);
				}
			}	
			free(bc_ind);
		}
		
		clear_colors(g);
		free(root_nodes);
	}
	
	return num_bc;
}


int color_graph_it(graph* g) {
    if (g == NULL || g->vertices == NULL || g->num_vertices <= 0) 
	{
		return 0;
	}
	
	clear_colors(g);
	
	int i, top, current_id, color = 0;
	vertex *v, *neighbour_v;
	neighbour *neighbour;
	int stack[g->num_vertices];
	
	for (i = 1; i <= g->num_vertices; i++) 
		{
		if (!g->vertices[i].color) 
			{
			color++;
			top = 0;
			stack[top] = i;
			g->vertices[i].queued = 1;
			
			while (top != -1) {
				current_id = stack[top--];
				v = &g->vertices[current_id];
				
				if (!v->color) 
					{
					v->color = color;
					
					neighbour = v->first_neighbour;
					while (neighbour != NULL)
					{
						neighbour_v = &g->vertices[neighbour->id];
						
						if (!neighbour_v->queued) 
						{
							stack[++top] = neighbour->id;
							neighbour_v->queued = 1;
						} 
						
						neighbour = neighbour->next;
					}
				}
			}
		}
	}
	
	return color;
}


void build_tree_graph_it(graph *g, int node_id)
{
	int stack[g->num_vertices];
	int top = 0;
	vertex *v, *neighbour_v;
	neighbour* neighbour;
	edge *link;
	
	stack[top] = node_id; 
	g->vertices[stack[top]].queued = 1;
	while (top != -1)
	{
		v = &g->vertices[stack[top--]];
		
		if (!v->color)
		{
			v->color = 1;
			
			v->id_tree = ++g->progress;
			neighbour = v->first_neighbour;
			
			while (neighbour != NULL)
			{
				link = &g->edges[neighbour->edge_id];
				if (!link->is_edge && !link->is_frond)
				{
					neighbour_v = &g->vertices[neighbour->id];
					if (!neighbour_v->color && !neighbour_v->queued)
					{
						link->is_edge = 1;
						neighbour_v->queued = 1;
						stack[++top] = neighbour_v->id;
					}
					else
					{
						link->is_frond = 1;
					}
				}
				
				neighbour = neighbour->next;
			}
		}
	}
}


int find_max_int(int* array, int size) {
	if (array == NULL || size <= 0) {
		return -1;
	}

	int max = array[0];
	for (int i = 1; i < size; i++) {
		if (array[i] > max) {
			max = array[i];
		}
	}

	return max;
}


int* find_node_coreness(graph* g) {
	if (g == NULL) {
		return NULL;
	}

	int i, max_degree, start, num;
	int* coreness = get_degrees(g);
	
	max_degree = find_max_int(coreness, g->num_vertices);

	int* bin = (int *)calloc(max_degree + 1, sizeof(int));
	int* sort = (int *)malloc(sizeof(int) * g->num_vertices);
	int* pos = (int *)malloc(sizeof(int) * g->num_vertices);

	for (i = 0; i < g->num_vertices; i++) {
		bin[coreness[i]]++;
	}

	start = 0;
	for (i = 0; i < max_degree + 1; i++) {
		num = bin[i];
		bin[i] = start;
		start += num;
	}
	
	for (i = 0; i < g->num_vertices; i++) {
		sort[bin[coreness[i]]++] = i;
	}

	for (i = max_degree; i > 0; i--) {
		bin[i] = bin[i - 1];
	}
	bin[i] = 0;

	for (i = 0; i < g->num_vertices; i++) {
		pos[sort[i]] = i;
	}

	int node_id, node_degree, neighbour_id, neighbour_degree;
	int f_sort, f_pos, s_pos, pos_from, pos_to;
	neighbour* neighbour;
	for (i = 0; i < g->num_vertices; i++) {
		node_id = sort[i];
		node_degree = coreness[node_id];

		neighbour = g->vertices[node_id + 1].first_neighbour;
		while (neighbour != NULL) {
			neighbour_id = neighbour->id - 1;
			neighbour_degree = coreness[neighbour_id];

			if (neighbour_degree > node_degree) {
				coreness[neighbour_id]--;

				pos_from = bin[neighbour_degree];
				pos_to = neighbour_id;

				f_sort = sort[pos_from];
				f_pos = pos[sort[pos_from]];
				s_pos = pos[pos_to];

        		sort[pos_from] = pos_to;
        		sort[pos[pos_to]] = f_sort;

        		pos[f_sort] = s_pos;
        		pos[pos_to] = f_pos;

				bin[neighbour_degree]++;
			}

			neighbour = neighbour->next;
		}
	}

	for (i = 0; i < g->num_vertices; i++) {
		g->vertices[i + 1].k_core = coreness[i];
	}

	free(bin);
	free(sort);
	free(pos);	

	return coreness;
}


int detect_k_cores(graph* g, int k) {
	clear_colors(g);

	int num_k_nodes = g->num_vertices;
	for (int i = 1; i <= g->num_vertices; i++) {
		if (g->vertices[i].k_core < k) {
			g->vertices[i].color = 100000;
			num_k_nodes--;
		}
	}

	return num_k_nodes;
}
