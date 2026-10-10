#define main application_main
#include "main.c"
#undef main
#include <assert.h>

int main(void) {
    long double values[4];
    const long double epsilon = 1e-6L;
    assert(series_sum('a', 1, epsilon, &values[0]) == OK && fabsl(values[0] - expl(1)) <= epsilon);
    assert(series_sum('a', -3, epsilon, &values[0]) == OK && fabsl(values[0] - expl(-3)) <= epsilon);
    assert(series_sum('b', 2, epsilon, &values[0]) == OK && fabsl(values[0] - cosl(2)) <= epsilon);
    assert(series_sum('c', 0, epsilon, &values[0]) == OK && values[0] == 1);
    assert(series_sum('c', 0.5L, 1e-12L, &values[0]) == OK && fabsl(values[0] - 2.900487651445253175795L) <= 1e-12L);
    assert(series_sum('d', 0.5L, epsilon, &values[0]) == OK && fabsl(values[0] - (1 / sqrtl(1.25L) - 1)) <= epsilon);
    assert(series_sum('d', 1, 1e-3L, &values[0]) == OK && fabsl(values[0] - (1 / sqrtl(2) - 1)) <= 1e-3L);
    assert(series_sum('c', 1, epsilon, &values[0]) == INVALID);
    assert(series_sum('d', 1.1L, epsilon, &values[0]) == INVALID);
    assert(series_sum('a', 1000, epsilon, &values[0]) == NO_CONVERGENCE);
    assert(integral('a', epsilon, &values[0]) == OK && fabsl(values[0] - 0.822467033424113218L) <= epsilon);
    assert(integral('b', epsilon, &values[1]) == OK && fabsl(values[1] - 0.855624391892148803L) <= epsilon);
    assert(integral('c', epsilon, &values[2]) == OK && fabsl(values[2] - 1) <= epsilon);
    assert(integral('d', epsilon, &values[3]) == OK && fabsl(values[3] - 0.783430510712134407L) <= epsilon);
    assert(integral('c', 1e-9L, &values[2]) == OK && fabsl(values[2] - 1) <= 1e-9L);
    assert(integral('d', 1e-9L, &values[3]) == OK && fabsl(values[3] - 0.783430510712134407L) <= 1e-9L);
    assert(integral('a', 0, &values[0]) == INVALID);
    long double next, ratio;
    assert(next_term('c', 0.5L, 0, 1, &next, &ratio) == OK);
    assert(fabsl(next - 1.125L) < 1e-12L);
    assert(series_sum('c', -0.5L, 1e-12L, &values[0]) == OK);
    assert(fabsl(values[0] - 2.900487651445253175795L) <= 1e-12L);
    assert(integral('z', epsilon, &values[0]) == INVALID);
    puts("task_05: series, integrals and convergence boundaries passed");
    return 0;
}
