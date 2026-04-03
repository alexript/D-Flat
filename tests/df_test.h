/* df_test.h - D-Flat Unit Testing Framework (single-header library)
   
   Usage:
       #define DF_TEST_IMPLEMENTATION
       #include "df_test.h"
   
   Or simply include for declarations:
       #include "df_test.h"
*/

#ifndef DF_TEST_H
#define DF_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#define DF_TEST_EXPORT

/* ============ Base Types ============ */

typedef void (*DF_TEST_FUNC)(void);
typedef void (*DF_TEST_SETUP)(void);
typedef void (*DF_TEST_TEARDOWN)(void);

typedef struct {
    const char *name;
    DF_TEST_FUNC func;
    int skipped;
} DF_TEST_CASE;

typedef struct {
    const char *name;
    DF_TEST_CASE *tests;
    int test_count;
    int tests_run;
    int tests_passed;
    int tests_failed;
    int tests_skipped;
} DF_TEST_SUITE;

/* ============ Test Results ============ */

typedef struct {
    const char *suite_name;
    const char *test_name;
    const char *file;
    int line;
    const char *failed_expr;
    char message[256];
} DF_TEST_RESULT;

#define DF_TEST_MAX_RESULTS 100

typedef struct {
    DF_TEST_RESULT results[DF_TEST_MAX_RESULTS];
    int count;
    int passed;
    int failed;
} DF_TEST_REPORT;

/* ============ Test Context ============ */

typedef struct {
    int mock_count;
    jmp_buf jump_buffer;
    int test_status;
} DF_TEST_CONTEXT;

/* ============ Assertions ============ */

#define DF_ASSERT(expr) \
    do { if (!(expr)) { \
        df_test_assert_fail(#expr, __FILE__, __LINE__); \
    } } while(0)

#define DF_ASSERT_TRUE(expr) DF_ASSERT(expr)
#define DF_ASSERT_FALSE(expr) DF_ASSERT(!(expr))

#define DF_ASSERT_EQ(a, b) \
    do { if ((a) != (b)) { \
        df_test_assert_eq_fail((long)(a), (long)(b), #a, #b, __FILE__, __LINE__); \
    } } while(0)

#define DF_ASSERT_NE(a, b) \
    do { if ((a) == (b)) { \
        df_test_assert_ne_fail((long)(a), (long)(b), #a, #b, __FILE__, __LINE__); \
    } } while(0)

#define DF_ASSERT_NULL(ptr) DF_ASSERT((ptr) == NULL)
#define DF_ASSERT_NOT_NULL(ptr) DF_ASSERT((ptr) != NULL)

#define DF_ASSERT_INT_EQ(a, b) DF_ASSERT_EQ((int)(a), (int)(b))
#define DF_ASSERT_INT_NE(a, b) DF_ASSERT_NE((int)(a), (int)(b))

#define DF_ASSERT_STR_EQ(a, b) \
    do { if (strcmp((a), (b)) != 0) { \
        df_test_assert_str_eq_fail((a), (b), __FILE__, __LINE__); \
    } } while(0)

#define DF_ASSERT_MEM_EQ(a, b, size) \
    do { if (memcmp((a), (b), (size)) != 0) { \
        df_test_assert_mem_eq_fail((a), (b), (size), __FILE__, __LINE__); \
    } } while(0)

/* D-Flat specific assertions */

#define DF_ASSERT_RECT_EQ(r1, r2) do { \
    DF_ASSERT_INT_EQ((r1).lf, (r2).lf); \
    DF_ASSERT_INT_EQ((r1).tp, (r2).tp); \
    DF_ASSERT_INT_EQ((r1).rt, (r2).rt); \
    DF_ASSERT_INT_EQ((r1).bt, (r2).bt); \
} while(0)

#define DF_ASSERT_WNDRECT_EQ(wnd, r) do { \
    DF_ASSERT_INT_EQ((wnd)->rc.lf, (r).lf); \
    DF_ASSERT_INT_EQ((wnd)->rc.tp, (r).tp); \
    DF_ASSERT_INT_EQ((wnd)->rc.rt, (r).rt); \
    DF_ASSERT_INT_EQ((wnd)->rc.bt, (r).bt); \
} while(0)

#define DF_ASSERT_BOOL_EQ(a, b) DF_ASSERT_INT_EQ((a) ? 1 : 0, (b) ? 1 : 0)

#define DF_ASSERT_RECT_VALID(r) do { \
    DF_ASSERT((r).lf >= 0); \
    DF_ASSERT((r).tp >= 0); \
    DF_ASSERT((r).rt >= (r).lf); \
    DF_ASSERT((r).bt >= (r).tp); \
} while(0)

