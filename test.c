#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

void run_test_cases(const char *input_file, const char *expected_file,
                    const char *output_file) {
  FILE *input = fopen(input_file, "r");
  FILE *expected = fopen(expected_file, "r");
  FILE *output = fopen(output_file, "w");

  if (!input || !expected || !output) {
    perror("Error opening files");
    return;
  }

  char test_case[MAX_LINE], expected_output[MAX_LINE], actual_output[MAX_LINE];
  int test_case_number = 1;

  while (fgets(test_case, sizeof(test_case), input) &&
         fgets(expected_output, sizeof(expected_output), expected)) {
    // Remove newline characters
    test_case[strcspn(test_case, "\n")] = 0;
    expected_output[strcspn(expected_output, "\n")] = 0;

    // Open the program with arguments and write input
    // FILE *program_output =
    // popen("/mnt/c/Users/adity/OneDrive/Desktop/clab/lab1_2023cs51009_2023cs50124_2023cs50838/target/release/spreadsheet
    // 999 18278", "w+"); if (!program_output) {
    //   perror("Error executing program");
    //   continue;
    // }

    // // Send test case as input
    // fprintf(program_output, "%s\n", test_case);
    // fflush(program_output); // Ensure input is sent

    // // Read actual output
    // if (fgets(actual_output, sizeof(actual_output), program_output) == NULL)
    // {
    //   actual_output[0] = '\0'; // Handle case where no output is returned
    // }
    // actual_output[strcspn(actual_output, "\n")] = 0; // Remove newline

    // pclose(program_output);
    char command[MAX_LINE];
    snprintf(command, sizeof(command),
             "sh -c "
             "'/mnt/c/Users/adity/OneDrive/Desktop/clab/"
             "lab1_2023cs51009_2023cs50124_2023cs50838/target/release/"
             "spreadsheet 999 18278'");

    FILE *program_output = popen(command, "w+");
    if (!program_output) {
      perror("Error executing program");
      return;
    }

    fprintf(program_output, "%s\n", test_case); // Send input
    fflush(program_output);

    if (fgets(actual_output, sizeof(actual_output), program_output) == NULL) {
      actual_output[0] = '\0'; // Handle empty output
    }

    pclose(program_output);

    // Write actual output to output file
    fprintf(output, "%s\n", actual_output);
    printf("%s", actual_output);

    // Compare actual output with expected output
    if (strcmp(actual_output, expected_output) != 0) {
      printf("Test Case %d FAILED: Input: \"%s\"\n", test_case_number,
             test_case);
      printf("  Expected: \"%s\"\n", expected_output);
      printf("  Got:      \"%s\"\n", actual_output);
    }

    test_case_number++;
  }

  fclose(input);
  fclose(expected);
  fclose(output);
}

int main() {
  run_test_cases("input.txt", "expected_output.txt", "output.txt");
  return 0;
}