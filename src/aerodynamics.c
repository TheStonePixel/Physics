#include "physics/aerodynamics.h"
#include "physics/vec.h"
#include "physics/dynamics.h"
#include <math.h>

phys_aero_params phys_aero_golf_ball_defaults(void) {
    phys_aero_params params;
    params.drag_coefficient = 0.25f;     /* golf ball with dimples */
    params.lift_coefficient = 0.15f;     /* typical for backspin */
    params.cross_section_area = 0.00143f; /* pi * (0.02135)^2 m^2 */
    params.radius = 0.02135f;            /* 42.67mm diameter */
    params.air_density = 1.225f;         /* sea level */
    params.spin_decay = 0.04f;           /* ~4% per second */
    return params;
}

phys_vec3 phys_aero_drag(phys_vec3 velocity, phys_float drag_coeff,
                         phys_float area, phys_float air_density) {
    phys_float speed = phys_vec3_length(velocity);
    if (speed < 1e-8f) return phys_vec3_zero();

    /* F_drag = -0.5 * Cd * rho * A * |v| * v */
    phys_float magnitude = 0.5f * drag_coeff * air_density * area * speed;
    return phys_vec3_scale(velocity, -magnitude / speed);
}

phys_vec3 phys_aero_magnus(phys_vec3 velocity, phys_vec3 spin_axis,
                           phys_float spin_rate, phys_float radius,
                           phys_float air_density) {
    phys_float speed = phys_vec3_length(velocity);
    if (speed < 1e-8f || spin_rate < 1e-8f) return phys_vec3_zero();

    /* Empirical Magnus/lift force for spinning spheres:
       F = 0.5 * Cl * rho * A * v^2 * lift_direction
       where Cl is derived from spin parameter S = omega*r / v.
       Typical relation for a golf ball: Cl ~ 0.54 * S (capped at ~0.4).
       lift_direction = normalize(omega x v) */
    phys_float S = spin_rate * radius / speed;
    phys_float Cl = 0.54f * S;
    if (Cl > 0.4f) Cl = 0.4f;

    phys_vec3 omega = phys_vec3_scale(spin_axis, spin_rate);
    phys_vec3 lift_dir = phys_vec3_cross(omega, velocity);
    phys_float lift_dir_len = phys_vec3_length(lift_dir);
    if (lift_dir_len < 1e-8f) return phys_vec3_zero();
    lift_dir = phys_vec3_scale(lift_dir, 1.0f / lift_dir_len);

    phys_float A = 3.14159265f * radius * radius;
    phys_float magnitude = 0.5f * Cl * air_density * A * speed * speed;
    return phys_vec3_scale(lift_dir, magnitude);
}

phys_vec3 phys_aero_total_force(phys_vec3 velocity, phys_vec3 spin_axis,
                                phys_float spin_rate,
                                phys_aero_params params) {
    phys_vec3 drag = phys_aero_drag(velocity, params.drag_coefficient,
                                    params.cross_section_area,
                                    params.air_density);
    phys_vec3 magnus = phys_aero_magnus(velocity, spin_axis, spin_rate,
                                        params.radius, params.air_density);
    return phys_vec3_add(drag, magnus);
}

void phys_aero_step(phys_body *body, phys_vec3 *spin_axis,
                    phys_float *spin_rate, phys_vec3 gravity,
                    phys_aero_params params, phys_float dt) {
    /* Apply gravity */
    phys_body_apply_gravity(body, gravity);

    /* Apply aerodynamic forces */
    phys_vec3 aero = phys_aero_total_force(body->velocity, *spin_axis,
                                           *spin_rate, params);
    phys_body_apply_force(body, aero);

    /* Integrate with Velocity Verlet */
    phys_body_step_verlet(body, dt);

    /* Decay spin */
    *spin_rate *= (1.0f - params.spin_decay * dt);
    if (*spin_rate < 0.1f) *spin_rate = 0;
}

int phys_golf_simulate(phys_vec3 pos0, phys_vec3 vel0,
                       phys_vec3 spin_axis, phys_float spin_rate,
                       phys_vec3 gravity, phys_aero_params params,
                       phys_float ground_y, phys_float dt,
                       phys_trajectory_point *out, int max_points) {
    phys_body ball;
    phys_body_init(&ball, 0.04593f); /* standard golf ball: 45.93g */
    ball.position = pos0;
    ball.velocity = vel0;
    ball.restitution = 0.6f;
    ball.friction = 0.4f;

    phys_float spin = spin_rate;
    phys_vec3 axis = spin_axis;
    int count = 0;
    phys_float time = 0;

    for (int i = 0; i < max_points; i++) {
        out[count].position = ball.position;
        out[count].velocity = ball.velocity;
        out[count].time = time;
        count++;

        if (i > 0 && ball.position.y < ground_y) break;

        phys_body_apply_gravity(&ball, gravity);
        phys_vec3 aero = phys_aero_total_force(ball.velocity, axis, spin, params);
        phys_body_apply_force(&ball, aero);
        phys_body_step_verlet(&ball, dt);

        spin *= (1.0f - params.spin_decay * dt);
        if (spin < 0.1f) spin = 0;
        time += dt;
    }
    return count;
}
