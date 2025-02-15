#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#define MAXDEP 10
#include "sheet_functions.h"
#include "utils.h"
#include "parser.h"




int main(int argc, char *argv[]){
    if (argc != 3)
    {
        printf("Enter m and n values");
        return 0;
    }

    int m = atoi(argv[1]);
    int n = atoi(argv[2]);


    int column_start = 0, row_start = 0; //starting position of the sheet
    bool print_allowed = true;
    float execution_time = 0.0;
    char status[50] = "ok";

    float newt = 0.0;

    char command[30];
    int ref_row;
    char ref_col[20];
    char val_col1[10], val_col2[10];
    int val_row1, val_row2;
    int val1, val2;
    int v1, v2;
    char op;
    char func[10];
    initialise_sheet(m, n);
    print_table(column_start, row_start); // Reprint the sheet
    printf("[%.1f] (%s) >", execution_time, status);
    while (1)
    {

        clock_t start = clock();
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        if (strcmp(command, "w") == 0){
            if (row_start > 0)
                row_start = max(row_start - 10, 0);
            strcpy(status, "ok");
        }
        else if (strcmp(command, "s") == 0){
            if (row_start < m - 10)
                row_start += 10;
            strcpy(status, "ok");
        }
        else if (strcmp(command, "a") == 0){
            if (column_start > 0)
                column_start = max(column_start - 10, 0);
            strcpy(status, "ok");
        }
        else if (strcmp(command, "d") == 0){
            if (column_start < n - 10)
                column_start += 10;
            strcpy(status, "ok");
        }
        else if (strcmp(command, "q") == 0){
            break;
        }
        else if (strncmp(command, "scroll_to ", 10) == 0){

            if (sscanf(command, "scroll_to %[A-Za-z]%d", ref_col, &ref_row) == 2)

            {
                int c = get_col(ref_col);
                int r = ref_row - 1; // Convert 1-based row to 0-based index

                if (r >= 0 && r < m && c >= 0 && c < n)
                {
                    column_start = (c < n - 10) ? c : n - 10;
                    row_start = (r < m - 10) ? r : m - 10;

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
            else
            {
                strcpy(status, "Invalid scroll_to command!");
            }
        }

        else if (strcmp(command, "enable_output") == 0)
        {
            print_allowed = true;
            strcpy(status, "ok");
        }
        else if (strcmp(command, "disable_output") == 0)
        {
            print_allowed = false;
            strcpy(status, "ok");
        }
        else if (sscanf(command, "%[A-Z]%d=%d%c%d", ref_col, &ref_row, &val1, &op, &val2) == 5)
        {
            char ans_if[10];
            int col = get_col(ref_col);
            int row = ref_row - 1; // Convert 1-based row to 0-based
            if ((row >= 0 && row < m) && (col >= 0 && col < n))
            {
                if (compute(command) != -1)
                {
                    sprintf(ans_if, "%d", compute(command));         // Convert the result to a string
                    strncpy(sheet.data[row][col].value, ans_if, 10); // Copy the string into the cell
                    strcpy(status, "ok");
                }
                else
                {
                    strcpy(status, "Invalid input");
                }
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
        else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%[A-Z]%d", ref_col, &ref_row, val_col1, &val_row1, &op, val_col2, &val_row2) == 7)
        {
            char ans_if[10];
            int col = get_col(ref_col);
            int row = ref_row - 1; // Convert 1-based if row to 0-based
            if ((row >= 0 && row < m) && (col >= 0 && col < n) && (val_row1 >= 0 && val_row1 < m) && (get_col(val_col1) >= 0 && get_col(val_col1) < n) && (val_row2 >= 0 && val_row2 < m) && (get_col(val_col2) >= 0 && get_col(val_col2) < n))
            {
                if (compute(command) != -1)
                {
                    sprintf(ans_if, "%d", compute(command));         // Convert the result to a string
                    strncpy(sheet.data[row][col].value, ans_if, 10); // Copy the string into the cell
                }
                else
                {
                    strcpy(status, "Invalid input");
                }
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
        else if (sscanf(command, "%[A-Z]%d=%d%c%[A-Z]%d", ref_col, &ref_row, &val1, &op, val_col1, &val_row1) == 6)
        {
            char ans_if[10];
            int col = get_col(ref_col);
            int row = ref_row - 1; // Convert 1-based if row to 0-based
            if ((row >= 0 && row < m) && (col >= 0 && col < n) && (val_row1 >= 0 && val_row1 < m) && (get_col(val_col1) >= 0 && get_col(val_col1) < n))
            {
                if (compute(command) != -1)
                {
                    sprintf(ans_if, "%d", compute(command));         // Convert the result to a string
                    strncpy(sheet.data[row][col].value, ans_if, 10); // Copy the string into the cell
                }
                else
                {
                    strcpy(status, "Invalid input");
                }
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
        else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%d", ref_col, &ref_row, val_col1, &val_row1, &op, &val1) == 6)
        {
            char ans_if[10];
            int col = get_col(ref_col);
            int row = ref_row - 1; // Convert 1-based if row to 0-based
            if ((row >= 0 && row < m) && (col >= 0 && col < n) && (val_row1 >= 0 && val_row1 < m) && (get_col(val_col1) >= 0 && get_col(val_col1) < n))
            {
                if (compute(command) != -1)
                {
                    sprintf(ans_if, "%d", compute(command));         // Convert the result to a string
                    strncpy(sheet.data[row][col].value, ans_if, 10); // Copy the string into the cell
                }
                else
                {
                    strcpy(status, "Invalid input");
                }
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
        else if (sscanf(command, "%[A-Z]%d=%5[A-Z](%[A-Z]%d:%[A-Z]%d)", ref_col, &ref_row, func, val_col1, &val_row1, val_col2, &val_row2) == 7)
        {
            char ans_if[10];
            int col = get_col(ref_col);
            int row = ref_row - 1; // Convert 1-based if row to 0-based
            if ((row >= 0 && row < m) && (col >= 0 && col < n) && (val_row1 >= 0 && val_row1 < m) && (get_col(val_col1) >= 0 && get_col(val_col1) < n) && (val_row2 >= 0 && val_row2 < m) && (get_col(val_col2) >= 0 && get_col(val_col2) < n))
            {
                if (compute(command) != -1)
                {
                    sprintf(ans_if, "%d", compute(command));         // Convert the result to a string
                    strncpy(sheet.data[row][col].value, ans_if, 10); // Copy the string into the cell
                }
                else
                {
                    strcpy(status, "Invalid input");
                }
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
         else if (sscanf(command, "%[A-Z]%d=%d", ref_col, &ref_row, &val1) == 3)
        {
            char ans_if[11];
            int col = get_col(ref_col);
            int row = ref_row - 1; // Convert 1-based if row to 0
            if ((row >= 0 && row < m) && (col >= 0 && col < n))
            {
                sprintf(ans_if, "%d", val1);
                strcpy(sheet.data[row][col].value, ans_if);
                strcpy(status, "ok");
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
        else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d", ref_col, &ref_row, val_col1, &val_row1) == 4)
        {
            int col = get_col(ref_col);
            int row = ref_row - 1; // Convert 1-based if row to 0
            int col1 = get_col(val_col1);
            int row1 = val_row1 - 1; // Convert 1-based if row to
            if ((row >= 0 && row < m) && (col >= 0 && col < n) && (row1 >= 0 && row1 < m) && (col1 >= 0 && col1 < n))
            {
                strcpy(sheet.data[row][col].value, sheet.data[row1][col1].value);
                strcpy(status, "ok");
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
         else if (sscanf(command, "%[A-z]%d=SLEEP(%d)", ref_col, &ref_row, &val1) == 3)
        {
            int col = get_col(ref_col);
            char ans_if[10];
            int row = ref_row - 1; // Convert 1-based if row to 0
            if ((row >= 0 && row < m) && (col >= 0 && col < n) && (val1 >= 0))
            {
                clock_t start1 = clock();
                sleep(val1);
                clock_t end1 = clock();
                newt = (float)(end1 - start1) / CLOCKS_PER_SEC;
                strcpy(status, "ok");
                sprintf(ans_if, "%d", val1);
                strcpy(sheet.data[row][col].value, ans_if);
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
        else if (sscanf(command, "%[A-z]%d=SLEEP(%[A-Z]%d)", ref_col, &ref_row, val_col1, &val_row1) == 4)
        {
            int col = get_col(ref_col);
            int row = ref_row - 1; // Convert 1-based if row to 0
            int col1 = get_col(val_col1);
            int row1 = val_row1 - 1; // Convert 1-based if row to
            if ((row >= 0 && row < m) && (col >= 0 && col < n) && (row1 >= 0 && row1 < m) && (col1 >= 0 && col1 < n))
            {
                clock_t start1 = clock();
                sleep(atoi(sheet.data[row1][col1].value));
                clock_t end1 = clock();
                newt = (float)(end1 - start1) / CLOCKS_PER_SEC;
                strcpy(status, "ok");
                strcpy(sheet.data[row][col].value, sheet.data[row1][col1].value);
            }
            else
            {
                strcpy(status, "Invalid input");
            }
        }
        else
        {
            strcpy(status, "invalid input");
        }
        if (newt == 0.0)
        {
            clock_t end = clock();
            execution_time = (float)(end - start) / CLOCKS_PER_SEC;
        }
        else
        {
            execution_time = newt;
        }
        if (print_allowed)
        {
            print_table(column_start, row_start); // Reprint the sheet
            printf("[%.1f] (%s) >", execution_time, status);
        }
        else
            printf("[%.1f] (%s) >", execution_time, status);
    }
    free_sheet();

    return 0;
}


