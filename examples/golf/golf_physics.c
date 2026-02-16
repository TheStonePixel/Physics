#include "golf_physics.h"
#include <math.h>

/* --- Club presets based on typical tour averages --- */
static const golf_club_preset s_presets[GOLF_CLUB_COUNT] = {
    /* name        speed(m/s) launch(deg) spin(rad/s) */
    { "Driver",     71.5f,     10.5f,      280.0f },
    { "3 Wood",     64.0f,     11.0f,      340.0f },
    { "5 Iron",     56.0f,     14.0f,      460.0f },
    { "7 Iron",     48.5f,     18.0f,      620.0f },
    { "9 Iron",     41.0f,     24.0f,      780.0f },
    { "PW",         37.0f,     28.0f,      880.0f },
    { "SW",         31.0f,     34.0f,      980.0f },
};

golf_club_preset golf_get_club_preset(golf_club_type club) {
    if (club < 0 || club >= GOLF_CLUB_COUNT) {
        return s_presets[GOLF_CLUB_DRIVER];
    }
    return s_presets[club];
}

phys_aero_params golf_ball_aero_params(void) {
    phys_aero_params p;
    p.drag_coefficient = 0.25f;       /* dimpled golf ball */
    p.lift_coefficient = 0.15f;       /* typical for backspin */
    p.cross_section_area = GOLF_BALL_AREA;
    p.radius = GOLF_BALL_RADIUS;
    p.air_density = 1.225f;           /* sea level, 15C */
    p.spin_decay = 0.04f;            /* ~4% per second */
    return p;
}

int golf_simulate_shot(golf_club_type club, phys_float speed_override,
                       phys_trajectory_point *out, int max_points) {
    golf_club_preset preset = golf_get_club_preset(club);
    phys_float speed = speed_override > 0 ? speed_override : preset.ball_speed;
    return golf_simulate_custom(speed, preset.launch_angle, preset.spin_rate,
                                0.0f, out, max_points);
}

int golf_simulate_custom(phys_float ball_speed, phys_float launch_angle_deg,
                         phys_float spin_rate, phys_float side_angle_deg,
                         phys_trajectory_point *out, int max_points) {
    phys_float la = launch_angle_deg * 3.14159265f / 180.0f;
    phys_float sa = side_angle_deg * 3.14159265f / 180.0f;

    phys_vec3 pos0 = {0, 0, 0};
    phys_vec3 vel0 = {
        ball_speed * cosf(la) * cosf(sa),
        ball_speed * sinf(la),
        ball_speed * cosf(la) * sinf(sa)
    };
    phys_vec3 spin_axis = {0, 0, 1}; /* backspin axis */
    phys_vec3 gravity = {0, -9.81f, 0};
    phys_aero_params params = golf_ball_aero_params();

    return phys_aero_simulate(pos0, vel0, spin_axis, spin_rate,
                              GOLF_BALL_MASS, GOLF_BALL_RESTITUTION,
                              GOLF_BALL_FRICTION, gravity, params,
                              0.0f, 0.005f, out, max_points);
}

phys_float golf_carry_distance(const phys_trajectory_point *pts, int count) {
    if (count < 1) return 0;
    phys_float x = pts[count - 1].position.x;
    phys_float z = pts[count - 1].position.z;
    return sqrtf(x * x + z * z);
}

phys_float golf_max_height(const phys_trajectory_point *pts, int count) {
    phys_float max_y = 0;
    for (int i = 0; i < count; i++) {
        if (pts[i].position.y > max_y)
            max_y = pts[i].position.y;
    }
    return max_y;
}

phys_float golf_lateral_deviation(const phys_trajectory_point *pts, int count) {
    if (count < 1) return 0;
    return pts[count - 1].position.z;
}

/* --- Surface presets --- */

typedef struct {
    const char *name;
    phys_float rolling_friction;
    phys_float restitution;
    phys_float firmness;
} golf_surface_data;

