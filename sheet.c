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
        if (argc != 3){
            printf("Enter m and n values\n");
            return 0;
        }
        int m = atoi(argv[1]);
        int n = atoi(argv[2]);

        if(m<1 || m>999){
            printf("Enter valid number of rows , number of rows should be between 1 and 999\n");
            return 0;
        }
        if(n<1 || n>18278){
            printf("Enter valid number of columns , number of columns should be between 1 and 18,278\n");
            return 0;
        }
        
        double execution_time = 0.0;
        char command[30]={0};
        
        initialise_sheet(m, n);
        print_table(column_start, row_start); 
        printf("[%.1f] (%s) >", execution_time, status);
        while (1){
            fgets(command, sizeof(command), stdin);     //take input command
                                //removing spaces from the command

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


