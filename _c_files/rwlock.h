#ifndef RWLOCK_H
#define RWLOCK_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_ok;
    pthread_cond_t writers_ok;
    int readers;
    int writer_active;
    int waiting_writers;
} RWLock;

void rwlock_init(RWLock* lock);
void rwlock_acquire_read(RWLock* lock);
void rwlock_release_read(RWLock* lock);
void rwlock_acquire_write(RWLock* lock);
void rwlock_release_write(RWLock* lock);
void rwlock_destroy(RWLock* lock);

#endif