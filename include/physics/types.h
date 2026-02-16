#ifndef PHYSICS_TYPES_H
#define PHYSICS_TYPES_H

#include <stdint.h>

/* --- Scalar type --- */
typedef float phys_float;

/* --- Vector types --- */
typedef struct {
    phys_float x, y;
} phys_vec2;

typedef struct {
    phys_float x, y, z;
} phys_vec3;

typedef struct {
    phys_float x, y, z, w;
} phys_vec4;

/* --- Matrix types (column-major for OpenGL compatibility) --- */
typedef struct {
    phys_float m[9]; /* 3x3, column-major */
} phys_mat3;

typedef struct {
    phys_float m[16]; /* 4x4, column-major */
} phys_mat4;

/* --- Rigid body --- */
typedef struct {
    phys_vec3 position;
    phys_vec3 velocity;
    phys_vec3 acceleration;
    phys_vec3 force_accum; /* accumulated forces this frame */
    phys_float mass;
    phys_float inv_mass;   /* 1/mass, 0 for infinite mass (static) */
    phys_float restitution;
    phys_float friction;
} phys_body;

/* --- Sphere collider --- */
typedef struct {
    phys_vec3 center;
    phys_float radius;
} phys_sphere;

/* --- Infinite plane (normal + distance from origin) --- */
typedef struct {
    phys_vec3 normal;
    phys_float d;
} phys_plane;

/* --- Collision result --- */
typedef struct {
    int hit;              /* 1 if collision, 0 otherwise */
    phys_vec3 point;      /* contact point */
    phys_vec3 normal;     /* collision normal */
    phys_float depth;     /* penetration depth */
} phys_collision;

/* --- Aerodynamic parameters --- */
typedef struct {
    phys_float drag_coefficient;   /* Cd */
    phys_float lift_coefficient;   /* Cl */
    phys_float cross_section_area; /* m^2 */
    phys_float radius;             /* for Magnus effect */
    phys_float air_density;        /* kg/m^3, ~1.225 at sea level */
    phys_float spin_decay;         /* spin decay rate per second */
} phys_aero_params;

/* --- Trajectory sample point --- */
typedef struct {
    phys_vec3 position;
    phys_vec3 velocity;
    phys_float time;
} phys_trajectory_point;

/* --- Surface definition --- */
typedef struct {
    phys_float rolling_friction;  /* rolling resistance coefficient */
    phys_float restitution;       /* bounce energy retained (0=dead, 1=perfect) */
    phys_float firmness;          /* 0=soft/absorptive, 1=hard/reflective */
    phys_vec3 normal;             /* surface normal (default: 0,1,0 = flat) */
} phys_surface;

/* --- Rolling simulation point --- */
typedef struct {
    phys_vec3 position;
    phys_vec3 velocity;
    phys_float spin_rate;
    phys_float time;
} phys_rolling_point;

#endif /* PHYSICS_TYPES_H */
