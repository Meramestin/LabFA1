#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include "../files.h"

int add_number(const char *text, const int base, long long *sum, long long *maximum, int *has_number) {
    if (text == NULL || sum == NULL || maximum == NULL || has_number == NULL ||
        base < 2 || base > 36 || (*has_number != 0 && *has_number != 1)) return INVALID;
    for (const char *p = text; *p != '\0'; ++p) {
        if (*p >= 'a' && *p <= 'z') return INVALID;
    }
    long long value;
    const int status = parse_integer(text, base, &value);
    if (status != OK) return status;
    if ((value > 0 && *sum > LLONG_MAX - value) || (value < 0 && *sum < LLONG_MIN - value)) return OVERFLOW;
    const unsigned long long magnitude = value < 0 ? 0ULL - (unsigned long long)value : (unsigned long long)value;
    const unsigned long long previous = *maximum < 0 ? 0ULL - (unsigned long long)*maximum : (unsigned long long)*maximum;
    if (!*has_number || magnitude > previous) *maximum = value;
    *sum += value;
    *has_number = 1;
    return OK;
}

int read_numbers(FILE *input, int *base, long long *maximum, long long *sum) {
    if (input == NULL || base == NULL || maximum == NULL || sum == NULL) return INVALID;
    char *token;
    int found;
    int status = read_token(input, &token, &found);
    if (status != OK || !found) return status == OK ? INVALID : status;
    long long number;
    status = parse_integer(token, 10, &number);
    free(token);
    if (status != OK || number < 2 || number > 36) return INVALID;
    *base = (int)number;
    *sum = 0;
    *maximum = 0;
    int has_number = 0;
    while (1) {
        status = read_token(input, &token, &found);
        if (status != OK || !found) return status == OK ? INVALID : status;
        if (strcmp(token, "Stop") == 0) {
            free(token);
            return has_number ? OK : INVALID;
        }
        status = add_number(token, *base, sum, maximum, &has_number);
        free(token);
        if (status != OK) return status;
    }
}

int print_results(const int base, const long long maximum, const long long sum) {
    if (base < 2 || base > 36) return INVALID;
    const int bases[5] = {base, 9, 18, 27, 36};
    for (int i = 0; i < 5; ++i) {
        char max_text[66], sum_text[66];
        int status = format_integer(maximum, bases[i], max_text, sizeof(max_text));
        if (status == OK) status = format_integer(sum, bases[i], sum_text, sizeof(sum_text));
        if (status != OK) return status;
        if (printf("max %d %s\nsum %d %s\n", bases[i], max_text, bases[i], sum_text) < 0) return FILE_ERROR;
    }
    return OK;
}

int main(const int argc, char *argv[]) {
    (void)argv;
    if (argc != 1) return INVALID;
    int base;
    long long maximum, sum;
    int status = read_numbers(stdin, &base, &maximum, &sum);
    if (status == OK) status = print_results(base, maximum, sum);
    if (status != OK) fprintf(stderr, "Invalid input or overflow (%d)\n", status);
    if (fflush(stdout) == EOF || ferror(stdout)) return FILE_ERROR;
    return status;
}
