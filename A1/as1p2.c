/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/ 

//Please enter your name and McGill ID below
//Name: Byungwoo Kim
//McGill ID: 260621422

//all the header files you would require
#include <stdio.h>  //for standard IO
#include <unistd.h> //for execvp/dup/dup2
#include <string.h> //for string manipulation
#include <stdlib.h> //for fork  
#include <ctype.h>  //for character type check (isnum,isalpha,isupper etc)
#include <sys/wait.h>//for waitpid
#include <fcntl.h>  //open function to open a file. type "man 2 open" in terminal
#include <time.h>   //to handle time

//pointer to Linked list head
struct node *head_job = NULL;

//pointer to
struct node *current_job = NULL;

//global variable used to store process id of process
//that has been just created
//can be used while adding a job to linked list
pid_t process_id;

//flag variable to check if redirection of output is required
int isred = 0;

//structure of a single node
//donot modify this structure
struct node
{
    int number;        //the job number
    int pid;           //the process id of the process
    char *cmd;         //string to store the command name
    time_t spawn;      //time to store the time it was spawned
    struct node *next; //when another process is called you add to the end of the linked list
};

// Add a job to the linked list
void addToJobList(char *args[])
{
    //allocate memory for the new job
    struct node *job = malloc(sizeof(struct node));
    //If the job list is empty, create a new head
    if (head_job == NULL)
    {
        //init the job number with 1
        job->number = 1;
        //set its pid from the global variable process_id
        job->pid = process_id;
        //cmd can be set to arg[0]
        job->cmd = args[0];
        //set the job->next to point to NULL.
        job->next = NULL;
        //set the job->spawn using time function
        job->spawn = (unsigned int)time(NULL);
        //set head_job to be the job
        head_job = job;
        //set current_job to be head_job
        current_job = head_job;
    }

    //Otherwise create a new job node and link the current node to it
    else
    {
        //point current_job to head_job
        current_job = head_job;
        //traverse the linked list to reach the last job
        while (current_job->next != NULL) {
            current_job = current_job->next;
        }



        //init all values of the job like above num,pid,cmd.spawn
        
        //init the job number with 1
        job->number = 1;
        //set its pid from the global variable process_id
        job->pid = process_id;
        //cmd can be set to arg[0]
        job->cmd = args[0];
        //set the job->spawn using time function
        job->spawn = (unsigned int)time(NULL);

        
        //make next of current_job point to job
        current_job->next = job;
        //make job to be current_job
        current_job = job;
        //set the next of job to be NULL
        job->next = NULL;
    }
}

//Function to refresh job list
//Run through jobs in linked list and check
//if they are done executing then remove it
void refreshJobList()
{
    //pointer require to perform operation 
    //on linked list
    struct node *current_job;
    struct node *prev_job;
    
    //variable to store returned pid 
    pid_t ret_pid;

    //perform init for pointers
    current_job = head_job;
    prev_job = head_job;

    //traverse through the linked list
    while (current_job != NULL)
    {
        if (current_job == head_job) { //if current job is the head job
            // no previous job
            //use waitpid to init ret_pid variable
            ret_pid = waitpid(current_job->pid, NULL, WNOHANG);
            //one of the below needs node removal from linked list
            if (ret_pid == 0)
            {
                //this means that the job is not finished and still running
                //update the prev_job and current_job to look at the next job
                prev_job = current_job;
                current_job = current_job->next;
            }
            else
            {
                //this means the job is done executing.
                //need to remove the current job that is finished from
                //the linked list.
                head_job = current_job->next;
                free(current_job); //free memory because it is "removed"
                current_job = head_job;
                prev_job = current_job;
            }
        }
        else {
            //use waitpid to init ret_pid variable
            ret_pid = waitpid(current_job->pid, NULL, WNOHANG);
            //one of the below needs node removal from linked list
            if (ret_pid == 0)
            {
                //this means that the job is not finished and still running
                //update the prev_job and current_job to look at the next job
                prev_job = current_job;
                current_job = current_job->next;
            }
            else
            {
                //this means the job is done executing.
                //need to remove the current job that is finished from
                //the linked list.
                prev_job->next = current_job->next; //next of prev is the next of current now
                free(current_job); //free memory because it is "removed"
                current_job = prev_job->next; //now the current job will be the next of the prev
            }
        }
    }
    return;
}

