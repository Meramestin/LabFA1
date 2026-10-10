#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include "../files.h"

int transform_token(const char *token, const size_t index, char **result) {
    if (token == NULL || result == NULL || index == 0) return INVALID;
    const size_t length = strlen(token);
    if (length > (SIZE_MAX - 1) / 4) return OVERFLOW;
    char *text = malloc(length * 4 + 1);
    if (text == NULL) return MEMORY_ERROR;
    const int base = index % 10 == 0 ? 4 : index % 5 == 0 ? 8 : 0;
    const int lowercase = index % 2 == 0;
    size_t pos = 0;
    for (size_t i = 0; i < length; ++i) {
        unsigned char c = (unsigned char)token[i];
        if (lowercase && c >= 'A' && c <= 'Z') c = (unsigned char)(c - 'A' + 'a');
        if (base == 0) text[pos++] = (char)c;
        else {
            char digits[4];
            int count = 0;
            do {
                digits[count++] = (char)('0' + c % base);
                c = (unsigned char)(c / base);
            } while (c != 0);
            while (count != 0) text[pos++] = digits[--count];
        }
    }
    text[pos] = '\0';
    *result = text;
    return OK;
}

int write_token(FILE *output, const char *token, int *first) {
    if (output == NULL || token == NULL || first == NULL) return INVALID;
    if (!*first && fputc(' ', output) == EOF) return FILE_ERROR;
    if (fputs(token, output) == EOF) return FILE_ERROR;
    *first = 0;
    return OK;
}

int merge_files(FILE *one, FILE *two, FILE *output) {
    if (one == NULL || two == NULL || output == NULL) return INVALID;
    FILE *inputs[2] = {one, two};
    int ended[2] = {0, 0};
    int first = 1;
    while (!ended[0] || !ended[1]) {
        for (int i = 0; i < 2; ++i) {
            if (ended[i]) continue;
            char *token;
            int found;
            int status = read_token(inputs[i], &token, &found);
            if (status != OK) return status;
            if (!found) { ended[i] = 1; continue; }
            status = write_token(output, token, &first);
            free(token);
            if (status != OK) return status;
        }
    }
    return OK;
}

int transform_file(FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return INVALID;
    size_t index = 0;
    int first = 1;
    while (1) {
        char *token;
        int found;
        int status = read_token(input, &token, &found);
        if (status != OK || !found) return status;
        if (index == SIZE_MAX) { free(token); return OVERFLOW; }
        char *text = NULL;
        status = transform_token(token, ++index, &text);
        free(token);
        if (status == OK) status = write_token(output, text, &first);
        free(text);
        if (status != OK) return status;
    }
}

int main(const int argc, char *argv[]) {
    if (argc < 2 || strlen(argv[1]) != 2 || (argv[1][0] != '-' && argv[1][0] != '/') ||
        (argv[1][1] != 'a' && argv[1][1] != 'r') || argc != (argv[1][1] == 'r' ? 5 : 4)) {
        fprintf(stderr, "Usage: task_07 -r input1 input2 output; -a input output\n");
        return INVALID;
    }
    const int merge = argv[1][1] == 'r';
    FILE *one = fopen(argv[2], "rb");
    FILE *two = merge ? fopen(argv[3], "rb") : NULL;
    FILE *output = NULL;
    int status = one == NULL || (merge && two == NULL) ? FILE_ERROR : OK;
    if (status == OK) status = open_output(argv[argc - 1], one, two, &output);
    if (status == OK) status = merge ? merge_files(one, two, output) : transform_file(one, output);
    if (output != NULL && fclose(output) != 0) status = FILE_ERROR;
    if (one != NULL && fclose(one) != 0) status = FILE_ERROR;
    if (two != NULL && fclose(two) != 0) status = FILE_ERROR;
    if (status != OK) fprintf(stderr, "File processing failed (%d)\n", status);
    return status;
}
