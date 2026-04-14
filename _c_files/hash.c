/***
 * Implement:
 * - jenkins_hash(const char *key)
 * - insert_record(...)
 * - delete_record(...)
 * - update_record(...)
 * - search_record(...)
 * - print_records(...)
 */

#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

hashRecord* createNode(const char* name, uint32_t salary) {
    hashRecord* newNode = (hashRecord*)malloc(sizeof(hashRecord));
    if (newNode == NULL) {
        return NULL;  // Memory allocation failed
    }
    newNode->hash = jenkins_hash(name);
    strncpy(newNode->name, name, sizeof(newNode->name) - 1);
    newNode->name[sizeof(newNode->name) - 1] = '\0';  // Ensure null-termination
    newNode->salary = salary;
    newNode->next = NULL;
    return newNode;
}

uint32_t jenkins_hash(const char* key) {
    uint32_t hash = 0;

    while (*key) {
        hash += (unsigned char)(*key++);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

int insert_record(hashRecord** head, const char* name, uint32_t salary) {
    if (search_record(*head, name) != NULL) {
        return -1;  // Duplicate
    }

    hashRecord* newNode = createNode(name, salary);
    if (newNode == NULL) {
        return -2;  // Memory allocation failed
    }

    newNode->next = *head;
    *head = newNode;

    return 0;
}

int delete_record(hashRecord** head, const char* name) {
    uint32_t hash = jenkins_hash(name);

    hashRecord* current = *head;
    hashRecord* previous = NULL;

    while (current != NULL) {
        if (current->hash == hash) {
            if (previous == NULL) {
                *head = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            return 0;
        }

        previous = current;
        current = current->next;
    }

    return -1;
}

int update_record(hashRecord* head, const char* name, uint32_t new_salary) {
    // Implementation of update_record
    hashRecord* current = search_record(head, name);
    if (current != NULL) {
        current->salary = new_salary;
        return 0;  // Success
    }
    return -1;  // Record not found
}

hashRecord* search_record(hashRecord* head, const char* name) {
    uint32_t hash = jenkins_hash(name);

    hashRecord* current = head;
    while (current != NULL) {
        if (current->hash == hash) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void print_records(hashRecord* head) {
    printf("Current Database:\n");

    hashRecord* current = head;
    while (current != NULL) {
        printf("%u,%s,%u\n", current->hash, current->name, current->salary);
        current = current->next;
    }
}

void free_records(hashRecord* head) {
    hashRecord* current = head;

    while (current != NULL) {
        hashRecord* next = current->next;
        free(current);
        current = next;
    }
}
