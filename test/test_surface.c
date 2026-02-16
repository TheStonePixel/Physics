#include "physics/physics.h"
#include "test_helpers.h"
#include <math.h>

int test_surface_run(void) {
    TEST_SUITE_START("surface");

    /* Surface creation */
    {
        phys_surface s = phys_surface_create(0.1f, 0.5f, 0.7f);
        ASSERT_FLOAT_EQ(s.rolling_friction, 0.1f, "rolling_friction set");
        ASSERT_FLOAT_EQ(s.restitution, 0.5f, "restitution set");
        ASSERT_FLOAT_EQ(s.firmness, 0.7f, "firmness set");
        ASSERT_FLOAT_EQ(s.normal.y, 1.0f, "default normal is +Y");
    }

    /* Sloped surface creation */
    {
        phys_vec3 slope = {0.1f, 0.9f, 0};
        phys_surface s = phys_surface_create_sloped(0.1f, 0.5f, 0.7f, slope);
        phys_float len = sqrtf(s.normal.x * s.normal.x +
                               s.normal.y * s.normal.y +
                               s.normal.z * s.normal.z);
        ASSERT_FLOAT_EQ(len, 1.0f, "sloped normal is normalized");
    }

    /* Bounce reduces vertical velocity */
    {
        phys_vec3 vel_in = {20, -10, 0}; /* coming down at angle */
        phys_vec3 spin_axis = {0, 0, 1};
        phys_float spin_rate = 100.0f;
        phys_surface s = phys_surface_create(0.1f, 0.5f, 0.5f);

        phys_vec3 vel_out;
        phys_vec3 axis_out;
        phys_float spin_out;
        phys_surface_bounce(vel_in, spin_axis, spin_rate, 0.02f, s,
                            &vel_out, &axis_out, &spin_out);

        ASSERT_TRUE(vel_out.y > 0, "bounce reverses vertical velocity");
        ASSERT_TRUE(vel_out.y < 10.0f, "bounce loses energy");
        ASSERT_TRUE(spin_out < spin_rate, "bounce reduces spin");
    }

    /* Rolling decelerates on flat surface */
    {
        phys_vec3 pos = {0, 0, 0};
        phys_vec3 vel = {5, 0, 0}; /* rolling at 5 m/s */
        phys_float spin = 50.0f;
        phys_vec3 gravity = {0, -9.81f, 0};
        phys_surface s = phys_surface_create(0.1f, 0.5f, 0.5f);

        int still_rolling = phys_surface_roll_step(
            &pos, &vel, &spin, 0.02f, 0.045f, gravity, s, 0.01f);

        ASSERT_TRUE(still_rolling, "ball still rolling at 5 m/s");
        ASSERT_TRUE(vel.x < 5.0f, "rolling friction slows ball");
        ASSERT_TRUE(pos.x > 0, "ball moved forward");
    }

    /* Full roll simulation: ball lands and eventually stops */
    {
        phys_vec3 pos0 = {0, 0, 0};
        phys_vec3 vel0 = {15, -5, 0}; /* landing with forward + downward velocity */
        phys_vec3 spin_axis = {0, 0, 1};
        phys_float spin_rate = 200.0f;
        phys_vec3 gravity = {0, -9.81f, 0};
        phys_surface s = phys_surface_create(0.15f, 0.4f, 0.6f);

        phys_rolling_point pts[8192];
        int n = phys_surface_simulate_roll(pos0, vel0, spin_axis, spin_rate,
                                           0.02f, 0.045f, gravity, s,
                                           0.01f, pts, 8192);

        ASSERT_TRUE(n > 10, "roll sim has multiple points");

        /* Ball should have moved forward */
        phys_float final_x = pts[n - 1].position.x;
        ASSERT_TRUE(final_x > 1.0f, "ball rolled forward > 1m");

        /* Ball should have stopped */
        phys_float final_speed = sqrtf(pts[n - 1].velocity.x * pts[n - 1].velocity.x +
                                       pts[n - 1].velocity.y * pts[n - 1].velocity.y +
                                       pts[n - 1].velocity.z * pts[n - 1].velocity.z);
        ASSERT_TRUE(final_speed < 0.1f, "ball came to rest");
    }

    /* Higher friction stops ball sooner */
    {
        phys_vec3 pos0 = {0, 0, 0};
        phys_vec3 vel0 = {10, 0, 0};
        phys_vec3 spin_axis = {0, 0, 1};
        phys_vec3 gravity = {0, -9.81f, 0};

        phys_surface low_friction = phys_surface_create(0.05f, 0.3f, 0.5f);
        phys_surface high_friction = phys_surface_create(0.25f, 0.3f, 0.5f);

        phys_rolling_point pts_low[4096], pts_high[4096];
        int n_low = phys_surface_simulate_roll(pos0, vel0, spin_axis, 0,
                                               0.02f, 0.045f, gravity,
                                               low_friction, 0.005f,
                                               pts_low, 4096);
        int n_high = phys_surface_simulate_roll(pos0, vel0, spin_axis, 0,
                                                0.02f, 0.045f, gravity,
                                                high_friction, 0.005f,
                                                pts_high, 4096);

        phys_float dist_low = pts_low[n_low - 1].position.x;
        phys_float dist_high = pts_high[n_high - 1].position.x;

        ASSERT_TRUE(dist_high < dist_low,
                    "high friction stops ball sooner than low friction");
    }

    TEST_SUITE_END("surface");
}