/* ============ Test Suites ============ */

#define DF_TEST_SUITE(suite_name) \
    static DF_TEST_CASE suite_name##_tests[] = {

#define DF_TEST_CASE(test_name) \
    { #test_name, test_name, 0 },

#define DF_TEST_CASE_SKIP(test_name) \
    { #test_name, test_name, 1 },

#define DF_TEST_SUITE_END(suite_name) \
    }; \
    static DF_TEST_SUITE suite_name##_suite = { \
        #suite_name, \
        suite_name##_tests, \
        sizeof(suite_name##_tests) / sizeof(DF_TEST_CASE), \
        0, 0, 0, 0 \
    }; \
    DF_TEST_SUITE *suite_name(void) { return &suite_name##_suite; }

/* ============ API Functions ============ */

DF_TEST_EXPORT void df_test_init(void);
DF_TEST_EXPORT void df_test_run_suite(DF_TEST_SUITE *suite);
DF_TEST_EXPORT void df_test_run_all(DF_TEST_SUITE **suites, int count);
DF_TEST_EXPORT DF_TEST_REPORT *df_test_get_report(void);
DF_TEST_EXPORT void df_test_print_report(void);
DF_TEST_EXPORT int df_test_get_exit_code(void);

DF_TEST_EXPORT void df_test_assert_fail(const char *expr, 
    const char *file, int line);
DF_TEST_EXPORT void df_test_assert_eq_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line);
DF_TEST_EXPORT void df_test_assert_ne_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line);
DF_TEST_EXPORT void df_test_assert_str_eq_fail(const char *a, const char *b, 
    const char *file, int line);
DF_TEST_EXPORT void df_test_assert_mem_eq_fail(const void *a, const void *b, 
    size_t size, const char *file, int line);

/* ============ Stub System for DOS Calls ============ */

#ifdef DF_TEST_ENABLE_STUBS

extern int stub_getvideo_enabled;
extern void *stub_video_buffer;
extern int stub_screen_width;
extern int stub_screen_height;

#define DF_STUB_GETVIDEO(buf, size) \
    stub_getvideo_enabled = 1; \
    stub_video_buffer = (buf)

extern int stub_keyhit_result;
extern int stub_getkey_result;

#define DF_STUB_KEYHIT(result) stub_keyhit_result = (result)
#define DF_STUB_GETKEY(result) stub_getkey_result = (result)

extern unsigned long stub_tick_count;

#define DF_STUB_TICKCOUNT(value) stub_tick_count = (value)
#define DF_STUB_ADVANCE_TICK(ms) stub_tick_count += (ms)

extern int stub_mouse_x;
extern int stub_mouse_y;
extern int stub_mouse_buttons;

#define DF_STUB_MOUSE(x, y, buttons) \
    stub_mouse_x = (x); stub_mouse_y = (y); stub_mouse_buttons = (buttons)

#endif /* DF_TEST_ENABLE_STUBS */

/* ============ Implementation ============ */

#ifdef DF_TEST_IMPLEMENTATION

#ifndef DF_TEST_H_IMPLEMENTATION
#define DF_TEST_H_IMPLEMENTATION

static DF_TEST_CONTEXT _df_test_ctx = {0};
static DF_TEST_REPORT _df_report = {0};

void df_test_init(void) {
    memset(&_df_test_ctx, 0, sizeof(_df_test_ctx));
    memset(&_df_report, 0, sizeof(_df_report));
}

