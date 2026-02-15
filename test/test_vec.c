#include "physics/vec.h"
#include "test_helpers.h"
#include <math.h>

int test_vec_run(void) {
    TEST_SUITE_START("vec");

    /* vec2 basics */
    {
        phys_vec2 a = phys_vec2_create(3, 4);
        ASSERT_FLOAT_EQ(phys_vec2_length(a), 5.0f, "vec2 length (3,4)=5");

        phys_vec2 b = phys_vec2_create(1, 2);
        phys_vec2 sum = phys_vec2_add(a, b);
        ASSERT_FLOAT_EQ(sum.x, 4.0f, "vec2 add x");
        ASSERT_FLOAT_EQ(sum.y, 6.0f, "vec2 add y");

        phys_vec2 diff = phys_vec2_sub(a, b);
        ASSERT_FLOAT_EQ(diff.x, 2.0f, "vec2 sub x");
        ASSERT_FLOAT_EQ(diff.y, 2.0f, "vec2 sub y");

        ASSERT_FLOAT_EQ(phys_vec2_dot(a, b), 11.0f, "vec2 dot");

        phys_vec2 n = phys_vec2_normalize(a);
        ASSERT_FLOAT_EQ(phys_vec2_length(n), 1.0f, "vec2 normalize length");
    }

    /* vec3 basics */
    {
        phys_vec3 a = phys_vec3_create(1, 2, 3);
        phys_vec3 b = phys_vec3_create(4, 5, 6);

        phys_vec3 sum = phys_vec3_add(a, b);
        ASSERT_VEC3_EQ(sum, 5, 7, 9, "vec3 add");

        phys_vec3 diff = phys_vec3_sub(a, b);
        ASSERT_VEC3_EQ(diff, -3, -3, -3, "vec3 sub");

        ASSERT_FLOAT_EQ(phys_vec3_dot(a, b), 32.0f, "vec3 dot");

        phys_vec3 cross = phys_vec3_cross(a, b);
        ASSERT_VEC3_EQ(cross, -3, 6, -3, "vec3 cross");

        phys_vec3 scaled = phys_vec3_scale(a, 2.0f);
        ASSERT_VEC3_EQ(scaled, 2, 4, 6, "vec3 scale");

        phys_vec3 n = phys_vec3_normalize(a);
        ASSERT_FLOAT_EQ(phys_vec3_length(n), 1.0f, "vec3 normalize length");
    }

    /* vec3 zero and negate */
    {
        phys_vec3 z = phys_vec3_zero();
        ASSERT_VEC3_EQ(z, 0, 0, 0, "vec3 zero");

        phys_vec3 a = phys_vec3_create(1, -2, 3);
        phys_vec3 neg = phys_vec3_negate(a);
        ASSERT_VEC3_EQ(neg, -1, 2, -3, "vec3 negate");
    }

    /* vec3 distance */
    {
        phys_vec3 a = phys_vec3_create(0, 0, 0);
        phys_vec3 b = phys_vec3_create(3, 4, 0);
        ASSERT_FLOAT_EQ(phys_vec3_distance(a, b), 5.0f, "vec3 distance");
    }

    /* vec3 lerp */
    {
        phys_vec3 a = phys_vec3_create(0, 0, 0);
        phys_vec3 b = phys_vec3_create(10, 20, 30);
        phys_vec3 mid = phys_vec3_lerp(a, b, 0.5f);
        ASSERT_VEC3_EQ(mid, 5, 10, 15, "vec3 lerp 0.5");
    }

    /* vec4 basics */
    {
        phys_vec4 a = phys_vec4_create(1, 2, 3, 4);
        phys_vec4 b = phys_vec4_create(5, 6, 7, 8);
        ASSERT_FLOAT_EQ(phys_vec4_dot(a, b), 70.0f, "vec4 dot");

        phys_vec4 n = phys_vec4_normalize(a);
        ASSERT_FLOAT_EQ(phys_vec4_length(n), 1.0f, "vec4 normalize length");
    }

    TEST_SUITE_END("vec");
}
