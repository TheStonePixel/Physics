#include "physics/aerodynamics.h"
#include "physics/vec.h"
#include "test_helpers.h"
#include <math.h>

int test_aerodynamics_run(void) {
    TEST_SUITE_START("aerodynamics");

    /* Golf ball defaults are reasonable */
    {
        phys_aero_params p = phys_aero_golf_ball_defaults();
        ASSERT_TRUE(p.drag_coefficient > 0, "Cd > 0");
        ASSERT_TRUE(p.cross_section_area > 0, "area > 0");
        ASSERT_TRUE(p.radius > 0.02f && p.radius < 0.025f, "golf ball radius ~21mm");
        ASSERT_TRUE(p.air_density > 1.0f && p.air_density < 1.5f, "air density ~1.225");
    }

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

    /* Magnus effect: backspin creates lift */
    {
        /* Ball moving forward (+x) with backspin (spin axis = +z, i.e., topspin would be -z) */
        phys_vec3 vel = {50, 0, 0};
        phys_vec3 spin_axis = {0, 0, 1}; /* backspin axis */
        phys_float spin_rate = 300.0f;    /* rad/s */

        phys_vec3 magnus = phys_aero_magnus(vel, spin_axis, spin_rate,
                                            0.02135f, 1.225f);
        /* cross(z, x) = -y, so with backspin (+z) and velocity (+x),
           magnus = coeff * cross((0,0,spin*1), (50,0,0))
           = coeff * (0*0 - spin*0, spin*50 - 0*0, 0*0 - 0*50)
           = coeff * (0, spin*50, 0)
           So magnus should have positive y (lift) */
        ASSERT_TRUE(magnus.y > 0, "backspin creates lift (+y)");
    }

    /* Full golf simulation produces reasonable trajectory */
    {
        phys_vec3 pos0 = {0, 0, 0};
        /* Driver shot: ~70 m/s, ~10 degree loft */
        phys_float angle = 10.0f * 3.14159265f / 180.0f;
        phys_vec3 vel0 = {70.0f * cosf(angle), 70.0f * sinf(angle), 0};
        phys_vec3 spin_axis = {0, 0, 1}; /* backspin */
        phys_float spin_rate = 300.0f;     /* ~2900 rpm */
        phys_vec3 gravity = {0, -9.81f, 0};
        phys_aero_params params = phys_aero_golf_ball_defaults();

        phys_trajectory_point pts[4096];
        int n = phys_golf_simulate(pos0, vel0, spin_axis, spin_rate,
                                   gravity, params, 0.0f, 0.005f, pts, 4096);

        ASSERT_TRUE(n > 100, "golf sim has many points");

        /* Find max height and final distance */
        phys_float max_y = 0;
        for (int i = 0; i < n; i++) {
            if (pts[i].position.y > max_y) max_y = pts[i].position.y;
        }
        phys_float carry = pts[n-1].position.x;

        /* A driver shot should carry 150-300m and peak 20-50m */
        ASSERT_TRUE(carry > 100.0f, "golf carry > 100m");
        ASSERT_TRUE(carry < 350.0f, "golf carry < 350m");
        ASSERT_TRUE(max_y > 10.0f, "golf max height > 10m");
        ASSERT_TRUE(max_y < 80.0f, "golf max height < 80m");
    }

    TEST_SUITE_END("aerodynamics");
}
