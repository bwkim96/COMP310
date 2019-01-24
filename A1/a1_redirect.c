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
#include <fcntl.h> // added this line in order to use the syscall open()

int main()
    {
        printf("First : Print to stdout\n");
        
        int standout = dup(1); // The next line saves the file descriptor 1 (stdout) to int standout. This is to restore the original stdout later.
        close(1); // Close the stdout file descriptor. Now the file descriptor 1 is empty.
        int fd = open("redirect_out.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
        // In the previous line, I just opened the file descriptor for the text file to the next available smallest file descriptor,
        // which in this case would be 1, in place of the original stdout. The file was opened with O_WRONLY | O_APPEND | O_CREAT
        // because we want to either write or append to the text file, or create the file if it doesn't exist. It was also given
        // appropriate permissions.

        printf("Second : Print to redirect_out.txt\n");

        close(fd); // Close the file descriptor for the text file now that we are done with it.
        dup2(standout,1); // Restore the original stdout.
                        // I also could've used fflush(stdout)
        
        printf("Third : Print to stdout\n");

        return 0;
    }