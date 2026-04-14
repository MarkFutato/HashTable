/***
 * This will:
 * - open commands.txt
 * - read one line at a time
 * - parse each line into a Command
 * - call the right database function
 * - print results to stdout
 * - do a final print
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "command.h"
#include "hash.h"
#include "rwlock.h"

long long current_timestamp(void);

void log_command(FILE* logFile, int priority, const char* commandText);
void log_read_lock_acquired(FILE* logFile, int priority);
void log_read_lock_released(FILE* logFile, int priority);
void log_write_lock_acquired(FILE* logFile, int priority);
void log_write_lock_released(FILE* logFile, int priority);

typedef struct Scheduler {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int current_priority;
} Scheduler;
typedef struct ThreadData {
    Command cmd;
    hashRecord** head;
    FILE* logFile;
    Scheduler* scheduler;
    RWLock* dbLock;
} ThreadData;

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL);  // get current time
    long long microseconds =
        (te.tv_sec * 1000000LL) + te.tv_usec;  // calculate milliseconds
    return microseconds;
}

void* execute_command(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    Scheduler* scheduler = data->scheduler;

    RWLock* dbLock = data->dbLock;

    Command cmd = data->cmd;
    hashRecord** head = data->head;
    FILE* logFile = data->logFile;

    pthread_mutex_lock(&scheduler->mutex);

    while (cmd.priority != scheduler->current_priority) {
        fprintf(logFile, "%lld: THREAD %d WAITING FOR MY TURN\n",
                current_timestamp(), cmd.priority);

        pthread_cond_wait(&scheduler->cond, &scheduler->mutex);
    }

    fprintf(logFile, "%lld: THREAD %d AWAKENED FOR WORK\n", current_timestamp(),
            cmd.priority);

    pthread_mutex_unlock(&scheduler->mutex);

    if (cmd.type == CMD_INSERT) {
        log_command(logFile, cmd.priority, cmd.original_line);
        rwlock_acquire_write(dbLock);
        log_write_lock_acquired(logFile, cmd.priority);

        int result = insert_record(head, cmd.name, cmd.salary);

        if (result == 0) {
            hashRecord* record = search_record(*head, cmd.name);
            printf("Inserted %u,%s,%u\n", record->hash, record->name,
                   record->salary);
        } else {
            uint32_t hash = jenkins_hash(cmd.name);
            printf("Insert failed. Entry %u is a duplicate.\n", hash);
        }

        log_write_lock_released(logFile, cmd.priority);
        rwlock_release_write(dbLock);
    }

    else if (cmd.type == CMD_DELETE) {
        log_command(logFile, cmd.priority, cmd.original_line);
        rwlock_acquire_write(dbLock);
        log_write_lock_acquired(logFile, cmd.priority);

        int result = delete_record(head, cmd.name);

        if (result == 0) {
            printf("Deleted record for %s\n", cmd.name);
        } else {
            uint32_t hash = jenkins_hash(cmd.name);
            printf("Delete failed. Entry %u not found.\n", hash);
        }

        log_write_lock_released(logFile, cmd.priority);
        rwlock_release_write(dbLock);
    }

    else if (cmd.type == CMD_UPDATE) {
        log_command(logFile, cmd.priority, cmd.original_line);

        rwlock_acquire_write(dbLock);
        log_write_lock_acquired(logFile, cmd.priority);

        hashRecord* before = search_record(*head, cmd.name);
        uint32_t hash = jenkins_hash(cmd.name);

        if (before != NULL) {
            uint32_t old_salary = before->salary;

            update_record(*head, cmd.name, cmd.salary);

            hashRecord* after = search_record(*head, cmd.name);

            printf("Updated record %u from %s,%u to %s,%u\n", hash, after->name,
                   old_salary, after->name, after->salary);
        } else {
            printf("Update failed. Entry %u not found.\n", hash);
        }

        log_write_lock_released(logFile, cmd.priority);
        rwlock_release_write(dbLock);
    }

    else if (cmd.type == CMD_SEARCH) {
        log_command(logFile, cmd.priority, cmd.original_line);
        rwlock_acquire_read(dbLock);
        log_read_lock_acquired(logFile, cmd.priority);

        hashRecord* record = search_record(*head, cmd.name);

        if (record != NULL) {
            printf("Found: %u,%s,%u\n", record->hash, record->name,
                   record->salary);
        } else {
            printf("Not Found: %s not found.\n", cmd.name);
        }

        log_read_lock_released(logFile, cmd.priority);
        rwlock_release_read(dbLock);
    }

    else if (cmd.type == CMD_PRINT) {
        log_command(logFile, cmd.priority, cmd.original_line);
        rwlock_acquire_read(dbLock);
        log_read_lock_acquired(logFile, cmd.priority);

        print_records(*head);

        log_read_lock_released(logFile, cmd.priority);
        rwlock_release_read(dbLock);
    }

    pthread_mutex_lock(&scheduler->mutex);
    scheduler->current_priority++;
    pthread_cond_broadcast(&scheduler->cond);
    pthread_mutex_unlock(&scheduler->mutex);

    return NULL;
}

void log_command(FILE* logFile, int priority, const char* commandText) {
    fprintf(logFile, "%lld: THREAD %d,%s\n", current_timestamp(), priority,
            commandText);
}

void log_read_lock_acquired(FILE* logFile, int priority) {
    fprintf(logFile, "%lld: THREAD %d READ LOCK ACQUIRED\n",
            current_timestamp(), priority);
}

void log_read_lock_released(FILE* logFile, int priority) {
    fprintf(logFile, "%lld: THREAD %d READ LOCK RELEASED\n",
            current_timestamp(), priority);
}

void log_write_lock_acquired(FILE* logFile, int priority) {
    fprintf(logFile, "%lld: THREAD %d WRITE LOCK ACQUIRED\n",
            current_timestamp(), priority);
}

void log_write_lock_released(FILE* logFile, int priority) {
    fprintf(logFile, "%lld: THREAD %d WRITE LOCK RELEASED\n",
            current_timestamp(), priority);
}

int compare_commands_by_priority(const void* a, const void* b) {
    const Command* cmdA = (const Command*)a;
    const Command* cmdB = (const Command*)b;

    return cmdA->priority - cmdB->priority;
}

int main(void) {
    hashRecord* head = NULL;

    Command commands[100];
    int command_count = 0;

    FILE* fp = fopen("commands.txt", "r");
    if (fp == NULL) {
        perror("Unable to open commands.txt");
        return 1;
    }

    FILE* logFile = fopen("hash.log", "w");
    if (logFile == NULL) {
        printf("Unable to open hash.log\n");
        fclose(fp);
        return 1;
    }
    char line[256];

    Scheduler scheduler;
    pthread_mutex_init(&scheduler.mutex, NULL);
    pthread_cond_init(&scheduler.cond, NULL);
    scheduler.current_priority = 1;

    RWLock dbLock;
    rwlock_init(&dbLock);

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';

        char originalLine[256];
        strcpy(originalLine, line);

        char* token = strtok(line, ",");

        if (token == NULL) {
            continue;
        }

        Command cmd;
        cmd.type = CMD_INVALID;
        cmd.name[0] = '\0';
        cmd.salary = 0;
        cmd.priority = 0;
        strcpy(cmd.original_line, originalLine);

        if (strcmp(token, "insert") == 0) {
            char* name = strtok(NULL, ",");
            char* salary_str = strtok(NULL, ",");
            char* priority_str = strtok(NULL, ",");

            if (name != NULL && salary_str != NULL && priority_str != NULL) {
                cmd.type = CMD_INSERT;
                strncpy(cmd.name, name, sizeof(cmd.name) - 1);
                cmd.name[sizeof(cmd.name) - 1] = '\0';
                cmd.salary = (uint32_t)atoi(salary_str);
                cmd.priority = atoi(priority_str);
            }
        } else if (strcmp(token, "delete") == 0) {
            char* name = strtok(NULL, ",");
            char* priority_str = strtok(NULL, ",");

            if (name != NULL && priority_str != NULL) {
                cmd.type = CMD_DELETE;
                strncpy(cmd.name, name, sizeof(cmd.name) - 1);
                cmd.name[sizeof(cmd.name) - 1] = '\0';
                cmd.priority = atoi(priority_str);
            }
        } else if (strcmp(token, "update") == 0) {
            char* name = strtok(NULL, ",");
            char* salary_str = strtok(NULL, ",");

            if (name != NULL && salary_str != NULL) {
                cmd.type = CMD_UPDATE;
                strncpy(cmd.name, name, sizeof(cmd.name) - 1);
                cmd.name[sizeof(cmd.name) - 1] = '\0';
                cmd.salary = (uint32_t)atoi(salary_str);
                cmd.priority = 0;
            }
        } else if (strcmp(token, "search") == 0) {
            char* name = strtok(NULL, ",");
            char* priority_str = strtok(NULL, ",");

            if (name != NULL && priority_str != NULL) {
                cmd.type = CMD_SEARCH;
                strncpy(cmd.name, name, sizeof(cmd.name) - 1);
                cmd.name[sizeof(cmd.name) - 1] = '\0';
                cmd.priority = atoi(priority_str);
            }
        } else if (strcmp(token, "print") == 0) {
            char* priority_str = strtok(NULL, ",");

            if (priority_str != NULL) {
                cmd.type = CMD_PRINT;
                cmd.priority = atoi(priority_str);
            }
        }

        if (cmd.type != CMD_INVALID && command_count < 100) {
            commands[command_count++] = cmd;
        }
    }

    qsort(commands, command_count, sizeof(Command),
          compare_commands_by_priority);

    pthread_t threads[100];
    ThreadData threadData[100];

    for (int i = 0; i < command_count; i++) {
        threadData[i].cmd = commands[i];
        threadData[i].head = &head;
        threadData[i].logFile = logFile;
        threadData[i].scheduler = &scheduler;
        threadData[i].dbLock = &dbLock;

        pthread_create(&threads[i], NULL, execute_command, &threadData[i]);
    }

    for (int i = 0; i < command_count; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&scheduler.mutex);
    pthread_cond_destroy(&scheduler.cond);

    rwlock_destroy(&dbLock);

    fclose(fp);
    fclose(logFile);

    printf("\nFinal Database:\n");
    print_records(head);
    free_records(head);
    return 0;
}
