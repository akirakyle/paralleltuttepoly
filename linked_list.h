#ifndef  _LINKED_LIST_H_    /* only process this file once */
#define  _LINKED_LIST_H_

/*
  A doubly linked list implementing both stacks and queue
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct ll_node ll_node;
struct ll_node
{
  ll_node *next;
  ll_node *prev;
  void *data;
};

typedef struct linked_list
{
  ll_node *start;
  ll_node *end;
  int len;
} linked_list;

typedef linked_list stack;
typedef linked_list queue;

linked_list *linked_list_new();
void linked_list_free(linked_list *ll);
void linked_list_print(FILE *fout, linked_list *ll,
                       void(*print_fn)(FILE*, void *));

void linked_list_add_start(linked_list *ll, void *data);
void linked_list_add_end(linked_list *ll, void *data);

void *linked_list_remove_start(linked_list *ll);
void *linked_list_remove_end(linked_list *ll);

stack *stack_new();
void stack_free(stack *s);
void stack_print(FILE *fout, stack *s, void(*print_fn)(FILE*, void *));
void stack_push(stack *s, void *data);
void* stack_pop(stack *s);

queue *queue_new();
void queue_free(queue *q);
void queue_print(FILE *fout, stack *s, void(*print_fn)(FILE*, void *));
void queue_enq(stack *s, void *data);
void* queue_deq(stack *s);

#endif /* _LINKED_LIST_H_  */
