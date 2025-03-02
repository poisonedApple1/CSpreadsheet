#include <stdio.h>
#include <stdlib.h>

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
  char temp1[BUFFER_SIZE], temp2[BUFFER_SIZE], command[BUFFER_SIZE];

  // Remove bracketed content and save to temporary files
  snprintf(temp1, sizeof(temp1), "sed 's/\\[[^]]*\\]//g' %s > temp_output.txt",
           output_file);
  snprintf(temp2, sizeof(temp2),
           "sed 's/\\[[^]]*\\]//g' %s > temp_expected.txt", expected_file);

  system(temp1);
  system(temp2);

  // Compare the processed files
  snprintf(command, sizeof(command), "diff -u temp_expected.txt temp_output.txt");
  int status = system(command);

  if (status == 0) {
    printf("All test cases passed successfully!\n");
  } else {
    printf("Mismatch found!\n");
  }

  // Cleanup temporary files
  system("rm temp_output.txt temp_expected.txt");
}

int main() {
  run_test();
  compare_files("output.txt", "expected_output.txt");
  return 0;
}
