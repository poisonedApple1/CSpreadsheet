#include "sheet_functions.h"
#include "calculate_functions.h"
#include "parser.h"
#include "utils.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
Sheet sheet;



//initialises the sheet by allocating memory and giving default values to all the fields of cell struct
void initialise_sheet(int m, int n) {
  sheet.rows = m;
  sheet.cols = n;
  sheet.data = malloc(m * sizeof(Cell *));
  for (int i = 0; i < m; i++) {
    sheet.data[i] = malloc(n * sizeof(Cell));
    for (int j = 0; j < n; j++) {
      sheet.data[i][j].value = 0;
      sheet.data[i][j].isError = false;
      sheet.data[i][j].op_code = 'X';
      sheet.data[i][j].cell1.col = -1;
      sheet.data[i][j].cell1.row = -1;
      sheet.data[i][j].cell2.col = -1;
      sheet.data[i][j].cell2.row = -1; 
      sheet.data[i][j].dependencies = NULL;
    }
  }
}


// column number to col name
char *column_name(int column_number) {
  static char name[4];
  char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  name[2] = letters[column_number % 26];
  column_number /= 26;
  if (column_number != 0) {
    name[1] = letters[(column_number - 1) % 26];
    column_number = (column_number - 1) / 26;
  } else {
    name[1] = ' ';
  }

  if (column_number != 0)
    name[0] = letters[(column_number - 1) % 26];
  else
    name[0] = ' ';

  name[3] = '\0';
  return name;
}

// column name to col number
int get_col(char *col_name) { 
  int col = -1;
  if (strlen(col_name) == 1)
    col = col_name[0] - 'A';
  else if (strlen(col_name) == 2)
    col = (col_name[0] - 'A' + 1) * 26 + (col_name[1] - 'A');
  else if (strlen(col_name) == 3)
    col = 26 * 26 * (col_name[0] - 'A' + 1) + (col_name[1] - 'A' + 1) * 26 +
          (col_name[2] - 'A');
  return col;
}

// printing the table,column start and row start are the top-left most cell to be printed
void print_table(int column_start,int row_start) {
  int m = sheet.rows;
  int n = sheet.cols;
  const int display_column_number = column_start + min(n - column_start, 10),
            display_row_number = row_start + min(m - row_start, 10);
  const int space = 10;
  printf("%*s", space, "");
  for (int i = column_start; i < display_column_number; i++) {
    printf("%*s", space, column_name(i));
  }
  printf("\n");
  for (int i = row_start; i < display_row_number; i++) {
    printf("%*d", space, i + 1);
    for (int j = column_start; j < display_column_number; j++) {
      if (sheet.data[i][j].isError)
        printf("       ERR");
      else
        printf("%*d", space, sheet.data[i][j].value);
    }
    printf("\n");
  }
}


//frees the memory allocated to the sheet
void free_sheet(){
    for (int i = 0; i < sheet.rows; i++){
        for (int j = 0; j < sheet.cols; j++){
            dependency_node *temp = sheet.data[i][j].dependencies;
            while (temp != NULL){
                dependency_node *temp2 = temp->next;
                free(temp);
                temp = temp2;
            }
        }
        free(sheet.data[i]);
    }
    free(sheet.data);
}

//removes a dependency(delete_cell) from the dependency linked list in the cell struct of the target
void remove_from_list(Cell *target, int delete_cell) {
  dependency_node *curr = (target->dependencies);
  dependency_node *prev = NULL;
  while (curr != NULL) {
    if (curr->data == delete_cell) {
      if (prev == NULL) {
        target->dependencies = curr->next;
      } else {
        prev->next = curr->next;
      }
      free(curr); 
      return;
    }
    prev = curr;
    curr = curr->next;
  } 
  return;
}

//adds a dependency(insert_cell) to the dependency linked list of the target cell struct.
void insert_into_list(Cell *target, int insert_cell) {
  dependency_node *new_node = (dependency_node *)malloc(sizeof(dependency_node));
  new_node->data = insert_cell;
  new_node->next = target->dependencies;
  target->dependencies = new_node; 
  return;
}

