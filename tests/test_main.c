/* test_main.c - D-Flat Test Suite Entry Point */

#define DF_TEST_IMPLEMENTATION
#include "df_test.h"

/* Forward declarations of test suites */
DF_TEST_SUITE *rect_tests(void);
DF_TEST_SUITE *keys_tests(void);
DF_TEST_SUITE *tcccompat_tests(void);
DF_TEST_SUITE *tccwin32_tests(void);
DF_TEST_SUITE *console_tests(void);
DF_TEST_SUITE *message_tests(void);
DF_TEST_SUITE *video_tests(void);
DF_TEST_SUITE *mouse_tests(void);
DF_TEST_SUITE *direct_tests(void);

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printf("D-Flat Test Suite\n");
    printf("==================\n");
    
    /* Register all test suites */
    DF_TEST_SUITE *suites[] = {
        rect_tests(),
        keys_tests(),
        tcccompat_tests(),
        tccwin32_tests(),
        console_tests(),
        message_tests(),
        video_tests(),
        mouse_tests(),
        direct_tests()
    };
    
    int suite_count = sizeof(suites) / sizeof(suites[0]);
    
    /* Run all tests */
    df_test_run_all(suites, suite_count);
    
    /* Print final report */
    df_test_print_report();
    
    /* Return exit code based on test results */
    return df_test_get_exit_code();
}