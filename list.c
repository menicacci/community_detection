
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

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


void add_tail(List *lp, int item)
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

	printf("\t"); 
	while(node != NULL)
	{
		printf("%d ", node->item);
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


void initiate_array_list(List **l, int size) {
	for (int i = 0; i < size; i++) {
		l[i] = (List *) malloc(sizeof(List));
		initiate_list(l[i]);
	}
}