void df_test_assert_fail(const char *expr, const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        _df_report.results[_df_report.count].failed_expr = expr;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "Assertion failed: %s", expr);
        _df_report.count++;
    }
    _df_report.failed++;
    longjmp(_df_test_ctx.jump_buffer, 1);
}

void df_test_assert_eq_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "Expected %s (%ld) == %s (%ld)", expr_a, a, expr_b, b);
        _df_report.count++;
    }
    _df_report.failed++;
    longjmp(_df_test_ctx.jump_buffer, 1);
}

void df_test_assert_ne_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "Expected %s (%ld) != %s (%ld)", expr_a, a, expr_b, b);
        _df_report.count++;
    }
    _df_report.failed++;
    longjmp(_df_test_ctx.jump_buffer, 1);
}

void df_test_assert_str_eq_fail(const char *a, const char *b, 
    const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "String mismatch: \"%s\" vs \"%s\"", a ? a : "(null)", b ? b : "(null)");
        _df_report.count++;
    }
    _df_report.failed++;
    longjmp(_df_test_ctx.jump_buffer, 1);
}

void df_test_assert_mem_eq_fail(const void *a, const void *b, 
    size_t size, const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "Memory mismatch: %u bytes differ", (unsigned)size);
        _df_report.count++;
    }
    _df_report.failed++;
    longjmp(_df_test_ctx.jump_buffer, 1);
}

void df_test_run_suite(DF_TEST_SUITE *suite) {
    printf("\n=== Running suite: %s ===\n", suite->name);
    
    for (int i = 0; i < suite->test_count; i++) {
        DF_TEST_CASE *tc = &suite->tests[i];
        
        if (tc->skipped) {
            suite->tests_skipped++;
            printf("  SKIP: %s\n", tc->name);
            continue;
        }
        
        printf("  RUN: %s ... ", tc->name);
        fflush(stdout);
        
        suite->tests_run++;
        
        if (setjmp(_df_test_ctx.jump_buffer) == 0) {
            tc->func();
            suite->tests_passed++;
            _df_report.passed++;
            printf("PASS\n");
        } else {
            suite->tests_failed++;
            _df_report.failed++;
            printf("FAIL\n");
        }
    }
    
    printf("\nResults: %d/%d passed, %d failed, %d skipped\n",
        suite->tests_passed, suite->tests_run, 
        suite->tests_failed, suite->tests_skipped);
}

void df_test_run_all(DF_TEST_SUITE **suites, int count) {
    df_test_init();
    
    for (int i = 0; i < count; i++) {
        df_test_run_suite(suites[i]);
    }
}

DF_TEST_REPORT *df_test_get_report(void) {
    return &_df_report;
}

void df_test_print_report(void) {
    printf("\n");
    printf("========================================\n");
    printf("         FINAL TEST REPORT              \n");
    printf("========================================\n");
    printf("Total passed:  %d\n", _df_report.passed);
    printf("Total failed:  %d\n", _df_report.failed);
    printf("Total results: %d\n", _df_report.count);
    
    if (_df_report.count > 0) {
        printf("\nFailed tests:\n");
        for (int i = 0; i < _df_report.count; i++) {
            printf("  - %s:%d: %s\n", 
                _df_report.results[i].file,
                _df_report.results[i].line,
                _df_report.results[i].message);
        }
    }
    printf("========================================\n");
}

int df_test_get_exit_code(void) {
    return _df_report.failed > 0 ? 1 : 0;
}

#endif /* DF_TEST_H_IMPLEMENTATION */

#ifdef DF_TEST_ENABLE_STUBS

int stub_getvideo_enabled = 0;
void *stub_video_buffer = NULL;
int stub_screen_width = 80;
int stub_screen_height = 25;

int stub_keyhit_result = 0;
int stub_getkey_result = 0;

unsigned long stub_tick_count = 0;

int stub_mouse_x = 0;
int stub_mouse_y = 0;
int stub_mouse_buttons = 0;

#endif /* DF_TEST_ENABLE_STUBS */

#endif /* DF_TEST_IMPLEMENTATION */

#endif /* DF_TEST_H */