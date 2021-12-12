#include <stdio.h>
#include <stdlib.h>

typedef struct linked_list
{
    void *value;
    struct linked_list *next;
} linked_list;

linked_list *create_node(void *value, void *next)
{
    linked_list *new = malloc(sizeof(linked_list));
    if (!new)
    {
        printf("failed allocating new linked list node");
        return NULL;
    }

    new->next = next;
    new->value = value;
    return new;
}
void free_node(linked_list *ll)
{
    if (ll->value)
        free(ll->value);
    
    free(ll);
}
void free_ll(linked_list *ll)
{
    if (!ll)
        return;

    free_ll(ll->next);

    free_node(ll);
}
