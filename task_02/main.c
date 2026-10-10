#include <stdio.h>
#include <float.h>
#include "../common.h"

enum constant { E, PI, LN2, ROOT2, GAMMA };

int validate_request(const int kind, const long double epsilon, const long double *result) {
    if (kind < E || kind > GAMMA || result == NULL || !isfinite(epsilon) || epsilon <= 0) return INVALID;
    return epsilon <= 256 * LDBL_EPSILON ? NO_CONVERGENCE : OK;
}

int constant_limit(const int kind, const long double epsilon, long double *result) {
    int status = validate_request(kind, epsilon, result);
    if (status != OK) return status;
    if (kind == E || kind == LN2) {
        long double n = 1;
        for (int i = 0; i < 100; ++i) {
            /* expm1 avoids cancellation in 2^(1/n) - 1 for large n. */
            const long double value = kind == E ? expl(n * log1pl(1 / n)) : n * expm1l(logl(2) / n);
            const long double bound = kind == E ? 2 / n + 64 * LDBL_EPSILON : 1 / n + 64 * LDBL_EPSILON;
            if (!isfinite(value)) return OVERFLOW;
            if (bound <= epsilon / 2) { *result = value; return OK; }
            n *= 2;
        }
    } else if (kind == PI) {
        long double value = 4;
        for (unsigned long n = 1; n <= 100000000UL; ++n) {
            if (n > 1) {
                const long double odd = 2.0L * n - 1;
                value *= 1 - 1 / (odd * odd);
            }
            if (1.0L / n + 16 * n * LDBL_EPSILON <= epsilon / 2) { *result = value; return OK; }
            if (16 * n * LDBL_EPSILON > epsilon / 2) return NO_CONVERGENCE;
        }
    } else if (kind == ROOT2) {
        long double value = -0.5L;
        for (int i = 0; i < 1024; ++i) {
            value = value - value * value / 2 + 1;
            if (value > 1 - epsilon && fabsl(value * value - 2) / (value + 1) <= epsilon / 2) {
                *result = value;
                return OK;
            }
        }
    } else {
        long double sum = 0, correction = 0;
        for (unsigned long n = 1; n <= 100000000UL; ++n) {
            const long double adjusted = 1.0L / n - correction;
            const long double next = sum + adjusted;
            correction = (next - sum) - adjusted;
            sum = next;
            const long double bound = 1 / (12.0L * n * n) + 64 * LDBL_EPSILON * sum;
            if (bound <= epsilon / 2) {
                *result = sum - logl(n) - 1 / (2.0L * n);
                return OK;
            }
        }
    }
    return NO_CONVERGENCE;
}

int constant_series(const int kind, const long double epsilon, long double *result) {
    const int status = validate_request(kind, epsilon, result);
    if (status != OK) return status;
    if (kind == ROOT2) {
        long double value = 1;
        for (int k = 2; k < 1024; ++k) {
            value *= powl(2, ldexpl(1, -k));
            if (2 * ldexpl(1, -k) + 64 * LDBL_EPSILON <= epsilon / 2) { *result = value; return OK; }
        }
        return NO_CONVERGENCE;
    }
    long double sum = kind == E ? 1 : kind == GAMMA ? 0.5L : 0;
    long double term = 1, correction = 0;
    for (unsigned long n = 1; n <= 100000000UL; ++n) {
        long double bound;
        if (kind == E) {
            term /= n;
            bound = term / (n + 1) / (1 - 1.0L / (n + 2));
        } else if (kind == PI) {
            term = (n % 2 == 0 ? -4.0L : 4.0L) / (2.0L * n - 1);
            bound = 4 / (2.0L * n + 1);
        } else if (kind == LN2) {
            term = (n % 2 == 0 ? -1.0L : 1.0L) / n;
            bound = 1.0L / (n + 1);
        } else {
            const long double x = 1.0L / n;
            term = x - log1pl(x) - 1 / (2.0L * n * (n + 1));
            bound = 1 / (12.0L * n * n);
        }
        const long double adjusted = term - correction;
        const long double next = sum + adjusted;
        correction = (next - sum) - adjusted;
        sum = next;
        if (!isfinite(sum)) return OVERFLOW;
        if (bound + 64 * LDBL_EPSILON * fabsl(sum) <= epsilon / 2) { *result = sum; return OK; }
    }
    return NO_CONVERGENCE;
}

int equation_value(const int kind, const long double x, const long double derivative, long double *value) {
    if (value == NULL || !isfinite(x) || !isfinite(derivative)) return INVALID;
    switch (kind) {
        case E: *value = logl(x) - 1; break;
        case PI: *value = sinl(x); break;
        case LN2: *value = expl(x) - 2; break;
        case ROOT2: *value = x * x - 2; break;
        case GAMMA: *value = derivative + x; break;
        default: return INVALID;
    }
    return isfinite(*value) ? OK : OVERFLOW;
}

int constant_equation(const int kind, const long double epsilon, long double *result) {
    int status = validate_request(kind, epsilon, result);
    if (status != OK) return status;
    long double left = kind == E ? 2 : kind == PI ? 3 : kind == ROOT2 ? 1 : 0;
    long double right = kind == PI ? 4 : kind == E ? 3 : kind == ROOT2 ? 2 : 1;
    long double derivative = 0;
    if (kind == GAMMA) {
        long double h = sqrtl(epsilon / 128);
        if (h > 0.01L + epsilon * LDBL_EPSILON) h = 0.01L;
        if (40 * h * h / 6 + 64 * LDBL_EPSILON / h > epsilon / 4) return NO_CONVERGENCE;
        derivative = (tgammal(1 + h) - tgammal(1 - h)) / (2 * h);
        if (!isfinite(derivative)) return OVERFLOW;
    }
    long double left_value;
    status = equation_value(kind, left, derivative, &left_value);
    if (status != OK) return status;
    for (int i = 0; i < 2048; ++i) {
        const long double middle = left / 2 + right / 2;
        if (right - left <= epsilon / 2) { *result = middle; return OK; }
        long double value;
        status = equation_value(kind, middle, derivative, &value);
        if (status != OK) return status;
        if (fpclassify(value) == FP_ZERO) { *result = middle; return OK; }
        if (signbit(value) == signbit(left_value)) { left = middle; left_value = value; }
        else right = middle;
    }
    return NO_CONVERGENCE;
}

int main(const int argc, char *argv[]) {
    long double epsilon;
    if (argc != 2 || parse_real(argv[1], &epsilon) != OK || epsilon <= 0) {
        fprintf(stderr, "Usage: task_02 epsilon\n");
        return INVALID;
    }
    long double values[5][3];
    int status = OK;
    for (int i = 0; status == OK && i < 5; ++i) {
        status = constant_limit(i, epsilon, &values[i][0]);
        if (status == OK) status = constant_series(i, epsilon, &values[i][1]);
        if (status == OK) status = constant_equation(i, epsilon, &values[i][2]);
    }
    if (status != OK) { fprintf(stderr, "Calculation failed (%d)\n", status); return status; }
    const char *names[5] = {"e", "pi", "ln2", "sqrt2", "gamma"};
    for (int i = 0; i < 5; ++i) printf("%s limit %.18Lg series %.18Lg equation %.18Lg\n", names[i], values[i][0], values[i][1], values[i][2]);
    return fflush(stdout) == EOF || ferror(stdout) ? FILE_ERROR : OK;
}
