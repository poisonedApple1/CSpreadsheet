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
