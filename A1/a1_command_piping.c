/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/

//Please enter your name and McGill ID below
//Name: Byungwoo Kim
//McGill ID: 260621422

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2]; // Create an empty array of size 2
    pipe(fd); // Create a pipe and pass the array above to it
    int pid = fork(); // fork process, and store the Process ID of the child process for use later
    if (pid == 0) {
        // Child : execute ls using execvp
        close(1); // Close the stdout
        dup(fd[1]); // dup the write end of the child process, whose file descriptor will then be copied into #1

        char *argv[2];
        argv[0] = "ls"; // Store the executable command we want, "ls", as the first argument
        argv[1] = NULL;

        execvp(argv[0], argv); // Using execvp to execute the desired command "ls"
    }
    else {
        // Parent : print output from ls here
        waitpid(pid, NULL, WUNTRACED); // Wait for the child process to finish executing.

        close(0); // Close stdin
        dup(fd[0]); // dup the read end of the parent process.

        char *result = malloc(512*sizeof(char)); // Create and allocate memory for the incoming string
        read(fd[0], result, 512*sizeof(char)); // Read the string from the read end of the process

        printf("%s", result); // Print the passed string.
    }

    return 0;
}