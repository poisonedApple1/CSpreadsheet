#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
        printf("Enter m and n values");
        return 0;
    }
    int print_allowed = 1;
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
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
            break;
        }
        else if (sscanf(command, "scroll_to %[A-Z]%d", ref_col, &ref_row) == 2)
        {
            int c = get_col(ref_col);
            int r = ref_row - 1; // Convert 1-based row to 0-based index

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
