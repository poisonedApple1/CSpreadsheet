#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include "avl.h"

typedef struct queue {
    avl_node *node;
    struct queue *next;
} queue;

queue *create_node(avl_node *node);
queue *push(queue *head, avl_node *node);
avl_node *front(queue *head);
queue *pop(queue *head);
bool is_empty(queue *head);


int min(int a, int b);
int max(int a, int b);
bool is_valid_cell(int row, int col);
int count(queue *head);

#endif

