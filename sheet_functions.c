#include "sheet_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "utils.h"
#include "parser.h"
#include <math.h> 
Sheet sheet;

void initialise_sheet(int m, int n){
    sheet.rows = m;
    sheet.cols = n;
    sheet.data = malloc(m * sizeof(Cell *));
    for (int i = 0; i < m; i++){
        sheet.data[i] = malloc(n * sizeof(Cell));
        for (int j = 0; j < n; j++){
            sheet.data[i][j].value = 0;
            sheet.data[i][j].op_code = 'X';
            sheet.data[i][j].cell1_col = -1;
            sheet.data[i][j].cell1_row = -1;
            sheet.data[i][j].cell2_col = -1;
            sheet.data[i][j].cell2_row = -1;
            sheet.data[i][j].dep_count = 0;
            // allocating  dependencies list as NULL
            sheet.data[i][j].dependencies = NULL;
        }
    }
}

char *column_name(int column_number){  //column number to col name
    static char name[4];
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    name[2] = letters[column_number % 26];
    column_number /= 26;
    if (column_number != 0){
        name[1] = letters[(column_number - 1) % 26];
        column_number = (column_number - 1) / 26;
    }
    else{
        name[1] = ' ';
    }

    if (column_number != 0)
        name[0] = letters[(column_number - 1) % 26];
    else
        name[0] = ' ';
    
    name[3] = '\0';
    return name;
}

int get_col(char *col_name){ // column name to col number
    int col = -1;
    if (strlen(col_name) == 1)     
        col = col_name[0] - 'A';
    else if (strlen(col_name) == 2)
        col = (col_name[0] - 'A' + 1) * 26 + (col_name[1] - 'A');
    else if (strlen(col_name) == 3)
        col = 26 * 26 * (col_name[0] - 'A' + 1) + (col_name[1] - 'A' + 1) * 26 + (col_name[2] - 'A');
    return col;
}


void print_table( int column_start, int row_start){  //printing the table including the shift
    int m = sheet.rows;
    int n = sheet.cols;
    const int display_column_number = column_start + min(n, 10), display_row_number = row_start + min(m, 10);
    const int space = 10;
    printf("%*s", space, "");
    for (int i = column_start; i < display_column_number; i++){
        printf("%*s", space, column_name(i));
    }
    printf("\n");
    for (int i = row_start; i < display_row_number; i++){
        printf("%*d", space, i + 1);
        for (int j = column_start; j < display_column_number; j++){
            printf("%*d", space, sheet.data[i][j].value);
        }
        printf("\n");
    }
}


void free_sheet(){
    for(int i =0; i<sheet.rows; i++){
        for(int j = 0; j<sheet.cols; j++){
            Node *temp = sheet.data[i][j].dependencies;
            while(temp!=NULL){
                Node *temp2 = temp->next;
                free(temp);
                temp = temp2;
            }
        }
        free(sheet.data[i]);
    }
    free(sheet.data);
}


void remove_from_list(Cell *target, Cell *dep) {  // remove dependency from list of target cell
    Node **curr = &(target->dependencies);  // Pointer to head of list
    while (*curr != NULL) {
        if ((*curr)->data == dep) {
            Node *temp = *curr;
            *curr = temp->next;  // Unlink the node
            free(temp);  // Free only the node, not temp->data
            target->dep_count--; 
            break;  // Exit after removing the first match
        }
        curr = &((*curr)->next);  // Move to the next node
    }
}


