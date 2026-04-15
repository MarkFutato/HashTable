#include "scheduler.h"

extern long long current_timestamp(void);

void scheduler_init(Scheduler* scheduler) {
    pthread_mutex_init(&scheduler->mutex, NULL);
    pthread_cond_init(&scheduler->cond, NULL);
    scheduler->current_index = 0;
}

void scheduler_wait_turn(Scheduler* scheduler, int order, int priority,
                         FILE* logFile) {
    pthread_mutex_lock(&scheduler->mutex);

    while (order != scheduler->current_index) {
        fprintf(logFile, "%lld: THREAD %d WAITING FOR MY TURN\n",
                current_timestamp(), priority);

        pthread_cond_wait(&scheduler->cond, &scheduler->mutex);
    }

    fprintf(logFile, "%lld: THREAD %d AWAKENED FOR WORK\n", current_timestamp(),
            priority);

    pthread_mutex_unlock(&scheduler->mutex);
}

void scheduler_finish_turn(Scheduler* scheduler) {
    pthread_mutex_lock(&scheduler->mutex);

    scheduler->current_index++;
    pthread_cond_broadcast(&scheduler->cond);

    pthread_mutex_unlock(&scheduler->mutex);
}

void scheduler_destroy(Scheduler* scheduler) {
    pthread_mutex_destroy(&scheduler->mutex);
    pthread_cond_destroy(&scheduler->cond);
}