#ifndef SHEET_FUNCTIONS_H
#define SHEET_FUNCTIONS_H
#define MAXDEP 10


typedef struct
{
    char value[20];
    char formula[20];
    int dep_count;
    int dependencies[MAXDEP];
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

#endif
