#ifndef CALCULATE_FUNCTIONS_H
#define CALCULATE_FUNCTIONS_H

int SUM(int val_row1, int c1, int val_row2, int c2);
int MIN(int val_row1, int c1, int val_row2, int c2);
int MAX(int val_row1, int c1, int val_row2, int c2);
int AVG(int val_row1, int c1, int val_row2, int c2);
int STDEV(int val_row1, int c1, int val_row2, int c2);
int compute_cell(char op_code, int cell_value, int cell_value2);
int compute_range_func(char op_code, int row1, int col1, int row2, int col2);

#endif

