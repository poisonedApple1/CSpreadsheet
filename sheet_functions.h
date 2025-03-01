#ifndef SHEET_FUNCTIONS_H
#define SHEET_FUNCTIONS_H 
#include <stdbool.h>
#include <stdlib.h>
#include "avl.h"

typedef struct cell_info{
    short row;
    short col;
}cell_info;


typedef struct node{
    int data;
    struct node *next; 
}dependency_node;


typedef struct cell{
    int value;
    bool isError;
    char op_code;
    cell_info cell1;
    cell_info cell2; 
    dependency_node *dependencies;
} Cell;



typedef struct{
    int rows;
    int cols;
    Cell **data;
} Sheet;



extern Sheet sheet;

void initialise_sheet(int m, int n);
char *column_name(int column_number);
int get_col(char *col_name);
void print_table(int column_start, int row_start);
void free_sheet();
void remove_from_list(Cell *target,int delete_cell);
void insert_into_list(Cell *target,int insert_cell);
void remove_dependency(cell_info cell);
void recalculate(Cell *cell);
void add_constraints(cell_info cell,cell_info cell1,cell_info cell2,int value,char op_code);


#endif
