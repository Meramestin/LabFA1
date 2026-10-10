#define main application_main
#include "main.c"
#undef main
#include <assert.h>

int main(void) {
    const long double expected[5] = {
        2.718281828459045235360287L,
        3.141592653589793238462643L,
        0.693147180559945309417232L,
        1.414213562373095048801689L,
        0.577215664901532860606512L
    };
    const long double epsilons[2] = {1e-4L, 1e-6L};
    for (int k = 0; k < 2; ++k) for (int i = 0; i < 5; ++i) {
        long double value;
        assert(constant_limit(i, epsilons[k], &value) == OK);
        assert(fabsl(value - expected[i]) <= epsilons[k]);
        assert(constant_series(i, epsilons[k], &value) == OK);
        assert(fabsl(value - expected[i]) <= epsilons[k]);
        assert(constant_equation(i, epsilons[k], &value) == OK);
        assert(fabsl(value - expected[i]) <= epsilons[k]);
    }
    long double value;
    assert(constant_limit(-1, 1e-6L, &value) == INVALID);
    assert(constant_series(0, 0, &value) == INVALID);
    assert(constant_equation(4, 1e-30L, &value) == NO_CONVERGENCE);
    assert(constant_limit(LN2, 1e-12L, &value) == OK);
    assert(fabsl(value - expected[LN2]) <= 1e-12L);
    assert(constant_series(E, 1e-6L, NULL) == INVALID);
    assert(constant_equation(GAMMA, 1e30L, &value) == OK && isfinite(value));
    assert(equation_value(E, 0, 0, &value) == INVALID);
    assert(equation_value(E, -1, 0, &value) == INVALID);
    puts("task_02: 15 methods at two precisions passed");
    return 0;
}
