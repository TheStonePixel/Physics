#include "physics/dynamics.h"
#include "physics/vec.h"
#include "physics/collision.h"

void phys_body_init(phys_body *body, phys_float mass) {
    body->position = phys_vec3_zero();
    body->velocity = phys_vec3_zero();
    body->acceleration = phys_vec3_zero();
    body->force_accum = phys_vec3_zero();
    body->mass = mass;
    body->inv_mass = (mass > 0) ? 1.0f / mass : 0.0f;
    body->restitution = 0.5f;
    body->friction = 0.3f;
}

void phys_body_apply_force(phys_body *body, phys_vec3 force) {
    body->force_accum = phys_vec3_add(body->force_accum, force);
}

void phys_body_apply_gravity(phys_body *body, phys_vec3 gravity) {
    if (body->inv_mass <= 0) return; /* static body */
    phys_body_apply_force(body, phys_vec3_scale(gravity, body->mass));
}

void phys_body_clear_forces(phys_body *body) {
    body->force_accum = phys_vec3_zero();
}

void phys_body_step_euler(phys_body *body, phys_float dt) {
    if (body->inv_mass <= 0) return;

    /* a = F / m */
    body->acceleration = phys_vec3_scale(body->force_accum, body->inv_mass);

    /* Semi-implicit Euler: update velocity first, then position */
    body->velocity = phys_vec3_add(body->velocity,
                                   phys_vec3_scale(body->acceleration, dt));
    body->position = phys_vec3_add(body->position,
                                   phys_vec3_scale(body->velocity, dt));

    phys_body_clear_forces(body);
}

void phys_body_step_verlet(phys_body *body, phys_float dt) {
    if (body->inv_mass <= 0) return;

    /* Velocity Verlet:
       x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2
       a(t+dt) = F(t+dt) / m  (using accumulated forces as new acceleration)
       v(t+dt) = v(t) + 0.5*(a(t) + a(t+dt))*dt
    */
    phys_vec3 old_accel = body->acceleration;
    phys_vec3 new_accel = phys_vec3_scale(body->force_accum, body->inv_mass);

    /* Update position: x + v*dt + 0.5*a_old*dt^2 */
    body->position = phys_vec3_add(
        phys_vec3_add(body->position, phys_vec3_scale(body->velocity, dt)),
        phys_vec3_scale(old_accel, 0.5f * dt * dt)
    );

    /* Update velocity: v + 0.5*(a_old + a_new)*dt */
    body->velocity = phys_vec3_add(
        body->velocity,
        phys_vec3_scale(phys_vec3_add(old_accel, new_accel), 0.5f * dt)
    );

    body->acceleration = new_accel;
    phys_body_clear_forces(body);
}

void phys_body_resolve_plane(phys_body *body, phys_plane plane) {
    phys_sphere sphere;
    sphere.center = body->position;
    sphere.radius = 0.0f; /* treat as point */

    phys_float dist = phys_vec3_dot(body->position, plane.normal) + plane.d;
    if (dist >= 0) return; /* not penetrating */

    /* Push out */
    body->position = phys_vec3_add(body->position,
                                   phys_vec3_scale(plane.normal, -dist));

    /* Reflect velocity */
    phys_float vn = phys_vec3_dot(body->velocity, plane.normal);
    if (vn < 0) {
        phys_vec3 vn_vec = phys_vec3_scale(plane.normal, vn);
        phys_vec3 vt = phys_vec3_sub(body->velocity, vn_vec);

        /* Apply restitution to normal component, friction to tangential */
        body->velocity = phys_vec3_add(
            phys_vec3_scale(vn_vec, -body->restitution),
            phys_vec3_scale(vt, 1.0f - body->friction)
        );
    }
}

void phys_body_resolve_pair(phys_body *a, phys_body *b, phys_collision col) {
    if (!col.hit) return;
    if (a->inv_mass <= 0 && b->inv_mass <= 0) return;

    phys_float total_inv_mass = a->inv_mass + b->inv_mass;

    /* Separate bodies */
    if (a->inv_mass > 0) {
        a->position = phys_vec3_add(a->position,
            phys_vec3_scale(col.normal, col.depth * a->inv_mass / total_inv_mass));
    }
    if (b->inv_mass > 0) {
        b->position = phys_vec3_sub(b->position,
            phys_vec3_scale(col.normal, col.depth * b->inv_mass / total_inv_mass));
    }

    /* Impulse-based velocity resolution */
    phys_vec3 rel_vel = phys_vec3_sub(a->velocity, b->velocity);
    phys_float vel_along_normal = phys_vec3_dot(rel_vel, col.normal);

    if (vel_along_normal > 0) return; /* separating */

    phys_float e = (a->restitution < b->restitution) ?
                    a->restitution : b->restitution;
    phys_float j = -(1.0f + e) * vel_along_normal / total_inv_mass;

    phys_vec3 impulse = phys_vec3_scale(col.normal, j);
    a->velocity = phys_vec3_add(a->velocity,
                                phys_vec3_scale(impulse, a->inv_mass));
    b->velocity = phys_vec3_sub(b->velocity,
                                phys_vec3_scale(impulse, b->inv_mass));
}
