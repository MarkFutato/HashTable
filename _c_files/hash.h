/***
 * Puts:
 * - hashRecord struct
 * - function prototypes for record operations
 * - prototype for Jenkins hash
 */

#ifndef HASH_H
#define HASH_H

#include <stdint.h>

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct* next;
} hashRecord;

uint32_t jenkins_hash(const char* key);

hashRecord* search_record(hashRecord* head, const char* name);
int insert_record(hashRecord** head, const char* name, uint32_t salary);
int update_record(hashRecord* head, const char* name, uint32_t new_salary);
int delete_record(hashRecord** head, const char* name);
void print_records(hashRecord* head);
void free_records(hashRecord* head);

#endif