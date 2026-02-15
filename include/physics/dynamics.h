#ifndef PHYSICS_DYNAMICS_H
#define PHYSICS_DYNAMICS_H

#include "types.h"

/* Initialize a body with given mass. Sets inv_mass, zeroes forces. */
void phys_body_init(phys_body *body, phys_float mass);

/* Accumulate a force on the body (cleared each step) */
void phys_body_apply_force(phys_body *body, phys_vec3 force);

/* Apply gravity (convenience: adds mass * gravity to force accumulator) */
void phys_body_apply_gravity(phys_body *body, phys_vec3 gravity);

/* Clear accumulated forces */
void phys_body_clear_forces(phys_body *body);

/* Semi-implicit Euler integration step */
void phys_body_step_euler(phys_body *body, phys_float dt);

/* Velocity Verlet integration step (more accurate for ballistics) */
void phys_body_step_verlet(phys_body *body, phys_float dt);

/* Resolve collision between body and a static plane */
void phys_body_resolve_plane(phys_body *body, phys_plane plane);

/* Resolve collision between two bodies */
void phys_body_resolve_pair(phys_body *a, phys_body *b,
                            phys_collision col);

#endif /* PHYSICS_DYNAMICS_H */
