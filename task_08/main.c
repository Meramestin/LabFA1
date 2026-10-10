#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "../files.h"

int detect_base(const char *text, int *base) {
    if (text == NULL || base == NULL) return INVALID;
    if (*text == '+' || *text == '-') ++text;
    if (*text == '\0') return INVALID;
    int largest = 1;
    for (; *text != '\0'; ++text) {
        int digit;
        if (digit_value(*text, &digit) != OK) return INVALID;
        if (digit > largest) largest = digit;
    }
    *base = largest + 1;
    return OK;
}

int convert_number(const char *token, int *base, long long *value,
                   char *normalized, const size_t capacity) {
    if (token == NULL || base == NULL || value == NULL || normalized == NULL || capacity < 2)
        return INVALID;
    int status = detect_base(token, base);
    if (status == OK) status = parse_integer(token, *base, value);
    if (status == OK) status = format_integer(*value, *base, normalized, capacity);
    return status;
}

int convert_file(FILE *input, FILE *output) {
    if (input == NULL || output == NULL) return INVALID;
    while (1) {
        char *token;
        int found;
        int status = read_token(input, &token, &found);
        if (status != OK || !found) return status;
        int base;
        long long value;
        char normalized[66];
        status = convert_number(token, &base, &value, normalized, sizeof(normalized));
        free(token);
        if (status != OK) return status;
        if (fprintf(output, "%s %d %lld\n", normalized, base, value) < 0) return FILE_ERROR;
    }
}

int main(const int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: task_08 input output\n");
        return INVALID;
    }
    FILE *input = fopen(argv[1], "rb");
    if (input == NULL) { fprintf(stderr, "Cannot open input\n"); return FILE_ERROR; }
    FILE *output = NULL;
    int status = open_output(argv[2], input, NULL, &output);
    if (status == OK) status = convert_file(input, output);
    if (output != NULL && fclose(output) != 0) status = FILE_ERROR;
    if (fclose(input) != 0) status = FILE_ERROR;
    if (status != OK) fprintf(stderr, "Conversion failed (%d)\n", status);
    return status;
}
