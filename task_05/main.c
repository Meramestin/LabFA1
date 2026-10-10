#include <stdio.h>
#include <float.h>
#include "../common.h"

int next_term(const char mode, const long double x, const unsigned long n,
              const long double term, long double *next, long double *ratio) {
    if (next == NULL || ratio == NULL || !isfinite(x) || !isfinite(term)) return INVALID;
    const long double k = (long double)n + 1;
    switch (mode) {
        case 'a': *ratio = x / k; break;
        case 'b': *ratio = -x * x / ((2 * k - 1) * (2 * k)); break;
        case 'c': *ratio = (x * x) / ((1 - 1 / (3 * k)) * (1 - 2 / (3 * k))); break;
        case 'd': *ratio = -(2 * k - 1) * x * x / (2 * k); break;
        default: return INVALID;
    }
    *next = term * *ratio;
    return isfinite(*next) && isfinite(*ratio) ? OK : OVERFLOW;
}

int series_sum(const char mode, const long double x, const long double epsilon, long double *result) {
    if (result == NULL || !isfinite(x) || !isfinite(epsilon) || epsilon <= 0 ||
        (mode != 'a' && mode != 'b' && mode != 'c' && mode != 'd')) return INVALID;
    if ((mode == 'c' && fabsl(x) >= 1) || (mode == 'd' && fabsl(x) > 1)) return INVALID;
    long double term = mode == 'd' ? -x * x / 2 : 1;
    long double sum = term, absolute_sum = fabsl(term), correction = 0;
    unsigned long n = mode == 'd' ? 1 : 0;
    for (; n < 10000000UL; ++n) {
        long double next, ratio;
        int status = next_term(mode, x, n, term, &next, &ratio);
        if (status != OK) return status;
        long double bound = -1;
        if (mode == 'd') bound = fabsl(next);
        else if (fabsl(ratio) < 1) bound = fabsl(next) / (1 - fabsl(ratio));
        const long double rounding = 32 * LDBL_EPSILON * absolute_sum;
        if (rounding > epsilon / 2) return NO_CONVERGENCE;
        if (bound >= 0 && bound + rounding <= epsilon) { *result = sum; return OK; }
        const long double adjusted = next - correction;
        const long double updated = sum + adjusted;
        correction = (updated - sum) - adjusted;
        sum = updated;
        absolute_sum += fabsl(next);
        if (!isfinite(sum) || !isfinite(absolute_sum)) return OVERFLOW;
        term = next;
    }
    return NO_CONVERGENCE;
}

int integrand(const char mode, const long double x, const long double epsilon, long double *value) {
    if (value == NULL || !isfinite(x) || !isfinite(epsilon) || epsilon <= 0 || x < 0 || x > 1) return INVALID;
    switch (mode) {
        case 'a': *value = fpclassify(x) == FP_ZERO ? 1 : log1pl(x) / x; break;
        case 'b': *value = expl(-x * x / 2); break;
        /* Substitute t = 1 - x*x in integral c: -log(1-t) dt = -4*x*log(x) dx. */
        case 'c': *value = fpclassify(x) == FP_ZERO ? 0 : -4 * x * logl(x); break;
        case 'd': *value = fpclassify(x) == FP_ZERO ? 1 : expl(x * logl(x)); break;
        default: return INVALID;
    }
    return isfinite(*value) ? OK : OVERFLOW;
}

int midpoint_error(const char mode, const unsigned long n, long double *bound) {
    if (bound == NULL || n < 4) return INVALID;
    const long double h = 1.0L / n;
    switch (mode) {
        case 'a': *bound = h * h / 36; break;
        case 'b': *bound = h * h / 24; break;
        case 'c': *bound = h * h * (2 * logl(2) - 1 - logl(h) / 2); break;
        case 'd': {
            const long double value = expl(h * logl(h));
            *bound = h * (1 - value) + h * h * (1 - value * (logl(h) + 1)) / 8;
            break;
        }
        default: return INVALID;
    }
    return OK;
}

int integral(const char mode, const long double epsilon, long double *result) {
    if (result == NULL || !isfinite(epsilon) || epsilon <= 0) return INVALID;
    unsigned long n = 4;
    long double bound;
    while (1) {
        const int status = midpoint_error(mode, n, &bound);
        if (status != OK) return status;
        if (bound <= epsilon / 2) break;
        if (n >= 16777216UL) return NO_CONVERGENCE;
        n *= 2;
    }
    long double sum = 0, correction = 0;
    for (unsigned long i = 0; i < n; ++i) {
        long double value;
        const int status = integrand(mode, ((long double)i + 0.5L) / n, epsilon, &value);
        if (status != OK) return status;
        const long double adjusted = value - correction;
        const long double next = sum + adjusted;
        correction = (next - sum) - adjusted;
        sum = next;
    }
    if (!isfinite(sum)) return OVERFLOW;
    if (64 * LDBL_EPSILON * fabsl(sum / n) > epsilon / 2) return NO_CONVERGENCE;
    *result = sum / n;
    return OK;
}

int main(const int argc, char *argv[]) {
    long double epsilon, x;
    if (argc != 3 || parse_real(argv[1], &epsilon) != OK || epsilon <= 0 || parse_real(argv[2], &x) != OK) {
        fprintf(stderr, "Usage: task_05 epsilon x\n");
        return INVALID;
    }
    long double sums[4], integrals[4];
    int status = OK;
    for (int i = 0; status == OK && i < 4; ++i) {
        status = series_sum((char)('a' + i), x, epsilon, &sums[i]);
        if (status == OK) status = integral((char)('a' + i), epsilon, &integrals[i]);
    }
    if (status != OK) { fprintf(stderr, "Calculation failed (%d)\n", status); return status; }
    for (int i = 0; i < 4; ++i) printf("%c series %.18Lg integral %.18Lg\n", 'a' + i, sums[i], integrals[i]);
    return fflush(stdout) == EOF || ferror(stdout) ? FILE_ERROR : OK;
}
