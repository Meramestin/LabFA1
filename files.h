#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include "common.h"

static inline int open_output(const char *path, FILE *input1, FILE *input2, FILE **output) {
    if (path == NULL || input1 == NULL || output == NULL) return INVALID;
    FILE *file = fopen(path, "ab");
    if (file == NULL) return FILE_ERROR;
    struct stat out_info, in_info;
    int status = OK;
    if (fstat(fileno(file), &out_info) != 0 || !S_ISREG(out_info.st_mode)) status = FILE_ERROR;
    FILE *inputs[2] = {input1, input2};
    for (int i = 0; status == OK && i < 2; ++i) {
        if (inputs[i] == NULL) continue;
        if (fstat(fileno(inputs[i]), &in_info) != 0) status = FILE_ERROR;
        else if (in_info.st_dev == out_info.st_dev && in_info.st_ino == out_info.st_ino) status = INVALID;
    }
    if (status == OK && ftruncate(fileno(file), 0) != 0) status = FILE_ERROR;
    if (status != OK) {
        if (fclose(file) != 0) return FILE_ERROR;
        return status;
    }
    *output = file;
    return OK;
}

static inline int read_token(FILE *file, char **token, int *found) {
    if (file == NULL || token == NULL || found == NULL) return INVALID;
    *token = NULL;
    *found = 0;
    int c;
    do { c = fgetc(file); } while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
    if (c == EOF) return ferror(file) ? FILE_ERROR : OK;
    size_t size = 32, length = 0;
    char *text = malloc(size);
    if (text == NULL) return MEMORY_ERROR;
    while (c != EOF && c != ' ' && c != '\t' && c != '\n' && c != '\r') {
        if (c == 0) { free(text); return INVALID; }
        if (length == size - 1) {
            if (size > SIZE_MAX / 2) { free(text); return OVERFLOW; }
            char *next = realloc(text, size * 2);
            if (next == NULL) { free(text); return MEMORY_ERROR; }
            text = next;
            size *= 2;
        }
        text[length++] = (char)c;
        c = fgetc(file);
    }
    if (ferror(file)) { free(text); return FILE_ERROR; }
    text[length] = '\0';
    *token = text;
    *found = 1;
    return OK;
}

#endif
