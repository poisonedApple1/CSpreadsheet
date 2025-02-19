#include "calculate_functions.h"
#include "sheet_functions.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <math.h>
#include <unistd.h>
int column_start = 0, row_start = 0;
bool print_allowed = true;
char status[50] = "ok";
bool calc_error=false;




char get_op_code(char op_code){
    //function to get opcode for the case of int op cell or cell op int
    if (op_code == '+')      return 'p';
    else if (op_code == '-') return 's';
    else if (op_code == '*') return 'u';
    else if (op_code == '/') return 'd';
    return -1;
}

char get_op_code_rev(char op_code){
    //function to get operation from opcode for the case of int op cell or cell op int
    if (op_code == 'p')      return '+';
    else if (op_code == 's') return '-';
    else if (op_code == 'u') return '*';
    else if (op_code == 'd') return '/';
    return -1;
}

char func_to_op_code(char *func){
    //function for getting opcode for the case of func(cell:cell)
    if (strcmp(func, "SUM") == 0)       return 'S';
    else if (strcmp(func, "MIN") == 0)  return 'm';
    else if (strcmp(func, "MAX") == 0)  return 'M';
    else if (strcmp(func, "AVG") == 0)  return 'A';
    else if (strcmp(func, "STDEV") == 0)return 'D';
    return 'X';
}

int compute_cell(char op_code,int cell_value,int cell_value2){
    //function to compute the value of the cell
    if(op_code=='+')      return cell_value + cell_value2;
    else if(op_code=='-') return cell_value - cell_value2;
    else if(op_code=='*') return cell_value * cell_value2;
    else if(op_code=='/'){
        if(cell_value2==0){
            calc_error=true;
            return -1;
        }
        return cell_value / cell_value2;}
    strcpy(status,"Invalid cmd");
    return -1;
}

int compute_range_func(char op_code,int row1,int col1,int row2,int col2){
    if (col1 > col2 || row1 > row2){strcpy(status,"Invalid cmd");return -1;}
    if (op_code == 'S')      return SUM(row1, col1, row2, col2);
    else if (op_code == 'm') return MIN(row1, col1, row2, col2);
    else if (op_code == 'M') return MAX(row1, col1, row2, col2);
    else if (op_code == 'A') return AVG(row1, col1, row2, col2);
    else if (op_code == 'D') return STDEV(row1, col1, row2, col2);
    return -1;
}

void remove_space(char *command){
    char *i = command, *j = command;
    while (*i != '\0') {
        if (*i != ' ') {
            *j++ = *i;
        }
        i++;
    }
    *j = '\0';
}

