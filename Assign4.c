/*This code forks n processes and outputs it into the terminal*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#define MAX_PROCESSES 10
void error_exit(const char* msg) {
perror(msg);
exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]) {
if (argc != 2) {
fprintf(stderr, "Usage: %s <number_of_processes>\n", argv[0]);
exit(EXIT_FAILURE);
}
int n = atoi(argv[1]);
if (n < 1 || n > MAX_PROCESSES) {
fprintf(stderr, "Number of processes must be between 1 and 10\n");
exit(EXIT_FAILURE);
}
pid_t child_pids[MAX_PROCESSES];
time_t start_time = time(NULL);
for (int i = 0; i < n; i++) {
pid_t pid = fork();
if (pid < 0) {
error_exit("fork");
}
if (pid == 0) { // Child process
srand(time(NULL) * getpid());
int sleep_time = 1 + (random() % 10);
fprintf(stdout, "Child: pid %d executing with sleep %d at %s",
getpid(), sleep_time, ctime(&start_time));
fflush(stdout);
sleep(sleep_time);
exit(sleep_time);
} else { // Parent process
child_pids[i] = pid;
fprintf(stdout, "Parent: pid %d forked\n", getpid());
fflush(stdout);
}
}
for (int i = 0; i < n; i++) {
int status;
pid_t pid = waitpid(child_pids[i], &status, 0);
if (pid == -1) {
error_exit("waitpid");
}
if (WIFEXITED(status)) {
fprintf(stdout, "Parent: child pid %d exited normally with status %d at
%s", pid, WEXITSTATUS(status), ctime(&start_time));
fflush(stdout);
}
}
return 0;
}