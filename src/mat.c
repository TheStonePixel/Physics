#include "physics/mat.h"
#include <math.h>
#include <string.h>

/* Column-major indexing: element at row r, column c = m[c*4 + r] (mat4)
   or m[c*3 + r] (mat3) */

/* ---- mat4 ---- */

phys_mat4 phys_mat4_identity(void) {
    phys_mat4 result;
    memset(&result, 0, sizeof(result));
    result.m[0]  = 1.0f;
    result.m[5]  = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;
    return result;
}

phys_mat4 phys_mat4_multiply(phys_mat4 a, phys_mat4 b) {
    phys_mat4 result;
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            phys_float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += a.m[k * 4 + r] * b.m[c * 4 + k];
            }
            result.m[c * 4 + r] = sum;
        }
    }
    return result;
}

phys_vec4 phys_mat4_mul_vec4(phys_mat4 m, phys_vec4 v) {
    phys_vec4 result;
    result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8]  * v.z + m.m[12] * v.w;
    result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9]  * v.z + m.m[13] * v.w;
    result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w;
    result.w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w;
    return result;
}

phys_mat4 phys_mat4_translate(phys_vec3 t) {
    phys_mat4 result = phys_mat4_identity();
    result.m[12] = t.x;
    result.m[13] = t.y;
    result.m[14] = t.z;
    return result;
}

phys_mat4 phys_mat4_scale(phys_vec3 s) {
    phys_mat4 result = phys_mat4_identity();
    result.m[0]  = s.x;
    result.m[5]  = s.y;
    result.m[10] = s.z;
    return result;
}

phys_mat4 phys_mat4_rotate_x(phys_float radians) {
    phys_mat4 result = phys_mat4_identity();
    phys_float c = cosf(radians);
    phys_float s = sinf(radians);
    result.m[5]  =  c;
    result.m[6]  =  s;
    result.m[9]  = -s;
    result.m[10] =  c;
    return result;
}

phys_mat4 phys_mat4_rotate_y(phys_float radians) {
    phys_mat4 result = phys_mat4_identity();
    phys_float c = cosf(radians);
    phys_float s = sinf(radians);
    result.m[0]  =  c;
    result.m[2]  = -s;
    result.m[8]  =  s;
    result.m[10] =  c;
    return result;
}

phys_mat4 phys_mat4_rotate_z(phys_float radians) {
    phys_mat4 result = phys_mat4_identity();
    phys_float c = cosf(radians);
    phys_float s = sinf(radians);
    result.m[0] =  c;
    result.m[1] =  s;
    result.m[4] = -s;
    result.m[5] =  c;
    return result;
}

phys_mat4 phys_mat4_perspective(phys_float fov_y, phys_float aspect,
                                phys_float near, phys_float far) {
    phys_mat4 result;
    memset(&result, 0, sizeof(result));
    phys_float tan_half = tanf(fov_y * 0.5f);
    result.m[0]  = 1.0f / (aspect * tan_half);
    result.m[5]  = 1.0f / tan_half;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    return result;
}

phys_mat4 phys_mat4_ortho(phys_float left, phys_float right,
                          phys_float bottom, phys_float top,
                          phys_float near, phys_float far) {
    phys_mat4 result;
    memset(&result, 0, sizeof(result));
    result.m[0]  =  2.0f / (right - left);
    result.m[5]  =  2.0f / (top - bottom);
    result.m[10] = -2.0f / (far - near);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -(far + near) / (far - near);
    result.m[15] =  1.0f;
    return result;
}

phys_mat4 phys_mat4_look_at(phys_vec3 eye, phys_vec3 center, phys_vec3 up) {
    phys_vec3 f = phys_vec3_normalize(phys_vec3_sub(center, eye));
    phys_vec3 s = phys_vec3_normalize(phys_vec3_cross(f, up));
    phys_vec3 u = phys_vec3_cross(s, f);

    phys_mat4 result = phys_mat4_identity();
    result.m[0]  =  s.x;
    result.m[4]  =  s.y;
    result.m[8]  =  s.z;
    result.m[1]  =  u.x;
    result.m[5]  =  u.y;
    result.m[9]  =  u.z;
    result.m[2]  = -f.x;
    result.m[6]  = -f.y;
    result.m[10] = -f.z;
    result.m[12] = -phys_vec3_dot(s, eye);
    result.m[13] = -phys_vec3_dot(u, eye);
    result.m[14] =  phys_vec3_dot(f, eye);
    return result;
}

phys_mat4 phys_mat4_transpose(phys_mat4 m) {
    phys_mat4 result;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            result.m[c * 4 + r] = m.m[r * 4 + c];
    return result;
}

/* ---- mat3 ---- */

phys_mat3 phys_mat3_identity(void) {
    phys_mat3 result;
    memset(&result, 0, sizeof(result));
    result.m[0] = 1.0f;
    result.m[4] = 1.0f;
    result.m[8] = 1.0f;
    return result;
}

phys_mat3 phys_mat3_multiply(phys_mat3 a, phys_mat3 b) {
    phys_mat3 result;
    for (int c = 0; c < 3; c++) {
        for (int r = 0; r < 3; r++) {
            phys_float sum = 0.0f;
            for (int k = 0; k < 3; k++) {
                sum += a.m[k * 3 + r] * b.m[c * 3 + k];
            }
            result.m[c * 3 + r] = sum;
        }
    }
    return result;
}

phys_vec3 phys_mat3_mul_vec3(phys_mat3 m, phys_vec3 v) {
    phys_vec3 result;
    result.x = m.m[0] * v.x + m.m[3] * v.y + m.m[6] * v.z;
    result.y = m.m[1] * v.x + m.m[4] * v.y + m.m[7] * v.z;
    result.z = m.m[2] * v.x + m.m[5] * v.y + m.m[8] * v.z;
    return result;
}

phys_mat3 phys_mat3_from_mat4(phys_mat4 m) {
    phys_mat3 result;
    result.m[0] = m.m[0]; result.m[1] = m.m[1]; result.m[2] = m.m[2];
    result.m[3] = m.m[4]; result.m[4] = m.m[5]; result.m[5] = m.m[6];
    result.m[6] = m.m[8]; result.m[7] = m.m[9]; result.m[8] = m.m[10];
    return result;
}
