#ifndef ARGS_H
#define ARGS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"

#define ARG_NOARG 1
#define ARG_SHORT 2
#define ARG_LONG  4
#define ARG_ERROR 8

typedef struct {
    char *value;
    int size;
    short type;
    char *data;
} arg_t;

int readarg(int argc, char *argv[], int offset, arg_t *val) {
    if(offset >= argc) return (val->type = ARG_ERROR), 0;
    if(val->size < 2) return (val->type = ARG_ERROR), 0;
    char *arg = argv[offset];
    int argsize = strlen(arg);
    if(argsize < 2) return (val->type = ARG_ERROR), 0;
    if(arg[0] == '-' && arg[1] != '-') {
        val->type = ARG_SHORT;
        return copystr(arg + 1, val->value, val->size);
    } else if(arg[0] == '-' && arg[1] == '-') {
        val->type = ARG_LONG;
        return copystr(arg + 2, val->value, val->size);
    }
    val->type = ARG_NOARG;
    copystr(arg, val->value, val->size);
    return argsize + 1;
}

typedef struct {
    int length;
    arg_t *args;
} arglist_t;

int readargs(int argc, char *argv[], int offset, arglist_t *args) {
    if(argc == 1) return 0;
    if(offset >= argc) return 0;
    if(args->length == 0) return 0;
    int count = 0;
    while(count + offset < argc && count < args->length) {
        readarg(argc, argv, count + offset, &(args->args[count]));
        if(args->args[count].type == ARG_NOARG && count > 0) {
            args->args[count - 1].data = args->args[count].value;
        }
        ++count;
    }
    return count;
}

typedef struct
{
    int id;
    char *data;
} option_t;

typedef struct
{
    int length;
    option_t *options;
} used_options_t;

void print_all_used(used_options_t *used) {
    for(int i = 0; i < used->length; ++i) {
        printf("\tused option#%d: %d\n", i, used->options[i].id);
    }
}

void add_used_option(used_options_t *used, int opt, char *data) {
    used->options = realloc(used->options,
        (used->length + 1) * sizeof(option_t));
    used->options[used->length].id = opt;
    used->options[used->length].data = data;
    ++used->length;
}

bool check_used_option(used_options_t *used, int opt) {
    for(int i = 0; i < used->length; ++i)
        if(opt == used->options[i].id) return true;
    return false;
}


void duplicate_short_option(char name) {
    fprintf(stderr, "Duplicate short option: '%c'!\n", name);
}

void duplicate_long_option(char *name) {
    fprintf(stderr, "Duplicate long option: '%s'!\n", name);
}

bool check_short_arg(arg_t *arg, char name) {
    if(arg->type != ARG_SHORT) return false;
    char *loc = strchr(arg->value, name);
    if(loc == 0) return false;
    if(strchr(loc + 1, name)) duplicate_short_option(name);
    return true;
}

bool check_long_arg(arg_t *arg, char *name) {
    return arg->type == ARG_LONG && strcmp(arg->value, name) == 0;
}

bool parse_long_arg(arg_t *arg, used_options_t *opts, char *value, int id) {
    if(check_long_arg(arg, value)) {
        if(check_used_option(opts, id)) {
            duplicate_long_option(value);
        } else {
            add_used_option(opts, id, arg->data);
            return true;
        }
    }
    return false;
}

bool parse_short_arg(arg_t *arg, used_options_t *opts, char value, int id) {
    if(check_short_arg(arg, value)) {
        if(check_used_option(opts, id)) {
            duplicate_short_option(value);
        } else {
            add_used_option(opts, id, arg->data);
            return true;
        }
    }
    return false;
}

typedef struct
{
    char *long_name, short_name;
    int id;
    char *descr;
} arg_type_t;

void print_help(arg_type_t *type) {
    if(type->long_name) printf("--%s", type->long_name);
    if(type->long_name && type->short_name) printf(", ");
    if(type->short_name) printf("-%c", type->short_name);
    printf(" %s\n", type->descr);
}

void print_help_list(arg_type_t types[], int c) {
    puts("OPTIONS:");
    for(int i = 0; i < c; ++i) {
        printf("  "); print_help(&types[i]);
    }
}

void run_arg_list(arg_t *arg, used_options_t *opts, arg_type_t types[], int c) {
    for(int i = 0; i < c; ++i) {
        if(arg->type == ARG_LONG && types[i].long_name != 0) {
            if(parse_long_arg(arg, opts, types[i].long_name, types[i].id))
                return;
        } else if(arg->type == ARG_SHORT && types[i].short_name != 0) {
            parse_short_arg(arg, opts, types[i].short_name, types[i].id);
        }
    }
}

arglist_t *init_args(arglist_t *args, int maxlength, int maxarglength) {
    args->length = maxlength;
    args->args = calloc(args->length, sizeof(arg_t));
    for(int i = 0; i < args->length; ++i) {
        args->args[i].value = calloc(maxarglength, sizeof(char));
        args->args[i].size = maxarglength;
        args->args[i].type = 0;
        args->args[i].data = 0;
    }
    return args;
}

void free_args(arglist_t *args) {
    for(int i = 0; i < args->length; ++i) {
        free(args->args[i].value);
    }
    free(args->args);
}

void parse_args(arglist_t *args,
                used_options_t *opts,
                arg_type_t arg_types[],
                int count) {
    opts->length = 0;
    opts->options = NULL;
    for(int i = 0; i < args->length; ++i) {
        run_arg_list(&args->args[i], opts, arg_types, count);
    }
}

#endif
