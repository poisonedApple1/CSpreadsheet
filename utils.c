#include "utils.h"
#include "sheet_functions.h"
#include <stdbool.h>
#include "avl.h"

int min(int a, int b){
    return (a < b) ? a : b;
}

int max(int a, int b){
    return (a > b) ? a : b;
}

bool is_valid_cell(int row, int col){
    return (row >= 0 && row < sheet.rows && col >= 0 && col < sheet.cols);
}

//queue implementation


queue *create_node(avl_node *node){
    queue *new_node = (queue *)malloc(sizeof(queue));
    new_node->node = node;
    new_node->next = NULL;
    return new_node;
}

queue *push(queue *head, avl_node *node){
    queue *temp = head;
    if(temp==NULL){
        return create_node(node);
    }
    while(temp->next!=NULL){
        temp=temp->next;
    }
    temp->next = create_node(node);
    return head;
}


avl_node *front(queue *head){
    return head->node;
}

queue *pop(queue *head){
    queue *temp = head->next;
    free(head);
    return temp;
}

bool is_empty(queue *head){
    return head == NULL;
}

int count(queue *head){
    int count = 0;
    queue *temp = head;
    while(temp!=NULL){
        count++;
        temp = temp->next;
    }
    return count;   
}
