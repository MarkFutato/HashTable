/***
 * Implement parsing of a single line from commands.txt.
 */

#include "command.h"

#include <stdlib.h>
#include <string.h>

int parse_command_line(const char* line, Command* cmd, int order) {
    char buffer[256];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    cmd->type = CMD_INVALID;
    cmd->name[0] = '\0';
    cmd->salary = 0;
    cmd->priority = 0;
    cmd->order = order;

    strncpy(cmd->original_line, line, sizeof(cmd->original_line) - 1);
    cmd->original_line[sizeof(cmd->original_line) - 1] = '\0';

    char* token = strtok(buffer, ",");

    if (token == NULL) {
        return 0;
    }

    if (strcmp(token, "insert") == 0) {
        char* name = strtok(NULL, ",");
        char* salary_str = strtok(NULL, ",");
        char* priority_str = strtok(NULL, ",");

        if (name && salary_str && priority_str) {
            cmd->type = CMD_INSERT;
            strncpy(cmd->name, name, sizeof(cmd->name) - 1);
            cmd->name[sizeof(cmd->name) - 1] = '\0';
            cmd->salary = (uint32_t)atoi(salary_str);
            cmd->priority = atoi(priority_str);
            return 1;
        }
    }

    else if (strcmp(token, "delete") == 0) {
        char* name = strtok(NULL, ",");
        char* priority_str = strtok(NULL, ",");

        if (name && priority_str) {
            cmd->type = CMD_DELETE;
            strncpy(cmd->name, name, sizeof(cmd->name) - 1);
            cmd->name[sizeof(cmd->name) - 1] = '\0';
            cmd->priority = atoi(priority_str);
            return 1;
        }
    }

    else if (strcmp(token, "update") == 0) {
        char* name = strtok(NULL, ",");
        char* salary_str = strtok(NULL, ",");

        if (name && salary_str) {
            cmd->type = CMD_UPDATE;
            strncpy(cmd->name, name, sizeof(cmd->name) - 1);
            cmd->name[sizeof(cmd->name) - 1] = '\0';
            cmd->salary = (uint32_t)atoi(salary_str);
            cmd->priority =
                order + 1;  // temporary priority for logging/sorting
            return 1;
        }
    }

    else if (strcmp(token, "search") == 0) {
        char* name = strtok(NULL, ",");
        char* priority_str = strtok(NULL, ",");

        if (name && priority_str) {
            cmd->type = CMD_SEARCH;
            strncpy(cmd->name, name, sizeof(cmd->name) - 1);
            cmd->name[sizeof(cmd->name) - 1] = '\0';
            cmd->priority = atoi(priority_str);
            return 1;
        }
    }

    else if (strcmp(token, "print") == 0) {
        char* priority_str = strtok(NULL, ",");

        if (priority_str) {
            cmd->type = CMD_PRINT;
            cmd->priority = atoi(priority_str);
            return 1;
        }
    }

    return 0;
}