//Function that list all the jobs
void listAllJobs()
{
    struct node *current_job;
    int ret_pid;

    //refresh the linked list
    refreshJobList();

    //init current_job with head_job
    current_job = head_job;
    //heading row print only once.
    printf("\nID\tPID\tCmd\tstatus\tspawn-time\n");
        
    //traverse the linked list and print using the following statement for each job
    while (current_job != NULL) {
        printf("%d\t%d\t%s\tRUNNING\t%s\n", current_job->number, current_job->pid, current_job->cmd, ctime(&(current_job->spawn)));
        current_job = current_job->next;
    }

}

// wait till the linked list is empty
// you would have to look for a place 
// where you would call this function.
// donot modify this function
void waitForEmptyLL(int nice, int bg)
{
    if (nice == 1 && bg == 0)
    {
        while (head_job != NULL)
        {
            sleep(1);
            refreshJobList();
        }
    }
    return;
}

//function to perform word count
int wordCount(char *filename,char* flag)
{
    int cnt = 0;
    char character;
    FILE *file = fopen(filename, "r"); // open the file for "read" mode
    if (file == NULL) {
        printf("The file does not exist\n");
        return -1;
    }
    //if flag is l 
    //count the number of lines in the file 
    //set it in cnt
    if (!strcmp("-l", flag)) {
        while (!feof(file)) {
            // read each character, and increment count every time
            // we hit '\n'
            character = fgetc(file);
            if (character == '\n') {
                cnt++;
            }
        }
    }

    //if flag is w
    //count the number of words in the file
    //set it in cnt
    else if (!strcmp("-w", flag)) {
        while (/*(character = fgetc(file)) != EOF*/!feof(file)) {
            // if (character == '\n') {
            //     cnt++;
            // }
            character = fgetc(file);
            if ((character == ' ') || (character == '\n') || (character == EOF)) {
                cnt++;
            }
        }
    }

    else {
        cnt = -1;
    }

    fclose(file);
    return cnt;
 }


// function to augment waiting times for a process
// donot modify this function
void performAugmentedWait()
{
    int w, rem;
    time_t now;
    srand((unsigned int)(time(&now)));
    w = rand() % 15;
    printf("sleeping for %d\n", w);
    rem = sleep(w);
    return;
}

//simulates running process to foreground
//by making the parent process wait for
//a particular process id.
int waitforjob(char *jobnc)
{
    struct node *trv;
    int jobn = (*jobnc) - '0';
    trv = head_job;
    //traverse through linked list and find the corresponding job
    //hint : traversal done in other functions too
    while (trv->pid != jobn) {
        trv = trv->next;
    }
    //if correspoding job is found 
    //use its pid to make the parent process wait.
    //waitpid with proper argument needed here
    waitpid(trv->pid, NULL, WUNTRACED);

    return (trv->pid);
}

// splits whatever the user enters and sets the background/nice flag variable
// and returns the number of tokens processed
// donot modify this function
int getcmd(char *prompt, char *args[], int *background, int *nice)
{
    int length, i = 0;
    char *token, *loc;
    char *line = NULL;
    size_t linecap = 0;
    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);
    if (length <= 0)
    {
        exit(-1);
    }
    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL)
    {
        *background = 1;
        *loc = ' ';
    }
    else
        *background = 0;
    while ((token = strsep(&line, " \t\n")) != NULL)
    {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
        {
            if (!strcmp("nice", token))
            {
                *nice = 1;
            }
            else
            {
                args[i++] = token;
            }
        }
    }
    return i;
}

// this initialises the args to All null.
// donot modify
void initialize(char *args[])
{
    for (int i = 0; i < 20; i++)
    {
        args[i] = NULL;
    }
    return;
}

