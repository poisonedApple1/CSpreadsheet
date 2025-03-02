#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define MAXDEP 10
#include "parser.h"
#include "sheet_functions.h"


int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Enter m and n values\n");
    return 0;
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);
  if (m < 1 || m > 999) {
    printf("Enter valid number of rows , number of rows should be between 1 and 999\n");
    return 0;
  }
  if (n < 1 || n > 18278) {
    printf("Enter valid number of columns , number of columns should be between 1 and 18,278\n");
    return 0;
  }

  double execution_time = 0.0;
  char command[30] = {0};

  initialise_sheet(m, n);
  print_table(column_start, row_start);
  printf("[%.1f] (%s) >", execution_time, status);
  while (1) {
    fgets(command, sizeof(command), stdin); // take input command

    if (strcmp(command, "q\n") == 0) //quit if user input is q
      break;

    time_t start = time(NULL);
    parser(command); // this function parses command and does the computation
    time_t end = time(NULL);
    //if sleep function is executed in any cell, the remaining time after the computation(if any left) is given as input to 
    //sleep and program execution is stopped
    if (end - start < sleep_timer)
      sleep(sleep_timer - end + start); 
    end = time(NULL);
    execution_time = (double)(end - start);
    sleep_timer=0;

    //not printing the table if disable_output is active
    if (print_allowed)
      print_table(column_start, row_start);
    else
      printf("\n");
    printf("[%.1f] (%s) >", execution_time, status);
  }

  //frees the memory alloted to the spreadsheet
  free_sheet();
  
  return 0;
}
