#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* input_file_name;

typedef struct {
    FILE* input_file;
    int left;
    int right;
    int size_of_section;
} thread_data;

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {

    // Initialize pivot to be the first element
    int p = arr[low];
    int i = low;
    int j = high;

    while (i < j) {

        // Find the first element greater than
        // the pivot (from starting)
        while (arr[i] <= p && i <= high - 1) {
            i++;
        }

        // Find the first element smaller than
        // the pivot (from last)
        while (arr[j] > p && j >= low + 1) {
            j--;
        }
        if (i < j) {
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[low], &arr[j]);
    return j;
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {

        // call partition function to find Partition Index
        int pi = partition(arr, low, high);

        // Recursively call quickSort() for left and right
        // half based on Partition Index
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void* thread_child(void* arg){

    thread_data *curr =(thread_data*)arg;
    int mid=0;
    thread_data left_child_data;
    thread_data right_child_data;
    pthread_t left_child;
    pthread_t right_child;
    int *array;

    if(curr->size_of_section>64){
        mid = curr->left + (curr->right - curr->left)/2 ;

        left_child_data.left = curr->left;

        left_child_data.right = mid;

        right_child_data.left = mid+1;

        right_child_data.right = curr->right;

        left_child_data.size_of_section = mid - curr->left +1;

        right_child_data.size_of_section = curr->right - (mid+1) +1;

        right_child_data.input_file = fopen(input_file_name, "rb+");

        left_child_data.input_file = fopen(input_file_name, "rb+");

        //left_child_data.input_file = curr->input_file;

        //right_child_data.input_file = curr->input_file;

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
        //Allocate memory for the array that the thread will use to perform quicksort
        array = malloc(curr->size_of_section * sizeof(int));

        fseek(curr->input_file, curr->left * sizeof(int), SEEK_SET);
        fread(array, sizeof(int), curr->size_of_section, curr->input_file);

        printf("Array entered in quicksort: ");
        for(int i = 0; i < curr->size_of_section; i++  ){
            printf("%d ",array[i]);
        }

        printf("\n");

        quickSort(array,0,curr->size_of_section -1);
        printf("Sorted array after quicksort: ");
        for(int i = 0; i < curr->size_of_section; i++  ){
            printf("%d ",array[i]);
        }

        //Writing back to the file
        fseek(curr->input_file, curr->left * sizeof(int), SEEK_SET);
        //printf("Writing from %d");
        fwrite(array, sizeof(int), curr->size_of_section, curr->input_file);
        //fclose(curr->input_file);

        free(array);

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
    data.input_file = fopen(argv[1], "rb+");

    input_file_name = (char*)malloc(strlen(argv[1]) *sizeof(char));
    input_file_name = argv[1];

    // Check if the file opened successfully
    if (!data.input_file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Get the file's size
    fseek(data.input_file, 0, SEEK_END);
    long file_size = (int)ftell(data.input_file);
    rewind(data.input_file);

    // Calculate number of integers in the file
    int num_of_ints = file_size / sizeof(int);

    

    data.left = 0;
    data.right = num_of_ints-1;
    data.size_of_section=num_of_ints;
    pthread_create(&root, NULL, thread_child, &data);
    pthread_join(root, NULL);

    // Allocate memory for the integers
    // int *array_print = malloc(num_of_ints * sizeof(int));
    // if (!array_print)
    // {
    //     perror("Failed to allocate memory");
    //     fclose(input_file);
    //     exit(EXIT_FAILURE);
    // }

    // Read integers from the binary file
    // size_t read_elements = fread(array_print, sizeof(int), num_of_ints, input_file);
    // if (read_elements != num_of_ints)
    // {
    //     fprintf(stderr, "Error reading from file\n");
    //     free(array_print);
    //     fclose(input_file);
    //     exit(EXIT_FAILURE);
    // }

    // Close the file
    fclose(data.input_file);

    // //Print the integers read
    // printf("Integers read from file:\n");
    // for (int i = 0; i < num_of_ints; i++)
    // {
    //     printf("%d\n", array_print[i]);
    // }

    // Free the allocated memory
    // free(array);


    FILE *file;
    int number;

    // Open the .bin file for reading in binary mode
    file = fopen("integers_binary.bin", "rb");

    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    // Read and print each integer from the file
    while (fread(&number, sizeof(int), 1, file)) {
        printf("%d ", number);
    }

    // Close the file
    fclose(file);
    




    return 0;
}