// dfs to find indegree 0 node in the avl tree
avl_node *dfs(avl_node *root) {
  if (root == NULL)
    return NULL;
  if (root->indegree == 0) {
    return root;
  } else {
    avl_node *left = dfs(root->left);
    avl_node *right = dfs(root->right);
    if (right != NULL)
      return right;
    else if (left != NULL)
      return left;
    else
      return NULL;
  }
}

//uses khan's algorithm to topologically sort the affected cells by the constraint and returns the sorted cells as a queue
queue *topological_sort(avl_tree *tree) {
  queue *q = NULL;
  avl_node *indegree_0_node = dfs(tree->root);
  q = push(q, indegree_0_node);
  queue *final_sorted = NULL;
  while (!is_empty(q)) {
    avl_node *node = front(q);
    q = pop(q);
    final_sorted = push(final_sorted, node);
    dependency_node *curr = sheet.data[node->data % 1000][node->data / 1000].dependencies;
    while (curr != NULL) {
      avl_node *temp = avl_find(tree, curr->data);
      temp->indegree--;
      if (temp->indegree == 0) {
        q = push(q, temp);
      }
      curr = curr->next;
    }
  }
  return final_sorted;
}

//given a cell, recalculates its value using the op code and cell values stored in its struct
void recalculate(Cell *cell) {
  if (cell == NULL)
    return;
  int ans;

  switch (cell->op_code) {
    case '=':
      if(sheet.data[cell->cell1.row][cell->cell1.col].isError){
        cell->isError = true;
        return;
      }
      cell->isError=false;
      ans = sheet.data[cell->cell1.row][cell->cell1.col].value;
      break;
    case '+':
    case '-':
    case '*':
    case '/':
      if (sheet.data[cell->cell1.row][cell->cell1.col].isError ||
          sheet.data[cell->cell2.row][cell->cell2.col].isError) {
        cell->isError = true;
        return;
      }
      int val1 = sheet.data[cell->cell1.row][cell->cell1.col].value;
      int val2 = sheet.data[cell->cell2.row][cell->cell2.col].value;
      ans = compute_cell(cell->op_code, val1, val2);
      if (calc_error)
        cell->isError = true;
      else
        cell->isError = false;
      break;
    case 'p':
    case 's':
    case 'u':
    case 'd':
    case 'b':
      if (sheet.data[cell->cell1.row][cell->cell1.col].isError) {
        cell->isError = true;
        return;
      }
      
      int val = (cell->cell2.row << 16) | (cell->cell2.col & 0xFFFF); 
      ans = compute_cell(cell->op_code,
                        sheet.data[cell->cell1.row][cell->cell1.col].value, val);
      if (calc_error)
        cell->isError = true;
      else
        cell->isError = false;
      cell->value = ans;
      break;
    case 'Z':
      if (sheet.data[cell->cell1.row][cell->cell1.col].isError) {
        cell->isError = true;
        return;
      }
      ans = sheet.data[cell->cell1.row][cell->cell1.col].value;
      sleep_timer+=ans;
      break;
    case 'S':
    case 'm':
    case 'M':
    case 'A':
    case 'D':
      for (int i = cell->cell1.row; i <= cell->cell2.row; i++) {
        for (int j = cell->cell1.col; j <= cell->cell2.col; j++) {
          if (sheet.data[i][j].isError) {
            cell->isError = true;
            return;
          }
        }
      }
      ans = compute_range_func(cell->op_code, cell->cell1.row, cell->cell1.col,
                              cell->cell2.row, cell->cell2.col);
      if (calc_error)
        cell->isError = true;
      else
        cell->isError = false;
      break;
    default:
      return;
  }
  cell->value = ans;
  calc_error=false;
  return;
}

