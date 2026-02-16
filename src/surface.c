#include "physics/surface.h"
#include "physics/vec.h"
#include <math.h>

/* Minimum speed below which we consider the ball stopped */
#define STOP_SPEED 0.15f
/* Minimum bounce velocity to keep bouncing */
#define MIN_BOUNCE_VEL 0.3f

phys_surface phys_surface_create(phys_float rolling_friction,
                                 phys_float restitution,
                                 phys_float firmness) {
    phys_surface s;
    s.rolling_friction = rolling_friction;
    s.restitution = restitution;
    s.firmness = firmness;
    s.normal = (phys_vec3){0, 1, 0};
    return s;
}

phys_surface phys_surface_create_sloped(phys_float rolling_friction,
                                        phys_float restitution,
                                        phys_float firmness,
                                        phys_vec3 normal) {
    phys_surface s;
    s.rolling_friction = rolling_friction;
    s.restitution = restitution;
    s.firmness = firmness;
    s.normal = phys_vec3_normalize(normal);
    return s;
}

int phys_surface_bounce(phys_vec3 vel_in, phys_vec3 spin_axis_in,
                        phys_float spin_rate_in, phys_float radius,
                        phys_surface surface,
                        phys_vec3 *vel_out, phys_vec3 *spin_axis_out,
                        phys_float *spin_rate_out) {
    phys_vec3 n = surface.normal;

    /* Decompose velocity into normal and tangential components */
    phys_float v_normal = phys_vec3_dot(vel_in, n);
    phys_vec3 v_n = phys_vec3_scale(n, v_normal);
    phys_vec3 v_t = phys_vec3_sub(vel_in, v_n);

    /* Reflect normal component with restitution dampening */
    phys_float bounce_restitution = surface.restitution;
    phys_vec3 v_n_out = phys_vec3_scale(n, -v_normal * bounce_restitution);

    /* Impact friction: the normal impulse creates a friction force that
       decelerates tangential velocity. Stronger impacts (higher v_normal)
       and softer surfaces (lower firmness) absorb more horizontal energy.
       friction_ratio = mu_impact * |v_normal| / |v_tangential| clamped to [0,1] */
    phys_float v_t_speed = phys_vec3_length(v_t);
    if (v_t_speed > 0.01f) {
        phys_float mu_impact = 0.4f + 0.3f * (1.0f - surface.firmness);
        phys_float impulse_ratio = mu_impact * fabsf(v_normal) / v_t_speed;
        if (impulse_ratio > 0.8f) impulse_ratio = 0.8f;
        v_t = phys_vec3_scale(v_t, 1.0f - impulse_ratio);
    }

    /* Spin-surface interaction:
       Backspin creates a friction force at the contact point that opposes
       forward motion. spin_velocity = omega x r_contact, where
       r_contact = -radius * normal. Firmness controls transfer efficiency. */
    phys_vec3 r_contact = phys_vec3_scale(n, -radius);
    phys_vec3 omega = phys_vec3_scale(spin_axis_in, spin_rate_in);
    phys_vec3 spin_vel = phys_vec3_cross(omega, r_contact);

    phys_float spin_factor = surface.firmness * 0.4f;
    v_t = phys_vec3_add(v_t, phys_vec3_scale(spin_vel, spin_factor));

    /* Reduce spin on bounce — surface contact eats some angular momentum */
    phys_float spin_loss = 0.6f + 0.3f * surface.firmness;
    *spin_rate_out = spin_rate_in * (1.0f - spin_loss);
    if (*spin_rate_out < 0.1f) *spin_rate_out = 0;
    *spin_axis_out = spin_axis_in;

    /* Combine */
    *vel_out = phys_vec3_add(v_n_out, v_t);

    /* Energy cap: total post-bounce speed cannot exceed restitution * pre-bounce
       speed. This models deformation energy loss across all components, not just
       the normal direction. Without this, shallow-angle impacts retain too much
       tangential energy (a rigid-body artifact). */
    phys_float pre_speed = phys_vec3_length(vel_in);
    phys_float post_speed = phys_vec3_length(*vel_out);
    phys_float max_post_speed = pre_speed * bounce_restitution;
    if (post_speed > max_post_speed && post_speed > 0.01f) {
        *vel_out = phys_vec3_scale(*vel_out, max_post_speed / post_speed);
    }

    /* Check if there's enough energy to keep bouncing */
    phys_float bounce_speed = fabsf(v_normal * bounce_restitution);
    return bounce_speed > MIN_BOUNCE_VEL ? 1 : 0;
}

