#include <stdio.h>
#include <string.h>
#include "../common.h"

int get_multiples(const long long x, int values[100], int *count) {
    if (values == NULL || count == NULL) return INVALID;
    *count = 0;
    if (x == 0 || x < -100 || x > 100) return OK;
    const int step = (int)(x < 0 ? -x : x);
    for (int multiple = step; multiple <= 100; multiple += step)
        values[(*count)++] = multiple;
    return OK;
}

int classify_number(const long long x, int *kind) {
    if (kind == NULL) return INVALID;
    *kind = 0;
    if (x < 2) return OK;
    *kind = 1;
    if (x > 2 && x % 2 == 0) *kind = 2;
    for (long long divisor = 3; *kind == 1 && divisor <= x / divisor; divisor += 2) {
        if (x % divisor == 0) *kind = 2;
    }
    return OK;
}

int make_powers(const long long x, unsigned long long table[10][10]) {
    if (x < 1 || x > 10 || table == NULL) return INVALID;
    for (int base = 1; base <= 10; ++base) {
        unsigned long long value = 1;
        for (int power = 0; power < x; ++power) {
            value *= (unsigned)base;
            table[base - 1][power] = value;
        }
    }
    return OK;
}

int natural_sum(const long long x, unsigned long long *result) {
    if (x < 1 || result == NULL) return INVALID;
    unsigned long long a = (unsigned long long)x;
    unsigned long long b = a + 1;
    if (a % 2 == 0) a /= 2;
    else b /= 2;
    if (a > ULLONG_MAX / b) return OVERFLOW;
    *result = a * b;
    return OK;
}

int factorial(const long long x, unsigned long long *result) {
    if (x < 0 || result == NULL) return INVALID;
    unsigned long long value = 1;
    for (long long i = 2; i <= x; ++i) {
        if (value > ULLONG_MAX / (unsigned long long)i) return OVERFLOW;
        value *= (unsigned long long)i;
    }
    *result = value;
    return OK;
}

int main(const int argc, char *argv[]) {
    long long x;
    if (argc != 3 || parse_integer(argv[1], 10, &x) != OK || strlen(argv[2]) != 2 ||
        (argv[2][0] != '-' && argv[2][0] != '/')) {
        fprintf(stderr, "Usage: task_01 integer -h|-p|-s|-e|-a|-f\n");
        return INVALID;
    }
    int status = OK;
    switch (argv[2][1]) {
        case 'h': {
            int values[100], count;
            status = get_multiples(x, values, &count);
            if (status != OK) break;
            if (count == 0) puts("No multiples");
            else {
                for (int i = 0; i < count; ++i) printf("%s%d", i == 0 ? "" : " ", values[i]);
                putchar('\n');
            }
            break;
        }
        case 'p': {
            int kind;
            status = classify_number(x, &kind);
            if (status == OK) puts(kind == 0 ? "Neither prime nor composite" : kind == 1 ? "Prime" : "Composite");
            break;
        }
        case 's': {
            char text[66];
            status = format_integer(x, 16, text, sizeof(text));
            if (status == OK) {
                for (size_t i = 0; text[i] != '\0'; ++i) printf("%s%c", i == 0 ? "" : " ", text[i]);
                putchar('\n');
            }
            break;
        }
        case 'e': {
            unsigned long long table[10][10];
            status = make_powers(x, table);
            if (status == OK) {
                for (int base = 0; base < 10; ++base) {
                    printf("%d", base + 1);
                    for (int power = 0; power < x; ++power) printf(" %llu", table[base][power]);
                    putchar('\n');
                }
            }
            break;
        }
        case 'a':
        case 'f': {
            unsigned long long result;
            status = argv[2][1] == 'a' ? natural_sum(x, &result) : factorial(x, &result);
            if (status == OK) printf("%llu\n", result);
            break;
        }
        default: status = INVALID;
    }
    if (status != OK) fprintf(stderr, "Invalid argument or arithmetic overflow\n");
    if (fflush(stdout) == EOF || ferror(stdout)) return FILE_ERROR;
    return status;
}
