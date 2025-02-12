#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
<<<<<<< HEAD
#include <unistd.h>
#include <ctype.h>
typedef struct
{
    char value[20];
    char formula[20];
} Cell;
typedef struct
{
    int rows;
    int cols;
    Cell **data;
} Sheet;

int min(int a, int b)
{
    return (a < b) ? a : b;
=======
#include <unistd.h> 
#include <stdbool.h>

int ROWS,COLUMNS;

int min(int a,int b){
    return (a<b)?a:b;
>>>>>>> 54000ff0a89796aeca06a882a818c1fdc3f8775f
}
Sheet sheet;
void initialise_sheet(int m, int n)
{
    sheet.rows = m;
    sheet.cols = n;
    sheet.data = malloc(m * sizeof(Cell *));
    for (int i = 0; i < m; i++)
    {
        sheet.data[i] = malloc(n * sizeof(Cell));
        for (int j = 0; j < n; j++)
        {
            strcpy(sheet.data[i][j].value, "0");
            strcpy(sheet.data[i][j].formula, "");
        }
    }
}
char *column_name(int column_number)
{
    static char name[4];
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
<<<<<<< HEAD
=======
    
    name[2]=letters[column_number% 26];
    column_number/= 26;
    
    if (column_number!=0){
        name[1]=letters[(column_number-1)% 26];
        column_number=(column_number-1)/ 26;
    } else name[1]=' ';
    
    if (column_number!=0) name[0]=letters[(column_number-1)%26];
     else name[0]=' ';
    
    name[3]='\0';
    return name;
}
int get_col(char *col_name){ // opposite to column_name function
    int ans = 0;
    int len = strlen(col_name);
    int mul = 1;
    for(int i = len-1; i>=0; i--){
        int temp = *(col_name + i) - 'A' + 1;
        ans += temp * mul;
        mul *= 26;
    } 
    return ans - 1;
}

void print_table(int m,int n,int column_start,int row_start){
    const int display_column_number=column_start+min(n,10),display_row_number=row_start+min(m,10);
    const int space=10;
    
>>>>>>> 54000ff0a89796aeca06a882a818c1fdc3f8775f

    name[2] = letters[column_number % 26];
    column_number /= 26;

    if (column_number != 0)
    {
        name[1] = letters[(column_number - 1) % 26];
        column_number = (column_number - 1) / 26;
    }
    else
    {
        name[1] = ' ';
    }

    if (column_number != 0)
    {
        name[0] = letters[(column_number - 1) % 26];
    }
    else
    {
        name[0] = ' ';
    }

    name[3] = '\0';
    return name;
}
int get_col(char *col_name)
{
    int col = 0;
    if (strlen(col_name) == 1)
    {
        col = col_name[0] - 'A';
    }
    else if (strlen(col_name) == 2)
    {
        col = (col_name[0] - 'A' + 1) * 26 + (col_name[1] - 'A');
    }
    else if (strlen(col_name) == 3)
    {
        col = 26 * 26 * (col_name[0] - 'A' + 1) + (col_name[1] - 'A' + 1) * 26 + (col_name[2] - 'A');
    }
    return col;
}

void print_table(int m, int n, int column_start, int row_start)
{
    const int display_column_number = column_start + min(n, 10), display_row_number = row_start + min(m, 10);
    const int space = 10;
    printf("%*s", space, "");
    for (int i = column_start; i < display_column_number; i++)
    {
        printf("%*s", space, column_name(i));
    }
    printf("\n");
    for (int i = row_start; i < display_row_number; i++)
    {
        printf("%*d", space, i + 1);
        for (int j = column_start; j < display_column_number; j++)
        {
            printf("%*s", space, sheet.data[i][j].value);
        }
        printf("\n");
    }
}
<<<<<<< HEAD
void free_sheet()
{
    for (int i = 0; i < sheet.rows; i++)
    {
        free(sheet.data[i]); // Free each row
    }
    free(sheet.data); // Free the main array
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
=======

bool is_valid_cell(char *cell){
    if(strlen(cell)>3) return false;
    char col[3];
    int i = 0;
    while(i<3){
        if(cell[i]>='A' && cell[i]<='Z'){
            col[i] = cell[i];
        }else{
            return false;
        }
        i++;
    }
    col[i] = '\0';
    //check row is covetive to int
    while(i<strlen(cell)){
        if(cell[i]>='0' && cell[i]<='9'){
            i++;
        }else{
            return false;
        }
    }
    int row = atoi(cell+i);
    if(row>0 && row<=ROWS && get_col(col)>=0 && get_col(col)<COLUMNS){
        return true;
    }
    return false;
}

typedef struct spreadsheet{
    int val;
    char optype;
    char *cell1,*cell2;
    
} spreadsheet;
 
spreadsheet **sheet;

void make_sheet(int row,int col){
    sheet = (spreadsheet **)malloc(row * sizeof(spreadsheet *));
    for(int i =0; i<row; i++){
        sheet[i] = (spreadsheet *)malloc(col * sizeof(spreadsheet));
        for(int j=0; j<col; j++){
            sheet[i][j].val = 0;
            sheet[i][j].optype = '0';
            sheet[i][j].cell1 = NULL;
            sheet[i][j].cell2 = NULL;
        }
    }
}


int main(int argc, char *argv[]){

    if (argc != 3) {
>>>>>>> 54000ff0a89796aeca06a882a818c1fdc3f8775f
        printf("Enter m and n values");
        return 0;
    }
    int print_allowed = 1;
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
<<<<<<< HEAD
    int column_start = 0, row_start = 0;
    float execution_time = 0.0;
    char status[50] = "ok";
    char command[30];
    int ref_row;
    char ref_col[20];
    initialise_sheet(m, n);
    while (1)
    {
        if (print_allowed == 1)
            print_table(m, n, column_start, row_start); // Reprint the sheet
        printf("[%.1f] (%s) >", execution_time, status);

        clock_t start = clock();
        scanf("%s", command);

        if (strcmp(command, "w") == 0)
        {
            if (row_start > 0)
                row_start -= 10;
            strcpy(status, "ok");
        }
        else if (strcmp(command, "s") == 0)
        {
            if (row_start < m - 10)
                row_start += 10;
            strcpy(status, "ok");
        }
        else if (strcmp(command, "a") == 0)
        {
            if (column_start > 0)
                column_start -= 10;
            strcpy(status, "ok");
        }
        else if (strcmp(command, "d") == 0)
        {
            if (column_start < n - 10)
                column_start += 10;
            strcpy(status, "ok");
        }
        else if (strcmp(command, "q") == 0)
        {
=======
    ROWS = m;
    COLUMNS = n;
    int column_start=0,row_start=0;
    float execution_time=0.0;
    char status[20]="ok";
    char command[10];
    while(1){
        print_table(m,n,column_start,row_start);
        printf("[%.1f] (%s) >",execution_time,status);
        scanf("%s",command);  //remove all spaces from command if present
        
        if(strcmp(command,"w")==0){
            if(row_start>0) row_start-=10;
            strcpy(status,"ok");
        }
        else if(strcmp(command,"s")==0){
            if(row_start<m-10)row_start+=10;
            strcpy(status,"ok");
        }
        else if(strcmp(command,"a")==0){
            if(column_start>0)column_start-=10;
            strcpy(status,"ok");
        }
        else if(strcmp(command,"d")==0){
            if(column_start<n-10) column_start+=10;
            strcpy(status,"ok");
        }
        else if(strcmp(command,"q")==0){
>>>>>>> 54000ff0a89796aeca06a882a818c1fdc3f8775f
            break;
        }else{
            char assigned_cell[7],temp1[12],temp2[12];
            int len = strlen(command);
            int j =0;
            int i =0;
            while(i<len && command[i]!='='){
                assigned_cell[j] = command[i];
                j++;                
                i++;
            }
            assigned_cell[j] = '\0';
            if(i == len || !is_valid_cell(assigned_cell)){
                strcpy(status,"unrecognized cmd");
                continue;
            }
            i++;
            j=0;
            while(i<len && command[i]!='(' && command[i]!='+' && command[i]!='-' && command[i]!='*' && command[i]!='/'){
                if(command[i]!=' '){
                    temp1[j] = command[i];
                    j++;
                }
                i++;
            }
            temp1[j] = '\0';
            if(command[i]=='('){
                if (strcmp(temp1, "MAX") == 0) {
                
                } else if (strcmp(temp1, "MIN") == 0) {
                
                } else if (strcmp(temp1, "SUM") == 0) {
                    
                } else if (strcmp(temp1, "AVG") == 0) {
                
                } else if (strcmp(temp1, "STDEV") == 0) {
                    
                } else if (strcmp(temp1, "SLEEP") == 0) {
                
                } else {
                    strcpy(status,"unrecognized cmd");
                    continue;
                }
            }else if(command[i]=='+'){
                i++;
                j=0;
                while(i<len && command[i]!=' '){
                    if(command[i]!=' '){
                        temp2[j] = command[i];
                        j++;
                    }
                    i++;
                }
                temp2[j] = '\0';
                if(!is_valid_cell(temp2)){
                    strcpy(status,"unrecognized cmd");
                    continue;
                }
                // peroform addition

            }else if(command[i]=='-'){
                i++;
                j=0;
                while(i<len && command[i]!=' '){
                    if(command[i]!=' '){
                        temp2[j] = command[i];
                        j++;
                    }
                    i++;
                }
                temp2[j] = '\0';
                if(!is_valid_cell(temp2)){
                    strcpy(status,"unrecognized cmd");
                    continue;
                }

            }else if(command[i]=='*'){
                i++;
                j=0;
                while(i<len && command[i]!=' '){
                    if(command[i]!=' '){
                        temp2[j] = command[i];
                        j++;
                    }
                    i++;
                }
                temp2[j] = '\0';
                if(!is_valid_cell(temp2)){
                    strcpy(status,"unrecognized cmd");
                    continue;
                }

            }else if(command[i]=='/'){
                i++;
                j=0;
                while(i<len && command[i]!=' '){
                    if(command[i]!=' '){
                        temp2[j] = command[i];
                        j++;
                    }
                    i++;
                }
                temp2[j] = '\0';
                if(!is_valid_cell(temp2)){
                    strcpy(status,"unrecognized cmd");
                    continue;
                }

            }else{
                strcpy(status,"unrecognized cmd");
                continue;
            }
            

            
        }
<<<<<<< HEAD
        else if (sscanf(command, "scroll_to %[A-Z]%d", ref_col, &ref_row) == 2)
        {
            int c = get_col(ref_col);
            int r = ref_row - 1; // Convert 1-based row to 0-based index
=======
        // else{
        //     strcpy(status,"unrecognized cmd");
        // }
    }
    
>>>>>>> 54000ff0a89796aeca06a882a818c1fdc3f8775f

            // Validate that the cell is within sheet bounds
            if (r >= 0 && r < m && c >= 0 && c < n)
            {
                column_start = (c < n) ? c : n - 10;
                row_start = (r < m) ? r : m - 10;
                if (row_start < 0)
                    row_start = 0;
                if (column_start < 0)
                    column_start = 0;

                strcpy(status, "ok");
            }
            else
            {
                strcpy(status, "Invalid cell reference!");
            }
        }
        else if (strcmp(command, "enable_output") == 0)
        {
            print_allowed = 1;
            strcpy(status, "ok");
        }
        else if (strcmp(command, "disable_output") == 0)
        {
            print_allowed = 0;
            strcpy(status, "ok");
        }

        clock_t end = clock();
        execution_time = (float)(end - start) / CLOCKS_PER_SEC * 1000;
    }
    free_sheet();

    return 0;
}
