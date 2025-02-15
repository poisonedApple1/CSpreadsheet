#include "calculate_functions.h"
#include <math.h>
#include <stdlib.h>
#include "sheet_functions.h"


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
    if (count > 0)
    {
        ans = total / count;
    }
    else
    {
        return -1;
    }
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
            total += (atoi(sheet.data[i][j].value) - avg) * (atoi(sheet.data[i][j].value) - avg);
        }
    }
    return (int)(round)(sqrt(total / ((val_row2 - val_row1 + 1) * (c2 - c1 + 1))));
}
