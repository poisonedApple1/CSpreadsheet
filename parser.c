#include "parser.h"
#include "calculate_functions.h"
#include "sheet_functions.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int column_start = 0, row_start = 0;
bool print_allowed = true;
char status[50] = "ok";
int sleep_timer = 0;

/*
    = ->cell
    + -> cell+cell
    - ->cell-cell
    * -> cell*cell
    / -> cell/cell
    S -> SUM
    m -> MIN
    M -> MAX
    A -> AVG
    D -> STDEV
    Z -> sleep(cell)
    X -> const op const,const , sleep(const)
    p -> const+cell or cell+const
    s -> const-cell or cell-const
    u -> const*cell or cell*const
    d -> cell/const
    b -> const/cell
*/

char get_op_code(char op_code,bool constopcell) {
  // function to get opcode for the case of int op cell or cell op int
  if (op_code == '+')
    return 'p';
  else if (op_code == '-')
    return 's';
  else if (op_code == '*')
    return 'u';
  else if (op_code == '/' && constopcell)
    return 'b';
  else if (op_code == '/' && !constopcell)
    return 'd';
  return -1;
}

char get_op_code_rev(char op_code) {
  // function to get operation from opcode for the case of int op cell or cell
  // op int
  if (op_code == 'p')
    return '+';
  else if (op_code == 's')
    return '-';
  else if (op_code == 'u')
    return '*';
  else if (op_code == 'd')
    return '/';
  return -1;
}

char func_to_op_code(char *func) {
  // function for getting opcode for the case of func(cell:cell)
  if (strcmp(func, "SUM") == 0)
    return 'S';
  else if (strcmp(func, "MIN") == 0)
    return 'm';
  else if (strcmp(func, "MAX") == 0)
    return 'M';
  else if (strcmp(func, "AVG") == 0)
    return 'A';
  else if (strcmp(func, "STDEV") == 0)
    return 'D';
  return 'X';
}


void remove_space(char *command) {
  //funciton to remove the spaces in the input command
  char *i = command, *j = command;
  while (*i != '\0') {
    if (*i != ' ') {
      *j++ = *i;
    }
    i++;
  }
  *j = '\0';
}

