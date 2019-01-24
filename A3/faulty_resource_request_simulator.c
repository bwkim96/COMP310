#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
int numProc;
int numRes;
int* avail;
int** max;
int** hold;
int** need;

// Simulates resource requests by processes
void request_simulator(int pr_id, int* request_vector) {
    for (int i = 0; i < numRes; i++) {
        request_vector[i] = rand() % need[pr_id][i];
    }
}

// Implementation of isSafe() as described in the slides
int isSafe() {
    int isSafe, found = 0;
    int foundi;
    int work[numRes], finish[numProc];
    for (int i = 0; i < numRes; i++) {
        work[i] = avail[i];
    }
    for (int i = 0; i < numProc; i++) {
        finish[i] = 0;
    }

    while (1) {
        // find the process meeting conditions
        for (int i = 0; i < numProc; i++) {
            for (int j = 0; j < numRes; j++) {
               if ((finish[i] == 0) && (need[i][j] <= work[j])) {
                   foundi = i;
                   i = numProc;
                   found = 1;
                   break;
               }
            }
        }
        if (found == 0) break;          // if no process meeting condition found, break
        for (int j = 0; j < numRes; j++) {
            work[j] = work[j] + hold[foundi][j];
            finish[foundi] = 1;
        }
        found = 0;          // set found back to 0
    }
    for (int i = 0; i < numProc; i++) {
        if (finish[i] == 0) {
            isSafe = 0;
        }
    }

    return isSafe;
}

// Simulates a fault occuring on the system.
void* fault_simulator(void* pr_id){
    int prob, faulty;
    while(1) {
        prob = rand() % 2;  //probability of a "faulty resource"
        if (prob == 1) {
            faulty = rand() % numRes;   // which resource is faulty
            pthread_mutex_lock(&buffer_mutex);
            avail[faulty] -= 1;
            pthread_mutex_unlock(&buffer_mutex);
        }
        sleep(10);
    }
    pthread_exit(0);
}


// Checks for deadlock
void* deadlock_checker(){
    
    while(1) {
        for (int i = 0; i < numProc; i++) {
            for (int j = 0; j < numRes; j++) {
                if (need[i][j] > avail[j]) {
                    printf("Deadlock will occur as processes request more resources, exiting...\n");
                    pthread_exit(NULL);
                }
            }
        }
    }
}

// Implementation of Bankers Algorithm as described in the slides 
// returns 1 if safe allocation 0 if not safe
int bankers_algorithm(int pr_id, int* request_vector) {
    for (int i = 0; i < numRes; ) {
        // first, "verify that a process matches its needs"
        for (int i = 0; i < numRes; i++) {
            if (request_vector[i] > need[pr_id][i]) {
                printf("Error, impossible request.\n");
                return -1;
            }
        }
        // next, "check if the requested amount is available"
        if (request_vector[i] <= avail[i]) {
            i++;    //only let the for loop continue if condition is met
        }
    }

    // provisional allocation
    for (int j = 0; j < numRes; j++) {
        pthread_mutex_lock(&buffer_mutex);
        avail[j] -= request_vector[j];
        hold[pr_id][j] += request_vector[j];
        need[pr_id][j] -= request_vector[j];
        pthread_mutex_unlock(&buffer_mutex);
    }
    // if allocation results in a safe state, continue
    printf("Checking if allocation is safe\n");
    if (isSafe()) {
        printf("System is safe: allocating\n");
        sleep(3);
    }
    // if allocation results in an unsafe state, cancel (revert) the provisional allocations
    else {
        for (int j = 0; j < numRes; j++) {
            printf("Allocation is not safe, cancelling\n");
            pthread_mutex_lock(&buffer_mutex);
            avail[j] += request_vector[j];
            hold[pr_id][j] -= request_vector[j];
            need[pr_id][j] += request_vector[j];
            pthread_mutex_unlock(&buffer_mutex);
        }
    }

    return 0;
}


// Simulates processes running on the system.
void* process_simulator(void* pr_id) {
    int *prID_ptr = (int*) pr_id;       // cast pr_id to int
    int prID = *prID_ptr;
    int processDone;

    int request_vector[numRes];         // array to store the request or each resource

    // repeat as long as there are remaining requests (remaining needs) to be made by the process
    do {
        processDone = 1;
        printf("\nRequesting resources for process %d\n", prID);
        request_simulator(prID, request_vector);    // fill in the resource request vector
        printf("The Resource vector requested array is: ");
        for (int i = 0; i < numRes; i++) {
            printf("%d ", request_vector[i]);
        }
        printf("\n");
        bankers_algorithm(prID, request_vector);    // call Banker's algorithm function
        for (int i = 0; i < numRes; i++) {
            if (need[prID][i] > 0) {
                processDone = 0;
                i = numRes;
            }
        }
    } while (!processDone);

    pthread_exit(0);
}


int main() {
    //Initialize all inputs to banker's algorithm
    //Get user inputs necessary
    printf("Enter number of processes: ");
    scanf("%d", &numProc);

    printf("Enter number of resources: ");
    scanf("%d", &numRes);

    //Allocate memories for the arrays
    avail = malloc(numRes * sizeof(int));
    max = (int**)malloc(numProc * sizeof(int*));
    hold = (int**)malloc(numProc * sizeof(int*));
    need = (int**)malloc(numProc * sizeof(int*));
    for (int i = 0; i < numProc; i++) {
        max[i] = (int*)malloc(numRes * sizeof(int));
        hold[i] = (int*)malloc(numRes * sizeof(int));
        need[i] = (int*)malloc(numRes * sizeof(int));
    }

    //Get user inputs for arrays
    printf("\nEnter Available Resources:\n");
    for (int i = 0; i < numRes; i++) {
        printf("For Resource %d: ", i+1);
        scanf("%d", &avail[i]);
    }

    printf("\nEnter Maximum Resources Each Process Can Claim:\n");
    for (int i = 0; i < numProc; i++) {
        printf("For Process %d:\n", i+1);
        for (int j = 0; j < numRes; j++) {
            printf("\tFor Resource %d: ", j+1);
            scanf("%d", &max[i][j]);
            need[i][j] = max[i][j];
            hold[i][j] = 0;
        }
    }

    printf("The Number of each resource in the system is: ");
    for (int i = 0; i < numRes; i++) {
        printf("%d ", avail[i]);
    }
    printf("\nThe Allocated Resources table is:\n");
    for (int i = 0; i < numProc; i++) {
        for (int j = 0; j < numRes; j++) {
            printf("%d ", hold[i][j]);
        }
        printf("\n");
    }
    printf("The Maximum Claim table is:\n");
    for (int i = 0; i < numProc; i++) {
        for (int j = 0; j < numRes; j++) {
            printf("%d ", max[i][j]);
        }
        printf("\n");
    }

    //create threads simulating processes (process_simulator)
    pthread_t tids[numProc+2];
    int ids[numProc+2];
    for (int i = 0; i < numProc; i++) {
        ids[i] = i;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tids[i], &attr, process_simulator, &ids[i]);
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tids[numProc], &attr, fault_simulator, &ids[numProc]);
    pthread_attr_init(&attr);
    pthread_create(&tids[numProc+1], &attr, deadlock_checker, &ids[numProc+1]);


    for (int i = 0; i < numProc+2; i++) {
        pthread_join(tids[i], NULL);
    }

    pthread_exit(NULL);

    return 0;
}