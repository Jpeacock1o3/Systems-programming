/*This assignment involves creating a threaded program to simulate deadlocks using mutex locks. 
The program takes a number (1 to 10) as a command line argument, creates that many threads, 
and each thread locks two mutexes in different orders based on its number, 
then sleeps to induce potential deadlocks. 
The main routine periodically checks for deadlocks using pthread_mutex_trylock. 
If the number of failed lock attempts equals or exceeds the number of threads, it declares a deadlock.*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t even;
pthread_mutex_t odd;

void *thread_func(void *arg) {
    int *ind = (int *) arg;
    pid_t pid;
    pthread_t tid;
    pid = getpid();
    tid = pthread_self();

    printf("Thread %d, pid %d, tid %ld\n",*ind + 1, pid, tid);

    if ((*ind + 1) % 2 == 0) {
        if (pthread_mutex_lock(&even) != 0) {
            perror("Error with mutex lock even in EVEN->ODD\n");
            pthread_exit(arg);
        }
        else {
            printf("Thread %d, even locked\n", *ind + 1);
        }
        sleep(5);
        if (pthread_mutex_lock(&odd) != 0) {
            perror("Error with mutex lock odd in EVEN->ODD\n");
            pthread_exit(arg);
        }
        else {
            printf("Thread %d, odd locked\n", *ind + 1);
        }

    }
    else {
        if (pthread_mutex_lock(&odd) != 0) {
            perror("Error with mutex lock odd in ODD->EVEN\n");
            pthread_exit(arg);
        }
        else {
            printf("Thread %d, odd locked\n", *ind + 1);
        }
        sleep(5);
        if (pthread_mutex_lock(&even) != 0) {
      perror("Error with mutex lock even in ODD->EVEN\n");
            pthread_exit(arg);
        }
        else {
            printf("Thread %d, even locked\n", *ind + 1);
        }
    }




    if ((*ind + 1) % 2 == 0) {
        pthread_mutex_unlock(&odd);
        pthread_mutex_unlock(&even);
    }
    else {
        pthread_mutex_unlock(&even);
        pthread_mutex_unlock(&odd);
    }
   
    pthread_exit(arg);
}


int main(int argc, char **argv) {
    if (argc == 2) {
        int bad = 0;
        int count = atoi(argv[1]);
        if (count >= 1 && count <= 10) {
            if (pthread_mutex_init(&even, NULL) != 0) {
                perror("Problem initializing mutex (even)\n");
                exit(0);
            }
            if (pthread_mutex_init(&odd, NULL) != 0) {
                perror("Problem initializing mutex (odd)\n");
                exit(0);
            }
            pthread_t thread[10];
            int countArray[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            void *val;
            for (int i = 0; i < count; i++) {
                if ( pthread_create(&thread[i], NULL, thread_func, (void*) &countArray[i]) != 0) {
 perror("Problem creating thread\n");
                    return -1;
                }
                printf("Main: thread %d created\n", i+1);
            }

            for (int i = 0; i < count; i++) {
                sleep(count);
                if (pthread_mutex_trylock(&even) == 0) {

                    pthread_mutex_unlock(&even);
                }
                else {
                    bad++;
                    printf("Possible deadlock detected at %d seconds\n", (i + 1) * count);
                }
            }

            pthread_mutex_destroy(&even);
            pthread_mutex_destroy(&odd);
            printf("Trylock attempts that failed: %d\n", bad);
            if (bad >= count) {
                printf("Deadlock detected\n\n");
            }
            else {
                printf("\n");
            }
       }
       else {
            printf("count must be between 1 and 10.\n\n");
       }
    }
    else {
        printf("Incorrect number of arguments entered.\n\n");
    }  



    return 0;
}




