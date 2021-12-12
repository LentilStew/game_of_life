#include <stdio.h>
#include <stdlib.h>
typedef struct linked_list
{
    void *value;
    struct linked_list *next;
} linked_list;

linked_list *create_node(void *value, void *next);
void free_node(linked_list *ll);
void free_ll(linked_list *ll);