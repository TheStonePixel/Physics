#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "types.h"

/* Sphere vs infinite plane */
phys_collision phys_collide_sphere_plane(phys_sphere sphere, phys_plane plane);

/* Sphere vs sphere */
phys_collision phys_collide_sphere_sphere(phys_sphere a, phys_sphere b);

/* Point vs sphere (is point inside?) */
int phys_point_in_sphere(phys_vec3 point, phys_sphere sphere);

/* Point distance to plane (signed: positive = same side as normal) */
phys_float phys_point_plane_distance(phys_vec3 point, phys_plane plane);

/* Ray vs sphere. Returns t >= 0 on hit, < 0 on miss. */
phys_float phys_ray_sphere(phys_vec3 origin, phys_vec3 direction,
                           phys_sphere sphere);

/* Ray vs plane. Returns t >= 0 on hit, < 0 if parallel/behind. */
phys_float phys_ray_plane(phys_vec3 origin, phys_vec3 direction,
                          phys_plane plane);

#endif /* PHYSICS_COLLISION_H */
