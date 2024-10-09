#ifndef PIPE_H
#define PIPE_H

//Define FIFO pipe / ring buffer struct used to transfer characters between the threads
typedef struct {
    char* buffer;
    int size;
    // char* read_ptr;
    // char* write_ptr;
    int read_pos, write_pos;
    int id;
    int write_open;//, read_open;
}pipe;

// extern pipe** pipe_array;
// extern int id_num;
// extern int num_pipes; //Contains the information about the amount of currently open pipes

//Find if a pipe with id: p exists. If it exists return a pointer to it, else return a NULL pointer 
// pipe* find_pipe(int p);

//Open a new pipe for reading and writing and return its id to user
int pipe_open(int size);

////Write the new 1 byte char in the pipe
int pipe_write(int p, char c);

//Close the pipe when write is completed
int pipe_writeDone(int p);

//Read the next byte from the pipe (make it NULL after reading)
int pipe_read(int p, char *c);

//Delete an existing pipe with id: p
// int delete_pipe(int p);

//Check if a thread can write into a pipe.
// int check_to_write(int p);

// Check if the thread can read the next byte in the pipe
// int check_to_read(int p);

#endif