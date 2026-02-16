#ifndef PHYSICS_AERODYNAMICS_H
#define PHYSICS_AERODYNAMICS_H

#include "types.h"

/* Aerodynamic drag force: -0.5 * Cd * rho * A * |v| * v */
phys_vec3 phys_aero_drag(phys_vec3 velocity, phys_float drag_coeff,
                         phys_float area, phys_float air_density);

/* Magnus force (spin-induced lateral deflection):
   F = 0.5 * Cl * rho * A * (spin x velocity) / |spin| * |velocity|
   simplified: proportional to cross(spin, velocity) */
phys_vec3 phys_aero_magnus(phys_vec3 velocity, phys_vec3 spin_axis,
                           phys_float spin_rate, phys_float radius,
                           phys_float air_density);

/* Combined aerodynamic force (drag + Magnus) using param struct */
phys_vec3 phys_aero_total_force(phys_vec3 velocity, phys_vec3 spin_axis,
                                phys_float spin_rate,
                                phys_aero_params params);

/* Step a body with full aerodynamics + gravity.
   Updates position, velocity, and spin_rate (spin decays over time).
   Uses Velocity Verlet integration. */
void phys_aero_step(phys_body *body, phys_vec3 *spin_axis,
                    phys_float *spin_rate, phys_vec3 gravity,
                    phys_aero_params params, phys_float dt);

/* Simulate a full aerodynamic trajectory for a spinning body.
   Returns number of points written (<= max_points).
   Stops when position.y < ground_y. */
int phys_aero_simulate(phys_vec3 pos0, phys_vec3 vel0,
                       phys_vec3 spin_axis, phys_float spin_rate,
                       phys_float mass, phys_float restitution,
                       phys_float friction, phys_vec3 gravity,
                       phys_aero_params params,
                       phys_float ground_y, phys_float dt,
                       phys_trajectory_point *out, int max_points);

#endif /* PHYSICS_AERODYNAMICS_H */
