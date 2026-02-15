#include "physics/mat.h"
#include "test_helpers.h"
#include <math.h>

int test_mat_run(void) {
    TEST_SUITE_START("mat");

    /* Identity matrix */
    {
        phys_mat4 id = phys_mat4_identity();
        ASSERT_FLOAT_EQ(id.m[0], 1.0f, "mat4 identity [0,0]");
        ASSERT_FLOAT_EQ(id.m[5], 1.0f, "mat4 identity [1,1]");
        ASSERT_FLOAT_EQ(id.m[10], 1.0f, "mat4 identity [2,2]");
        ASSERT_FLOAT_EQ(id.m[15], 1.0f, "mat4 identity [3,3]");
        ASSERT_FLOAT_EQ(id.m[1], 0.0f, "mat4 identity off-diag");
    }

    /* Identity * vec4 = vec4 */
    {
        phys_mat4 id = phys_mat4_identity();
        phys_vec4 v = phys_vec4_create(1, 2, 3, 1);
        phys_vec4 result = phys_mat4_mul_vec4(id, v);
        ASSERT_FLOAT_EQ(result.x, 1.0f, "id*v x");
        ASSERT_FLOAT_EQ(result.y, 2.0f, "id*v y");
        ASSERT_FLOAT_EQ(result.z, 3.0f, "id*v z");
        ASSERT_FLOAT_EQ(result.w, 1.0f, "id*v w");
    }

    /* Translation matrix */
    {
        phys_mat4 t = phys_mat4_translate(phys_vec3_create(10, 20, 30));
        phys_vec4 v = phys_vec4_create(1, 2, 3, 1);
        phys_vec4 result = phys_mat4_mul_vec4(t, v);
        ASSERT_FLOAT_EQ(result.x, 11.0f, "translate x");
        ASSERT_FLOAT_EQ(result.y, 22.0f, "translate y");
        ASSERT_FLOAT_EQ(result.z, 33.0f, "translate z");
    }

    /* Scale matrix */
    {
        phys_mat4 s = phys_mat4_scale(phys_vec3_create(2, 3, 4));
        phys_vec4 v = phys_vec4_create(1, 1, 1, 1);
        phys_vec4 result = phys_mat4_mul_vec4(s, v);
        ASSERT_FLOAT_EQ(result.x, 2.0f, "scale x");
        ASSERT_FLOAT_EQ(result.y, 3.0f, "scale y");
        ASSERT_FLOAT_EQ(result.z, 4.0f, "scale z");
    }

    /* Rotation: 90 degrees around Z should map (1,0,0) -> (0,1,0) */
    {
        float pi_2 = 3.14159265f / 2.0f;
        phys_mat4 rz = phys_mat4_rotate_z(pi_2);
        phys_vec4 v = phys_vec4_create(1, 0, 0, 1);
        phys_vec4 result = phys_mat4_mul_vec4(rz, v);
        ASSERT_FLOAT_EQ(result.x, 0.0f, "rot_z 90 x");
        ASSERT_FLOAT_EQ(result.y, 1.0f, "rot_z 90 y");
        ASSERT_FLOAT_EQ(result.z, 0.0f, "rot_z 90 z");
    }

    /* Matrix multiply: identity * M = M */
    {
        phys_mat4 id = phys_mat4_identity();
        phys_mat4 t = phys_mat4_translate(phys_vec3_create(5, 6, 7));
        phys_mat4 result = phys_mat4_multiply(id, t);
        ASSERT_FLOAT_EQ(result.m[12], 5.0f, "id*T [12]");
        ASSERT_FLOAT_EQ(result.m[13], 6.0f, "id*T [13]");
        ASSERT_FLOAT_EQ(result.m[14], 7.0f, "id*T [14]");
    }

    /* Transpose */
    {
        phys_mat4 id = phys_mat4_identity();
        phys_mat4 t = phys_mat4_transpose(id);
        ASSERT_FLOAT_EQ(t.m[0], 1.0f, "transpose id diagonal");
        ASSERT_FLOAT_EQ(t.m[1], 0.0f, "transpose id off-diag");
    }

    /* mat3 identity and from_mat4 */
    {
        phys_mat3 id3 = phys_mat3_identity();
        ASSERT_FLOAT_EQ(id3.m[0], 1.0f, "mat3 identity [0,0]");
        ASSERT_FLOAT_EQ(id3.m[4], 1.0f, "mat3 identity [1,1]");

        phys_mat4 id4 = phys_mat4_identity();
        phys_mat3 from4 = phys_mat3_from_mat4(id4);
        ASSERT_FLOAT_EQ(from4.m[0], 1.0f, "mat3_from_mat4 [0,0]");
        ASSERT_FLOAT_EQ(from4.m[8], 1.0f, "mat3_from_mat4 [2,2]");
    }

    TEST_SUITE_END("mat");
}
