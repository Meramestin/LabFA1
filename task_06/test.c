#define main application_main
#include "main.c"
#undef main
#include <assert.h>

int main(void) {
    int convex;
    assert(convex_polygon(1e-9, &convex, 4, 0.0,0.0, 2.0,0.0, 2.0,2.0, 0.0,2.0) == OK && convex);
    assert(convex_polygon(1e-9, &convex, 4, 0.0,0.0, 0.0,2.0, 2.0,2.0, 2.0,0.0) == OK && convex);
    assert(convex_polygon(1e-9, &convex, 4, 0.0,0.0, 2.0,0.0, 1.0,0.5, 0.0,2.0) == OK && !convex);
    assert(convex_polygon(1e-9, &convex, 4, 0.0,0.0, 2.0,2.0, 0.0,2.0, 2.0,0.0) == OK && !convex);
    assert(convex_polygon(1e-9, &convex, 5, 0.0,3.0, 2.0,-3.0, -3.0,1.0, 3.0,1.0, -2.0,-3.0) == OK && !convex);
    assert(convex_polygon(1e-9, &convex, 3, 0.0,0.0, 1.0,0.0, 2.0,0.0) == OK && !convex);
    assert(convex_polygon(1e-9, &convex, 3, 0.0,0.0, 0.0,0.0, 2.0,0.0) == INVALID);
    double value;
    assert(polynomial(2.0, &value, 2, 1.0, -3.0, 2.0) == OK && fabs(value) < 1e-9);
    assert(polynomial(2.0, &value, 0, 7.0) == OK && value == 7.0);
    assert(polynomial(2.0, &value, -1) == INVALID);
    long long numbers[8];
    size_t count;
    assert(find_kaprekar(10, numbers, 8, &count, 8, "0", "1", "9", "45", "55", "10", "4879", "2") == OK);
    assert(count == 5 && numbers[0] == 1 && numbers[4] == 4879);
    assert(find_kaprekar(2, numbers, 8, &count, 3, "1", "110", "10") == OK && count == 2);
    assert(find_kaprekar(10, numbers, 8, &count, 1, "999999999999") == OVERFLOW);
    assert(find_kaprekar(10, numbers, 8, &count, 1, "-1") == INVALID);
    assert(geometric_mean(1e-12, &value, 3, 1.0, 4.0, 16.0) == OK && fabs(value - 4) < 1e-9);
    assert(geometric_mean(1e-12, &value, 2, 0.0, 4.0) == OK && value == 0);
    assert(geometric_mean(1e-12, &value, 2, -1.0, 4.0) == INVALID);
    assert(fast_power(2, -3, 1e-12, &value) == OK && fabs(value - 0.125) < 1e-12);
    assert(fast_power(-2, 3, 1e-12, &value) == OK && value == -8);
    assert(fast_power(1, INT_MIN, 1e-12, &value) == OK && value == 1);
    assert(fast_power(0, -1, 1e-12, &value) == INVALID);
    assert(fast_power(1e200, 2, 1e-12, &value) == OVERFLOW);
    assert(bisect(0, 2, 1e-9, square_equation, &value) == OK && fabs(value - sqrt(2.0)) < 1e-9);
    assert(bisect(0, 2, 1e-7, cosine_equation, &value) == OK && fabs(value - 0.739085133215) < 1e-7);
    assert(bisect(2, 3, 1e-9, square_equation, &value) == INVALID);
    assert(bisect(2, 0, 1e-9, square_equation, &value) == INVALID);
    assert(bisect(1.4142135, 1.4142136, 1e-6, square_equation, &value) == OK);
    assert(fabs(value - sqrt(2.0)) < 1e-6);
    assert(bisect(1, 2, 1e-30, square_equation, &value) == NO_CONVERGENCE);
    assert(bisect(0, 2, 1e-9, NULL, &value) == INVALID);
    assert(fast_power(0, 0, 1e-12, &value) == OK && value == 1);
    assert(geometric_mean(1e-12, &value, 3, 0.0, 1.0, -2.0) == INVALID);
    puts("task_06: all six functions and boundary cases passed");
    return 0;
}
