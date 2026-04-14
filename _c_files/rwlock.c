#include "rwlock.h"

void rwlock_init(RWLock* lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_ok, NULL);
    pthread_cond_init(&lock->writers_ok, NULL);
    lock->readers = 0;
    lock->writer_active = 0;
    lock->waiting_writers = 0;
}

void rwlock_acquire_read(RWLock* lock) {
    pthread_mutex_lock(&lock->mutex);

    while (lock->writer_active || lock->waiting_writers > 0) {
        pthread_cond_wait(&lock->readers_ok, &lock->mutex);
    }

    lock->readers++;

    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_release_read(RWLock* lock) {
    pthread_mutex_lock(&lock->mutex);

    lock->readers--;

    if (lock->readers == 0) {
        pthread_cond_signal(&lock->writers_ok);
    }

    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_acquire_write(RWLock* lock) {
    pthread_mutex_lock(&lock->mutex);

    lock->waiting_writers++;

    while (lock->writer_active || lock->readers > 0) {
        pthread_cond_wait(&lock->writers_ok, &lock->mutex);
    }

    lock->waiting_writers--;
    lock->writer_active = 1;

    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_release_write(RWLock* lock) {
    pthread_mutex_lock(&lock->mutex);

    lock->writer_active = 0;

    if (lock->waiting_writers > 0) {
        pthread_cond_signal(&lock->writers_ok);
    } else {
        pthread_cond_broadcast(&lock->readers_ok);
    }

    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_destroy(RWLock* lock) {
    pthread_mutex_destroy(&lock->mutex);
    pthread_cond_destroy(&lock->readers_ok);
    pthread_cond_destroy(&lock->writers_ok);
}