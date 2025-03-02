#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void run_test() {
  // Run the test command
  int status =
      system("./target/release/spreadsheet 999 18278 < input.txt > output.txt");
  if (status != 0) {
    printf("Error: Failed to execute test command.\n");
    exit(1);
  }
}

void compare_files(const char *output_file, const char *expected_file) {
  FILE *out = fopen(output_file, "r");
  FILE *expected = fopen(expected_file, "r");

  if (!out || !expected) {
    printf("Error: Unable to open output or expected output file.\n");
    exit(1);
  }

  char out_line[BUFFER_SIZE], exp_line[BUFFER_SIZE];
  int line_number = 1;
  int mismatch_found = 0;

  while (fgets(out_line, BUFFER_SIZE, out) &&
         fgets(exp_line, BUFFER_SIZE, expected)) {
    if (strcmp(out_line, exp_line) != 0) {
      printf("Mismatch at line %d:\nExpected: %sGot: %s\n", line_number,
             exp_line, out_line);
      mismatch_found = 1;
    }
    line_number++;
  }

  // Check if files have different lengths
  if (fgets(out_line, BUFFER_SIZE, out)) {
    printf("Extra output found in output.txt starting at line %d\n",
           line_number);
    mismatch_found = 1;
  }
  if (fgets(exp_line, BUFFER_SIZE, expected)) {
    printf("Missing expected output from expected_output.txt starting at line "
           "%d\n",
           line_number);
    mismatch_found = 1;
  }

  fclose(out);
  fclose(expected);

  if (!mismatch_found) {
    printf("All test cases passed successfully!\n");
  }
}

int main() {
  run_test();
  compare_files("output.txt", "expected_output.txt");
  return 0;
}
