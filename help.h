#ifndef HELP_H
#define HELP_H
#include <stdio.h>
#include <stdlib.h>

int copystr(char *from, char *dest, int maxsize) {
    int i = 0;
    while(from[i] != 0 && i < maxsize - 1) {
        dest[i] = from[i];
        ++i;
    }
    dest[i] = 0;
    return i;
}

int readline(FILE *stream, char *buffer, int length) {
    if(length <= 0) return 0;
    int read = 0;
    char c;
    while(!feof(stream) && (c = getc(stream)) != EOF) {
        if(c == '\n') break;
        if(read == length - 1) break;
        buffer[read] = c;
        ++read;
    }
    buffer[read] = 0;
    return read;
}

#endif
