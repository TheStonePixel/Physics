#include "physics/kinematics.h"
#include "physics/vec.h"
#include "test_helpers.h"
#include <math.h>

int test_kinematics_run(void) {
    TEST_SUITE_START("kinematics");

    phys_vec3 gravity = {0, -9.81f, 0};

    /* Projectile at t=0 should be at initial position */
    {
        phys_vec3 pos0 = {0, 0, 0};
        phys_vec3 vel0 = {10, 20, 0};
        phys_vec3 p = phys_projectile_position(pos0, vel0, gravity, 0);
        ASSERT_VEC3_EQ(p, 0, 0, 0, "projectile pos at t=0");
    }

    /* Projectile position at t=1 */
    {
        phys_vec3 pos0 = {0, 0, 0};
        phys_vec3 vel0 = {10, 20, 0};
        phys_vec3 p = phys_projectile_position(pos0, vel0, gravity, 1.0f);
        /* x = 10, y = 20 - 4.905 = 15.095, z = 0 */
        ASSERT_FLOAT_EQ(p.x, 10.0f, "proj pos t=1 x");
        ASSERT_FLOAT_EQ(p.y, 15.095f, "proj pos t=1 y");
        ASSERT_FLOAT_EQ(p.z, 0.0f, "proj pos t=1 z");
    }

    /* Velocity at t=1 */
    {
        phys_vec3 vel0 = {10, 20, 0};
        phys_vec3 v = phys_projectile_velocity(vel0, gravity, 1.0f);
        ASSERT_FLOAT_EQ(v.x, 10.0f, "proj vel t=1 x");
        ASSERT_FLOAT_EQ(v.y, 10.19f, "proj vel t=1 y");
    }

    /* Time to ground */
    {
        phys_vec3 pos0 = {0, 10, 0};
        phys_vec3 vel0 = {10, 0, 0}; /* horizontal shot from height 10 */
        phys_float t = phys_projectile_time_to_y(pos0, vel0, gravity.y, 0.0f);
        /* 10 + 0*t - 4.905*t^2 = 0 => t = sqrt(10/4.905) = 1.4278 */
        ASSERT_FLOAT_EQ(t, sqrtf(10.0f / 4.905f), "time to ground");
    }

    /* Max height */
    {
        phys_vec3 pos0 = {0, 0, 0};
        phys_vec3 vel0 = {0, 20, 0};
        phys_float h = phys_projectile_max_height(pos0, vel0, gravity.y);
        /* h = v^2 / (2g) = 400 / 19.62 = 20.387 */
        ASSERT_FLOAT_EQ(h, 400.0f / 19.62f, "max height");
    }

    /* Range */
    {
        /* 45 degree launch at 20 m/s */
        phys_float angle = 3.14159265f / 4.0f;
        phys_vec3 vel0 = {20.0f * cosf(angle), 20.0f * sinf(angle), 0};
        phys_float r = phys_projectile_range(vel0, gravity.y);
        /* R = v^2 * sin(2*45) / g = 400/9.81 = 40.77 */
        ASSERT_FLOAT_EQ(r, 400.0f / 9.81f, "range at 45 deg");
    }

    /* Trajectory sampling */
    {
        phys_vec3 pos0 = {0, 0, 0};
        phys_vec3 vel0 = {10, 20, 0};
        phys_trajectory_point pts[10000];
        int n = phys_trajectory_sample(pos0, vel0, gravity, 0.0f, 0.001f, pts, 10000);
        ASSERT_TRUE(n > 100, "trajectory has multiple points");
        ASSERT_FLOAT_EQ(pts[0].position.x, 0.0f, "trajectory start x");
        ASSERT_FLOAT_EQ(pts[0].position.y, 0.0f, "trajectory start y");
        /* Last point should be near ground (within one timestep) */
        ASSERT_TRUE(pts[n-1].position.y <= 0.05f, "trajectory ends near ground");
    }

    TEST_SUITE_END("kinematics");
}
