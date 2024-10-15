#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    FILE *input_file;

    // Check if the filename is provided
    if (argc < 2)
    {
        fprintf(stderr, "No filename provided\n");
        exit(EXIT_FAILURE);
    }

    // Open the file in binary mode
    input_file = fopen(argv[1], "rb");

    // Check if the file opened successfully
    if (!input_file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Get the file's size
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    rewind(input_file);

    // Calculate number of integers in the file
    int num_of_ints = file_size / sizeof(int);

    // Allocate memory for the integers
    int *array = malloc(num_of_ints * sizeof(int));
    if (!array)
    {
        perror("Failed to allocate memory");
        fclose(input_file);
        exit(EXIT_FAILURE);
    }

    // Read integers from the binary file
    size_t read_elements = fread(array, sizeof(int), num_of_ints, input_file);
    if (read_elements != num_of_ints)
    {
        fprintf(stderr, "Error reading from file\n");
        free(array);
        fclose(input_file);
        exit(EXIT_FAILURE);
    }

    // Close the file
    fclose(input_file);

    // Print the integers read
    printf("Integers read from file:\n");
    for (int i = 0; i < num_of_ints; i++)
    {
        printf("%d\n", array[i]);
    }

    // Free the allocated memory
    free(array);

    return 0;
}
