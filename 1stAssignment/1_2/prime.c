#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Struct used for saving some variables for better operation and synchronization of the workers
// typedef struct {
//     int avaliable;
//     int termination;
//     int number_to_check;
//     int notification;
// } workers;
static int program_complete;

typedef struct {
    int num_of_workers;
    int *number_array;
    int numbers_amount;
}master;

typedef struct {
    int given_number;
    int is_available;
    int worker_id;//for debugging
    int terminate;
    int notify;
}worker;


// static int *worker_available;

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

int check_workers_availability(worker* worker_args, int num_of_workers) {
    int i;
    for(i = 0; i < num_of_workers; i++) {
        if(worker_args[i].is_available == 0) {
            return 0;
        }
    }
    return 1;
}

void *worker_op(void *arg) {
    worker *worker_args = (worker *)arg;
    int result;
    worker_args->is_available = 1;

    while(worker_args->terminate == 0) {
        // worker_args->is_available = 1;
        // printf("Im available worker with id: %d\n", worker_args->worker_id);
        if(worker_args->notify == 1){
            // worker_args->notify = 0;
            worker_args->is_available = 0;
            worker_args->notify = 0;
            printf("Worker with id: %d, checking the number %d: ", worker_args->worker_id, worker_args->given_number);
            result = is_prime(worker_args->given_number);
            if(result) {
                printf("Prime\n");
            }
            else printf("Not Prime\n");
            worker_args->is_available = 1;
        }
    }
    return NULL;

}

void* master_op(void* arg) {
    master *master_args = (master *)arg;
    int i, j;
    worker *worker_args;
    pthread_t *workers;
    
    printf("Entered master op\n");

    worker_args = (worker*)malloc(sizeof(worker) * master_args->num_of_workers);
    workers = (pthread_t *)malloc(sizeof(pthread_t) * master_args->num_of_workers);

    for(i = 0; i < master_args->num_of_workers; i++) {
        worker_args[i].given_number = -1;
        worker_args[i].is_available = 0;
        worker_args[i].worker_id = i;
        worker_args[i].terminate = 0;
        worker_args[i].notify = 0;
    }


    for(i = 0; i < master_args->num_of_workers; i++) {
        if (pthread_create(&workers[i], NULL, worker_op, &worker_args[i]) != 0) {
        fprintf(stderr, "Error creating thread %d.\n", i);
        return NULL;
        }
    }

    i = 0;
    while(i < master_args->numbers_amount) {
        for(j = 0; j < master_args->num_of_workers; j++) {
            if(i == master_args->numbers_amount) {
                    break;
                }
            if(worker_args[j].is_available == 1){
                printf("worker %d is available, passing to it number %d\n", j, master_args->number_array[i]);
                worker_args[j].given_number = master_args->number_array[i];
                worker_args[j].notify = 1;
                i = i + 1;
                while(worker_args[j].notify);// ****************AYTO MPOREI XREIAZETAI DEN EIMAI SIGOUROS AKOMA AN YPARXEI KALUTEROS TROPOS ALLA XWRIS AUTO OLA KATARREOUN***************************
                
                
                
                // break;
                // if(i == master_args->numbers_amount - 1) {
                //     break;
                // }
                // while(worker_args[j].notify);
            }
        }
    }

    printf("Master waiting for workers to stop\n");
    while(!check_workers_availability(worker_args, master_args->num_of_workers));

    printf("Master shutting down workers\n");
    for(i = 0; i < master_args->num_of_workers; i++) {
        worker_args[i].terminate = 1;
        printf("Shut down worker %d\n", i);
    }

    program_complete = 1;

    return NULL;
}

int main(int argc, char* argv[]) {

    int num_of_workers;
    int *number_array;
    int numbers_amount;
    int i;
    master master_args;
    pthread_t master_thread;

    //Exit if the user did not input arguments to the command line
    if(argc == 1) {
        printf("No commands given!\n");
        return 0;
    }
    //Exit if the user did not give any numbers fot th
    if(argc == 2) {
        printf("No numbers to check given!\n");
        return 0;
    }

    num_of_workers = atoi(argv[1]);
    numbers_amount = argc - 2;
    //Create the array that contains the numbers to be checked
    number_array = (int*)malloc(sizeof(int) * (numbers_amount));
    //Store the numbers to the array
    for(i = 2; i < argc; i++) {
        number_array[i-2] = atoi(argv[i]);
    }

    master_args.num_of_workers = num_of_workers;
    master_args.number_array = number_array;
    master_args.numbers_amount = numbers_amount;

    printf("Master num of workers: %d\n", master_args.num_of_workers);
    printf("Master num of numbers: %d\n", master_args.numbers_amount);
    printf("Master table: ");
    for(i = 0; i < master_args.numbers_amount; i++) {
        printf("%d ", master_args.number_array[i]);
    }
    printf("\n");

    // Create the master thread 
    if (pthread_create(&master_thread, NULL, master_op, &master_args) != 0) {
        fprintf(stderr, "Error creating master thread.\n");
        return 1;
    }

    while(!program_complete);
    printf("Done, exiting program\n");


    return 0;
}