void parser(char* command){
    command[strcspn(command, "\n")] = 0;  // remove \n
    remove_space(command);                // remove spaces4
    char ref_col[10];
    int ref_row;
    char val_col1[10], val_col2[10];
    int val_row1, val_row2;
    int val1, val2;
    int v1, v2;
    char op;
    char func[10];
    int ans;
    
    
    if (strcmp(command, "w") == 0){
        if (row_start > 0)
        row_start = max(row_start - 10, 0);
        strcpy(status, "ok");
    }
    else if (strcmp(command, "s") == 0){
        if (row_start < sheet.rows - 10)
        row_start += 10;
        strcpy(status, "ok");
    }
    else if (strcmp(command, "a") == 0){
        if (column_start > 0)
            column_start = max(column_start - 10, 0);
            strcpy(status, "ok");
    }
    else if (strcmp(command, "d") == 0){
        if (column_start < sheet.cols - 10)
        column_start += 10;
        strcpy(status, "ok");
    }
    else if (strncmp(command, "scroll_to", 9) == 0){ 
        // i removed space thats why 
        if (sscanf(command, "scroll_to %[A-Z]%d", ref_col, &ref_row) == 2){
            int c = get_col(ref_col);
            int r = ref_row - 1;
            if (is_valid_cell(r,c)){
                column_start = c;
                row_start = r;
                row_start = min(row_start, sheet.rows - 10);
                column_start = min(column_start, sheet.cols - 10);
                row_start= max(row_start,0);
                column_start=max(column_start,0);
                strcpy(status, "ok");
            }
            else
            strcpy(status, "Invalid cmd");
            
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    else if (strcmp(command, "enable_output") == 0){
        print_allowed = true;
        strcpy(status, "ok");
    }
    else if (strcmp(command, "disable_output") == 0){
        print_allowed = false;
        strcpy(status, "ok");
    }
    // cell = int op int
    else if (sscanf(command, "%[A-Z]%d=%d%c%d", ref_col, &ref_row, &val1, &op, &val2) == 5){
        int col = get_col(ref_col);
        int row = ref_row - 1;
        if (is_valid_cell(row,col)){
            ans=compute_cell(op,val1,val2);
            if(calc_error)sheet.data[row][col].isError=true;
            else {
                sheet.data[row][col].isError=false;
            }
            op='X';
            calc_error=false;
            add_constraints(&sheet.data[row][col],-1,-1,-1,-1,ans,op);
            strcpy(status, "ok");
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    // cell = cell op cell
    else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%[A-Z]%d", ref_col, &ref_row, val_col1, &val_row1, &op, val_col2, &val_row2) == 7){
        int col = get_col(ref_col);
        int row = ref_row - 1;
        int col1 = get_col(val_col1);
        int col2 = get_col(val_col2);
        if (is_valid_cell(row,col) && is_valid_cell(val_row1-1,col1) && is_valid_cell(val_row2-1,col2)){
            if(sheet.data[val_row1-1][col1].isError || sheet.data[val_row2-1][col2].isError)
                sheet.data[row][col].isError=true;
            else{
                ans=compute_cell(op,sheet.data[val_row1-1][col1].value,sheet.data[val_row2-1][col2].value);
                if(calc_error)
                    sheet.data[row][col].isError=true;
                else
                    sheet.data[row][col].isError=false;
            }
            calc_error=false;
            
            add_constraints(&sheet.data[row][col],col1,val_row1-1,col2,val_row2-1,ans,op);
            strcpy(status, "ok");
            
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }

        /*
            for case of int op cell and cell op int 
            i divide the constant integer(int->32 bits) into two short (16 bit) variables and store it in place of cell2_col and cell2_row
            and then use it whenever recalculation needed.
         */
        // cell = int op cell
    else if (sscanf(command, "%[A-Z]%d=%d%c%[A-Z]%d", ref_col, &ref_row, &val1, &op, val_col1, &val_row1) == 6){
        int col = get_col(ref_col);
        int row = ref_row - 1;
        int col1 = get_col(val_col1); 
        if (is_valid_cell(row,col) && is_valid_cell(val_row1-1,col1)){
            if(sheet.data[val_row1-1][col1].isError)
                sheet.data[row][col].isError=true;
            else{
                ans=compute_cell(op,val1,sheet.data[val_row1-1][col1].value);
                if(calc_error)
                    sheet.data[row][col].isError=true;
                else
                    sheet.data[row][col].isError=false;
            }
            calc_error=false;
            //splitting the constant into two 16 bit variables using bit operations
            int const1 = val1 & 0xFFFF;
            int const2 = (val1 >> 16) & 0xFFFF;
          
            op=get_op_code(op);
            add_constraints(&sheet.data[row][col],col1,val_row1-1,const1,const2,ans,op);
            strcpy(status, "ok");
            
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    // cell = cell op int
    else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%d", ref_col, &ref_row, val_col1, &val_row1, &op, &val1) == 6){ 
        int col = get_col(ref_col);
        int row = ref_row - 1;
        int col1 = get_col(val_col1);

        if (is_valid_cell(row,col) && is_valid_cell(val_row1-1,col1)){
            if(sheet.data[val_row1-1][col1].isError)
                sheet.data[row][col].isError=true;
            else{
                ans=compute_cell(op,sheet.data[val_row1-1][col1].value,val1);
                if(calc_error)
                    sheet.data[row][col].isError=true;
                else
                    sheet.data[row][col].isError=false;
            }
            calc_error=false;
            //splitting the constant into two 16 bit variables
            int const1 = val1 & 0xFFFF;
            int const2 = (val1 >> 16) & 0xFFFF;
            
            op=get_op_code(op);
            add_constraints(&sheet.data[row][col],col1,val_row1-1,const1,const2,ans,op);
            strcpy(status, "ok");
            
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    // cell = func(cell:cell)
    else if (sscanf(command, "%[A-Z]%d=%5[A-Z](%[A-Z]%d:%[A-Z]%d)", ref_col, &ref_row, func, val_col1, &val_row1, val_col2, &val_row2) == 7){
        int col = get_col(ref_col);
        int row = ref_row - 1;
        int col1 = get_col(val_col1);
        int col2 = get_col(val_col2);
        

        if (is_valid_cell(row,col) && is_valid_cell(val_row1-1,col1) && is_valid_cell(val_row2-1,col2)){
            bool input_cell_error=false;
            for(int i=val_row1-1;i<=val_row2-1;i++){
                for(int j=col1;j<=col2;j++){
                    if(sheet.data[i][j].isError) {input_cell_error=true;break;}
                }
                if(input_cell_error) break;
            }

            char op_code=func_to_op_code(func);
            if(input_cell_error)
                sheet.data[row][col].isError=true;
            else{
            ans=compute_range_func(op_code,val_row1-1,col1,val_row2-1,col2);
            if(calc_error)
                sheet.data[row][col].isError=true;
            else
                sheet.data[row][col].isError=false;
            calc_error=false;
            }
            add_constraints(&sheet.data[row][col],col1,val_row1-1,col2,val_row2-1,ans,op_code);
            strcpy(status, "ok");
            
                
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    // cell = int
    else if (sscanf(command, "%[A-Z]%d=%d", ref_col, &ref_row, &val1) == 3){   
        int col = get_col(ref_col);
        int row = ref_row - 1;
        sheet.data[row][col].isError=false;
        calc_error=false;
        if (is_valid_cell(row,col)){
            add_constraints(&sheet.data[row][col],-1,-1,-1,-1,val1,'X');
            strcpy(status, "ok");
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    // cell = cell
    else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d", ref_col, &ref_row, val_col1, &val_row1) == 4){
        int col = get_col(ref_col);
        int row = ref_row - 1;
        int col1 = get_col(val_col1);
        int row1 = val_row1 - 1; 
        sheet.data[row][col].isError=false;
        calc_error=false;
        if (is_valid_cell(row,col) && is_valid_cell(row1,col1)){
            if(sheet.data[row1][col1].isError)
                sheet.data[row][col].isError=true;
            else
                sheet.data[row][col].isError=false;
            
            add_constraints(&sheet.data[row][col],col1,row1,-1,-1,sheet.data[row1][col1].value,'=');
            
            strcpy(status, "ok");
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    // sleep(int)
    else if (sscanf(command, "%[A-z]%d=SLEEP(%d)", ref_col, &ref_row, &val1) == 3){
        int col = get_col(ref_col);
        int row = ref_row - 1;
        sheet.data[row][col].isError=false;
        calc_error=false;
        if (is_valid_cell(row,col) && val1 >= 0){
            add_constraints(&sheet.data[row][col],-1,-1,-1,-1,val1,'Z');
            sleep(val1);
            strcpy(status, "ok");
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    //cell = sleep(cell)
    else if (sscanf(command, "%[A-z]%d=SLEEP(%[A-Z]%d)", ref_col, &ref_row, val_col1, &val_row1) == 4){
        int col = get_col(ref_col);
        int row = ref_row - 1;
        int col1 = get_col(val_col1);
        int row1 = val_row1 - 1;
        sheet.data[row][col].isError=false;
        calc_error=false;
        if (is_valid_cell(row,col) && is_valid_cell(row1,col1)){
            if(sheet.data[row1][col1].isError)
                sheet.data[row][col].isError=true;
            else
                sleep(sheet.data[row1][col1].value);
            
            add_constraints(&sheet.data[row][col],col1,row1,-1,-1,sheet.data[row1][col1].value,'Z');
            strcpy(status, "ok");
        }
        else{
            strcpy(status, "Invalid cmd");
        }
    }
    else{
        strcpy(status, "Invalid cmd");
    }
}  