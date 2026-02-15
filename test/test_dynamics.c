#include "physics/dynamics.h"
#include "physics/vec.h"
#include "test_helpers.h"

int test_dynamics_run(void) {
    TEST_SUITE_START("dynamics");

    /* Body init */
    {
        phys_body body;
        phys_body_init(&body, 2.0f);
        ASSERT_FLOAT_EQ(body.mass, 2.0f, "body mass");
        ASSERT_FLOAT_EQ(body.inv_mass, 0.5f, "body inv_mass");
        ASSERT_VEC3_EQ(body.position, 0, 0, 0, "body init pos");
        ASSERT_VEC3_EQ(body.velocity, 0, 0, 0, "body init vel");
    }

    /* Zero mass = static */
    {
        phys_body body;
        phys_body_init(&body, 0.0f);
        ASSERT_FLOAT_EQ(body.inv_mass, 0.0f, "static body inv_mass");

        /* Step should do nothing */
        phys_body_apply_force(&body, phys_vec3_create(100, 0, 0));
        phys_body_step_euler(&body, 1.0f);
        ASSERT_VEC3_EQ(body.position, 0, 0, 0, "static body doesn't move");
    }

    /* Euler integration: constant force */
    {
        phys_body body;
        phys_body_init(&body, 1.0f);

        /* F=10 on 1kg body => a=10 m/s^2 */
        phys_body_apply_force(&body, phys_vec3_create(10, 0, 0));
        phys_body_step_euler(&body, 1.0f);

        /* After 1s with semi-implicit Euler: v = 10, x = 10 */
        ASSERT_FLOAT_EQ(body.velocity.x, 10.0f, "euler vel after 1s");
        ASSERT_FLOAT_EQ(body.position.x, 10.0f, "euler pos after 1s");
    }

    /* Gravity */
    {
        phys_body body;
        phys_body_init(&body, 1.0f);
        phys_vec3 gravity = {0, -9.81f, 0};
        phys_body_apply_gravity(&body, gravity);

        /* Force should be mass*g = -9.81 */
        ASSERT_FLOAT_EQ(body.force_accum.y, -9.81f, "gravity force");
    }

    /* Verlet integration */
    {
        phys_body body;
        phys_body_init(&body, 1.0f);
        body.velocity = phys_vec3_create(10, 0, 0);

        /* No force, constant velocity */
        phys_body_step_verlet(&body, 1.0f);
        ASSERT_FLOAT_EQ(body.position.x, 10.0f, "verlet const vel pos");
        ASSERT_FLOAT_EQ(body.velocity.x, 10.0f, "verlet const vel vel");
    }

    /* Plane collision */
    {
        phys_body body;
        phys_body_init(&body, 1.0f);
        body.position = phys_vec3_create(0, -0.5f, 0); /* below ground */
        body.velocity = phys_vec3_create(5, -10, 0);    /* moving down */
        body.restitution = 1.0f; /* perfect bounce */
        body.friction = 0.0f;

        phys_plane ground = {{0, 1, 0}, 0}; /* y=0 plane */
        phys_body_resolve_plane(&body, ground);

        ASSERT_TRUE(body.position.y >= 0.0f, "body pushed above ground");
        ASSERT_TRUE(body.velocity.y > 0.0f, "body bounces up");
    }

    TEST_SUITE_END("dynamics");
}