static const golf_surface_data s_surfaces[GOLF_SURFACE_COUNT] = {
    /* name          roll_friction  restitution  firmness */
    { "Fairway",     0.065f,        0.45f,       0.65f },
    { "Rough",       0.20f,         0.30f,       0.35f },
    { "Green",       0.045f,        0.35f,       0.55f },
    { "Fringe",      0.09f,         0.40f,       0.55f },
    { "Bunker",      0.40f,         0.15f,       0.15f },
    { "Cart Path",   0.03f,         0.70f,       0.95f },
};

phys_surface golf_get_surface(golf_surface_type type) {
    if (type < 0 || type >= GOLF_SURFACE_COUNT) type = GOLF_SURFACE_FAIRWAY;
    const golf_surface_data *d = &s_surfaces[type];
    return phys_surface_create(d->rolling_friction, d->restitution, d->firmness);
}

phys_surface golf_get_sloped_surface(golf_surface_type type, phys_vec3 slope_normal) {
    if (type < 0 || type >= GOLF_SURFACE_COUNT) type = GOLF_SURFACE_FAIRWAY;
    const golf_surface_data *d = &s_surfaces[type];
    return phys_surface_create_sloped(d->rolling_friction, d->restitution,
                                      d->firmness, slope_normal);
}

const char *golf_surface_name(golf_surface_type type) {
    if (type < 0 || type >= GOLF_SURFACE_COUNT) return "Unknown";
    return s_surfaces[type].name;
}

int golf_simulate_roll(phys_vec3 landing_pos, phys_vec3 landing_vel,
                       phys_float spin_rate, golf_surface_type surface,
                       phys_rolling_point *out, int max_points) {
    phys_vec3 spin_axis = {0, 0, 1}; /* backspin axis */
    phys_vec3 gravity = {0, -9.81f, 0};
    phys_surface surf = golf_get_surface(surface);

    return phys_surface_simulate_roll(landing_pos, landing_vel,
                                      spin_axis, spin_rate,
                                      GOLF_BALL_RADIUS, GOLF_BALL_MASS,
                                      gravity, surf, 0.005f,
                                      out, max_points);
}

phys_float golf_simulate_full_shot(golf_club_type club,
                                   phys_float speed_override,
                                   golf_surface_type landing_surface,
                                   phys_trajectory_point *flight_out,
                                   int *flight_count, int max_flight,
                                   phys_rolling_point *roll_out,
                                   int *roll_count, int max_roll) {
    /* Flight phase */
    *flight_count = golf_simulate_shot(club, speed_override,
                                       flight_out, max_flight);

    /* Get landing conditions from last flight point */
    int fn = *flight_count;
    phys_vec3 land_pos = flight_out[fn - 1].position;
    phys_vec3 land_vel = flight_out[fn - 1].velocity;

    /* Estimate remaining spin at landing (rough: flight decays spin) */
    golf_club_preset preset = golf_get_club_preset(club);
    phys_float flight_time = flight_out[fn - 1].time;
    phys_float spin_at_landing = preset.spin_rate *
                                 powf(1.0f - 0.04f, flight_time);

    /* Roll phase */
    *roll_count = golf_simulate_roll(land_pos, land_vel, spin_at_landing,
                                     landing_surface, roll_out, max_roll);

    /* Total distance */
    return golf_total_distance(flight_out, fn, roll_out, *roll_count);
}

phys_float golf_roll_distance(const phys_rolling_point *pts, int count) {
    if (count < 2) return 0;
    phys_float dx = pts[count - 1].position.x - pts[0].position.x;
    phys_float dz = pts[count - 1].position.z - pts[0].position.z;
    return sqrtf(dx * dx + dz * dz);
}

phys_float golf_total_distance(const phys_trajectory_point *flight, int flight_n,
                               const phys_rolling_point *roll, int roll_n) {
    if (roll_n < 1) return golf_carry_distance(flight, flight_n);
    phys_float x = roll[roll_n - 1].position.x;
    phys_float z = roll[roll_n - 1].position.z;
    return sqrtf(x * x + z * z);
}
