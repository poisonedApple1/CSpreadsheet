#ifndef SHEET_FUNCTIONS_H
#define SHEET_FUNCTIONS_H 
#include <stdbool.h>
#include <stdlib.h>



typedef struct cell{
    int value;
    bool isError;
    char op_code;
    short cell1_col;
    short cell1_row;
    short cell2_col;
    short cell2_row;
    int dep_count;
    struct node *dependencies;
} Cell;

typedef struct node{
    struct cell *data;
    struct node *next; 
}Node;

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
void remove_from_list(Cell *target,Cell *dep);
void remove_dependency(Cell *cell);
void recalculate(Cell *cell,bool error);
void add_constraints(Cell *cell,short cell1_col,short cell1_row,short cell2_col,short cell2_row,int value,char op_code);


#endif