int phys_surface_roll_step(phys_vec3 *position, phys_vec3 *velocity,
                           phys_float *spin_rate, phys_float radius,
                           phys_float mass, phys_vec3 gravity,
                           phys_surface surface, phys_float dt) {
    phys_vec3 n = surface.normal;
    (void)mass; /* mass cancels out in rolling friction: a = mu * g */

    phys_float speed = phys_vec3_length(*velocity);
    if (speed < STOP_SPEED && *spin_rate < 0.1f) return 0;

    /* Gravity component along the surface (slope effect):
       g_tangent = gravity - (gravity . n) * n */
    phys_float g_dot_n = phys_vec3_dot(gravity, n);
    phys_vec3 g_tangent = phys_vec3_sub(gravity, phys_vec3_scale(n, g_dot_n));

    /* Rolling friction deceleration: opposes velocity direction */
    phys_float g_mag = phys_vec3_length(gravity);
    phys_vec3 friction_accel = phys_vec3_zero();
    if (speed > STOP_SPEED) {
        phys_vec3 dir = phys_vec3_scale(*velocity, 1.0f / speed);
        friction_accel = phys_vec3_scale(dir,
                                         -surface.rolling_friction * g_mag);
    }

    /* Total acceleration = slope gravity + rolling friction */
    phys_vec3 accel = phys_vec3_add(g_tangent, friction_accel);

    /* Integrate velocity */
    *velocity = phys_vec3_add(*velocity, phys_vec3_scale(accel, dt));

    /* Clamp: if friction reversed the velocity direction, stop */
    phys_float new_speed = phys_vec3_length(*velocity);
    if (speed > STOP_SPEED && new_speed > speed && phys_vec3_length(g_tangent) < 0.01f) {
        /* Friction can't accelerate — ball has stopped on flat ground */
        *velocity = phys_vec3_zero();
        *spin_rate = 0;
        return 0;
    }

    /* Integrate position */
    *position = phys_vec3_add(*position, phys_vec3_scale(*velocity, dt));

    /* Keep ball on surface plane (project back) */
    phys_float height = phys_vec3_dot(*position, n);
    if (height < 0) {
        *position = phys_vec3_sub(*position, phys_vec3_scale(n, height));
    }

    /* Decay spin during roll (surface contact kills spin faster than air) */
    *spin_rate *= (1.0f - 0.15f * dt);
    if (*spin_rate < 0.1f) *spin_rate = 0;

    return (new_speed > STOP_SPEED || phys_vec3_length(g_tangent) > 0.01f) ? 1 : 0;
}

int phys_surface_simulate_roll(phys_vec3 pos0, phys_vec3 vel0,
                               phys_vec3 spin_axis, phys_float spin_rate,
                               phys_float radius, phys_float mass,
                               phys_vec3 gravity, phys_surface surface,
                               phys_float dt,
                               phys_rolling_point *out, int max_points) {
    phys_vec3 pos = pos0;
    phys_vec3 vel = vel0;
    phys_float spin = spin_rate;
    phys_vec3 s_axis = spin_axis;
    phys_float time = 0;
    int count = 0;

    /* Phase 1: Bouncing — ball hits surface and bounces until energy dies */
    int bouncing = 1;
    while (bouncing && count < max_points) {
        /* Record landing point */
        out[count].position = pos;
        out[count].velocity = vel;
        out[count].spin_rate = spin;
        out[count].time = time;
        count++;

        /* Compute bounce */
        phys_vec3 vel_after, axis_after;
        phys_float spin_after;
        bouncing = phys_surface_bounce(vel, s_axis, spin, radius, surface,
                                       &vel_after, &axis_after, &spin_after);

        vel = vel_after;
        s_axis = axis_after;
        spin = spin_after;

        if (!bouncing) break;

        /* Simulate sub-flight of this bounce (parabolic arc) */
        phys_vec3 bounce_pos = pos;
        phys_vec3 bounce_vel = vel;
        phys_float sub_time = 0;
        phys_vec3 n = surface.normal;

        for (int i = 0; i < 2000 && count < max_points; i++) {
            /* Simple Euler for short bounces */
            bounce_vel = phys_vec3_add(bounce_vel,
                                       phys_vec3_scale(gravity, dt));
            bounce_pos = phys_vec3_add(bounce_pos,
                                       phys_vec3_scale(bounce_vel, dt));
            sub_time += dt;
            time += dt;

            /* Check if ball is below surface */
            phys_float height = phys_vec3_dot(bounce_pos, n);
            if (height <= 0 && i > 2) {
                /* Clamp to surface */
                bounce_pos = phys_vec3_sub(bounce_pos,
                                           phys_vec3_scale(n, height));
                pos = bounce_pos;
                vel = bounce_vel;

                out[count].position = pos;
                out[count].velocity = vel;
                out[count].spin_rate = spin;
                out[count].time = time;
                count++;
                break;
            }
        }
    }

    /* Strip the normal component from velocity for rolling */
    phys_vec3 n = surface.normal;
    phys_float v_dot_n = phys_vec3_dot(vel, n);
    vel = phys_vec3_sub(vel, phys_vec3_scale(n, v_dot_n));

    /* Phase 2: Rolling — ball decelerates along the surface */
    int rolling = 1;
    while (rolling && count < max_points) {
        out[count].position = pos;
        out[count].velocity = vel;
        out[count].spin_rate = spin;
        out[count].time = time;
        count++;

        rolling = phys_surface_roll_step(&pos, &vel, &spin, radius,
                                         mass, gravity, surface, dt);
        time += dt;
    }

    /* Record final resting point */
    if (count < max_points) {
        out[count].position = pos;
        out[count].velocity = phys_vec3_zero();
        out[count].spin_rate = 0;
        out[count].time = time;
        count++;
    }

    return count;
}
