#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Struct for saving some variables for better operation and synchronization of the workers
typedef struct {
    int avaliable;
    int termination;
    int number_to_check;
    int notification;
} workers;

// Function to check if a number is prime , returns 1 if it is or else 0
int is_prime(int number){

    int i;

    if (number <= 1){
        return 0;
    }

    if (number == 2){
        return 1;
    }    
    if (number % 2 == 0){
        return 0;
    }
    for (i = 3; i <= sqrt(number); i += 2) {
        if (number % i == 0){
            return 0;
        }    
    }
    return 1;
}

// Function for the workers thread
void* worker_op(void* arg){
    workers* worker = (workers*)arg;

    //worker = (workers*)malloc(sizeof(workers));
    int result;

    while(1){
        //notify main that this worker is avaliable
        worker->avaliable = 1; 
        printf("I AM AVAILABLE\n");

        //wait until notification from main
        while(worker->notification==0){
           printf("I AM WAITING\n"); 
        }

        //if must terminate , terminate else check the number
        if(worker->termination==1){
            printf("I AM ABOUT TO TERMINATE\n");
            break;
        }
        else{
            result= is_prime(worker->number_to_check);
            printf("I checked the number\n");
            if(result==0){
                printf("Number %d is not prime.\n",worker->number_to_check);
            }
            else{
                printf("Number %d is prime.\n",worker->number_to_check);
            }
        }
        worker->notification=0;
    }

    //notify that the thread is terminating

    worker->termination=1;

    return NULL;
}



int main(int argc,char* argv[]){
    int number_workers; //number of the workers
    int number_array[argc-1]; //an array to store the numbers to check from command line
    int i,j; //counter
    int loop=0;
    int number_check;
    int found_available;
    int finished_threads=0;
    
    //Indicate how many workers there will be from command line
    number_workers = atoi(argv[1]);

    pthread_t threads[number_workers];
    workers worker[number_workers];

    //if the user did not type enough numbers print a message
    if (argc < 3) {
        printf("You have to indicate also the number of the workers and the numbers you want to check!\n");
        return 1;
    }

    //Storing the numbers to the array
    for (i = 2; i < argc; i++) {
        number_array[i - 2] = atoi(argv[i]);  // Convert string to integer
    }

    //initialize the workers
    for (j=0;j<number_workers;j++){
        worker[j].avaliable = 0;
        worker[j].notification=0;
        worker[j].number_to_check=0;
        worker[j].termination=0;
        pthread_create(&threads[j], NULL, worker_op, &worker[j]);
    }

    while(loop<(argc-2)){ //while the input is reached

        number_check = number_array[loop];

        //Wait until find available worker
        while(1){
            for(i=0;i<number_workers;i++){
                if(worker[i].avaliable==1){
                    found_available =1;
                    break;
                }
            }
            if(found_available==1){
                break;
            }
        }

        worker[i].notification=1; //notify the worker 
        worker[i].number_to_check = number_check; //let the worker do his work

        loop++; //next loop to check
    }

    //notify workers to terminate
    for(i=0;i<number_workers;i++){
      worker[i].termination=1;
      worker[i].avaliable=0;  
    }

    //wait for all the workers to terminate
    while(1){
        finished_threads=0;
        for(i=0;i<number_workers;i++){
            worker[i].termination=1;
            finished_threads++;
        }
        if(finished_threads==number_workers){
            break;
        }
    }

    //Free the threads
    for(i=0;i<number_workers;i++){ // *************den leitourgeiii********
        pthread_detach(threads[i]);
    }

    //PREPEI KAPOU NA GINEI FREE TO WORKER APO TO WORKER_OP

    return 0;
}