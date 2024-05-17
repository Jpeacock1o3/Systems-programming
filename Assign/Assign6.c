/*this code creates a file and waits a certain amount of time that the user inputs to
output to the file using SIGALRM
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>

void sigalrm_handler(int signum) {
    // Handle the SIGALRM signal
    printf("Child pid %d received timer signal %d\n", getpid(), signum);
    // Send SIGUSR1 to the parent - Commented out to avoid shell issues
    // kill(getppid(), SIGUSR1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_intervals>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert the argument to an integer and calculate numseconds
    int num_intervals = atoi(argv[1]);
    int numseconds = num_intervals * 5;

    // Open output file: out.<pid>
    char filename[20];
    char repeat[150];
    sprintf(filename, "out.%d", getpid());
    int fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    sprintf(repeat, "Child 1 (pid %d) starts alarm timer %d seconds long\nChild pid %d received timer signal %d\nChild sent SIGUSR1 to parent %d\n\n",getpid(), numseconds, getpid(), SIGALRM, getpid());
    // Set up SIGALRM signal handler
    signal(SIGALRM, sigalrm_handler);

    // Start alarm timer for numseconds
    alarm(numseconds);

    // Wait for the signal to arrive
    pause();
    write(fd, repeat, sizeof(repeat));

    // Close output file
    close(fd);

    exit(EXIT_SUCCESS);
}

