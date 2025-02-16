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
#include "calculate_functions.h"




int main(int argc, char *argv[]){
    if (argc != 3)
    {
        printf("Enter m and n values");
        return 0;
    }

    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
   
    double execution_time = 0.0;
    char command[30]={0};
    
    initialise_sheet(m, n);
    print_table(column_start, row_start); 
    printf("[%.1f] (%s) >", execution_time, status);
    while (1)
    {

        fgets(command, sizeof(command), stdin); 
        if (strcmp(command, "q\n") == 0)break;
        time_t start = time(NULL); 
        parser(command);//this function parses command , then does calculations and return status string
        time_t end = time(NULL); 
        execution_time = (double)(end - start); 

        if (print_allowed)print_table(column_start, row_start); 
        printf("[%.1f] (%s) >", execution_time, status);

    }
    free_sheet();

    return 0;
}


