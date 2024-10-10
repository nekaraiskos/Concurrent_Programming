#include <stdio.h>
#include <stdlib.h>
#include "pipe.h"

//Initialize pipe_array. It will contain the pointers to all the set pipes
static pipe** pipe_array = NULL;
static int id_num = 0;
static int num_pipes = 0; // elegxos gia kathe neo pipe

//Open a new pipe for reading amd writing and return its id to user
int pipe_open(int size) {
    pipe* new_pipe;
    int i;// test i

    //Allocate memory for the struct and its ring buffer
    new_pipe = (pipe*)malloc(sizeof(pipe));
    new_pipe->buffer = (char*)malloc(size * sizeof(char));

    for(i = 0; i < size; i++) {
        new_pipe->buffer[i] = '\0';
    }
    
    //Set the struct's values
    new_pipe->size = size;
    new_pipe->id = id_num;
    id_num++;

    num_pipes++; //Increase the number of pipes by 1

    //Open pipe for writing
    new_pipe->write_open = 1;
    
    //Set read and write pointers to the beginning of the ring buffer 
    // new_pipe->write_ptr = new_pipe->buffer;
    // new_pipe->read_ptr = new_pipe->buffer;
    new_pipe->read_pos = -1;
    new_pipe->write_pos = -1;
    //printf("read_pos from pipe_open=%")

    // char i = new_pipe->buffer[0];

    //Save the pipe's address on the pipes array
    pipe** new_pipe_array = (pipe**)realloc(pipe_array, sizeof(pipe*)*num_pipes);
    if(new_pipe_array != NULL) {
        new_pipe_array[id_num-1] = new_pipe;
        pipe_array = new_pipe_array;
        //Print all pipe ids for the test
        printf("Pipes: ");
        for(i = 0; i < num_pipes; i++) {
            printf("%d, ", pipe_array[i]->id);
        }
        putchar('\n');
    }
    // pipe_array = new_pipe_array;
    else {
    //
    fprintf(stderr, "Failed to allocate memory for pipe_array.\n");
    }


    return new_pipe->id;
}

//Find if a pipe with id: p exists. If it exists return a pointer to it, else return a NULL pointer 
pipe* find_pipe(int p) {
    int i;
    if(num_pipes > 0 ) { //&& pipe_array != NULL) {
        for (i = 0; i < num_pipes; i++) {
            if(pipe_array[i]->id == p) {
                return pipe_array[i];
            }
        }
    }

    return NULL; 
}

//Delete an existing pipe with id: p
int delete_pipe(int p) {
    int i=0;
    int index;
    pipe* pipe_to_del = find_pipe(p);

    if(pipe_to_del != NULL) {

        for(i=0; pipe_array[i] != pipe_to_del; i++); // ********EIMAI I KATERINA KAI EXW DIKIO***************

        index = i;

        free(pipe_to_del->buffer);
        free(pipe_to_del);

        for (i=index; i<num_pipes-1;i++){
            pipe_array[i] = pipe_array[i+1];
        }

        num_pipes--;

        pipe** new_pipe_array = (pipe**)realloc(pipe_array, sizeof(pipe*)*num_pipes);
        if(new_pipe_array != NULL) {
            pipe_array = new_pipe_array;
        }

        return 1;
    }
    else return 0;
}

// Close the pipe when write is completed
int pipe_writeDone(int p)
{
    printf("I am in WriteDone you fuckers\n");
    pipe *pipe = find_pipe(p); //************MOVEWRITEPOS?*******
    if (pipe != NULL)
    {
        pipe->write_open = 0;
        return 1;
    }

    return -1;
}

//Check if the thread can read the next byte in the pipe
int check_to_read (pipe* pipe) {

    int next_pos;
    // if (pipe != NULL) {
    //Calculate next read position in the buffer
    if(pipe->read_pos == pipe->size - 1) {
        next_pos = 0;
    }
    else next_pos = pipe->read_pos + 1;

    //don't read if nothing has been written in the pipe or if the next position is being written
    if (pipe->write_pos == -1 || (pipe->write_open == 1 && next_pos == pipe->write_pos)) {
        return 0;
    }
    else {
        return 1;
    }
    
    // }
    
    return -1;
}

