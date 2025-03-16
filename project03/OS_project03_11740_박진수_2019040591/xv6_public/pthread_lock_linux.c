#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

volatile int shared_resource = 0;

#define NUM_ITERS 1000
#define NUM_THREADS 1000

volatile bool entering[NUM_THREADS] = {false};
volatile bool flag[NUM_THREADS] = {false};

void lock(int tid);
void unlock(int tid);


void lock(int tid) {
    // Show intention to enter the critical section
    entering[tid] = true;

    // Wait until no other thread is in the critical section
    for (int j = 0; j < NUM_THREADS; j++) {
        if (j != tid) {
            while (flag[j]) {
                // Busy wait
            }
        }
    }

    // Declare that it is in the critical section
    flag[tid] = true;

    // Check for any thread with lower index
    for (int j = 0; j < tid; j++) {
        if (entering[j]) {
            flag[tid] = false;
            while (entering[j]) {
                // Busy wait
            }
            j = -1; // restart the loop
        }
    }

    // Now safe to enter
    entering[tid] = false;
}

void unlock(int tid) {
    flag[tid] = false;
}

void* thread_func(void* arg) {
    int tid = *(int*)arg;
    
    lock(tid);
    
        for(int i = 0; i < NUM_ITERS; i++)    shared_resource++;
    
    unlock(tid);
    
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int tids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        tids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &tids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("shared: %d\n", shared_resource);
    
    return 0;
}