//given the cell_info of a cell, it goes to the cells which depends on this cell and
// removes the cell from its dependency linked list
void remove_dependency(cell_info cell) {
  Cell *curr_cell = &sheet.data[cell.row][cell.col];
  switch (curr_cell->op_code) {
    case 'X':
      break;
    case '=':
    case 'p':
    case 's':
    case 'u':
    case 'd':
    case 'b':
    case 'Z':
      remove_from_list(&sheet.data[curr_cell->cell1.row][curr_cell->cell1.col],
                      cell.col * 1000 + cell.row);
      break;
    case 'S':
    case 'm':
    case 'M':
    case 'A':
    case 'D':
      for (int i = curr_cell->cell1.row; i <= curr_cell->cell2.row; i++) {
        for (int j = curr_cell->cell1.col; j <= curr_cell->cell2.col; j++) {
          remove_from_list(&sheet.data[i][j], cell.col * 1000 + cell.row);
        }
      }
      break;
    case '+':
    case '-':
    case '*':
    case '/':
      remove_from_list(&sheet.data[curr_cell->cell1.row][curr_cell->cell1.col],
                      cell.col * 1000 + cell.row);
      remove_from_list(&sheet.data[curr_cell->cell2.row][curr_cell->cell2.col],
                      cell.col * 1000 + cell.row);
      break;
  }
  curr_cell->cell1.col = -1;
  curr_cell->cell1.row = -1;
  curr_cell->cell2.col = -1;
  curr_cell->cell2.row = -1;
}

//makes an avl tree of all the affected cells when a new constraint is added to check for circle errors
//and then the tree is used for topological sorting
void add_to_tree(avl_tree *head, cell_info cell) {
  if (sheet.data[cell.row][cell.col].dependencies == NULL) {
    return;
  }
  dependency_node *curr = sheet.data[cell.row][cell.col].dependencies;
  while (curr != NULL) {
    cell_info temp = {curr->data % 1000, curr->data / 1000};
    avl_node *node = avl_find(head, curr->data);
    if(node == NULL){
      avl_insert(head, curr->data);
      add_to_tree(head, temp);
    }
    else{
      node->indegree++;
    }
    curr = curr->next;
  }
}

//used to check if the newly input constraint(in case the function is not a range function) 
//to a cell is causing a circular error or not
// return true if cycle is found
bool check_cycle(avl_tree *tree, cell_info cell1, cell_info cell2) {
  avl_node *node = avl_find(tree, cell1.col * 1000 + cell1.row);
  if (node != NULL) {
    return true;
  }
  if (cell2.col == -1 && cell2.row == -1) {
    return false;
  }
  avl_node *node2 = avl_find(tree, cell2.col * 1000 + cell2.row);
  if (node2 != NULL) {
    return true;
  }
  return false;
}

//used to check if the newly input constraint(in case the function is a range function) 
//to a cell is causing a circular error or not
// return true if cycle is found
bool check_cycle_range_funcs(avl_node *root, cell_info cell1, cell_info cell2) {
  if (root == NULL)
    return false;
  if (root->data / 1000 >= cell1.col && root->data / 1000 <= cell2.col &&
      root->data % 1000 >= cell1.row && root->data % 1000 <= cell2.row) {
    return true;
  }
  return check_cycle_range_funcs(root->left, cell1, cell2) ||
         check_cycle_range_funcs(root->right, cell1, cell2);
}

