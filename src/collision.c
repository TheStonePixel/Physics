#include "physics/collision.h"
#include "physics/vec.h"
#include <math.h>

static phys_collision no_collision(void) {
    phys_collision c;
    c.hit = 0;
    c.point = phys_vec3_zero();
    c.normal = phys_vec3_zero();
    c.depth = 0;
    return c;
}

phys_collision phys_collide_sphere_plane(phys_sphere sphere, phys_plane plane) {
    phys_float dist = phys_vec3_dot(sphere.center, plane.normal) + plane.d;
    phys_float penetration = sphere.radius - dist;

    if (penetration <= 0) return no_collision();

    phys_collision col;
    col.hit = 1;
    col.normal = plane.normal;
    col.depth = penetration;
    col.point = phys_vec3_sub(sphere.center,
                              phys_vec3_scale(plane.normal, dist));
    return col;
}

phys_collision phys_collide_sphere_sphere(phys_sphere a, phys_sphere b) {
    phys_vec3 diff = phys_vec3_sub(a.center, b.center);
    phys_float dist_sq = phys_vec3_length_sq(diff);
    phys_float r_sum = a.radius + b.radius;

    if (dist_sq >= r_sum * r_sum) return no_collision();

    phys_float dist = sqrtf(dist_sq);
    phys_collision col;
    col.hit = 1;
    col.depth = r_sum - dist;

    if (dist > 1e-8f) {
        col.normal = phys_vec3_scale(diff, 1.0f / dist);
    } else {
        col.normal = phys_vec3_create(0, 1, 0); /* arbitrary */
    }

    col.point = phys_vec3_add(b.center,
                              phys_vec3_scale(col.normal, b.radius));
    return col;
}

int phys_point_in_sphere(phys_vec3 point, phys_sphere sphere) {
    return phys_vec3_distance(point, sphere.center) <= sphere.radius;
}

phys_float phys_point_plane_distance(phys_vec3 point, phys_plane plane) {
    return phys_vec3_dot(point, plane.normal) + plane.d;
}

phys_float phys_ray_sphere(phys_vec3 origin, phys_vec3 direction,
                           phys_sphere sphere) {
    phys_vec3 oc = phys_vec3_sub(origin, sphere.center);
    phys_float a = phys_vec3_dot(direction, direction);
    phys_float b = 2.0f * phys_vec3_dot(oc, direction);
    phys_float c = phys_vec3_dot(oc, oc) - sphere.radius * sphere.radius;

    phys_float disc = b * b - 4.0f * a * c;
    if (disc < 0) return -1.0f;

    phys_float sqrt_disc = sqrtf(disc);
    phys_float t1 = (-b - sqrt_disc) / (2.0f * a);
    phys_float t2 = (-b + sqrt_disc) / (2.0f * a);

    if (t1 >= 0) return t1;
    if (t2 >= 0) return t2;
    return -1.0f;
}

phys_float phys_ray_plane(phys_vec3 origin, phys_vec3 direction,
                          phys_plane plane) {
    phys_float denom = phys_vec3_dot(direction, plane.normal);
    if (fabsf(denom) < 1e-8f) return -1.0f; /* parallel */

    phys_float t = -(phys_vec3_dot(origin, plane.normal) + plane.d) / denom;
    return t >= 0 ? t : -1.0f;
}
