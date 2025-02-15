#include "sheet_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#define MAXDEP 10
Sheet sheet;

void initialise_sheet(int m, int n){
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

char *column_name(int column_number){
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

    if (column_number != 0){
        name[0] = letters[(column_number - 1) % 26];
    }
    else{
        name[0] = ' ';
    }

    name[3] = '\0';
    return name;
}


int get_col(char *col_name){
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


void print_table( int column_start, int row_start){
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
            printf("%*s", space, sheet.data[i][j].value);
        }
        printf("\n");
    }
}


void free_sheet(){
    for (int i = 0; i < sheet.rows; i++)
    {
        free(sheet.data[i]); // Free each row
    }
    free(sheet.data); // Free the main array
}

bool valid_cell(int row, int col){
    return row >= 0 && row < sheet.rows && col >= 0 && col < sheet.cols;
}
