
#ifndef LIST_H
#define LIST_H

typedef struct node
{
	int item;
	struct node *next;
}Node;


typedef struct
{
	Node *head;
	Node *tail;

}List;

void initiate_list(List*);
void initiate_array_list(List**,int);
void add_tail(List*,int);
void free_list(List*);
void print_list(List*);

#endif