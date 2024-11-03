#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* input_file_name;

//Arguments passed from parent thread to children
typedef struct {
    int left;
    int right;
    int size_of_section;
    int comm_flag;
} thread_data;

//quicksort swap
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
//quicksort partition
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
//quicksort used when 64 ints RAM is available
void quickSort(int arr[], int low, int high) {
    int index;
    if (low < high) {

        // call partition function to find Partition Index
        index = partition(arr, low, high);

        // Recursively call quickSort() for left and right
        // half based on Partition Index
        quickSort(arr, low, index - 1);
        quickSort(arr, index + 1, high);
    }
}

//Thread function
void* thread_child(void* arg){

    thread_data *curr =(thread_data*)arg;
    int mid=0;
    thread_data left_child_data;
    thread_data right_child_data;
    pthread_t left_child;
    pthread_t right_child;
    int *array;
    FILE* input_file;

    //If you have to handle more than 64 integers on this file's section, split into 2 thread children
    if(curr->size_of_section>64){
        //Create data and pass it to children
        mid = curr->left + (curr->right - curr->left)/2 ;

        left_child_data.left = curr->left;

        left_child_data.right = mid;

        right_child_data.left = mid+1;

        right_child_data.right = curr->right;

        left_child_data.size_of_section = mid - curr->left +1;

        right_child_data.size_of_section = curr->right - (mid+1) +1;

        // right_child_data.input_file = fopen(input_file_name, "rb+");

        // left_child_data.input_file = fopen(input_file_name, "rb+");

        left_child_data.comm_flag = 0;

        right_child_data.comm_flag = 0;
        //left_child_data.input_file = curr->input_file;

        //right_child_data.input_file = curr->input_file;

        //Create 2 children 
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
        //Actively wait for the current thread's children threads to finish sorting file
        while(right_child_data.comm_flag == 0 || left_child_data.comm_flag == 0);
        pthread_detach(left_child);
        pthread_detach(right_child);

        //Set flag to 1 so the parent thread stops waiting and return
        printf("Parent of %d %d %d, %d %d %d returning\n", left_child_data.left,left_child_data.right,left_child_data.size_of_section, right_child_data.left,right_child_data.right,right_child_data.size_of_section);
        curr->comm_flag = 1;

    }
    //Perform quicksort on the <=64 integers viewed by the branch on the file and return
    else{
        //Open file
        input_file = fopen(input_file_name, "rb+");
        if (!input_file)
        {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }

        //Allocate memory for the array that the thread will use to perform quicksort and pass the file's current
        //section to it, then sort
        array = malloc(curr->size_of_section * sizeof(int));
        fseek(input_file, curr->left * sizeof(int), SEEK_SET);
        fread(array, sizeof(int), curr->size_of_section, input_file);

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

        //Write sorted section back to the file
        fseek(input_file, curr->left * sizeof(int), SEEK_SET);
        //printf("Writing from %d");
        fwrite(array, sizeof(int), curr->size_of_section, input_file);
        fclose(input_file);

        free(array);

        curr->comm_flag = 1;

        return NULL;

    }

    return NULL;
}
//Print file contents
void print_file(FILE *filename) {
    int number;

    if (filename == NULL) {
        printf("Error opening file!\n");
        return;
    }
    printf("PRINTING FINAL FILE NUMBERS\n");
    // Read and print each integer from the file
    while (fread(&number, sizeof(int), 1, filename)) {
        printf("%d ", number);
    }
    return;
}


int main(int argc, char *argv[])
{
    FILE *input_file;
    pthread_t root;
    thread_data data;
    int num_of_ints;
    long file_size;


    // Check if the filename is provided
    if (argc < 2)
    {
        fprintf(stderr, "No filename provided\n");
        exit(EXIT_FAILURE);
    }

    // Open the file in binary mode
    input_file = fopen(argv[1], "rb+");

    // input_file_name = (char*)malloc(strlen(argv[1]) *sizeof(char));
    input_file_name = argv[1];

    // Check if the file opened successfully
    if (!input_file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Get the file's size
    fseek(input_file, 0, SEEK_END);
    file_size = (int)ftell(input_file);
    rewind(input_file);

    // Calculate number of integers in the file
    num_of_ints = file_size / sizeof(int);

    data.left = 0;
    data.right = num_of_ints-1;
    data.size_of_section=num_of_ints;
    data.comm_flag = 0;
    // fclose(data.input_file);
    pthread_create(&root, NULL, thread_child, &data);

    printf("Main waiting for children to finish\n");
    while(data.comm_flag == 0);
    printf("Main finished\n");

    pthread_detach(root);

    // Close the file
    fclose(input_file);

    // Open the .bin file for reading in binary mode
    input_file = fopen(input_file_name, "rb");
    print_file(input_file);
    // Close the file
    fclose(input_file);

    // free(input_file_name);
    input_file_name = '\0';

    return 0;
}


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

    // //Print the integers read
    // printf("Integers read from file:\n");
    // for (int i = 0; i < num_of_ints; i++)
    // {
    //     printf("%d\n", array_print[i]);
    // }

    // Free the allocated memory
    // free(array);
