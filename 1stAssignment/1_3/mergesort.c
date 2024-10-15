#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static FILE* input_file;

typedef struct {
    int left;
    int right;
    int size_of_section;
} thread_data;

void quicksort(int *array, int left, int right) {
    if (left >= right) return;

    int pivot = array[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (array[i] < pivot) i++;
        while (array[j] > pivot) j--;
        if (i <= j) {
            int tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            i++;
            j--;
        }
    }

    quicksort(array, left, j);
    quicksort(array, i, right);
}


void sort_small_section(FILE *file, long left, long right) {
    int n = right - left + 1;
    int *buffer = malloc(n * sizeof(int));

    fseek(file, left * sizeof(int), SEEK_SET);
    fread(buffer, sizeof(int), n, file);

    // Perform an in-memory sort (using quicksort for example)
    quicksort(buffer, 0, n - 1);

    fseek(file, left * sizeof(int), SEEK_SET);
    fwrite(buffer, sizeof(int), n, file);

    free(buffer);
}


void* thread_child(void* arg){

    thread_data *curr =(thread_data*)arg;
    int mid=0;
    thread_data left_child_data;
    thread_data right_child_data;
    pthread_t left_child;
    pthread_t right_child;

    if(curr->size_of_section>64){
        mid = curr->left + (curr->right - curr->left)/2 ;

        left_child_data.left = curr->left;

        left_child_data.right = mid;

        right_child_data.left = mid+1;

        right_child_data.right = curr->right;

        left_child_data.size_of_section = mid - curr->left;

        right_child_data.size_of_section = curr->right - (mid+1);

        if(pthread_create(&right_child, NULL, thread_child, &right_child_data)!=0){
            fprintf(stderr, "Error creating thread \n");
            return 0;
        }

        if(pthread_create(&left_child, NULL, thread_child, &left_child_data)!=0){
            fprintf(stderr, "Error creating thread.\n");
            return 0;
        }


        printf("Info of the left child is %d %d %d\n", left_child_data.left,left_child_data.right,left_child_data.size_of_section);
        printf("Info of the right child is %d %d %d\n",right_child_data.left,right_child_data.right,right_child_data.size_of_section);

        pthread_join(left_child,NULL);
        pthread_join(right_child,NULL);

        printf("Parent of %d %d %d, %d %d %d returning\n", left_child_data.left,left_child_data.right,left_child_data.size_of_section, right_child_data.left,right_child_data.right,right_child_data.size_of_section);

    }
    else{

        printf("I AM THE FINAL YOU FUCKERS AND MY POSITIONS ARE %d and %d and my size is %d.\n", curr->left , curr->right, curr->size_of_section);
        return NULL;

    }

    return NULL;
}


int main(int argc, char *argv[])
{
    pthread_t root;
    thread_data data;


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
    long file_size = (int)ftell(input_file);
    rewind(input_file);

    // Calculate number of integers in the file
    int num_of_ints = file_size / sizeof(int);

    

    data.left = 0;
    data.right = num_of_ints-1;
    data.size_of_section=num_of_ints;
    pthread_create(&root, NULL, thread_child, &data);
    pthread_join(root, NULL);

    /*// Allocate memory for the integers
    int *array = malloc(num_of_ints * sizeof(int));
    if (!array)
    {
        perror("Failed to allocate memory");
        fclose(input_file);
        exit(EXIT_FAILURE);
    }*/

    // Read integers from the binary file
    // size_t read_elements = fread(array, sizeof(int), num_of_ints, input_file);
    // if (read_elements != num_of_ints)
    // {
    //     fprintf(stderr, "Error reading from file\n");
    //     free(array);
    //     fclose(input_file);
    //     exit(EXIT_FAILURE);
    // }

    // Close the file
    fclose(input_file);

    // Print the integers read
    // printf("Integers read from file:\n");
    // for (int i = 0; i < num_of_ints; i++)
    // {
    //     printf("%d\n", array[i]);
    // }

    // Free the allocated memory
    // free(array);

    return 0;
}
