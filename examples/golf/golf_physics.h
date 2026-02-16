#ifndef GOLF_PHYSICS_H
#define GOLF_PHYSICS_H

#include "physics/physics.h"

/*
 * Golf Physics — domain-specific layer built on top of the generic physics
 * library. Provides standard golf ball parameters, club presets, and
 * convenience functions for simulating golf shots.
 *
 * This serves as both a practical utility and a showcase of the library's
 * aerodynamic simulation accuracy against real-world golf data.
 */

/* --- Standard golf ball constants --- */
#define GOLF_BALL_MASS       0.04593f  /* 45.93 g (USGA max) */
#define GOLF_BALL_DIAMETER   0.04267f  /* 42.67 mm (USGA min) */
#define GOLF_BALL_RADIUS     0.02135f  /* half diameter */
#define GOLF_BALL_AREA       0.00143f  /* pi * radius^2 */
#define GOLF_BALL_RESTITUTION 0.6f
#define GOLF_BALL_FRICTION   0.4f

/* --- Club types --- */
typedef enum {
    GOLF_CLUB_DRIVER,
    GOLF_CLUB_3_WOOD,
    GOLF_CLUB_5_IRON,
    GOLF_CLUB_7_IRON,
    GOLF_CLUB_9_IRON,
    GOLF_CLUB_PW,
    GOLF_CLUB_SW,
    GOLF_CLUB_COUNT
} golf_club_type;

/* --- Club preset data --- */
typedef struct {
    const char *name;
    phys_float ball_speed;    /* m/s — typical ball speed off the face */
    phys_float launch_angle;  /* degrees */
    phys_float spin_rate;     /* rad/s */
} golf_club_preset;

/* Get preset data for a club type */
golf_club_preset golf_get_club_preset(golf_club_type club);

/* Get standard aerodynamic params for a golf ball (dimpled sphere at sea level) */
phys_aero_params golf_ball_aero_params(void);

/* Simulate a golf shot using a club preset.
   Optionally override ball speed with speed_override > 0.
   Returns the number of trajectory points written to `out`. */
int golf_simulate_shot(golf_club_type club, phys_float speed_override,
                       phys_trajectory_point *out, int max_points);

/* Simulate a custom golf shot with full control. */
int golf_simulate_custom(phys_float ball_speed, phys_float launch_angle_deg,
                         phys_float spin_rate, phys_float side_angle_deg,
                         phys_trajectory_point *out, int max_points);

/* Utility: compute carry distance (horizontal) from trajectory */
phys_float golf_carry_distance(const phys_trajectory_point *pts, int count);

/* Utility: compute max height from trajectory */
phys_float golf_max_height(const phys_trajectory_point *pts, int count);

/* Utility: compute lateral deviation from trajectory */
phys_float golf_lateral_deviation(const phys_trajectory_point *pts, int count);

#endif /* GOLF_PHYSICS_H */