void remove_dependency(Cell *cell){
    /*removes the cell from dependency array of cell1 and cell2*/
    if(cell->cell1_col == -1) return;
    Cell *cell1 = &sheet.data[cell->cell1_row][cell->cell1_col];
    remove_from_list(cell1, cell); 
    if(cell->cell2_col == -1) return;
    Cell *cell2 = &sheet.data[cell->cell2_row][cell->cell2_col];
    remove_from_list(cell2, cell); 
    cell->cell1_col = -1;
    cell->cell1_row = -1;
    cell->cell2_col = -1;
    cell->cell2_row = -1;
}
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
    Z -> sleep
    X -> const op const,const
    p -> const+cell or cell+const
    s -> const-cell or cell-const
    u -> const*cell or cell*const
    d -> const/cell or cell/const
    */
   
   
   bool recalculate(Cell *cell){
       //returns false if error in calculation
       if(cell == NULL) return true;
       int ans=-1;
       switch(cell->op_code){
           case '=':  ans=sheet.data[cell->cell1_row][cell->cell1_col].value; break;
           case '+':  ans=compute_cell(cell->op_code,sheet.data[cell->cell1_row][cell->cell1_col].value,sheet.data[cell->cell2_row][cell->cell2_col].value); break;
           case '-':  ans=compute_cell(cell->op_code,sheet.data[cell->cell1_row][cell->cell1_col].value,sheet.data[cell->cell2_row][cell->cell2_col].value); break;
           case '*':  ans=compute_cell(cell->op_code,sheet.data[cell->cell1_row][cell->cell1_col].value,sheet.data[cell->cell2_row][cell->cell2_col].value);    break;
           case '/':  ans=compute_cell(cell->op_code,sheet.data[cell->cell1_row][cell->cell1_col].value,sheet.data[cell->cell2_row][cell->cell2_col].value);    break;
           case 'p':  ans=compute_cell(cell->op_code,sheet.data[cell->cell1_row][cell->cell1_col].value,cell->cell2_col + cell->cell2_row* pow(2,16));    break;
           case 's':  ans=compute_cell(cell->op_code,sheet.data[cell->cell1_row][cell->cell1_col].value,cell->cell2_col + cell->cell2_row* pow(2,16));  break;
           case 'u':  ans=compute_cell(cell->op_code,sheet.data[cell->cell1_row][cell->cell1_col].value,cell->cell2_col + cell->cell2_row* pow(2,16));   break;
           case 'd':
           int val=cell->cell2_col + cell->cell2_row* pow(2,16);
           char op_code=get_op_code_rev(cell->op_code);
           ans=compute_cell(op_code,sheet.data[cell->cell1_row][cell->cell1_col].value,val);
           if(ans==-1) return false;
           cell->value=ans;
           break;
           case 'Z':
            ans=sheet.data[cell->cell1_row][cell->cell1_col].value;
            sleep(ans);
            break;
            case 'S':  ans=compute_range_func(cell->op_code,cell->cell1_row,cell->cell1_col,cell->cell2_row,cell->cell2_col);  break;
            case 'm':  ans=compute_range_func(cell->op_code,cell->cell1_row,cell->cell1_col,cell->cell2_row,cell->cell2_col);   break;
            case 'M':  ans=compute_range_func(cell->op_code,cell->cell1_row,cell->cell1_col,cell->cell2_row,cell->cell2_col);   break;
            case 'A':  ans=compute_range_func(cell->op_code,cell->cell1_row,cell->cell1_col,cell->cell2_row,cell->cell2_col);  break;
            case 'D':  ans=compute_range_func(cell->op_code,cell->cell1_row,cell->cell1_col,cell->cell2_row,cell->cell2_col);   break;
            case 'X': return true;
        }
        if(ans==-1) return false;
        cell->value=ans;
        Node *temp = cell->dependencies;
        
        bool result = true;
        while(temp!=NULL){
            result = result && recalculate(temp->data);
            if(!result) return false;
            temp = temp->next;
        }
        return true;
    }
    
    void add_constraints(Cell *cell,short cell1_col,short cell1_row,short cell2_col,short cell2_row,int value,char op_code){
        remove_dependency(cell);
        cell->cell1_col = cell1_col;
        cell->cell1_row = cell1_row;
        cell->cell2_col = cell2_col;
        cell->cell2_row = cell2_row;
        
        if(op_code=='S' || op_code=='m' || op_code=='M' || op_code=='A' || op_code=='D'){
        for(int i=cell1_row;i<=cell2_row;i++){
            for(int j=cell1_col;j<=cell2_col;j++){ 
                Node *new_node = malloc(sizeof(Node));
                new_node->data = cell;
                new_node->next = sheet.data[i][j].dependencies;  // Insert at the head
                sheet.data[i][j].dependencies = new_node;
                sheet.data[i][j].dep_count++; 
            }
        }
    }
    else{
        if(cell1_col!=-1){  
            Node *temp = malloc(sizeof(Node));
            temp->data = cell;
            temp->next = sheet.data[cell1_row][cell1_col].dependencies;
            sheet.data[cell1_row][cell1_col].dependencies = temp;

            // sheet.data[cell1_row][cell1_col].dependencies[sheet.data[cell1_row][cell1_col].dep_count] = cell;
            sheet.data[cell1_row][cell1_col].dep_count++;
        }
        if(cell2_col!=-1){
            Node *temp = malloc(sizeof(Node));
            temp->data = cell;
            temp->next = sheet.data[cell2_row][cell2_col].dependencies;
            sheet.data[cell2_row][cell2_col].dependencies = temp;

            // sheet.data[cell2_row][cell2_col].dependencies[sheet.data[cell2_row][cell2_col].dep_count] = cell;
            sheet.data[cell2_row][cell2_col].dep_count++;
        }
    }

    cell->value = value;
    cell->op_code = op_code;
    Node *temp = cell->dependencies;
    while(temp!=NULL){  
        recalculate(temp->data);
        temp = temp->next;
    }
    // for(int i = 0; i < cell->dep_count; i++){
    //     recalculate(cell->dependencies[i]);
    // }
}
