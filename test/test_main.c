#include <stdio.h>

/* Test runner declarations */
int test_vec_run(void);
int test_mat_run(void);
int test_kinematics_run(void);
int test_dynamics_run(void);
int test_aerodynamics_run(void);
int test_collision_run(void);

int main(void) {
    int total_fail = 0;

    printf("=== Physics Library Tests ===\n\n");

    total_fail += test_vec_run();
    total_fail += test_mat_run();
    total_fail += test_kinematics_run();
    total_fail += test_dynamics_run();
    total_fail += test_aerodynamics_run();
    total_fail += test_collision_run();

    printf("\n=== Results: ");
    if (total_fail == 0) {
        printf("ALL TESTS PASSED ===\n");
    } else {
        printf("%d TEST(S) FAILED ===\n", total_fail);
    }

    return total_fail > 0 ? 1 : 0;
}
