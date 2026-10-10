#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "../common.h"

struct point { double x; double y; };

int convex_polygon(const double epsilon, int *result, const int count, ...) {
    if (result == NULL || count < 3 || !isfinite(epsilon) || epsilon <= 0 ||
        (size_t)count > SIZE_MAX / sizeof(struct point)) return INVALID;
    struct point *points = malloc((size_t)count * sizeof(*points));
    if (points == NULL) return MEMORY_ERROR;
    va_list args;
    va_start(args, count);
    int status = OK;
    for (int i = 0; i < count; ++i) {
        points[i].x = va_arg(args, double);
        points[i].y = va_arg(args, double);
        if (!isfinite(points[i].x) || !isfinite(points[i].y)) status = INVALID;
    }
    va_end(args);
    for (int i = 0; status == OK && i < count; ++i) for (int j = i + 1; j < count; ++j) {
        if (fabs(points[i].x - points[j].x) <= epsilon && fabs(points[i].y - points[j].y) <= epsilon) status = INVALID;
    }
    int direction = 0;
    *result = 1;
    for (int i = 0; status == OK && *result && i < count; ++i) {
        const struct point a = points[i], b = points[(i + 1) % count];
        for (int j = 0; j < count; ++j) {
            if (j == i || j == (i + 1) % count) continue;
            const double cross = (b.x - a.x) * (points[j].y - a.y) - (b.y - a.y) * (points[j].x - a.x);
            if (!isfinite(cross)) { status = OVERFLOW; break; }
            if (fabs(cross) <= epsilon) continue;
            const int current = cross > epsilon ? 1 : -1;
            if (direction == 0) direction = current;
            else if (direction != current) { *result = 0; break; }
        }
    }
    if (direction == 0) *result = 0;
    free(points);
    return status;
}

int polynomial(const double x, double *result, const int degree, ...) {
    if (result == NULL || degree < 0 || degree == INT_MAX || !isfinite(x)) return INVALID;
    va_list args;
    va_start(args, degree);
    double value = 0;
    int status = OK;
    for (int i = 0; i <= degree; ++i) {
        const double coefficient = va_arg(args, double);
        if (!isfinite(coefficient)) { status = INVALID; break; }
        value = value * x + coefficient;
        if (!isfinite(value)) { status = OVERFLOW; break; }
    }
    va_end(args);
    if (status == OK) *result = value;
    return status;
}

int is_kaprekar(const long long value, const int base, int *result) {
    if (value < 0 || base < 2 || base > 36 || result == NULL) return INVALID;
    *result = value == 1;
    if (value < 2) return OK;
    const unsigned long long number = (unsigned long long)value;
    if (number > ULLONG_MAX / number) return OVERFLOW;
    const unsigned long long square = number * number;
    unsigned long long power = (unsigned)base;
    while (power <= square) {
        const unsigned long long left = square / power, right = square % power;
        if (right != 0 && left + right == number) { *result = 1; return OK; }
        if (power > ULLONG_MAX / (unsigned)base) break;
        power *= (unsigned)base;
    }
    return OK;
}

int find_kaprekar(const int base, long long *values, const size_t capacity, size_t *found, const int count, ...) {
    if (base < 2 || base > 36 || values == NULL || found == NULL || count < 0) return INVALID;
    *found = 0;
    va_list args;
    va_start(args, count);
    int status = OK;
    for (int i = 0; i < count; ++i) {
        const char *text = va_arg(args, const char *);
        if (text == NULL || *text == '-' || *text == '+') { status = INVALID; break; }
        long long value;
        int matches;
        status = parse_integer(text, base, &value);
        if (status == OK) status = is_kaprekar(value, base, &matches);
        if (status != OK) break;
        if (matches) {
            if (*found == capacity) { status = OVERFLOW; break; }
            values[(*found)++] = value;
        }
    }
    va_end(args);
    return status;
}

int geometric_mean(const double epsilon, double *result, const int count, ...) {
    if (result == NULL || count < 1 || !isfinite(epsilon) || epsilon <= 0) return INVALID;
    va_list args;
    va_start(args, count);
    long double sum = 0;
    int zero = 0, status = OK;
    for (int i = 0; i < count; ++i) {
        const double value = va_arg(args, double);
        if (!isfinite(value) || (signbit(value) && fpclassify(value) != FP_ZERO)) { status = INVALID; break; }
        if (fpclassify(value) == FP_ZERO) zero = 1;
        else sum += logl(value) / count;
    }
    va_end(args);
    if (status == OK) {
        const double value = zero ? 0 : (double)expl(sum);
        if (!isfinite(value)) status = OVERFLOW;
        else *result = value;
    }
    return status;
}

