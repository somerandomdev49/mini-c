#ifndef MAP_H
#define MAP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "help.h"

typedef struct
{
    char *name;
    int length;
    char **values; 
} entry_t;

typedef struct
{
    int length;
    entry_t *entries;
} values_t;

void view_entries(const char *filename, values_t *values) {
    printf("%s\n", filename);
    for(int i = 0; i < values->length; ++i) {
        printf("|\n+- %s\n", values->entries[i].name);
        for(int j = 0; j < values->entries[i].length; ++j) {
            printf("|  |\n|  +- %s\n", values->entries[i].values[j]);
        }
    }
}

void read_map(const char *filename, values_t *values) {
    FILE *f = fopen(filename, "r");
    if(!f) {
        fprintf(stderr, "Could not open file '%s'!\n", filename);
        return;
    }

    int lineno = 0;
    while(true) {
        ++lineno;

        char *line = calloc(128, sizeof(char));
        if(!line) {
            fprintf(stderr, "Could not allocate 'line'!\n");
            return;
        }
        int read = 0;
        if((read = readline(f, line, 128)) == 0)
            break;
        
        // printf("Read %d bytes: '%s'\n", read, line);

        if(line[0] == ':') {
            // printf("Entry '%s'\n", line + 1);
        
            if(read - 1 == 0) {
                fprintf(stderr, "No entry name on line %d!", lineno);
                continue;
            }

            // printf("old length: %d (%zu), new length: %d (%zu)",
            //     (values->length), (values->length) * sizeof(entry_t),
            //     (values->length + 1), (values->length + 1) * sizeof(entry_t));

            values->entries = realloc(values->entries,
                (values->length + 1) * sizeof(entry_t));
            if(!values->entries) {
                fprintf(stderr, "Could not reallocate 'values->entries'!\n");
                return;
            }
            
            values->entries[values->length].length = 0;
            values->entries[values->length].values = NULL;
            values->entries[values->length].name =
                calloc(read + 1, sizeof(char));
            
            if(!values->entries[values->length].name) {
                fprintf(stderr, "Could not allocate an entry's name!\n");
                return;
            }
            
            copystr(line + 1, values->entries[values->length].name, read);
            ++values->length;
        } else {
            if(values->length == 0) {
                fprintf(stderr, "No entry to add to no line %d!", lineno);
                continue;
            }
            entry_t *last = &values->entries[values->length - 1];
            
            last->values = realloc(last->values,
                (last->length + 1) * sizeof(char*));
            if(!last->values) {
                fprintf(stderr, "Could not reallocate 'last->values'!\n");
                return;
            }

            last->values[last->length] = calloc(read + 1, sizeof(char));
            if(!last->values[last->length]) {
                fprintf(stderr, "Could not allocate an entry's value!\n");
                return;
            }

            copystr(line, last->values[last->length], read + 1);
            ++last->length;
        }
    }

    fclose(f);
}

#endif
