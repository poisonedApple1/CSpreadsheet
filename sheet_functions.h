#ifndef SHEET_FUNCTIONS_H
#define SHEET_FUNCTIONS_H
#define MAXDEP 10
#include <stdbool.h>


typedef struct cell{
    int value;
    char op_code;
    short cell1_col;
    short cell1_row;
    short cell2_col;
    short cell2_row;
    int dep_count;
    struct cell *dependencies[MAXDEP];
} Cell;

typedef struct
{
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
void remove_dependency(Cell *cell);
bool recalculate(Cell *cell);
void add_constraints(Cell *cell,short cell1_col,short cell1_row,short cell2_col,short cell2_row,int value,char op_code);

#endif
