#ifndef PHYSICS_VEC_H
#define PHYSICS_VEC_H

#include "types.h"

/* ---- vec2 ---- */
phys_vec2 phys_vec2_create(phys_float x, phys_float y);
phys_vec2 phys_vec2_add(phys_vec2 a, phys_vec2 b);
phys_vec2 phys_vec2_sub(phys_vec2 a, phys_vec2 b);
phys_vec2 phys_vec2_scale(phys_vec2 v, phys_float s);
phys_float phys_vec2_dot(phys_vec2 a, phys_vec2 b);
phys_float phys_vec2_length(phys_vec2 v);
phys_float phys_vec2_length_sq(phys_vec2 v);
phys_vec2 phys_vec2_normalize(phys_vec2 v);
phys_vec2 phys_vec2_negate(phys_vec2 v);
phys_vec2 phys_vec2_lerp(phys_vec2 a, phys_vec2 b, phys_float t);

/* ---- vec3 ---- */
phys_vec3 phys_vec3_create(phys_float x, phys_float y, phys_float z);
phys_vec3 phys_vec3_zero(void);
phys_vec3 phys_vec3_add(phys_vec3 a, phys_vec3 b);
phys_vec3 phys_vec3_sub(phys_vec3 a, phys_vec3 b);
phys_vec3 phys_vec3_scale(phys_vec3 v, phys_float s);
phys_float phys_vec3_dot(phys_vec3 a, phys_vec3 b);
phys_vec3 phys_vec3_cross(phys_vec3 a, phys_vec3 b);
phys_float phys_vec3_length(phys_vec3 v);
phys_float phys_vec3_length_sq(phys_vec3 v);
phys_vec3 phys_vec3_normalize(phys_vec3 v);
phys_vec3 phys_vec3_negate(phys_vec3 v);
phys_vec3 phys_vec3_lerp(phys_vec3 a, phys_vec3 b, phys_float t);
phys_float phys_vec3_distance(phys_vec3 a, phys_vec3 b);

/* ---- vec4 ---- */
phys_vec4 phys_vec4_create(phys_float x, phys_float y, phys_float z, phys_float w);
phys_vec4 phys_vec4_add(phys_vec4 a, phys_vec4 b);
phys_vec4 phys_vec4_sub(phys_vec4 a, phys_vec4 b);
phys_vec4 phys_vec4_scale(phys_vec4 v, phys_float s);
phys_float phys_vec4_dot(phys_vec4 a, phys_vec4 b);
phys_float phys_vec4_length(phys_vec4 v);
phys_vec4 phys_vec4_normalize(phys_vec4 v);

#endif /* PHYSICS_VEC_H */
