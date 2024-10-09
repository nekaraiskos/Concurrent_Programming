#include <stdio.h>
#include "pipe.h"
#include <pthread.h>

/*
Function for the first thread.
At first create first pipe and send the contents of given file via the pipe to the other thread.
After all the contents of the file are written to the pipe, close pipe for writing and wait for another pipe to open
Read every character from the second pipe opened by the other thread and save it to a new file .copy2
Compare original file with the second copy. If they are the same, the process is a success 
*/
void *thread1_op() {

} 


/*
Function of the second thread.
At first when a pipe is created read all the contents until the pipe is closed for writing and empty.
Pass the content read from the pipe to a new file .copy.
Open a new pipe and start passing the contents of the copy file to the original thread.
When the writing is done, close the file for writing.
*/
void *thread2_op() {
    
}


int main(int argc, char *argv[]) {

}