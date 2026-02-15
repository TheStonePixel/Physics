#ifndef PHYSICS_MAT_H
#define PHYSICS_MAT_H

#include "types.h"
#include "vec.h"

/* ---- mat4 ---- */
phys_mat4 phys_mat4_identity(void);
phys_mat4 phys_mat4_multiply(phys_mat4 a, phys_mat4 b);
phys_vec4 phys_mat4_mul_vec4(phys_mat4 m, phys_vec4 v);
phys_mat4 phys_mat4_translate(phys_vec3 t);
phys_mat4 phys_mat4_scale(phys_vec3 s);
phys_mat4 phys_mat4_rotate_x(phys_float radians);
phys_mat4 phys_mat4_rotate_y(phys_float radians);
phys_mat4 phys_mat4_rotate_z(phys_float radians);
phys_mat4 phys_mat4_perspective(phys_float fov_y, phys_float aspect,
                                phys_float near, phys_float far);
phys_mat4 phys_mat4_ortho(phys_float left, phys_float right,
                          phys_float bottom, phys_float top,
                          phys_float near, phys_float far);
phys_mat4 phys_mat4_look_at(phys_vec3 eye, phys_vec3 center, phys_vec3 up);
phys_mat4 phys_mat4_transpose(phys_mat4 m);

/* ---- mat3 ---- */
phys_mat3 phys_mat3_identity(void);
phys_mat3 phys_mat3_multiply(phys_mat3 a, phys_mat3 b);
phys_vec3 phys_mat3_mul_vec3(phys_mat3 m, phys_vec3 v);
phys_mat3 phys_mat3_from_mat4(phys_mat4 m);

#endif /* PHYSICS_MAT_H */
