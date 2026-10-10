#define main application_main
#include "main.c"
#undef main
#include <assert.h>

int main(void) {
    long double roots[2];
    int count;
    assert(solve_quadratic(1, -3, 2, 1e-12L, roots, &count) == OK && count == 2);
    assert(fabsl(roots[0] - 2) < 1e-12L && fabsl(roots[1] - 1) < 1e-12L);
    assert(solve_quadratic(1, 2, 1, 1e-12L, roots, &count) == OK && count == 1 && roots[0] == -1);
    assert(solve_quadratic(1, 0, 1, 1e-12L, roots, &count) == OK && count == 0);
    assert(solve_quadratic(0, 2, -4, 1e-12L, roots, &count) == OK && count == 1 && roots[0] == 2);
    assert(solve_quadratic(0, 0, 0, 1e-12L, roots, &count) == OK && count == -1);
    assert(solve_quadratic(0, 0, 1, 1e-12L, roots, &count) == OK && count == 0);
    assert(solve_quadratic(1, 1e12L, 1, 1e-12L, roots, &count) == OK);
    assert(fabsl(roots[1] + 1e-12L) < 1e-20L);
    int result;
    assert(right_triangle(3, 5, 4, 1e-12L, &result) == OK && result);
    assert(right_triangle(2, 2, 3, 1e-12L, &result) == OK && !result);
    assert(right_triangle(-3, 4, 5, 1e-12L, &result) == INVALID);
    assert(is_multiple(LLONG_MIN, -1, &result) == OK && result);
    assert(is_multiple(10, 3, &result) == OK && !result);
    assert(is_multiple(0, 2, &result) == INVALID);
    struct equation_solution solutions[6];
    int solution_count;
    const long double repeated[3] = {2, 2, -4};
    assert(solve_permutations(repeated, 1e-12L, solutions, &solution_count) == OK);
    assert(solution_count == 3);
    for (int i = 0; i < solution_count; ++i) {
        for (int j = 0; j < solutions[i].root_count; ++j) {
            const long double root = solutions[i].roots[j];
            const long double *coefficients = solutions[i].coefficients;
            assert(fabsl((coefficients[0] * root + coefficients[1]) * root + coefficients[2]) < 1e-10L);
        }
    }
    const long double invalid[3] = {NAN, 1, 2};
    assert(solve_permutations(invalid, 1e-9L, solutions, &solution_count) == INVALID);
    puts("task_03: equation, triangle and divisibility checks passed");
    return 0;
}
