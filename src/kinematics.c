#include "physics/kinematics.h"
#include "physics/vec.h"
#include <math.h>

phys_vec3 phys_projectile_position(phys_vec3 pos0, phys_vec3 vel0,
                                   phys_vec3 gravity, phys_float t) {
    /* p = p0 + v0*t + 0.5*g*t^2 */
    return phys_vec3_add(
        phys_vec3_add(pos0, phys_vec3_scale(vel0, t)),
        phys_vec3_scale(gravity, 0.5f * t * t)
    );
}

phys_vec3 phys_projectile_velocity(phys_vec3 vel0, phys_vec3 gravity,
                                   phys_float t) {
    /* v = v0 + g*t */
    return phys_vec3_add(vel0, phys_vec3_scale(gravity, t));
}

phys_float phys_projectile_time_to_y(phys_vec3 pos0, phys_vec3 vel0,
                                     phys_float gravity_y, phys_float ground_y) {
    /* Solve: pos0.y + vel0.y*t + 0.5*g*t^2 = ground_y
       0.5*g*t^2 + vel0.y*t + (pos0.y - ground_y) = 0 */
    phys_float a = 0.5f * gravity_y;
    phys_float b = vel0.y;
    phys_float c = pos0.y - ground_y;

    if (fabsf(a) < 1e-8f) {
        /* Linear case */
        if (fabsf(b) < 1e-8f) return -1.0f;
        phys_float t = -c / b;
        return t > 0 ? t : -1.0f;
    }

    phys_float disc = b * b - 4.0f * a * c;
    if (disc < 0) return -1.0f;

    phys_float sqrt_disc = sqrtf(disc);
    phys_float t1 = (-b + sqrt_disc) / (2.0f * a);
    phys_float t2 = (-b - sqrt_disc) / (2.0f * a);

    /* Return the smallest positive time */
    if (t1 > 0 && t2 > 0) return t1 < t2 ? t1 : t2;
    if (t1 > 0) return t1;
    if (t2 > 0) return t2;
    return -1.0f;
}

phys_float phys_projectile_max_height(phys_vec3 pos0, phys_vec3 vel0,
                                      phys_float gravity_y) {
    if (gravity_y >= 0 || vel0.y <= 0) return pos0.y;
    /* t_peak = -vy / g */
    phys_float t_peak = -vel0.y / gravity_y;
    return pos0.y + vel0.y * t_peak + 0.5f * gravity_y * t_peak * t_peak;
}

phys_float phys_projectile_range(phys_vec3 vel0, phys_float gravity_y) {
    if (fabsf(gravity_y) < 1e-8f) return -1.0f; /* no gravity = infinite range */
    /* range = |v0_xz|^2 * sin(2*theta) / |g|
       but simpler: 2 * v0.y * v0_horiz / |g| * horizontal_dir
       Horizontal speed: */
    phys_float v_horiz = sqrtf(vel0.x * vel0.x + vel0.z * vel0.z);
    /* Time of flight: 2 * v0.y / |g| */
    phys_float t_flight = -2.0f * vel0.y / gravity_y;
    if (t_flight <= 0) return 0;
    return v_horiz * t_flight;
}

int phys_trajectory_sample(phys_vec3 pos0, phys_vec3 vel0, phys_vec3 gravity,
                           phys_float ground_y, phys_float dt,
                           phys_trajectory_point *out, int max_points) {
    int count = 0;
    phys_float t = 0;
    for (int i = 0; i < max_points; i++) {
        phys_vec3 pos = phys_projectile_position(pos0, vel0, gravity, t);
        if (i > 0 && pos.y < ground_y) break;
        out[count].position = pos;
        out[count].velocity = phys_projectile_velocity(vel0, gravity, t);
        out[count].time = t;
        count++;
        t += dt;
    }
    return count;
}
