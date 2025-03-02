#include "calculate_functions.h"
#include "sheet_functions.h"
#include "parser.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int SUM(int val_row1, int c1, int val_row2, int c2) {
  int a = 0;
  for (int i = val_row1; i <= val_row2; i++) {
    for (int j = c1; j <= c2; j++) {
      a += sheet.data[i][j].value;
    }
  }
  return a;
}

int MIN(int val_row1, int c1, int val_row2, int c2) {
  int a = pow(2, 31) - 1;
  for (int i = val_row1; i <= val_row2; i++) {
    for (int j = c1; j <= c2; j++) {
      int b = sheet.data[i][j].value;
      if (b < a)
        a = b;
    }
  }
  return a;
}

int MAX(int val_row1, int c1, int val_row2, int c2) {
  int a = 0;
  for (int i = val_row1; i <= val_row2; i++) {
    for (int j = c1; j <= c2; j++) {
      int b = sheet.data[i][j].value;
      if (b > a)
        a = b;
    }
  }
  return a;
}

int AVG(int val_row1, int c1, int val_row2, int c2) {
  int ans = 0;
  int total = SUM(val_row1, c1, val_row2, c2);
  int count = (val_row2 - val_row1 + 1) * (c2 - c1 + 1);
  if (count > 0) {
    ans = total / count;
  } else {
    return -1;
  }
  return ans;
}

int STDEV(int val_row1, int c1, int val_row2, int c2) {
  int avg = AVG(val_row1, c1, val_row2, c2);
  int total = 0;
  for (int i = val_row1; i <= val_row2; i++) {
    for (int j = c1; j <= c2; j++) {
      total += (sheet.data[i][j].value - avg) * (sheet.data[i][j].value - avg);
    }
  }

  double std =
      sqrt((double)total / ((val_row2 - val_row1 + 1) * (c2 - c1 + 1)));
  return (int)round(std);
}


int compute_cell(char op_code, int cell_value, int cell_value2) {
  // function to compute the value of the cell
  calc_error = false;
  if (op_code == '+')
    return cell_value + cell_value2;
  else if (op_code == '-')
    return cell_value - cell_value2;
  else if (op_code == '*')
    return cell_value * cell_value2;
  else if (op_code == '/') {
    if (cell_value2 == 0) {
      calc_error = true;
      return -1;
    }
    return cell_value / cell_value2;
  }
  strcpy(status, "Invalid cmd");
  return -1;
}

int compute_range_func(char op_code, int row1, int col1, int row2, int col2) {
  if (col1 > col2 || row1 > row2) {
    strcpy(status, "Invalid cmd");
    return -1;
  }
  if (op_code == 'S')
    return SUM(row1, col1, row2, col2);
  else if (op_code == 'm')
    return MIN(row1, col1, row2, col2);
  else if (op_code == 'M')
    return MAX(row1, col1, row2, col2);
  else if (op_code == 'A')
    return AVG(row1, col1, row2, col2);
  else if (op_code == 'D')
    return STDEV(row1, col1, row2, col2);
  return -1;
}