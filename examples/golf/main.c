/*
 * Golf Physics Example
 *
 * Demonstrates the physics library's aerodynamic simulation applied to golf.
 * Simulates every club in the bag and prints carry distance, apex height,
 * flight time, and lateral deviation — comparable to real-world launch monitor
 * data for validation.
 *
 * Build:
 *   cmake -DPHYSICS_BUILD_EXAMPLES=ON .. && make golf_example
 *   ./golf_example
 */

#include "golf_physics.h"
#include <stdio.h>
#include <math.h>

#define MAX_POINTS 4096

static void print_separator(void) {
    printf("--------------------------------------------------------------\n");
}

static void simulate_club(golf_club_type club) {
    golf_club_preset preset = golf_get_club_preset(club);
    phys_trajectory_point pts[MAX_POINTS];

    int n = golf_simulate_shot(club, 0, pts, MAX_POINTS);

    phys_float carry = golf_carry_distance(pts, n);
    phys_float apex = golf_max_height(pts, n);
    phys_float flight = pts[n - 1].time;
    phys_float lateral = golf_lateral_deviation(pts, n);

    printf("  %-8s  %5.1f m/s  %5.1f°  %6.0f rad/s  |  %6.1f m  %5.1f m  %4.2f s  %+5.1f m\n",
           preset.name, preset.ball_speed, preset.launch_angle,
           preset.spin_rate, carry, apex, flight, lateral);
}

int main(void) {
    printf("\n  Golf Physics Simulation — Library Accuracy Showcase\n");
    printf("  Using: @thestonepixel/physics aerodynamic engine\n\n");

    printf("  Club      Speed    Loft    Spin         |  Carry   Apex   Time   Lateral\n");
    print_separator();

    for (int i = 0; i < GOLF_CLUB_COUNT; i++) {
        simulate_club((golf_club_type)i);
    }
    print_separator();

    /* Custom shot: high draw */
    printf("\n  Custom shot: 65 m/s, 12° launch, 350 rad/s spin, -2° side angle\n");
    {
        phys_trajectory_point pts[MAX_POINTS];
        int n = golf_simulate_custom(65.0f, 12.0f, 350.0f, -2.0f, pts, MAX_POINTS);
        printf("  Carry: %.1f m | Apex: %.1f m | Flight: %.2f s | Lateral: %+.1f m\n",
               golf_carry_distance(pts, n),
               golf_max_height(pts, n),
               pts[n - 1].time,
               golf_lateral_deviation(pts, n));
    }

    printf("\n");
    return 0;
}
