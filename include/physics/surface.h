#ifndef PHYSICS_SURFACE_H
#define PHYSICS_SURFACE_H

#include "types.h"

/* Create a flat surface with the given properties.
   Normal defaults to (0, 1, 0). */
phys_surface phys_surface_create(phys_float rolling_friction,
                                 phys_float restitution,
                                 phys_float firmness);

/* Create a sloped surface. normal should be normalized. */
phys_surface phys_surface_create_sloped(phys_float rolling_friction,
                                        phys_float restitution,
                                        phys_float firmness,
                                        phys_vec3 normal);

/* Compute bounce response.
   Given incoming velocity, spin axis/rate, body radius, and surface,
   writes the post-bounce velocity, spin axis, and spin rate.
   Returns 1 if the ball should keep bouncing, 0 if energy is exhausted. */
int phys_surface_bounce(phys_vec3 vel_in, phys_vec3 spin_axis_in,
                        phys_float spin_rate_in, phys_float radius,
                        phys_surface surface,
                        phys_vec3 *vel_out, phys_vec3 *spin_axis_out,
                        phys_float *spin_rate_out);

/* Advance one rolling step on a surface.
   Decelerates the body via rolling friction and slope gravity.
   Updates velocity, position, and spin_rate.
   Returns 1 if still rolling, 0 if stopped. */
int phys_surface_roll_step(phys_vec3 *position, phys_vec3 *velocity,
                           phys_float *spin_rate, phys_float radius,
                           phys_float mass, phys_vec3 gravity,
                           phys_surface surface, phys_float dt);

/* Simulate full ground interaction: bouncing until settled, then rolling
   until stopped. Writes rolling points to `out`.
   Returns number of points written (<= max_points). */
int phys_surface_simulate_roll(phys_vec3 pos0, phys_vec3 vel0,
                               phys_vec3 spin_axis, phys_float spin_rate,
                               phys_float radius, phys_float mass,
                               phys_vec3 gravity, phys_surface surface,
                               phys_float dt,
                               phys_rolling_point *out, int max_points);

#endif /* PHYSICS_SURFACE_H */