// Read the next byte from the pipe (make it NULL after reading)
int pipe_read(int p, char *c) {

    
    pipe *pipe = find_pipe(p);
    if (pipe != NULL && check_to_read(pipe) != -1) {
        
        printf("in pipe read: pipe: %d, read_pos: %d, write_open: %d\n", pipe->id, pipe->read_pos, pipe->write_open);

        int next_pos = 0;
        int i;//testing integer
        while (check_to_read(pipe)==0); //wait

        //Calculate next read position in the buffer
        if(pipe->read_pos == pipe->size - 1) {
            next_pos = 0;
        }
        else next_pos = pipe->read_pos + 1;
        printf("Next position: %d\nNext poaition char: %c\n", next_pos, pipe->buffer[next_pos]);

        if (pipe->write_open == 0 && pipe->buffer[next_pos] == '\0') {
            printf ("Before delete\n");
            delete_pipe(pipe->id);
            return 0;
        }
        else {
            *c = (pipe->buffer[next_pos]);
            pipe->buffer[next_pos] = '\0';

            pipe->read_pos = next_pos;
            printf("Total pipe buffer contents: ");
            for(i = 0; i < pipe->size; i++) {
                printf("%c ", pipe->buffer[i]);
            }
            printf("\n");
            //printf("Read on pipe with id %d, on buffer position %d, addr %p, character %c, new read position %d\n", pipe->id, next_pos, c, *c, pipe->read_pos);
            return 1;
        }
                            //********KLEINEI AGWGOS GIA DIABASMA????*******
    }
    return -1; 
}

//Check if a thread can write in a pipe.
int check_to_write(pipe* pipe) {

    int next_pos = -1;
    if (pipe->write_pos < pipe->size - 1)
    {
       next_pos =  pipe->write_pos + 1;
    }
    else
    {
        next_pos = 0;
    }
    if (pipe->write_open == 1) {

        //check that not yet read positions are not being overwritten
        if (pipe->buffer[next_pos] != '\0')         //*****NESTORAAAAA BGALAME OLH THN IF XD******
        {
            return 0;

        }
        else {
            return 1;
        }
    }
    return -1;
}


//Write the new 1 byte char in the pipe
int pipe_write(int p, char c) {
    int i;

    pipe *pipe = find_pipe(p);
    if (pipe != NULL && check_to_write(pipe) != -1) {
        //printf("pos %d\n", pipe->write_pos);

        //spin if write will cause an overwrite         //*************************WRITEDONE MHPOS THELEI ALLAGH*******
        while (check_to_write(pipe) == 0);

        if (pipe->write_pos < pipe->size - 1) {
            pipe->write_pos++;
        }
        else {
            pipe->write_pos = 0;
        }
        pipe->buffer[pipe->write_pos] = c;
        printf("For pipe %d, wrote on position %d, character %c\n", pipe->id, pipe->write_pos, c);
        printf("Total pipe buffer contents: ");
        for(i = 0; i < pipe->size; i++) {
            printf("%c ", pipe->buffer[i]);
        }
        printf("\n");
        return 1;
    }
    
    return -1;
}

/*
int main(int argc, char *argv[]) {

    int i = pipe_open(3);
    char gramma, gramma1;
    int j;
    char letter_read;
    int k;

    //letter_read= (char*)malloc(sizeof(char));

    //letter_read = NULL;
    letter_read = '\0';
    // int j = pipe_open(10);
    // printf("%d\n", i);
    // pipe_read(0, &letter_read);
    printf("Letter read %c\n", letter_read);
    scanf(" %c", &gramma);
    pipe_write(0,gramma);
    scanf(" %c", &gramma1);
    pipe_write(0, gramma1);
    scanf(" %c", &gramma);

    int l = pipe_write(0, gramma);
    printf("L: %d\n", l);
    pipe_read(0, &letter_read);
    printf("Letter read %c\n", letter_read);
    scanf(" %c", &gramma);

    l = pipe_write(0, gramma);
    printf("L: %d\n", l);
    // char *r = &pipe_array[0]->buffer[0];
    // int a = pipe_read(0, r);
    pipe_writeDone(0);
    for(k=0;k <= (pipe_array[0]->size) ;k++){

        pipe_read(0, &letter_read);
        printf("Letter read %c\n", letter_read);

    }

    //printf("read test %d\n", a);
    // free(pipe_array[0]->buffer);
    // free(pipe_array[0]);
    //delete_pipe(0);
    free(pipe_array);

    return 0;
}
*/