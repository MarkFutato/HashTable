#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <pthread.h>
#include <stdio.h>

typedef struct Scheduler {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int current_index;
} Scheduler;

void scheduler_init(Scheduler* scheduler);
void scheduler_wait_turn(Scheduler* scheduler, int order, int priority, FILE* logFile);
void scheduler_finish_turn(Scheduler* scheduler);
void scheduler_destroy(Scheduler* scheduler);

#endif