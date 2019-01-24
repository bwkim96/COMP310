#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc

#define LOW 0
#define HIGH 199
#define START 53

//compare function for qsort
//you might have to sort the request array
//use the qsort function 
// an argument to qsort function is a function that compares 2 quantities
//use this there.
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

//function to swap 2 integers
void swap(int *a, int *b)
{
    if (*a != *b)
    {
        *a = (*a ^ *b);
        *b = (*a ^ *b);
        *a = (*a ^ *b);
        return;
    }
}

//Prints the sequence and the performance metric
void printSeqNPerformance(int *request, int numRequest)
{
    int i, last, acc = 0;
    last = START;
    printf("\n");
    printf("%d", START);
    for (i = 0; i < numRequest; i++)
    {
        printf(" -> %d", request[i]);
        acc += abs(last - request[i]);
        last = request[i];
    }
    printf("\nPerformance : %d\n", acc);
    return;
}

//access the disk location in FCFS
void accessFCFS(int *request, int numRequest)
{
    //simplest part of assignment
    printf("\n----------------\n");
    printf("FCFS :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SSTF
void accessSSTF(int *request, int numRequest)
{
    //first, qsort the incoming requests in order of smallest to largest for comparison
    qsort(request, numRequest, sizeof(int), cmpfunc);
    
    int reqSeq[numRequest];             // used to store the sequence of requests
    int left, right, lastIndex, newIndex = 0;
    int last = START;                   // at beginning, the last request would be the START request
    int num = numRequest;
    for (int i = 0; i < numRequest; i++) {
        // first, find where the current request is positioned by finding the left and right request
        for (int j = 0; j < num; j++) {
            if (request[j] > last) {
                left = request[j-1];
                right = request[j];
                lastIndex = j;
                break;
            }
        }
        // below is in the case if we are at the leftmost request
        // we just choose the right request
        if ( lastIndex == 0 ) {
            reqSeq[newIndex++] = right;
            last = right;
            for (int k = lastIndex; k < num; k++) {
                request[k] = request[k+1];
            }
            num--;
        }
        // below is when we are at the rightmost request
        // we just choose the left request
        else if ( lastIndex == num ) {
            reqSeq[newIndex++] = left;
            last = left;
            for (int k = (lastIndex-1); k < num; k++) {
                request[k] = request[k+1];
            }
            num--;
        }
        // below is if the left request is "closer" to the current request than then right request
        else if ( (right - last) > (last - left) ) {
            reqSeq[newIndex++] = left;
            last = left;
            for (int k = (lastIndex-1); k < num; k++) {
                request[k] = request[k+1];
            }
            num--;
        }
        // below is if the right request is "closer" to the current request than then left request
        else {
            reqSeq[newIndex++] = right;
            last = right;
            for (int k = lastIndex; k < num; k++) {
                request[k] = request[k+1];
            }
            num--;
        }
    }
    // copy the sequence of requests into the request array to be printed
    for (int i = 0; i < numRequest; i++) {
        request[i] = reqSeq[i];
    }

    printf("\n----------------\n");
    printf("SSTF :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SCAN
void accessSCAN(int *request, int numRequest)
{
    //first, qsort the incoming requests in order of smallest to largest for comparison
    qsort(request, numRequest, sizeof(int), cmpfunc);
    int newRequest[numRequest + 1];         // where we will store the sequence of requests
    int position, newCnt = 0;

    // in the case that the left end is the nearer end
    if ((HIGH - START) > (START - LOW)) {
        // find where the start position is
        for (int i = 0; i < numRequest; i++) {
            if (START < request[i]) {
                position = i;
                break;
            }
        }
        // move the header to the left
        for (int i = (position-1); i >= 0; i--) {
            newRequest[newCnt++] = request[i];
        }
        newRequest[newCnt++] = LOW;  // header reaches the left end
        // move header to the right
        for (int i = position; i < numRequest; i++) {
            newRequest[newCnt++] = request[i];
        }
    }
    // in the case that the right end is the nearer end
    else {
        // find where the start position is
        for (int i = 0; i < numRequest; i++) {
            if (START < request[i]) {
                position = i;
                break;
            }
        }
        // move the header to the right
        for (int i = position; i < numRequest; i++) {
            newRequest[newCnt++] = request[i];
        }
        newRequest[newCnt++] = HIGH;  // header reaches the right end
        // move header to the left
        for (int i = (position-1); i >= 0; i--) {
            newRequest[newCnt++] = request[i];
        }
    }

    printf("\n----------------\n");
    printf("SCAN :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int *request, int numRequest)
{
    //first, qsort the incoming requests in order of smallest to largest for comparison
    qsort(request, numRequest, sizeof(int), cmpfunc);
    int newRequest[numRequest + 2];         // where we will store the sequence of requests
    int position, newCnt = 0;

    // in the case that the left end is the nearer end
    if ((HIGH - START) > (START - LOW)) {
        // find where the start position is
        for (int i = 0; i < numRequest; i++) {
            if (START < request[i]) {
                position = i;
                break;
            }
        }
        // move the header to the left
        for (int i = (position-1); i >= 0; i--) {
            newRequest[newCnt++] = request[i];
        }
        newRequest[newCnt++] = LOW;  // header reaches the left end
        newRequest[newCnt++] = HIGH; // header jumps back to the right end
        // move header to the left
        for (int i = (numRequest-1); i >= position; i--) {
            newRequest[newCnt++] = request[i];
        }
    }
    // in the case that the right end is the nearer end
    else {
        // find where the start position is
        for (int i = 0; i < numRequest; i++) {
            if (START < request[i]) {
                position = i;
                break;
            }
        }
        // move the header to the right
        for (int i = position; i < numRequest; i++) {
            newRequest[newCnt++] = request[i];
        }
        newRequest[newCnt++] = HIGH;  // header reaches the right end
        newRequest[newCnt++] = LOW;
        // move header to the right
        for (int i = 0; i < position; i++) {
            newRequest[newCnt++] = request[i];
        }
    }

    printf("\n----------------\n");
    printf("CSCAN :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in LOOK
void accessLOOK(int *request, int numRequest)
{
     //first, qsort the incoming requests in order of smallest to largest for comparison
    qsort(request, numRequest, sizeof(int), cmpfunc);
    int newRequest[numRequest + 1];         // where we will store the sequence of requests
    int position, newCnt = 0;

    // in the case that the left end is the nearer end
    if ((HIGH - START) > (START - LOW)) {
        // find where the start position is
        for (int i = 0; i < numRequest; i++) {
            if (START < request[i]) {
                position = i;
                break;
            }
        }
        // move the header to the left
        for (int i = (position-1); i >= 0; i--) {
            newRequest[newCnt++] = request[i];
        }
        // move header to the right
        for (int i = position; i < numRequest; i++) {
            newRequest[newCnt++] = request[i];
        }
    }
    // in the case that the right end is the nearer end
    else {
        // find where the start position is
        for (int i = 0; i < numRequest; i++) {
            if (START < request[i]) {
                position = i;
                break;
            }
        }
        // move the header to the right
        for (int i = position; i < numRequest; i++) {
            newRequest[newCnt++] = request[i];
        }
        // move header to the left
        for (int i = (position-1); i >= 0; i--) {
            newRequest[newCnt++] = request[i];
        }
    }

    printf("\n----------------\n");
    printf("LOOK :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in CLOOK
void accessCLOOK(int *request, int numRequest)
{
    //first, qsort the incoming requests in order of smallest to largest for comparison
    qsort(request, numRequest, sizeof(int), cmpfunc);
    int newRequest[numRequest + 2];         // where we will store the sequence of requests
    int position, newCnt = 0;

    // in the case that the left end is the nearer end
    if ((HIGH - START) > (START - LOW)) {
        // find where the start position is
        for (int i = 0; i < numRequest; i++) {
            if (START < request[i]) {
                position = i;
                break;
            }
        }
        // move the header to the left
        for (int i = (position-1); i >= 0; i--) {
            newRequest[newCnt++] = request[i];
        }
        newRequest[newCnt++] = HIGH; // header jumps back to the right end
        // move header to the left
        for (int i = (numRequest-1); i >= position; i--) {
            newRequest[newCnt++] = request[i];
        }
    }
    // in the case that the right end is the nearer end
    else {
        // find where the start position is
        for (int i = 0; i < numRequest; i++) {
            if (START < request[i]) {
                position = i;
                break;
            }
        }
        // move the header to the right
        for (int i = position; i < numRequest; i++) {
            newRequest[newCnt++] = request[i];
        }
        newRequest[newCnt++] = LOW; //header quickly jumps back to the left end
        // move header to the right
        for (int i = 0; i < position; i++) {
            newRequest[newCnt++] = request[i];
        }
    }

    printf("\n----------------\n");
    printf("CLOOK :");
    printSeqNPerformance(newRequest,newCnt);
    printf("----------------\n");
    return;
}

int main()
{
    int *request, numRequest, i,ans;

    //allocate memory to store requests
    printf("Enter the number of disk access requests : ");
    scanf("%d", &numRequest);
    request = malloc(numRequest * sizeof(int));

    printf("Enter the requests ranging between %d and %d\n", LOW, HIGH);
    for (i = 0; i < numRequest; i++)
    {
        scanf("%d", &request[i]);
    }

    printf("\nSelect the policy : \n");
    printf("----------------\n");
    printf("1\t FCFS\n");
    printf("2\t SSTF\n");
    printf("3\t SCAN\n");
    printf("4\t CSCAN\n");
    printf("5\t LOOK\n");
    printf("6\t CLOOK\n");
    printf("----------------\n");
    scanf("%d",&ans);

    switch (ans)
    {
    //access the disk location in FCFS
    case 1: accessFCFS(request, numRequest);
        break;

    //access the disk location in SSTF
    case 2: accessSSTF(request, numRequest);
        break;

        //access the disk location in SCAN
     case 3: accessSCAN(request, numRequest);
        break;

        //access the disk location in CSCAN
    case 4: accessCSCAN(request,numRequest);
        break;

    //access the disk location in LOOK
    case 5: accessLOOK(request,numRequest);
        break;

    //access the disk location in CLOOK
    case 6: accessCLOOK(request,numRequest);
        break;

    default:
        break;
    }
    return 0;
}