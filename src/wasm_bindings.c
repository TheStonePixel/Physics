#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "physics/physics.h"

/*
 * WASM bindings — flat C functions callable from JavaScript via ccall/cwrap.
 * When compiled with Emscripten, these are exported. When compiled natively,
 * they serve as integration examples and can be ignored.
 */

#ifdef __EMSCRIPTEN__
#define PHYS_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define PHYS_EXPORT
#endif

/* --- Golf simulation binding --- */

static phys_trajectory_point g_trajectory[4096];
static int g_trajectory_count = 0;

PHYS_EXPORT int golf_simulate(
    float px, float py, float pz,
    float vx, float vy, float vz,
    float sx, float sy, float sz,
    float spin_rate, float ground_y, float dt
) {
    phys_vec3 pos = {px, py, pz};
    phys_vec3 vel = {vx, vy, vz};
    phys_vec3 spin_axis = phys_vec3_normalize((phys_vec3){sx, sy, sz});
    phys_vec3 gravity = {0, -9.81f, 0};
    phys_aero_params params = phys_aero_golf_ball_defaults();

    g_trajectory_count = phys_golf_simulate(
        pos, vel, spin_axis, spin_rate, gravity, params,
        ground_y, dt, g_trajectory, 4096
    );
    return g_trajectory_count;
}

PHYS_EXPORT int golf_get_count(void) {
    return g_trajectory_count;
}

PHYS_EXPORT float golf_get_x(int i) {
    if (i < 0 || i >= g_trajectory_count) return 0;
    return g_trajectory[i].position.x;
}

PHYS_EXPORT float golf_get_y(int i) {
    if (i < 0 || i >= g_trajectory_count) return 0;
    return g_trajectory[i].position.y;
}

PHYS_EXPORT float golf_get_z(int i) {
    if (i < 0 || i >= g_trajectory_count) return 0;
    return g_trajectory[i].position.z;
}

PHYS_EXPORT float golf_get_time(int i) {
    if (i < 0 || i >= g_trajectory_count) return 0;
    return g_trajectory[i].time;
}
