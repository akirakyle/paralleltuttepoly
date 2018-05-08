#include "linked_list.h"

linked_list *
linked_list_new()
{
  linked_list *ll = (linked_list *)malloc(sizeof(linked_list));
  ll->len = 0;
  ll->start = NULL;
  ll->end = NULL;
  return ll;
}

void
linked_list_free(linked_list *ll)
{
  while (ll->len > 0) linked_list_remove_end(ll);
  free(ll);
}

void
linked_list_print(FILE *fout, linked_list *ll, void(*print_fn)(FILE *, void *))
{
  ll_node *lln = ll->start;
  fprintf(fout, "len=%d\n", ll->len);
  while (lln != NULL)
    {
      print_fn(fout, (void *)(lln->data));
      lln = lln->next;
    }
}

void
linked_list_add_start(linked_list *ll, void *data)
{
  ll_node *lln = (ll_node *)malloc(sizeof(ll_node));
  lln->next = ll->start;
  lln->prev = NULL;
  lln->data = data;
  if (ll->len != 0) ll->start->prev = lln;
  else ll->end = lln;
  ll->start = lln;
  ll->len++;
}

void
linked_list_add_end(linked_list *ll, void *data)
{
  ll_node *lln = (ll_node *)malloc(sizeof(ll_node));
  lln->next = NULL;
  lln->prev = ll->end;
  lln->data = data;
  if (ll->len != 0) ll->end->next = lln;
  else ll->start = lln;
  ll->end = lln;
  ll->len++;
}

void *
linked_list_remove_start(linked_list *ll)
{
  if (ll->len == 0) return NULL;
  ll_node *lln = ll->start;
  if (ll->len == 1)
    {
      ll->start = NULL;
      ll->end = NULL;
    }
  else {
    ll->start->next->prev = NULL;
    ll->start = ll->start->next;
  }
  ll->len--;
  void *data = lln->data;
  free(lln);
  return data;
}

void *
linked_list_remove_end(linked_list *ll)
{
  if (ll->len == 0) return NULL;
  ll_node *lln = ll->end;
  if (ll->len == 1)
    {
      ll->end = NULL;
      ll->start = NULL;
    }
  else {
    ll->end->prev->next = NULL;
    ll->end = ll->end->prev;
  }
  ll->len--;
  void *data = lln->data;
  free(lln);
  return data;
}


stack * stack_new() {return (stack *)linked_list_new();}
void stack_free(stack *s) {linked_list_free((linked_list *)s);}
void stack_print(FILE *fout, stack *s, void(*print_fn)(FILE*, void *))
{linked_list_print(fout, (linked_list *)s, print_fn);}
void stack_push(stack *s, void *data) {linked_list_add_end((linked_list *)s, data);}
void* stack_pop(stack *s) {return linked_list_remove_end((linked_list *)s);}

queue *queue_new() {return (queue *)linked_list_new();}
void queue_free(queue *q) {linked_list_free((linked_list *)q);}
void queue_print(FILE *fout, queue *s, void(*print_fn)(FILE*, void *))
{linked_list_print(fout, (linked_list *)s, print_fn);}
void queue_enq(queue *q, void *data) {linked_list_add_start((linked_list *)q, data);}
void* queue_deq(queue *q) {return linked_list_remove_start((linked_list *)q);}
