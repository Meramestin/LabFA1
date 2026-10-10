#ifndef COMMON_H
#define COMMON_H

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

enum status { OK, INVALID, OVERFLOW, MEMORY_ERROR, FILE_ERROR, NO_CONVERGENCE };

static inline int digit_value(const char c, int *value) {
    if (value == NULL) return INVALID;
    if (c >= '0' && c <= '9') *value = c - '0';
    else if (c >= 'A' && c <= 'Z') *value = c - 'A' + 10;
    else if (c >= 'a' && c <= 'z') *value = c - 'a' + 10;
    else return INVALID;
    return OK;
}

static inline int parse_integer(const char *text, const int base, long long *value) {
    if (text == NULL || value == NULL || base < 2 || base > 36) return INVALID;
    int negative = *text == '-';
    if (*text == '-' || *text == '+') ++text;
    if (*text == '\0') return INVALID;
    unsigned long long number = 0;
    const unsigned long long limit = negative ? (unsigned long long)LLONG_MAX + 1ULL : (unsigned long long)LLONG_MAX;
    for (; *text != '\0'; ++text) {
        int digit;
        if (digit_value(*text, &digit) != OK || digit >= base) return INVALID;
        if (number > (limit - (unsigned)digit) / (unsigned)base) return OVERFLOW;
        number = number * (unsigned)base + (unsigned)digit;
    }
    if (negative && number == (unsigned long long)LLONG_MAX + 1ULL) *value = LLONG_MIN;
    else *value = negative ? -(long long)number : (long long)number;
    return OK;
}

static inline int parse_real(const char *text, long double *value) {
    if (text == NULL || value == NULL) return INVALID;
    const char *start = text;
    if (*text == '+' || *text == '-') ++text;
    int has_digits = 0;
    int nonzero = 0;
    while (*text >= '0' && *text <= '9') {
        nonzero |= *text != '0';
        has_digits = 1;
        ++text;
    }
    if (*text == '.') {
        ++text;
        while (*text >= '0' && *text <= '9') {
            nonzero |= *text != '0';
            has_digits = 1;
            ++text;
        }
    }
    if (!has_digits) return INVALID;
    if (*text == 'e' || *text == 'E') {
        ++text;
        if (*text == '+' || *text == '-') ++text;
        if (*text < '0' || *text > '9') return INVALID;
        while (*text >= '0' && *text <= '9') ++text;
    }
    if (*text != '\0') return INVALID;
    char *end;
    const long double number = strtold(start, &end);
    if (*end != '\0' || !isfinite(number) || (fpclassify(number) == FP_ZERO && nonzero)) return OVERFLOW;
    *value = number;
    return OK;
}

static inline int format_integer(const long long value, const int base, char *text, const size_t size) {
    if (base < 2 || base > 36 || text == NULL || size < 2) return INVALID;
    char digits[65];
    size_t count = 0;
    unsigned long long number = value < 0 ? 0ULL - (unsigned long long)value : (unsigned long long)value;
    do {
        const unsigned digit = number % (unsigned)base;
        digits[count++] = digit < 10 ? (char)('0' + digit) : (char)('A' + digit - 10);
        number /= (unsigned)base;
    } while (number != 0);
    const size_t sign = value < 0;
    if (count + sign + 1 > size) return OVERFLOW;
    size_t pos = 0;
    if (sign) text[pos++] = '-';
    while (count != 0) text[pos++] = digits[--count];
    text[pos] = '\0';
    return OK;
}

#endif
