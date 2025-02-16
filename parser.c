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

char get_op_code(char op_code){
    //function to get opcode for the case of int op cell or cell op int
    switch(op_code){
        case '+':
            op_code = 'p';
            break;
        case '-':
            op_code = 's';
            break;
        case '*':
            op_code = 'u';
            break;
        case '/':
            op_code = 'd';
            break;
    }
    return op_code;
}


char get_op_code_rev(char op_code){
    //function to get operation from opcode for the case of int op cell or cell op int
    switch(op_code){
        case 'p':
            op_code = '+';
            break;
        case 's':
            op_code = '-';
            break;
        case 'u':
            op_code = '*';
            break;
        case 'd':
            op_code = '/';
            break;
    }
    return op_code;
}

char func_to_op_code(char *func){
    //function for getting opcode for the case of func(cell:cell)
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
    else
        return 'X';
}

int compute_cell(char op_code,int cell_value,int cell_value2){
    //function to compute the value of the cell
    switch(op_code){
        case '+':
            return cell_value + cell_value2;
        case '-':
            return cell_value - cell_value2;
        case '*':
            return cell_value * cell_value2;
        case '/':
            if(cell_value2 != 0)
                return cell_value / cell_value2;
            else
                return -1;
        default:
            return -1;
    }
}

int compute_range_func(char op_code,int row1,int col1,int row2,int col2){
    if(col1>col2  || row1>row2) 
        return -1;
    switch(op_code){
        case 'S':
            return SUM(row1,col1,row2,col2);
        case 'm':
            return MIN(row1,col1,row2,col2);
        case 'M':
            return MAX(row1,col1,row2,col2);
        case 'A':
            return AVG(row1,col1,row2,col2);
        case 'D':
            return STDEV(row1,col1,row2,col2);
        default:
            return -1;
    }   
}

