#ifndef PHYSICS_KINEMATICS_H
#define PHYSICS_KINEMATICS_H

#include "types.h"

/* Projectile motion (no drag, constant gravity) */
phys_vec3 phys_projectile_position(phys_vec3 pos0, phys_vec3 vel0,
                                   phys_vec3 gravity, phys_float t);
phys_vec3 phys_projectile_velocity(phys_vec3 vel0, phys_vec3 gravity,
                                   phys_float t);

/* Time for projectile to reach ground plane (y=ground_y), returns <0 if never */
phys_float phys_projectile_time_to_y(phys_vec3 pos0, phys_vec3 vel0,
                                     phys_float gravity_y, phys_float ground_y);

/* Maximum height reached by a projectile */
phys_float phys_projectile_max_height(phys_vec3 pos0, phys_vec3 vel0,
                                      phys_float gravity_y);

/* Horizontal range on flat ground (y = pos0.y) */
phys_float phys_projectile_range(phys_vec3 vel0, phys_float gravity_y);

/* Sample a full trajectory into an array of points.
   Returns number of points written (<= max_points).
   Stops when position.y < ground_y. */
int phys_trajectory_sample(phys_vec3 pos0, phys_vec3 vel0, phys_vec3 gravity,
                           phys_float ground_y, phys_float dt,
                           phys_trajectory_point *out, int max_points);

#endif /* PHYSICS_KINEMATICS_H */
