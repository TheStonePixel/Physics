#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <stdio.h>
#include <math.h>

#define TEST_EPSILON 1e-4f

#define ASSERT_FLOAT_EQ(a, b, name) do { \
    if (fabsf((a) - (b)) > TEST_EPSILON) { \
        printf("  FAIL: %s — expected %.6f, got %.6f\n", name, (double)(b), (double)(a)); \
        fails++; \
    } \
} while(0)

#define ASSERT_VEC3_EQ(v, ex, ey, ez, name) do { \
    if (fabsf((v).x - (ex)) > TEST_EPSILON || \
        fabsf((v).y - (ey)) > TEST_EPSILON || \
        fabsf((v).z - (ez)) > TEST_EPSILON) { \
        printf("  FAIL: %s — expected (%.4f, %.4f, %.4f), got (%.4f, %.4f, %.4f)\n", \
               name, (double)(ex), (double)(ey), (double)(ez), \
               (double)(v).x, (double)(v).y, (double)(v).z); \
        fails++; \
    } \
} while(0)

#define ASSERT_TRUE(cond, name) do { \
    if (!(cond)) { \
        printf("  FAIL: %s\n", name); \
        fails++; \
    } \
} while(0)

#define TEST_SUITE_START(suite_name) \
    int fails = 0; \
    printf("[%s]\n", suite_name);

#define TEST_SUITE_END(suite_name) \
    if (fails == 0) printf("  All %s tests passed.\n", suite_name); \
    return fails;

#endif /* TEST_HELPERS_H */
