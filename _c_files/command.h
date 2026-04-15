/***
 * Defined command struct and enum
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>

typedef enum {
    CMD_INSERT,
    CMD_DELETE,
    CMD_UPDATE,
    CMD_SEARCH,
    CMD_PRINT,
    CMD_INVALID
} CommandType;

typedef struct {
    CommandType type;
    char name[50];
    uint32_t salary;
    int priority;
    int order;
    char original_line[256];
} Command;

int parse_command_line(const char* line, Command* cmd, int order);

#endif