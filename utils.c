#include "utils.h"
#include "sheet_functions.h"
#include <stdbool.h>

int min(int a, int b){
    return (a < b) ? a : b;
}

int max(int a, int b){
    return (a > b) ? a : b;
}

bool is_valid_cell(int row, int col){
    return (row >= 0 && row < sheet.rows && col >= 0 && col < sheet.cols);
}

//stack implementation
typedef struct stack {
    Cell *cell;
    struct stack *next;
} stack;

stack *create_node(Cell *cell){
    stack *new_node = (stack *)malloc(sizeof(stack));
    new_node->cell = cell;
    new_node->next = NULL;
    return new_node;
}

void push(stack *head, Cell *new_node){
    if (head == NULL){
        head = create_node(new_node);
    }
    else{
        stack *temp = head;
        while (temp->next != NULL){
            temp = temp->next;
        }
        temp->next = create_node(new_node);
    }
}

Cell *top(struct stack *head){
    return head->cell;
}

void pop(stack *head){
    stack *temp = head;
    head = head->next;
    free(temp);
}

bool is_empty(stack *head){
    return head == NULL;
}