void parser(char* command){
    char ref_col[10];
    int ref_row;
    char val_col1[10], val_col2[10];
    int val_row1, val_row2;
    int val1, val2;
    int v1, v2;
    char op;
    char func[10];
    int ans;

    command[strcspn(command, "\n")] = 0;
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
    else if (strncmp(command, "scroll_to ", 10) == 0){
        if (sscanf(command, "scroll_to %[A-Za-z]%d", ref_col, &ref_row) == 2){
            int c = get_col(ref_col);
            int r = ref_row - 1;

            if (is_valid_cell(r,c)){
                column_start = (c < sheet.cols - 10) ? c : sheet.cols - 10;
                row_start = (r < sheet.rows - 10) ? r : sheet.rows - 10;
                if (row_start < 0)
                    row_start = 0;
                if (column_start < 0)
                    column_start = 0;
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
            if (ans != -1){
                add_constraints(&sheet.data[row][col],-1,-1,-1,-1,ans,'X');
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
    // cell = cell op cell
    else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%[A-Z]%d", ref_col, &ref_row, val_col1, &val_row1, &op, val_col2, &val_row2) == 7){
        int col = get_col(ref_col);
        int row = ref_row - 1;
        int col1 = get_col(val_col1);
        int col2 = get_col(val_col2);
        if (is_valid_cell(row,col) && is_valid_cell(val_row1-1,col1) && is_valid_cell(val_row2-1,col2)){
            ans=compute_cell(op,sheet.data[val_row1-1][col1].value,sheet.data[val_row2-1][col2].value);
            if (ans != -1){
                add_constraints(&sheet.data[row][col],col1,val_row1-1,col2,val_row2-1,ans,op);
                strcpy(status, "ok");
            }
            else{
                strcpy(status, "Invalid cmd");
            }
        }
        else
        {
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
            printf("here\n");
            if (is_valid_cell(row,col) && is_valid_cell(val_row1-1,col1)){
                ans=compute_cell(op,val1,sheet.data[val_row1-1][col1].value);
                printf("ans=%d\n",ans);
                //splitting the constant into two 16 bit variables
                int temp=pow(2,16);
                int const1=val1%temp;
                val1/=temp;
                int const2=val1;

                if (ans != -1){
                    op=get_op_code(op);
                    add_constraints(&sheet.data[row][col],col1,val_row1-1,const1,const2,ans,op);
                    strcpy(status, "ok");
                }
                else
                {
                    strcpy(status, "Invalid cmd");
                }
            }
            else
            {
                strcpy(status, "Invalid cmd");
            }
        }
        // cell = cell op int
        else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%d", ref_col, &ref_row, val_col1, &val_row1, &op, &val1) == 6)
        {
            printf("here2\n");
            int col = get_col(ref_col);
            int row = ref_row - 1;
            int col1 = get_col(val_col1);


            if (is_valid_cell(row,col) && is_valid_cell(val_row1-1,col1)){
                ans=compute_cell(op,sheet.data[val_row1-1][col1].value,val1);
                //splitting the constant into two 16 bit variables
                int temp=pow(2,16);
                int const1=val1%temp;
                val1/=temp;
                int const2=val1;
                printf("ans=%d ,%d %d\n",ans,const1,const2);
                if (ans != -1){
                    op=get_op_code(op);
                    add_constraints(&sheet.data[row][col],col1,val_row1-1,const1,const2,ans,op);
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
        // cell = func(cell:cell)
        else if (sscanf(command, "%[A-Z]%d=%5[A-Z](%[A-Z]%d:%[A-Z]%d)", ref_col, &ref_row, func, val_col1, &val_row1, val_col2, &val_row2) == 7){
            int col = get_col(ref_col);
            int row = ref_row - 1;
            int col1 = get_col(val_col1);
            int col2 = get_col(val_col2);
            if (is_valid_cell(row,col) && is_valid_cell(val_row1-1,col1) && is_valid_cell(val_row2-1,col2)){
                char op_code=func_to_op_code(func);
                ans=compute_range_func(op_code,val_row1-1,col1,val_row2-1,col2);
                if (ans != -1){
                    add_constraints(&sheet.data[row][col],col1,val_row1-1,col2,val_row2-1,ans,op_code);
                    strcpy(status, "ok");
                }
                else
                {
                    strcpy(status, "Invalid cmd");
                }
            }
            else
            {
                strcpy(status, "Invalid cmd");
            }
        }
        // cell = int
        else if (sscanf(command, "%[A-Z]%d=%d", ref_col, &ref_row, &val1) == 3){   
            int col = get_col(ref_col);
            int row = ref_row - 1;
            if (is_valid_cell(row,col)){
                add_constraints(&sheet.data[row][col],-1,-1,-1,-1,val1,'X');
                strcpy(status, "ok");
            }
            else
            {
                strcpy(status, "Invalid cmd");
            }
        }
        // cell = cell
        else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d", ref_col, &ref_row, val_col1, &val_row1) == 4){
            int col = get_col(ref_col);
            int row = ref_row - 1;
            int col1 = get_col(val_col1);
            int row1 = val_row1 - 1;
            if (is_valid_cell(row,col) && is_valid_cell(row1,col1)){
                add_constraints(&sheet.data[row][col],col1,row1,-1,-1,sheet.data[row1][col1].value,'=');
                strcpy(status, "ok");
            }
            else{
                strcpy(status, "Invalid cmd");
            }
        }
        // sleep(int)
         else if (sscanf(command, "%[A-z]%d=SLEEP(%d)", ref_col, &ref_row, &val1) == 3)
        {
            int col = get_col(ref_col);
            int row = ref_row - 1;
            if (is_valid_cell(row,col) && val1 >= 0){
                add_constraints(&sheet.data[row][col],-1,-1,-1,-1,val1,'Z');
                sleep(val1);
                strcpy(status, "ok");
            }
            else
            {
                strcpy(status, "Invalid cmd");
            }
        }

        //cell = sleep(cell)
        else if (sscanf(command, "%[A-z]%d=SLEEP(%[A-Z]%d)", ref_col, &ref_row, val_col1, &val_row1) == 4)
        {
            int col = get_col(ref_col);
            int row = ref_row - 1;
            int col1 = get_col(val_col1);
            int row1 = val_row1 - 1;
            if (is_valid_cell(row,col) && is_valid_cell(row1,col1)){
                sleep(sheet.data[row1][col1].value);
                add_constraints(&sheet.data[row][col],col1,row1,-1,-1,sheet.data[row1][col1].value,'Z');
                strcpy(status, "ok");
            }
            else
            {
                strcpy(status, "Invalid cmd");
            }
        }
        else
        {
            strcpy(status, "Invalid cmd");
        }

}



