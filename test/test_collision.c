#include "physics/collision.h"
#include "physics/vec.h"
#include "test_helpers.h"
#include <math.h>

int test_collision_run(void) {
    TEST_SUITE_START("collision");

    /* Sphere-plane: sphere resting on ground */
    {
        phys_sphere s = {{0, 1, 0}, 1.0f};  /* center at y=1, radius=1 */
        phys_plane p = {{0, 1, 0}, 0};       /* y=0 ground plane */
        phys_collision col = phys_collide_sphere_plane(s, p);
        /* Just touching, not penetrating */
        ASSERT_TRUE(!col.hit, "sphere on ground not penetrating");
    }

    /* Sphere-plane: sphere penetrating */
    {
        phys_sphere s = {{0, 0.5f, 0}, 1.0f}; /* center at y=0.5, radius=1 */
        phys_plane p = {{0, 1, 0}, 0};
        phys_collision col = phys_collide_sphere_plane(s, p);
        ASSERT_TRUE(col.hit, "sphere penetrating plane");
        ASSERT_FLOAT_EQ(col.depth, 0.5f, "penetration depth");
    }

    /* Sphere-sphere: not touching */
    {
        phys_sphere a = {{0, 0, 0}, 1.0f};
        phys_sphere b = {{3, 0, 0}, 1.0f};
        phys_collision col = phys_collide_sphere_sphere(a, b);
        ASSERT_TRUE(!col.hit, "spheres not touching");
    }

    /* Sphere-sphere: overlapping */
    {
        phys_sphere a = {{0, 0, 0}, 1.0f};
        phys_sphere b = {{1.5f, 0, 0}, 1.0f};
        phys_collision col = phys_collide_sphere_sphere(a, b);
        ASSERT_TRUE(col.hit, "spheres overlapping");
        ASSERT_FLOAT_EQ(col.depth, 0.5f, "sphere-sphere depth");
    }

    /* Point in sphere */
    {
        phys_sphere s = {{0, 0, 0}, 2.0f};
        ASSERT_TRUE(phys_point_in_sphere(phys_vec3_create(1, 0, 0), s),
                    "point inside sphere");
        ASSERT_TRUE(!phys_point_in_sphere(phys_vec3_create(3, 0, 0), s),
                    "point outside sphere");
    }

    /* Point-plane distance */
    {
        phys_plane p = {{0, 1, 0}, 0};
        phys_float d = phys_point_plane_distance(phys_vec3_create(0, 5, 0), p);
        ASSERT_FLOAT_EQ(d, 5.0f, "point-plane distance above");

        d = phys_point_plane_distance(phys_vec3_create(0, -3, 0), p);
        ASSERT_FLOAT_EQ(d, -3.0f, "point-plane distance below");
    }

    /* Ray-sphere */
    {
        phys_sphere s = {{5, 0, 0}, 1.0f};
        phys_vec3 origin = {0, 0, 0};
        phys_vec3 dir = {1, 0, 0};
        phys_float t = phys_ray_sphere(origin, dir, s);
        ASSERT_FLOAT_EQ(t, 4.0f, "ray-sphere hit t=4");

        /* Miss */
        phys_vec3 dir_miss = {0, 1, 0};
        t = phys_ray_sphere(origin, dir_miss, s);
        ASSERT_TRUE(t < 0, "ray-sphere miss");
    }

    /* Ray-plane */
    {
        phys_plane p = {{0, 1, 0}, -5.0f}; /* y=5 plane */
        phys_vec3 origin = {0, 0, 0};
        phys_vec3 dir = {0, 1, 0};
        phys_float t = phys_ray_plane(origin, dir, p);
        ASSERT_FLOAT_EQ(t, 5.0f, "ray-plane hit t=5");

        /* Parallel ray */
        phys_vec3 dir_par = {1, 0, 0};
        t = phys_ray_plane(origin, dir_par, p);
        ASSERT_TRUE(t < 0, "ray-plane parallel miss");
    }

    TEST_SUITE_END("collision");
}
