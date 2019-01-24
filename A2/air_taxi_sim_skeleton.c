/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------
 */

//Please enter your name and McGill ID below
//Name: <Byungwoo Kim>
//McGill ID: <260621422>

 

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>


int BUFFER_SIZE = 100; //size of queue
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t full_sem;
sem_t empty_sem;

// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};
 
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
 
// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{
    return ((queue->size ) >= queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
 
// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    //printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void print(struct Queue* queue){
    if (queue->size == 0){
        return;
    }
    
    for (int i = queue->front; i < queue->front +queue->size; i++){
        
        printf(" Element at position %d is %d \n ", i % (queue->capacity ), queue->array[i % (queue->capacity)]);
    }
    
}

struct Queue* queue;

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void* cl_id)
{
    int passenger;
    int *airplane_number_ptr = (int*) cl_id;
    int airplane_number = *airplane_number_ptr;
    //printf("AIRPLANE %d\n\n\n", airplane_number);
    srand(time(NULL)); //set up random generator for the number of passengers dumped by the plane
    int num_passengers = rand() % (10 + 1 - 5) + 5; // get random number from 5-10 people who take Taxi from plane
    //printf("Airplane %d arrives with %d passengers\n", airplane_number, num_passengers);
    /*for (int i = 0; i < num_passengers; i++) {
        passenger = 1000000 + (airplane_number * 1000) + i; // creating passenger ID
        enqueue(queue, passenger);      // adding the passenger to the queue
        printf("Passenger %d of airplane %d arrives to platform\n", passenger, airplane_number);
    }*/
    //int i = 0;
    //while (i++ < num_passengers) {
    for (int i = 0; i < num_passengers; i++) {
        sem_wait(&empty_sem);
        pthread_mutex_lock(&buffer_mutex);
        printf("Airplane %d arrives with %d passengers\n", airplane_number, num_passengers);
        passenger = 1000000 + (airplane_number * 1000) + i; // creating passenger ID
        if (isFull(queue)) {
            printf("Platform is full: Rest of passengers of plane %d take the bus\n", airplane_number);
            pthread_mutex_unlock(&buffer_mutex);
            //sem_post(&empty_sem);
            sem_post(&full_sem);
            pthread_exit(0);
        }
        //sem_wait(&full_sem);
        // sem_wait(&empty_sem);
        // pthread_mutex_lock(&buffer_mutex);
        enqueue(queue, passenger);      // adding the passenger to the queue
        printf("Passenger %d of airplane %d arrives to platform / %d\n", passenger, airplane_number, queue->size);
        pthread_mutex_unlock(&buffer_mutex);
        //sem_post(&empty_sem);
        sem_post(&full_sem);
        //printf("Passenger %d of airplane %d arrives to platform\n", passenger, airplane_number);
    }


    pthread_exit(0);    // exit thread
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void* pr_id)
{
    int passenger;
    int *taxi_number_ptr = (int*) pr_id;
    int taxi_number = *taxi_number_ptr;
    srand(time(NULL)); //set up random generator for how long it took for the taxi to take a passenger home and come back
    //int n = rand() % (30 + 1 - 10) + 10; // how many minutes it took for the taxi to come back
    printf("Taxi driver %d arrives\n", taxi_number);

    while (1) {
        if (isEmpty(queue)) {
            printf("Taxi driver %d waits for passengers to enter the platform\n", taxi_number);
            break;
        }
        int n = rand() % (30 + 1 - 10) + 10; // how many minutes it took for the taxi to come back
        //printf("Taxi driver %d arrives\n", taxi_number);
        //sem_wait(&empty_sem);
        sem_wait(&full_sem);
        pthread_mutex_lock(&buffer_mutex);
        passenger = dequeue(queue);
        pthread_mutex_unlock(&buffer_mutex);
        //sem_post(&full_sem);
        sem_post(&empty_sem);
        printf("Taxi driver %d picked up client %d from the platform\n", taxi_number, passenger);
        sleep(n/60); // 1 hour simulated as 1 second
        //printf("Taxi driver %d returned\n", taxi_number);
    }

    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    //error if the program is executed wrong
    if (argc != 3) {
        printf("Error: Please enter in this form: ./air_taxi_sim num_airplanes num_taxis\n");
        return -1;
    }

    int num_airplanes;
    int num_taxis;

    num_airplanes=atoi(argv[1]);
    num_taxis=atoi(argv[2]);

    printf("You entered: %d airplanes per hour\n",num_airplanes);
    printf("You entered: %d taxis\n", num_taxis);


    //initialize queue
    queue = createQueue(BUFFER_SIZE);

    //declare arrays of threads and initialize semaphore(s)
    pthread_t taxi_tids[num_taxis];
    pthread_t airplane_tids[num_airplanes];
    //pthread_mutex_init(&buffer_mutex, NULL);
    sem_init(&full_sem, 0, BUFFER_SIZE);
    sem_init(&empty_sem, 0, 0);

    //create arrays of integer pointers to ids for taxi / airplane threads
    int *taxi_ids[num_taxis];
    int *airplane_ids[num_airplanes];

    //create threads for airplanes
    for (int i = 0; i < num_airplanes; i++) {
        printf("Creating airplane thread %d\n", i);
        airplane_ids[i] = i;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&airplane_tids[i], &attr, FnAirplane, &airplane_ids[i]);
    }

    // Wait until thread is done its work
    for (int i = 0; i < num_airplanes; i++) {
        pthread_join(airplane_tids[i], NULL);
    }
    
    //create threads for taxis
    for (int i = 0; i < num_taxis; i++) {
        //taxi_ids[i] = &i;
        taxi_ids[i] = i;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&taxi_tids[i], &attr, FnTaxi, &taxi_ids[i]);
    }

    // Wait until thread is done its work
    for (int i = 0; i < num_taxis; i++) {
        pthread_join(taxi_tids[i], NULL);
    }

    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);
    pthread_exit(NULL);
    }
