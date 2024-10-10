#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "pipe.h"

int write_complete = 0; //A global variable to indicate that a thread completed writing
int read_complete = 0; //A global variable to indicate that a thread completed reading 

// struct to use pipes in threads
typedef struct {
    int pipe1_id;  
    int pipe2_id;  
}pipes_fds ;

/*
Function for the first thread.
At first create first pipe and send the contents of given file via the pipe to the other thread.
After all the contents of the file are written to the pipe, close pipe for writing and wait for another pipe to open
Read every character from the second pipe opened by the other thread and save it to a new file .copy2
Compare original file with the second copy. If they are the same, the process is a success 
*/
void *thread1_op(void *arg) {
    pipes_fds *pipes_id = (pipes_fds *)arg;
    FILE* input_text_file;
    char text_buffer;
    ssize_t bytes_read;
    ssize_t i;
    int copy2_fd;
    ssize_t bytes_written;
    char data;
    ssize_t total_bytes = 0;
    char curr_c = '\0';
    
    input_text_file = fopen("input.txt", "r"); //Opening the file only for reading

    //Check if the file opened
    if (input_text_file < 0) {
        perror("Thread 1:input.txt cannot open");
        write_complete = 1;  // Writing is done if the is an error
    }

    // Reading from the file and writing to pipe
    // while ((bytes_read = read(input_text_file, text_buffer, sizeof(text_buffer))) > 0) {
       // for (i = 0; i < bytes_read; i++) {
            // if (pipe_write(pipes_id->pipe1_id, text_buffer) != 0) {
            //     printf("Thread 1: Error writing to pipe.\n");
            // }
       // }
    // }    

    //While we have not reached the end of the input file, scan each character and write them into the pipe
    do {
        curr_c = fgetc(input_text_file);
        pipe_write(pipes_id->pipe1_id, curr_c);
        printf("Just put char %c on pipe 1\n", curr_c);
    }
    while(curr_c != 'EOF');

    fclose(input_text_file); // Closing the input file

    pipe_writeDone(pipes_id->pipe1_id);

    write_complete = 1;

    //waiting for the reaiding to complete
    while(read_complete=0);

    copy2_fd = open("copy.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (copy2_fd < 0) {
        perror("Thread 2:Cannot open copy2.txt");
        return NULL;
    }

    while (pipe_read(pipes_id->pipe2_id, &data) == 0) {
        bytes_written = write(copy2_fd, &data, 1);
        if (bytes_written < 0) {
            perror("Thread 2: Error writing to copy.txt");
            close(copy2_fd);
            read_complete = 1;
            return NULL;
        }
        total_bytes += bytes_written;
    }

    close(copy2_fd);  // Κλείσιμο του αρχείου

    

    read_complete = 1;

    return NULL;

} 


/*
Function of the second thread.
At first when a pipe is created read all the contents until the pipe is closed for writing and empty.
Pass the content read from the pipe to a new file .copy.
Open a new pipe and start passing the contents of the copy file to the original thread.
When the writing is done, close the file for writing.
*/
void *thread2_op(void *arg) {
    pipes_fds *pipes_id = (pipes_fds *)arg;
    int copy_fd;
    ssize_t bytes_written;
    char data;
    ssize_t total_bytes = 0;
    char text_buffer;
    ssize_t bytes_read;

    //Creating and opening the new file
    copy_fd = open("copy.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (copy_fd < 0) {
        perror("Thread 2:Cannot open copy.txt");
        return NULL;
    }

    while (pipe_read(pipes_id->pipe1_id, &data) == 0) {
        bytes_written = write(copy_fd, &data, 1);
        if (bytes_written < 0) {
            perror("Thread 2: Error writing to copy.txt");
            close(copy_fd);
            read_complete = 1;
            return NULL;
        }
        total_bytes += bytes_written;
    }

    close(copy_fd);  // Κλείσιμο του αρχείου

    read_complete = 1;

    copy_fd = open("input.txt", O_RDONLY); //Opening the file only for reading

    //Check if the file opened
    if (copy_fd < 0) {
        perror("Thread 2:copy.txt cannot open");
        write_complete = 1;  // Writing is done if the is an error
    }

    // Reading from the file and writing to pipe
    while ((bytes_read = read(copy_fd, text_buffer, sizeof(text_buffer))) > 0) {
       // for (i = 0; i < bytes_read; i++) {
            if (pipe_write(pipes_id->pipe2_id, text_buffer) != 0) {
                printf("Thread 1: Error writing to pipe.\n");
            }
       // }
    }    

    return NULL;

    
}


int main(int argc, char *argv[]) {
    pthread_t thread1, thread2;
    int pipe_id;
    pipes_fds pipes_id;

    //If the user did not enter a size , print a message to remind him
    if (argc < 2) {
        printf("You have to indicate also the size of the pipe!\n");
        return 1;
    }

    //Indicate the size of the pipe from command line
    int pipe_size = atoi(argv[1]);

    // Opening a new pipes
    pipes_id.pipe1_id = pipe_open(pipe_size);
    pipes_id.pipe2_id = pipe_open(pipe_size);
    
    if (pipe_id < 0) {
        fprintf(stderr, "Pipe did not open.\n");
        return 1;
    }

    // Create the first thread 
    if (pthread_create(&thread1, NULL, thread1_op, &pipes_id) != 0) {
        fprintf(stderr, "Error creating thread 1.\n");
        return 1;
    }

    // Create the second thread
    if (pthread_create(&thread2, NULL, thread2_op, &pipes_id) != 0) {
        fprintf(stderr, "Error creating thread 2.\n");
        return 1;
    }

    // waiting for the threads to finish writing and reading
    while (!write_complete || !read_complete) ;
    //{
     //   sleep(1);  // Περιμένουμε για την ολοκλήρωση των threads
    //}

    //printf("Main completed .\n");

    return 0;
}