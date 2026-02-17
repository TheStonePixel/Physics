#include "physics/aerodynamics.h"
#include "physics/vec.h"
#include "test_helpers.h"
#include <math.h>

int test_aerodynamics_run(void) {
    TEST_SUITE_START("aerodynamics");

    /* Drag opposes velocity */
    {
        phys_vec3 vel = {50, 0, 0}; /* 50 m/s in x */
        phys_vec3 drag = phys_aero_drag(vel, 0.25f, 0.00143f, 1.225f);
        ASSERT_TRUE(drag.x < 0, "drag opposes +x velocity");
        ASSERT_FLOAT_EQ(drag.y, 0.0f, "drag has no y component");
        ASSERT_FLOAT_EQ(drag.z, 0.0f, "drag has no z component");
    }

    /* Drag is zero for zero velocity */
    {
        phys_vec3 vel = {0, 0, 0};
        phys_vec3 drag = phys_aero_drag(vel, 0.25f, 0.00143f, 1.225f);
        ASSERT_VEC3_EQ(drag, 0, 0, 0, "no drag at rest");
    }

    /* Magnus effect: spin creates lift perpendicular to velocity */
    {
        /* Body moving forward (+x) with spin axis +z */
        phys_vec3 vel = {50, 0, 0};
        phys_vec3 spin_axis = {0, 0, 1};
        phys_float spin_rate = 300.0f; /* rad/s */

        phys_vec3 magnus = phys_aero_magnus(vel, spin_axis, spin_rate,
                                            0.02135f, 0.25f, 1.225f);
        /* cross(z, x) yields +y, so magnus should have positive y (lift) */
        ASSERT_TRUE(magnus.y > 0, "spin creates lift (+y)");
    }

    /* Full aerodynamic simulation produces reasonable trajectory */
    {
        phys_vec3 pos0 = {0, 0, 0};
        phys_float angle = 10.0f * 3.14159265f / 180.0f;
        phys_vec3 vel0 = {70.0f * cosf(angle), 70.0f * sinf(angle), 0};
        phys_vec3 spin_axis = {0, 0, 1};
        phys_float spin_rate = 300.0f;
        phys_vec3 gravity = {0, -9.81f, 0};

        /* Small sphere: mass 0.046 kg, radius 0.021 m */
        phys_aero_params params;
        params.drag_coefficient = 0.25f;
        params.lift_coefficient = 0.15f;
        params.cross_section_area = 0.00143f;
        params.radius = 0.02135f;
        params.air_density = 1.225f;
        params.spin_decay = 0.04f;

        phys_trajectory_point pts[4096];
        int n = phys_aero_simulate(pos0, vel0, spin_axis, spin_rate,
                                   0.04593f, 0.6f, 0.4f,
                                   gravity, params, 0.0f, 0.005f, pts, 4096);

        ASSERT_TRUE(n > 100, "aero sim has many points");

        /* Find max height and final distance */
        phys_float max_y = 0;
        for (int i = 0; i < n; i++) {
            if (pts[i].position.y > max_y) max_y = pts[i].position.y;
        }
        phys_float carry = pts[n-1].position.x;

        /* With Cl=0.15 and Cd=0.25, should carry 50-300m */
        ASSERT_TRUE(carry > 50.0f, "carry > 50m");
        ASSERT_TRUE(carry < 300.0f, "carry < 300m");
        ASSERT_TRUE(max_y > 5.0f, "max height > 5m");
        ASSERT_TRUE(max_y < 80.0f, "max height < 80m");
    }

    TEST_SUITE_END("aerodynamics");
}
