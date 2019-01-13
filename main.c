#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#define NUM_PHIL 5
#define L_PHIL (i-1)%5
#define R_PHIL (i + 1) % 5

pthread_mutex_t lock;

#define EATING 2
#define WAITING 1
#define THINKING 0
int state;
//Each state represents the state by index
int states[5] = {0,0,0,0,0};
pthread_cond_t pickUpCondition[5];
pthread_t philosopher[5];
int philID[5] = {0,1,2,3,4};

void *philosopherRoutine(void* arg);
int main() {
    if(pthread_mutex_init(&lock, NULL) == -1) 
    {
        return errno;
    }

    for(int i = 0; i < NUM_PHIL; i++)
    {
        if(pthread_cond_init(&pickUpCondition[i], NULL)==-1){
            return errno;
        }
    }

    for(int i = 0; i < NUM_PHIL; i++)
    {
	printf("Starting Phil %d\n", i);
	pthread_create(&philosopher[i], NULL, philosopherRoutine, &philID[i]);
    }

    for (int i = 0; i < NUM_PHIL; i++ )			
    {
	pthread_join (philosopher[i], NULL);						
	printf ( "Thread of Philospher %d is finished executing \n", i+1 );
    }
    return 0;
}

void *philosopherRoutine(void *arg){
    srand(time(NULL));
    int *temp = (int *)arg;
    int phil = *temp;
    double timeToChangeState;
    printf("Phil %d is starting\n", phil);
    //Initially
    states[phil] = THINKING;

    int iteration = 0;
    while(iteration < 20) {
        timeToChangeState = (double)rand()/RAND_MAX * 3.0;
        sleep(timeToChangeState);
        printf("Philosopher %d is THINKING for %f seconds\n", phil, timeToChangeState);

        //We want to eat now
        pthread_mutex_lock(&lock);
        states[phil] = WAITING;
        timeToChangeState = (double)rand()/RAND_MAX * 3.0;
        sleep(timeToChangeState);
        printf("Philosopher %d is WAITING\n", phil);
        pthread_mutex_unlock(&lock);

        //looking for conditions to eat
        pthread_mutex_lock(&lock);
        while(states[(phil + 1) % 5] == EATING || states[(phil + 5-1) % 5] == EATING){
	    printf("Philosopher %d is WAITING STILL\n", phil);
            pthread_cond_wait(&pickUpCondition[phil], &lock);
        }

        //break out

        states[phil] = EATING;
        timeToChangeState = (double)rand()/RAND_MAX * 3.0;
        sleep(timeToChangeState);
        printf("Philosopher %d is EATING for %f seconds\n", phil, timeToChangeState);
        pthread_mutex_unlock(&lock);

	pthread_mutex_lock(&lock);
	pthread_cond_broadcast(&pickUpCondition[phil]);
	states[phil] = THINKING;
	pthread_mutex_unlock(&lock);
        //change to thinking safely
     

        timeToChangeState = (double)rand()/RAND_MAX * 3.0;
        sleep(timeToChangeState);
        //pthread_cond_signal(&pickUpCondition[phil]);
        printf("Philosopher %d is THINKING READY TO BE USED for %f seconds\n", phil, timeToChangeState);
        iteration++;
    }
}
