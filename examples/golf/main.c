/*
 * Golf Physics Example
 *
 * Demonstrates the physics library applied to golf: aerodynamic ball flight
 * and ground interaction (bounce + roll) across different surface types.
 *
 * Build:
 *   cmake -DPHYSICS_BUILD_EXAMPLES=ON .. && make golf_example
 *   ./golf_example
 */

#include "golf_physics.h"
#include <stdio.h>
#include <math.h>

#define MAX_FLIGHT 4096
#define MAX_ROLL   4096

static void print_separator(void) {
    printf("  -------------------------------------------------------------------------\n");
}

static void simulate_club_full(golf_club_type club, golf_surface_type surface) {
    golf_club_preset preset = golf_get_club_preset(club);
    phys_trajectory_point flight[MAX_FLIGHT];
    phys_rolling_point roll[MAX_ROLL];
    int fn, rn;

    phys_float total = golf_simulate_full_shot(club, 0, surface,
                                               flight, &fn, MAX_FLIGHT,
                                               roll, &rn, MAX_ROLL);

    phys_float carry = golf_carry_distance(flight, fn);
    phys_float roll_dist = golf_roll_distance(roll, rn);
    phys_float apex = golf_max_height(flight, fn);

    printf("  %-8s  %5.1f m/s  %5.1f°  %6.0f rad/s  |"
           "  %6.1f m  %+5.1f m  %6.1f m  %5.1f m\n",
           preset.name, preset.ball_speed, preset.launch_angle,
           preset.spin_rate, carry, roll_dist, total, apex);
}

int main(void) {
    printf("\n  Golf Physics Simulation — Flight + Roll Showcase\n");
    printf("  Using: @thestonepixel/physics (aerodynamics + surface)\n");

    /* --- Full bag on fairway --- */
    printf("\n  Landing surface: Fairway\n\n");
    printf("  Club      Speed    Loft    Spin         |  Carry    Roll    Total   Apex\n");
    print_separator();

    for (int i = 0; i < GOLF_CLUB_COUNT; i++) {
        simulate_club_full((golf_club_type)i, GOLF_SURFACE_FAIRWAY);
    }
    print_separator();

    /* --- Surface comparison: 7 Iron landing on different surfaces --- */
    printf("\n  7 Iron landing on different surfaces:\n\n");
    printf("  Surface     |  Carry    Roll    Total\n");
    printf("  -----------------------------------------\n");

    for (int s = 0; s < GOLF_SURFACE_COUNT; s++) {
        phys_trajectory_point flight[MAX_FLIGHT];
        phys_rolling_point roll[MAX_ROLL];
        int fn, rn;

        phys_float total = golf_simulate_full_shot(GOLF_CLUB_7_IRON, 0,
                                                   (golf_surface_type)s,
                                                   flight, &fn, MAX_FLIGHT,
                                                   roll, &rn, MAX_ROLL);

        phys_float carry = golf_carry_distance(flight, fn);
        phys_float roll_dist = golf_roll_distance(roll, rn);

        printf("  %-10s  |  %6.1f m  %+5.1f m  %6.1f m\n",
               golf_surface_name((golf_surface_type)s),
               carry, roll_dist, total);
    }
    printf("  -----------------------------------------\n");

    printf("\n");
    return 0;
}