int main(void)
{
    //args is a array of charater pointers
    //where each pointer points to a string
    //which may be command , flag or filename
    char *args[20];

    //flag variables for background, status and nice
    //bg set to 1 if the command is to executed in background
    //nice set to 1 if the command is nice
    //status  
    int bg, status, nice;

    //variable to store the process id.
    pid_t pid;

    //variable to store the file des
    //helpful in output redirection
    int fd1, fd2;

    //your terminal executes endlessly unless 
    //exit command is received
    while (1)
    {
        //init background to zero
        bg = 0;
        //init nice to zero
        nice = 0;
        //init args to null
        initialize(args);
        //get the user input command
        int cnt = getcmd("\n>> ", args, &bg, &nice);
        //keep asking unless the user enters something
        while (!(cnt >= 1))
            cnt = getcmd("\n>> ", args, &bg, &nice);

        //use the if-else ladder to handle built-in commands
        //built in commands don't need redirection
        //also no need to add them to jobs linked list
        //as they always run in foreground
        if (!strcmp("jobs", args[0]))
        {
            //call the listalljobs function
            listAllJobs();
        }
        else if (!strcmp("exit", args[0]))
        {
            //exit the execution of endless while loop 
            exit(0);
        }
        else if (!strcmp("fg", args[0]))
        {
            //bring a background process to foregrounf
            int jobpid = waitforjob(args[1]);
            printf("bringing jobno %s and pid %d to foreground", args[1], jobpid);
        }
        else if (!strcmp("cd", args[0]))
        {
            int result = 0;
            // if no destination directory given 
            // change to home directory 
            if (args[1] == NULL) {
                // Use chdir to change directory to home directory
                chdir(getenv("HOME"));
            }
            //if given directory does not exist
            //print directory does not exit
            
            //if everthing is fine 
            //change to destination directory 
            else {
                // Use chdir to change to destination directory
                // If the directory doesn't exist, -1 would be returned to result
                result = chdir(args[1]);
                if (result == -1) {
                    // Print the error message if directory doesn't exist
                    printf("cd: %s: No such file or directory", args[1]);
                }
            }
        }
        else if (!strcmp("pwd", args[0]))
        {
            //use getcwd and print the current working directory
            char buffer[1024];
            if (getcwd(buffer, sizeof(buffer)) != NULL) {
                printf("%s\n", buffer);
            }
            else {
                printf("Failed to find the current working directory.");
            }
        }
        else if(!strcmp("wc",args[0]))
        {
            //call the word count function
            int cnt = wordCount(args[2],args[1]);
            if (cnt == -1) {
                printf("Unrecognized flag\n0");
            }
            else {
                printf("%d", cnt);
            }
        }
        else
        {
            //Now handle the executable commands here 
            /* the steps can be..:
            (1) fork a child process using fork()
            (2) the child process will invoke execvp()
            (3) if background is not specified, the parent will wait,
                otherwise parent starts the next command... */


            //hint : samosas are nice but often there 
            //is a long waiting line for it.

            //create a child
            pid = fork();

            //to check if it is parent
            if (pid > 0)
            {
                //we are inside parent
                //printf("inside the parent\n");
                if (bg == 0)
                {
                    //FOREGROUND
                    // waitpid with proper argument required
                    waitpid(pid, NULL, WUNTRACED);
                }
                else
                {
                    //BACKGROUND
                    process_id = pid;
                    addToJobList(args);
                    // waitpid with proper argument required
                    waitpid(pid, NULL, WUNTRACED);
                }
            }
            else
            {
                // we are inside the child

                //introducing augmented delay
                performAugmentedWait();

                //check for redirection
                //now you know what does args store
                //check if args has ">"
                //if yes set isred to 1
                //else set isred to 0
                int i = 0;
                while (args[i] != NULL) {
                    if (!strcmp(">", args[i])) {
                        isred = 1;
                        continue;
                    }
                    i++;
                }
                // The ">" will be at the arg[i], and the filename will
                // be at arg[i+1]

                //if redirection is enabled
                if (isred == 1)
                {
                    //open file and change output from stdout to that  
                    //make sure you use all the read write exec authorisation flags
                    //while you use open (man 2 open) to open file
                    int standout = dup(1);
                    close(1);
                    int fd = open(args[i+1], O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

                    //set ">" and redirected filename to NULL
                    args[i] = NULL;
                    args[i + 1] = NULL;

                    //run your command
                    execvp(args[0], args);

                    //restore to stdout
                    fflush(stdout);
                }
                else
                {
                    //simply execute the command.
                    execvp(args[0], args);
                }
            }
        }
    }

    return 0;
}