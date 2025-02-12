#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// typedef struct
// {
//     char value[20];
//     char formula[20];
// } Cell;
// typedef struct
// {
//     int rows;
//     int cols;
//     Cell **data;
// } Sheet;
// Sheet sheet;
// int main()
// {
//     sheet.data = malloc(5 * sizeof(Cell *));
//     for (int i = 0; i < 5; i++)
//     {
//         sheet.data[i] = malloc(5 * sizeof(Cell));
//         for (int j = 0; j < 5; j++)
//         {
//             sheet.data[i][j].value[0] = 'A';
//         }
//     }
//     for (int i = 0; i < 5; i++)
//     {
//         for (int j = 0; j < 5; j++)
//         {
//             printf("%c", sheet.data[i][j].value[0]);
//         }
//         printf("\n");
//     }
// }
int main()
{
    char expr[100];
    scanf("%s", expr);
    char ref_col[10];
    int ref_row, offset = 0;

    if (sscanf(expr, "%[A-Z]%d + %d", ref_col, &ref_row, &offset) == 3)
    {
        printf("%s%d%d", ref_col, ref_row, offset);
    }
}