void parser(char *command) {
  command[strcspn(command, "\n")] = 0; // remove \n
  remove_space(command);               // remove spaces
  char ref_col[10];
  int ref_row;
  char val_col1[10], val_col2[10];
  int val_row1, val_row2;
  int val1, val2;
  int v1, v2;
  char op;
  char func[10];
  int slashn;
  int ans;

  if (strcmp(command, "w") == 0) {

    if (row_start > 0)
      row_start = max(row_start - 10, 0);
    strcpy(status, "ok");
  } 
  
  else if (strcmp(command, "s") == 0) {
    if (row_start < sheet.rows - 10)
      row_start += 10;
    strcpy(status, "ok");
  }
  
  else if (strcmp(command, "a") == 0) {
    if (column_start > 0)
      column_start = max(column_start - 10, 0);
    strcpy(status, "ok");
  } 
  
  else if (strcmp(command, "d") == 0) {
    if (column_start < sheet.cols - 10)
      column_start += 10;
    strcpy(status, "ok");
  }
  
  else if (strncmp(command, "scroll_to", 9) == 0) {
    if (sscanf(command, "scroll_to%[A-Z]%d%n", ref_col, &ref_row,&slashn) == 2
        && command[slashn]=='\0') {
      int c = get_col(ref_col);
      int r = ref_row - 1;
      if (is_valid_cell(r, c)) {
        column_start = c;
        row_start = r;
        strcpy(status, "ok");
      } else
        strcpy(status, "Invalid cmd");
    } else {
      strcpy(status, "Invalid format: Use 'scroll_to [A-Z][row]'");
    }
  } 
  
  else if (strcmp(command, "enable_output") == 0) {
    print_allowed = true;
    strcpy(status, "ok");
  } 
  
  else if (strcmp(command, "disable_output") == 0) {
    print_allowed = false;
    strcpy(status, "ok");
  }
  // cell = int op int
  else if (sscanf(command, "%[A-Z]%d=%d%c%d%n", ref_col, &ref_row, &val1, &op,
                  &val2,&slashn) == 5
                  && command[slashn]=='\0' && (op == '+' || op == '-' || op == '*' || op == '/')) {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    if (is_valid_cell(row, col)) {
      ans = compute_cell(op, val1, val2);
      sheet.data[row][col].value = ans;
      op = 'X';
      cell_info cell = {row, col};
      cell_info cell1 = {-1, -1};
      cell_info cell2 = {-1, -1};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, op);
    } else {
      strcpy(status, "Invalid cmd");
    }
  }
  // cell = cell op cell
  else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%[A-Z]%d%n", ref_col, &ref_row,
                  val_col1, &val_row1, &op, val_col2, &val_row2,&slashn) == 7
                  && command[slashn]=='\0' && (op == '+' || op == '-' || op == '*' || op == '/')) {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    int col1 = get_col(val_col1);
    int col2 = get_col(val_col2);
    if (is_valid_cell(row, col) && is_valid_cell(val_row1 - 1, col1) && 
    is_valid_cell(val_row2 - 1, col2) ) {  
      cell_info cell = {row, col};
      cell_info cell1 = {val_row1 - 1, col1};
      cell_info cell2 = {val_row2 - 1, col2};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, op);
    } else strcpy(status, "Invalid cmd");
  }

  /*
      for case of int op cell and cell op int
      i divide the constant integer(int->32 bits) into two short (16 bit)
     variables and store it in place of cell2_col and cell2_row and then use it
     whenever recalculation needed.
   */
  // cell = int op cell
  else if (sscanf(command, "%[A-Z]%d=%d%c%[A-Z]%d%n", ref_col, &ref_row, &val1,
                  &op, val_col1, &val_row1,&slashn) == 6
                  && command[slashn]=='\0' && (op == '+' || op == '-' || op == '*' || op == '/')) {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    int col1 = get_col(val_col1);
    if (is_valid_cell(row, col) && is_valid_cell(val_row1 - 1, col1) ) {


      // splitting the constant into two 16 bit variables using bit operations
      int const1 = val1 & 0xFFFF;
      int const2 = (val1 >> 16) & 0xFFFF;
      
      op = get_op_code(op,true);
      cell_info cell = {row, col};
      cell_info cell1 = {val_row1 - 1, col1};
      cell_info cell2 = {const2, const1};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, op);

    } else strcpy(status, "Invalid cmd");

  }
  // cell = cell op int
  else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%d%n", ref_col, &ref_row, val_col1,
                  &val_row1, &op, &val1,&slashn) == 6
                  && command[slashn]=='\0' && (op == '+' || op == '-' || op == '*' || op == '/')) {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    int col1 = get_col(val_col1);

    if (is_valid_cell(row, col) && is_valid_cell(val_row1 - 1, col1) ) {
      
      // splitting the constant into two 16 bit variables
      int const1 = val1 & 0xFFFF;
      int const2 = (val1 >> 16) & 0xFFFF;

      op = get_op_code(op,false);
      cell_info cell = {row, col};
      cell_info cell1 = {val_row1 - 1, col1};
      cell_info cell2 = {const2, const1};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, op);
    } else strcpy(status, "Invalid cmd");
  }
  // cell = func(cell:cell)
  else if (sscanf(command, "%[A-Z]%d=%5[A-Z](%[A-Z]%d:%[A-Z]%d)%n", ref_col,
                  &ref_row, func, val_col1, &val_row1, val_col2,
                  &val_row2,&slashn) == 7
                  && command[slashn]=='\0') {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    int col1 = get_col(val_col1);
    int col2 = get_col(val_col2);
    if (is_valid_cell(row, col) && is_valid_cell(val_row1 - 1, col1) &&
        is_valid_cell(val_row2 - 1, col2)) { 


      char op_code = func_to_op_code(func);

      cell_info cell1 = {val_row1 - 1, col1};
      cell_info cell2 = {val_row2 - 1, col2};
      cell_info cell = {row, col};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, op_code);
    } else {
      strcpy(status, "Invalid cmd");
    }
  }
  // cell = int
  else if (sscanf(command, "%[A-Z]%d=%d%n", ref_col, &ref_row, &val1,&slashn) == 3
           && command[slashn]=='\0' ) {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    sheet.data[row][col].isError = false;
    sheet.data[row][col].value = val1;
    if (is_valid_cell(row, col) ) {
      cell_info cell = {row, col};
      cell_info cell1 = {-1, -1};
      cell_info cell2 = {-1, -1};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, 'X');
    } else {
      strcpy(status, "Invalid cmd");
    }
  }
  // cell = cell
  else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%n", ref_col, &ref_row, val_col1,
                  &val_row1,&slashn) == 4 
                  && command[slashn]=='\0') {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    int col1 = get_col(val_col1);
    int row1 = val_row1 - 1; 
    if (is_valid_cell(row, col) && is_valid_cell(row1, col1)  ) {
      cell_info cell = {row, col};
      cell_info cell1 = {row1, col1};
      cell_info cell2 = {-1, -1};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, '=');
    } else {
      strcpy(status, "Invalid cmd");
    }
  }
  // sleep(int)
  else if (sscanf(command, "%[A-z]%d=SLEEP(%d)%n", ref_col, &ref_row, &val1,&slashn) == 3
           && command[slashn]=='\0') {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    sheet.data[row][col].isError = false;
    sheet.data[row][col].value = val1;
    if (is_valid_cell(row, col) && val1 >= 0 ) {
      cell_info cell = {row, col};
      cell_info cell1 = {-1, -1};
      cell_info cell2 = {-1, -1};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, 'X');
      sleep(val1);
    } else if (is_valid_cell(row, col) && val1 < 0 ) {
      cell_info cell = {row, col};
      cell_info cell1 = {-1, -1};
      cell_info cell2 = {-1, -1};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, 'X');
    } else {
      strcpy(status, "Invalid cmd");
    }
  }
  // cell = sleep(cell)
  else if (sscanf(command, "%[A-z]%d=SLEEP(%[A-Z]%d)%n", ref_col, &ref_row,
                  val_col1, &val_row1,&slashn) == 4
                  && command[slashn]=='\0') {
    int col = get_col(ref_col);
    int row = ref_row - 1;
    int col1 = get_col(val_col1);
    int row1 = val_row1 - 1; 
    sheet.data[row][col].isError = false;
    sheet.data[row][col].value = sheet.data[row1][col1].value;
    if (is_valid_cell(row, col) && is_valid_cell(row1, col1) ) {
      cell_info cell = {row, col};
      cell_info cell1 = {row1, col1};
      cell_info cell2 = {-1, -1};
      strcpy(status, "ok");
      add_constraints(cell, cell1, cell2, 'Z');
    } else {
      strcpy(status, "Invalid cmd");
    }
  } else {
    strcpy(status, "Invalid cmd");
  }
}