int positive_power(const double base, const unsigned int exponent, double *result) {
    if (result == NULL || !isfinite(base)) return INVALID;
    if (exponent == 0) { *result = 1; return OK; }
    double half;
    const int status = positive_power(base, exponent / 2, &half);
    if (status != OK) return status;
    double value = half * half;
    if (exponent % 2 != 0) value *= base;
    if (!isfinite(value) || (fpclassify(value) == FP_ZERO && fpclassify(base) != FP_ZERO)) return OVERFLOW;
    *result = value;
    return OK;
}

int fast_power(const double base, const int exponent, const double epsilon, double *result) {
    if (result == NULL || !isfinite(base) || !isfinite(epsilon) || epsilon <= 0) return INVALID;
    if (exponent < 0 && fpclassify(base) == FP_ZERO) return INVALID;
    const unsigned int magnitude = exponent < 0 ? 0U - (unsigned)exponent : (unsigned)exponent;
    const double factor = exponent < 0 ? 1 / base : base;
    if (!isfinite(factor)) return OVERFLOW;
    return positive_power(factor, magnitude, result);
}

int square_equation(const double x, const double epsilon, double *value) {
    if (value == NULL || !isfinite(x) || !isfinite(epsilon) || epsilon <= 0) return INVALID;
    *value = x * x - 2;
    return isfinite(*value) ? OK : OVERFLOW;
}

int cosine_equation(const double x, const double epsilon, double *value) {
    if (value == NULL || !isfinite(x) || !isfinite(epsilon) || epsilon <= 0) return INVALID;
    *value = cos(x) - x;
    return isfinite(*value) ? OK : OVERFLOW;
}

int bisect(double left, double right, const double epsilon,
           int (*function)(double, double, double *), double *result) {
    if (result == NULL || function == NULL || !isfinite(left) || !isfinite(right) ||
        !isfinite(epsilon) || epsilon <= 0 || left > right) return INVALID;
    double left_value, right_value;
    int status = function(left, epsilon, &left_value);
    if (status == OK) status = function(right, epsilon, &right_value);
    if (status != OK) return status;
    if (!isfinite(left_value) || !isfinite(right_value)) return OVERFLOW;
    if (fpclassify(left_value) == FP_ZERO) { *result = left; return OK; }
    if (fpclassify(right_value) == FP_ZERO) { *result = right; return OK; }
    if (signbit(left_value) == signbit(right_value)) return INVALID;
    for (int i = 0; i < 2048; ++i) {
        const double middle = left / 2 + right / 2;
        if (right / 2 - left / 2 <= epsilon) { *result = middle; return OK; }
        if (middle == left || middle == right) return NO_CONVERGENCE;
        double value;
        status = function(middle, epsilon, &value);
        if (status != OK) return status;
        if (!isfinite(value)) return OVERFLOW;
        if (fpclassify(value) == FP_ZERO) { *result = middle; return OK; }
        if (signbit(value) == signbit(left_value)) { left = middle; left_value = value; }
        else right = middle;
    }
    return NO_CONVERGENCE;
}

int main(const int argc, char *argv[]) {
    long double parsed = 1e-9L;
    if (argc > 2 || (argc == 2 && parse_real(argv[1], &parsed) != OK) || parsed <= 0) return INVALID;
    const double epsilon = (double)parsed;
    if (!isfinite(epsilon) || epsilon <= 0) return INVALID;
    int convex, status;
    double value;
    status = convex_polygon(epsilon, &convex, 4, 0.0,0.0, 2.0,0.0, 2.0,2.0, 0.0,2.0);
    if (status == OK) printf("convex: %d\n", convex);
    if (status == OK) status = polynomial(2, &value, 2, 1.0,-3.0,2.0);
    if (status == OK) printf("polynomial: %.12g\n", value);
    long long numbers[5];
    size_t count;
    if (status == OK) status = find_kaprekar(10, numbers, 5, &count, 5, "1", "9", "45", "55", "2");
    if (status == OK) {
        printf("kaprekar:");
        for (size_t i = 0; i < count; ++i) printf(" %lld", numbers[i]);
        putchar('\n');
    }
    if (status == OK) status = geometric_mean(epsilon, &value, 3, 1.0,4.0,16.0);
    if (status == OK) printf("geometric mean: %.12g\n", value);
    if (status == OK) status = fast_power(2, -3, epsilon, &value);
    if (status == OK) printf("power: %.12g\n", value);
    if (status == OK) status = bisect(0, 2, epsilon, square_equation, &value);
    if (status == OK) printf("root x*x-2: %.12g\n", value);
    if (status == OK) status = bisect(0, 1, epsilon * 10, cosine_equation, &value);
    if (status == OK) printf("root cos(x)-x: %.12g\n", value);
    if (status != OK) fprintf(stderr, "Calculation failed (%d)\n", status);
    return fflush(stdout) == EOF || ferror(stdout) ? FILE_ERROR : status;
}
