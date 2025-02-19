#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
void parser(char* command);
extern int column_start, row_start; 
extern bool print_allowed;
extern char status[50];
char get_op_code(char op_code);
char get_op_code_rev(char op_code);
char func_to_op_code(char *func);
int compute_cell(char op_code,int cell_value,int cell_value2);
extern bool calc_error;
int compute_range_func(char op_code,int row1,int col1,int row2,int col2);
void remove_space(char *command);
#endif

