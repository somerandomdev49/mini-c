#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "map.h"
#include "args.h"
#include "help.h"

enum { opt_id_askname, opt_id_cat, opt_id_read, opt_id_help };

void option_askname() {
    char *name = calloc(32, sizeof(char));
    printf("What's your name? ");
    if(readline(stdin, name, 32) != 0) {
        printf("Hello, %s!\n", name);
    }
    free(name);
}

void option_cat(const char *filename) {
    FILE *f = fopen(filename, "r");
    if(!f) {
        fprintf(stderr, "Could not open file '%s'!\n", filename);
        return;
    }

    char c;
    while((c = fgetc(f)) != EOF) {
        putc(c, stdout);
    }

    fclose(f);
}

void option_read(const char *filename) {
    values_t vals;
    vals.entries = NULL;
    vals.length = 0;
    read_map(filename, &vals);
    view_entries(filename, &vals);
    for(int i = 0; i < vals.length; ++i) {
        free(vals.entries[i].name);
        for(int j = 0; j < vals.entries[i].length; ++j) {
            free(vals.entries[i].values[j]);
        }
    }
    free(vals.entries);
}

void option_help(char *name, arg_type_t types[], int c) {
    printf("OVERVIEW: Useful things.\n");
    printf("USAGE: %s <options>\n", name);
    print_help_list(types, c);
}

int main(int argc, char *argv[]) {
    arglist_t args;
    init_args(&args, 16, 32);
    readargs(argc, argv, 1, &args);

    arg_type_t arg_types[] = {
        (arg_type_t){ "help", 'h', opt_id_help, "- show this help message." },
        (arg_type_t){ "cat", 'c', opt_id_cat, "<file> - output file's contents to stdout." },
        (arg_type_t){ "ask-name", 'a', opt_id_askname, "- ask for a name." },
        (arg_type_t){ "read", 'r', opt_id_read, "<file> - read a file in a special format." },
    };

    int length = sizeof(arg_types) / sizeof(arg_type_t);

    used_options_t used;
    parse_args(&args, &used, arg_types, length);
    
    for(int i = 0; i < used.length; ++i) {
        switch(used.options[i].id) {
            case opt_id_askname: option_askname(); break;
            case opt_id_cat: option_cat(used.options[i].data); break;
            case opt_id_read: option_read(used.options[i].data); break;
            case opt_id_help: option_help(argv[0], arg_types, length); break;
            default:
                printf("Bad option id %d!\n", used.options[i].id);
                break;
        }
    }

    free(used.options);
    free_args(&args);
}
 