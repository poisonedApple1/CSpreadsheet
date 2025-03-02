#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void run_test(const char *program, const char *input_file, const char *output_file) {
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "%s < %s > %s", program, input_file, output_file);
    int status = system(command);
    if (status != 0) {
        printf("Error: Failed to execute test command.\n");
        exit(1);
    }
}

void compare_files(const char *output_file, const char *expected_file) {
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "sed 's/\\[[^]]*\\]//g' %s > output_filtered.txt", output_file);
    system(command);
    snprintf(command, sizeof(command), "sed 's/\\[[^]]*\\]//g' %s > expected_filtered.txt", expected_file);
    system(command);

    int status = system("diff -u expected_filtered.txt output_filtered.txt");

    system("rm -f output_filtered.txt expected_filtered.txt");

    if (status == 0) {
        printf("All test cases passed successfully!\n");
    } else {
        printf("Mismatch found!\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <program> <input_file> <expected_output_file>\n", argv[0]);
        return 1;
    }
    
    const char *program = argv[1];
    const char *input_file = argv[2];
    const char *expected_file = argv[3];
    const char *output_file = "output.txt";
    
    run_test(program, input_file, output_file);
    compare_files(output_file, expected_file);
    
    return 0;
}