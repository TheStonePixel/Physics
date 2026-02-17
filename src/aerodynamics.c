#include "physics/aerodynamics.h"
#include "physics/vec.h"
#include "physics/dynamics.h"
#include <math.h>

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
                           phys_float lift_coeff, phys_float air_density) {
    phys_float speed = phys_vec3_length(velocity);
    if (speed < 1e-8f || spin_rate < 1e-8f) return phys_vec3_zero();

    /* Magnus/lift force for spinning spheres:
       F = 0.5 * Cl * rho * A * v^2 * lift_direction
       where Cl = lift_coeff * S, S = omega*r / v (spin parameter).
       lift_coeff is caller-provided to match surface geometry:
         ~0.5 for smooth spheres, ~0.2 for dimpled.
       lift_direction = normalize(omega x v) */
    phys_float S = spin_rate * radius / speed;
    phys_float Cl = lift_coeff * S;
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
                                        params.radius,
                                        params.lift_coefficient,
                                        params.air_density);
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

int phys_aero_simulate(phys_vec3 pos0, phys_vec3 vel0,
                       phys_vec3 spin_axis, phys_float spin_rate,
                       phys_float mass, phys_float restitution,
                       phys_float friction, phys_vec3 gravity,
                       phys_aero_params params,
                       phys_float ground_y, phys_float dt,
                       phys_trajectory_point *out, int max_points) {
    phys_body body;
    phys_body_init(&body, mass);
    body.position = pos0;
    body.velocity = vel0;
    body.restitution = restitution;
    body.friction = friction;

    phys_float spin = spin_rate;
    phys_vec3 axis = spin_axis;
    int count = 0;
    phys_float time = 0;

    for (int i = 0; i < max_points; i++) {
        out[count].position = body.position;
        out[count].velocity = body.velocity;
        out[count].time = time;
        count++;

        if (i > 0 && body.position.y < ground_y) break;

        phys_body_apply_gravity(&body, gravity);
        phys_vec3 aero = phys_aero_total_force(body.velocity, axis, spin, params);
        phys_body_apply_force(&body, aero);
        phys_body_step_verlet(&body, dt);

        spin *= (1.0f - params.spin_decay * dt);
        if (spin < 0.1f) spin = 0;
        time += dt;
    }
    return count;
}