//function to manage addition of constraint 
//calls the make_tree function, cycle detection function
//calls the topological sorting function
//updates the dependency lists of the cells
//calls the recalculation function
void add_constraints(cell_info curr_cell, cell_info cell1, cell_info cell2,
                     char op_code) {
  Cell *cell = &sheet.data[curr_cell.row][curr_cell.col];
  int curr_cell_row_col = curr_cell.col * 1000 + curr_cell.row;
  int ans = 0;
  cell_info temp = {-1, -1};
  avl_tree *tree = avl_create();
  avl_insert(tree, curr_cell_row_col);
  tree->root->indegree = 0;
  add_to_tree(tree, curr_cell);
  switch (op_code) {
    case 'X':
      ans = sheet.data[curr_cell.row][curr_cell.col].value;
      remove_dependency(curr_cell);
      cell->op_code = op_code;
      cell->cell1.col = -1;
      cell->cell1.row = -1;
      cell->cell2.col = -1;
      cell->cell2.row = -1;
      break;
    case '=':
      if (check_cycle(tree, cell1, temp)) {
        strcpy(status, "circular error");
        return;
      }
      remove_dependency(curr_cell);
      cell->cell1.col = cell1.col;
      cell->cell1.row = cell1.row;
      cell->cell2.col = cell2.col;
      cell->cell2.row = cell2.row;
      cell->op_code = op_code;
      if (sheet.data[cell1.row][cell1.col].isError)
        calc_error = true;
      else
        ans = sheet.data[cell1.row][cell1.col].value;
      insert_into_list(&sheet.data[cell1.row][cell1.col], curr_cell_row_col);
      break;
    case 'p':
    case 's':
    case 'u':
    case 'd':
    case 'b':
    case 'Z':
      if (check_cycle(tree, cell1, temp)) {
        strcpy(status, "circular error");
        return;
      }
      remove_dependency(curr_cell);
      cell->cell1.col = cell1.col;
      cell->cell1.row = cell1.row;
      cell->cell2.col = cell2.col;
      cell->cell2.row = cell2.row;
      cell->op_code = op_code;
      int value = (cell2.row << 16) | (cell2.col & 0xFFFF); 
      if (sheet.data[cell1.row][cell1.col].isError) calc_error = true;
      else ans = compute_cell(op_code, sheet.data[cell1.row][cell1.col].value, value);
      insert_into_list(&sheet.data[cell1.row][cell1.col], curr_cell_row_col);
      break;
    case 'S':
    case 'm':
    case 'M':
    case 'A':
    case 'D':
      if (check_cycle_range_funcs(tree->root, cell1, cell2)) {
        strcpy(status, "circular error");
        return;
      }
      for (int i = cell1.row; i <= cell2.row; i++) {
        for (int j = cell1.col; j <= cell2.col; j++) {
          if (sheet.data[i][j].isError) { 
            calc_error = true;
            break;
          }
        }
        if (calc_error)
          break;
      }
      remove_dependency(curr_cell);
      cell->cell1.col = cell1.col;
      cell->cell1.row = cell1.row;
      cell->cell2.col = cell2.col;
      cell->cell2.row = cell2.row;
      cell->op_code = op_code;
      if(!calc_error) {
        ans = compute_range_func(op_code, cell1.row, cell1.col, cell2.row, cell2.col);
      }
      for (int i = cell1.row; i <= cell2.row; i++) {
        for (int j = cell1.col; j <= cell2.col; j++) {
          insert_into_list(&sheet.data[i][j], curr_cell_row_col);
        }
      }
      break;
    case '+':
    case '-':
    case '*':
    case '/':
      if (check_cycle(tree, cell1, cell2)) {
        strcpy(status, "circular error");
        return;
      }
      remove_dependency(curr_cell);
      cell->cell1.col = cell1.col;
      cell->cell1.row = cell1.row;
      cell->cell2.col = cell2.col;
      cell->cell2.row = cell2.row;
      cell->op_code = op_code;
      if (sheet.data[cell1.row][cell1.col].isError || sheet.data[cell2.row][cell2.col].isError) calc_error = true;
      else ans = compute_cell(op_code, sheet.data[cell1.row][cell1.col].value, sheet.data[cell2.row][cell2.col].value);
      insert_into_list(&sheet.data[cell1.row][cell1.col], curr_cell_row_col);
      insert_into_list(&sheet.data[cell2.row][cell2.col], curr_cell_row_col);
      break;
  }
  

  cell->value = ans;
  if(calc_error)
    cell->isError = true;
  else
    cell->isError = false;
  calc_error = false;
  if (cell->op_code == 'Z')
    sleep_timer+=ans;

  pretty_print(tree);
  queue *sorted = topological_sort(tree);

  while (!is_empty(sorted)) {
    avl_node *node = front(sorted);
    sorted = pop(sorted);
    if (node->data != curr_cell_row_col)
      recalculate(&sheet.data[node->data % 1000][node->data / 1000]);
  }
  free_avl(tree);
}
