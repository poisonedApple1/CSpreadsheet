#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#define MAXDEP 10
typedef struct
{
    char value[20];
    char formula[20];
    int dep_count;
    int dependencies[MAXDEP];
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

    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    int column_start = 0, row_start = 0;
    float execution_time = 0.0;
    char status[50] = "ok";
    char command[30];
    initialise_sheet(m, n);
    int print_allowed = 1;
    while (1)
    {
        if (print_allowed == 1)
        {
            print_table(m, n, column_start, row_start);
        } // Reprint the sheet
        printf("[%.1f] (%s) >", execution_time, status);

        clock_t start = clock();
        scanf("%s", command);
        char ref_col[4];
        int ref_row;
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

int SUM(int val_row1, int c1, int val_row2, int c2)
{
    int a = 0;
    for (int i = val_row1; i <= val_row2; i++)
    {
        for (int j = c1; j <= c2; j++)
        {
            a += atoi(sheet.data[i][j].value);
        }
    }
    return a;
}

int MIN(int val_row1, int c1, int val_row2, int c2)
{
    int a = pow(2, 31) - 1;
    for (int i = val_row1; i <= val_row2; i++)
    {
        for (int j = c1; j <= c2; j++)
        {
            int b = atoi(sheet.data[i][j].value);
            if (b < a)
                a = b;
        }
    }
    return a;
}

int MAX(int val_row1, int c1, int val_row2, int c2)
{
    int a = 0;
    for (int i = val_row1; i <= val_row2; i++)
    {
        for (int j = c1; j <= c2; j++)
        {
            int b = atoi(sheet.data[i][j].value);
            if (b > a)
                a = b;
        }
    }
    return a;
}

int AVG(int val_row1, int c1, int val_row2, int c2)
{
    int ans = 0;
    int total = SUM(val_row1, c1, val_row2, c2);
    int count = (val_row2 - val_row1 + 1) * (c2 - c1 + 1);
    ans = total / count;
    return ans;
}

int STDEV(int val_row1, int c1, int val_row2, int c2)
{
    int avg = AVG(val_row1, c1, val_row2, c2);
    int total = 0;
    for (int i = val_row1; i <= val_row2; i++)
    {
        for (int j = c1; j <= c2; j++)
        {
            total += pow((atoi(sheet.data[i][j].value) - avg), 2);
        }
    }
    return (int)sqrt(total / ((val_row2 - val_row1 + 1) * (c2 - c1 + 1)));
}
int compute(char command[])
{
    char val_col1[10], val_col2[10];
    int val_row1, val_row2;
    int val1, val2;
    int v1, v2;
    char op;
    char func[10];
    int ans;
    // case 1: B1 = int op int;
    if (sscanf(command, "%d%c%d", &val1, &op, &val2) == 3)
    {

        switch (op)
        {
        case '+':
            ans = val1 + val2;
            break;
        case '-':
            ans = val1 - val2;
            break;
        case '*':
            ans = val1 * val2;
            break;
        case '/':
            ans = val1 / val2;
            break;
        default:
            break;
        }
        return ans;
    }
    // case 2: B1 = A1 op A1;
    else if (sscanf(command, "%[A-Z]%d%c%[A-Z]%d", val_col1, &val_row1, &op, val_col2, &val_row2) == 5)
    {

        int c = get_col(val_col1);
        v1 = sheet.data[val_row1][c].value;
        int d = get_col(val_col2);
        v2 = sheet.data[val_row2][d].value;

        switch (op)
        {
        case '+':
            ans = v1 + v2;
            break;
        case '-':
            ans = v1 - v2;
            break;
        case '*':
            ans = v1 * v2;
            break;
        case '/':
            ans = v1 / v2;
            break;
        default:
            break;
        }
        return ans;
    }
    // case 3 : B1 = int op A2
    else if (sscanf(command, "%d%c%[A-Z]%d", &val1, &op, val_col1, &val_row1) == 4)
    {
        int c = get_col(val_col1);
        v1 = sheet.data[val_row1][c].value;
        switch (op)
        {
        case '+':
            ans = val1 + v1;
            break;
        case '-':
            ans = val1 - v1;
            break;
        case '*':
            ans = val1 * v1;
            break;
        case '/':
            ans = val1 / v1;
            break;
        default:
            break;
        }
        return ans;
    }
    // case 4 : B1 = A1 op int
    else if (sscanf(command, "%[A-Z]%d%c%d", val_col1, &val_row1, &op, &val1) == 4)
    {
        int c = get_col(val_col1);
        v1 = sheet.data[val_row1][c].value;
        switch (op)
        {
        case '+':
            ans = val1 + v1;
            break;
        case '-':
            ans = val1 - v1;
            break;
        case '*':
            ans = val1 * v1;
            break;
        case '/':
            ans = val1 / v1;
            break;
        default:
            break;
        }
        return ans;
    }
    // case 5 : Func(A1:A2)
    else if (sscanf(command, "%s(%[A-Z]%d:%[A-Z]%d)", func, val_col1, &val_row1, val_col2, &val_row2) == 5)
    {
        int c1 = get_col(val_col1);
        int c2 = get_col(val_col2);
        if ((c1 <= c2) && (val_row1 <= val_row2))
        {
            if (strcmp(func, "SUM") == 0)
            {
                ans = SUM(val_row1, c1, val_row2, c2);
                return ans;
            }
            else if (strcmp(func, "MIN") == 0)
            {
                ans = MIN(val_row1, c1, val_row2, c2);
                return ans;
            }
            else if (strcmp(func, "MAX") == 0)
            {
                ans = MAX(val_row1, c1, val_row2, c2);
                return ans;
            }
            else if (strcmp(func, "AVG") == 0)
            {
                ans = AVG(val_row1, c1, val_row2, c2);
                return ans;
            }
            else if (strcmp(func, "STDEV") == 0)
            {
                ans = STDEV(val_row1, c1, val_row2, c2);
                return ans;
            }
            else
                return -1;
        }
        else
            return -1;
    }
    // case 6 : others
    else
        return -1;
}
