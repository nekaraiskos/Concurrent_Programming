#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *output_file;
    const char *filename = "testfile.bin"; // Name of the binary file

    // Array of integers to write to the file
    int array[] = {10, 20, 30, 40, 50};
    int num_of_ints = sizeof(array) / sizeof(array[0]);

    // Open the file in binary write mode
    output_file = fopen(filename, "wb");
    if (!output_file) {
        perror("Failed to open file for writing");
        exit(EXIT_FAILURE);
    }

    // Write the array to the binary file
    size_t written_elements = fwrite(array, sizeof(int), num_of_ints, output_file);
    if (written_elements != num_of_ints) {
        fprintf(stderr, "Error writing to file\n");
        fclose(output_file);
        exit(EXIT_FAILURE);
    }

    // Close the file
    fclose(output_file);

    printf("Binary file '%s' created successfully with %d integers.\n", filename, num_of_ints);

    return 0;
}
