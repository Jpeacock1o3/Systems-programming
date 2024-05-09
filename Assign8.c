#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *arg) {
    int *ind = (int *) arg;
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = pthread_self();

    printf("Thread %d, pid %d, tid %ld\n",*ind + 1, pid, tid);

    char *cmds[10] = {"echo What\0", "echo a\0",
    "echo beautiful\0", "echo day\0",
    "echo for\0", "echo a\0", "echo walk\0",
    "echo in\0", "echo the\0",
    "echo park.\0" };


    system(cmds[*ind]);
    pthread_exit(arg);
}


int main(int argc, char **argv) {
    if (argc != 2) {
        perror("invalid number of arguments\n");
    }
       int threads = atoi(argv[1]);
       if (threads >= 1 && threads <= 10) {
            pthread_t thread[10];
            int count[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            void *val;
            for (int i = 0; i < threads; i++) {
                if ( pthread_create(&thread[i], NULL, thread_func, (void*) &count[i]) != 0) {
                    perror("Problem creating thread\n");
                    return -1;
                }
                printf("Main: thread %d created\n", i+1);
            }

            for (int i = 0; i < threads; i++) {
                pthread_join(thread[i], &val);
                long *result = (long *)val;
  fprintf(stdout, "Main: thread %d returned %ld\n", i + 1, (*result & 0x7f) + 1);
            }

        printf("\n");
       }
       else {
            printf("threads must be between 1 and 10.\n\n");
       }
    




    return 0;
}