#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../files.h"

int transform_byte(const unsigned char c, const char mode, char text[3], int *length) {
    if (text == NULL || length == NULL || (mode != 'd' && mode != 'a')) return INVALID;
    const int digit = c >= '0' && c <= '9';
    *length = 0;
    if (mode == 'd' && digit) return OK;
    if (mode == 'd' || digit) {
        text[0] = (char)c;
        *length = 1;
    } else {
        const char *hex = "0123456789ABCDEF";
        if (c >= 16) text[(*length)++] = hex[c / 16];
        text[(*length)++] = hex[c % 16];
    }
    return OK;
}

int count_byte(const unsigned char c, const char mode, size_t *count) {
    if (count == NULL || (mode != 'i' && mode != 's')) return INVALID;
    const int letter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    const int digit = c >= '0' && c <= '9';
    if ((mode == 'i' && letter) || (mode == 's' && !letter && !digit && c != ' ')) {
        if (*count == SIZE_MAX) return OVERFLOW;
        ++*count;
    }
    return OK;
}

int process_file(FILE *input, FILE *output, const char mode) {
    if (input == NULL || output == NULL || mode == '\0' || strchr("disa", mode) == NULL) return INVALID;
    size_t count = 0;
    int c, has_line = 0;
    while ((c = fgetc(input)) != EOF) {
        if (mode == 'i' || mode == 's') {
            if (c == '\n') {
                if (fprintf(output, "%zu\n", count) < 0) return FILE_ERROR;
                count = 0;
                has_line = 0;
            } else {
                has_line = 1;
                const int status = count_byte((unsigned char)c, mode, &count);
                if (status != OK) return status;
            }
        } else {
            char text[3];
            int length;
            const int status = transform_byte((unsigned char)c, mode, text, &length);
            if (status != OK) return status;
            if (fwrite(text, 1, (size_t)length, output) != (size_t)length) return FILE_ERROR;
        }
    }
    if (ferror(input)) return FILE_ERROR;
    if (has_line && fprintf(output, "%zu\n", count) < 0) return FILE_ERROR;
    return OK;
}

int output_name(const char *input, char **name) {
    if (input == NULL || name == NULL || *input == '\0') return INVALID;
    const size_t length = strlen(input);
    if (length > SIZE_MAX - 5) return OVERFLOW;
    char *text = malloc(length + 5);
    if (text == NULL) return MEMORY_ERROR;
    const char *base = strrchr(input, '/');
    base = base == NULL ? input : base + 1;
    if (*base == '\0') { free(text); return INVALID; }
    const size_t prefix = (size_t)(base - input);
    memcpy(text, input, prefix);
    memcpy(text + prefix, "out_", 4);
    memcpy(text + prefix + 4, base, strlen(base) + 1);
    *name = text;
    return OK;
}

int main(const int argc, char *argv[]) {
    if (argc < 3 || (argv[1][0] != '-' && argv[1][0] != '/')) {
        fprintf(stderr, "Usage: task_04 -d|-i|-s|-a input; -nd|-ni|-ns|-na input output\n");
        return INVALID;
    }
    const size_t length = strlen(argv[1]);
    const int explicit_name = length == 3 && argv[1][1] == 'n';
    const char mode = length == 2 ? argv[1][1] : explicit_name ? argv[1][2] : '\0';
    if (mode == '\0' || strchr("disa", mode) == NULL || argc != (explicit_name ? 4 : 3)) return INVALID;
    char *generated = NULL;
    int status = explicit_name ? OK : output_name(argv[2], &generated);
    if (status != OK) return status;
    FILE *input = fopen(argv[2], "rb");
    FILE *output = NULL;
    if (input == NULL) status = FILE_ERROR;
    else status = open_output(explicit_name ? argv[3] : generated, input, NULL, &output);
    if (status == OK) status = process_file(input, output, mode);
    if (output != NULL && fclose(output) != 0) status = FILE_ERROR;
    if (input != NULL && fclose(input) != 0) status = FILE_ERROR;
    free(generated);
    if (status != OK) fprintf(stderr, "File processing failed (%d)\n", status);
    return status;
}
