#include "calculate_functions.h"
#include "sheet_functions.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int compute(char command[])
{
    char ref_col[10];
    int ref_row;
    char val_col1[10], val_col2[10];
    int val_row1, val_row2;
    int val1, val2;
    int v1, v2;
    char op;
    char func[10];
    int ans;
    // case 1: B1 = int op int;
    if (sscanf(command, "%[A-Z]%d=%d%c%d", ref_col, &ref_row, &val1, &op, &val2) == 5)
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
            if (val2 != 0)
                ans = val1 / val2;
            else
                return -1;
            break;
        default:
            break;
        }
        return ans;
    }
    // case 2: B1 = A1 op A1;
    else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%[A-Z]%d", ref_col, &ref_row, val_col1, &val_row1, &op, val_col2, &val_row2) == 7)
    {

        int c = get_col(val_col1);
        v1 = atoi(sheet.data[val_row1 - 1][c].value);
        int d = get_col(val_col2);
        v2 = atoi(sheet.data[val_row2 - 1][d].value);

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
            if (v2 != 0)
            {
                ans = v1 / v2;
            }
            else
            {

                return -1;
            }
            break;
        default:
            break;
        }
        return ans;
    }
    // case 3 : B1 = int op A2
    else if (sscanf(command, "%[A-Z]%d=%d%c%[A-Z]%d", ref_col, &ref_row, &val1, &op, val_col1, &val_row1) == 6)
    {
        int c = get_col(val_col1);
        v1 = atoi(sheet.data[val_row1 - 1][c].value);
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
            if (v1 != 0)
                ans = val1 / v1;
            else
                return -1;
            break;
        default:
            break;
        }
        return ans;
    }
    // case 4 : B1 = A1 op int
    else if (sscanf(command, "%[A-Z]%d=%[A-Z]%d%c%d", ref_col, &ref_row, val_col1, &val_row1, &op, &val1) == 6)
    {
        int c = get_col(val_col1);
        v1 = atoi(sheet.data[val_row1 - 1][c].value);
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
            if (v1 != 0)
                ans = val1 / v1;
            else
                return -1;
        default:
            break;
        }
        return ans;
    }
    // case 5 : Func(A1:A2)
    else if (sscanf(command, "%[A-Z]%d=%5[A-Z](%[A-Z]%d:%[A-Z]%d)", ref_col, &ref_row, func, val_col1, &val_row1, val_col2, &val_row2) == 7)
    {
        int c1 = get_col(val_col1);
        int c2 = get_col(val_col2);
        if ((c1 <= c2) && (val_row1 <= val_row2))
        {
            if (strcmp(func, "SUM") == 0)
            {
                ans = SUM(val_row1 - 1, c1, val_row2 - 1, c2);
                return ans;
            }
            else if (strcmp(func, "MIN") == 0)
            {
                ans = MIN(val_row1 - 1, c1, val_row2 - 1, c2);
                return ans;
            }
            else if (strcmp(func, "MAX") == 0)
            {
                ans = MAX(val_row1 - 1, c1, val_row2 - 1, c2);
                return ans;
            }
            else if (strcmp(func, "AVG") == 0)
            {
                ans = AVG(val_row1 - 1, c1, val_row2 - 1, c2);
                return ans;
            }
            else if (strcmp(func, "STDEV") == 0)
            {
                ans = STDEV(val_row1 - 1, c1, val_row2 - 1, c2);
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