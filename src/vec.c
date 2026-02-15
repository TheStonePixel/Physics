#include "physics/vec.h"
#include <math.h>

/* ---- vec2 ---- */

phys_vec2 phys_vec2_create(phys_float x, phys_float y) {
    return (phys_vec2){x, y};
}

phys_vec2 phys_vec2_add(phys_vec2 a, phys_vec2 b) {
    return (phys_vec2){a.x + b.x, a.y + b.y};
}

phys_vec2 phys_vec2_sub(phys_vec2 a, phys_vec2 b) {
    return (phys_vec2){a.x - b.x, a.y - b.y};
}

phys_vec2 phys_vec2_scale(phys_vec2 v, phys_float s) {
    return (phys_vec2){v.x * s, v.y * s};
}

phys_float phys_vec2_dot(phys_vec2 a, phys_vec2 b) {
    return a.x * b.x + a.y * b.y;
}

phys_float phys_vec2_length(phys_vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

phys_float phys_vec2_length_sq(phys_vec2 v) {
    return v.x * v.x + v.y * v.y;
}

phys_vec2 phys_vec2_normalize(phys_vec2 v) {
    phys_float len = phys_vec2_length(v);
    if (len < 1e-8f) return (phys_vec2){0, 0};
    phys_float inv = 1.0f / len;
    return (phys_vec2){v.x * inv, v.y * inv};
}

phys_vec2 phys_vec2_negate(phys_vec2 v) {
    return (phys_vec2){-v.x, -v.y};
}

phys_vec2 phys_vec2_lerp(phys_vec2 a, phys_vec2 b, phys_float t) {
    return (phys_vec2){a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

/* ---- vec3 ---- */

phys_vec3 phys_vec3_create(phys_float x, phys_float y, phys_float z) {
    return (phys_vec3){x, y, z};
}

phys_vec3 phys_vec3_zero(void) {
    return (phys_vec3){0, 0, 0};
}

phys_vec3 phys_vec3_add(phys_vec3 a, phys_vec3 b) {
    return (phys_vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

phys_vec3 phys_vec3_sub(phys_vec3 a, phys_vec3 b) {
    return (phys_vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

phys_vec3 phys_vec3_scale(phys_vec3 v, phys_float s) {
    return (phys_vec3){v.x * s, v.y * s, v.z * s};
}

phys_float phys_vec3_dot(phys_vec3 a, phys_vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

phys_vec3 phys_vec3_cross(phys_vec3 a, phys_vec3 b) {
    return (phys_vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

phys_float phys_vec3_length(phys_vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

phys_float phys_vec3_length_sq(phys_vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

phys_vec3 phys_vec3_normalize(phys_vec3 v) {
    phys_float len = phys_vec3_length(v);
    if (len < 1e-8f) return (phys_vec3){0, 0, 0};
    phys_float inv = 1.0f / len;
    return (phys_vec3){v.x * inv, v.y * inv, v.z * inv};
}

phys_vec3 phys_vec3_negate(phys_vec3 v) {
    return (phys_vec3){-v.x, -v.y, -v.z};
}

phys_vec3 phys_vec3_lerp(phys_vec3 a, phys_vec3 b, phys_float t) {
    return (phys_vec3){
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
}

phys_float phys_vec3_distance(phys_vec3 a, phys_vec3 b) {
    return phys_vec3_length(phys_vec3_sub(a, b));
}

/* ---- vec4 ---- */

phys_vec4 phys_vec4_create(phys_float x, phys_float y, phys_float z, phys_float w) {
    return (phys_vec4){x, y, z, w};
}

phys_vec4 phys_vec4_add(phys_vec4 a, phys_vec4 b) {
    return (phys_vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

phys_vec4 phys_vec4_sub(phys_vec4 a, phys_vec4 b) {
    return (phys_vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

phys_vec4 phys_vec4_scale(phys_vec4 v, phys_float s) {
    return (phys_vec4){v.x * s, v.y * s, v.z * s, v.w * s};
}

phys_float phys_vec4_dot(phys_vec4 a, phys_vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

phys_float phys_vec4_length(phys_vec4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

phys_vec4 phys_vec4_normalize(phys_vec4 v) {
    phys_float len = phys_vec4_length(v);
    if (len < 1e-8f) return (phys_vec4){0, 0, 0, 0};
    phys_float inv = 1.0f / len;
    return (phys_vec4){v.x * inv, v.y * inv, v.z * inv, v.w * inv};
}
