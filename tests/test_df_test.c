/* test_df_test.c - Unit tests for df_test.h (self-testing the test framework) */

#include "df_test.h"
#include "test_dflat.h"

/* Tests for framework FAIL mechanism - expected to fail */
static void framework_test_fail(void) {
    DF_ASSERT(0);
}

/* Tests for DF_ASSERT_EQ */
static void framework_test_assert_eq(void) {
    DF_ASSERT_EQ(42, 42);
    DF_ASSERT_EQ(0, 0);
    DF_ASSERT_EQ(-5, -5);
}

/* Tests for DF_ASSERT_NE */
static void framework_test_assert_ne(void) {
    DF_ASSERT_NE(42, 0);
    DF_ASSERT_NE(-1, 1);
}

/* Tests for DF_ASSERT_INT_EQ */
static void framework_test_assert_int_eq(void) {
    DF_ASSERT_INT_EQ(100, 100);
    DF_ASSERT_INT_EQ(0, 0);
    DF_ASSERT_INT_EQ(-5, -5);
}

/* Tests for DF_ASSERT_INT_NE */
static void framework_test_assert_int_ne(void) {
    DF_ASSERT_INT_NE(10, 20);
    DF_ASSERT_INT_NE(0, -1);
}

/* Tests for DF_ASSERT_TRUE */
static void framework_test_assert_true(void) {
    DF_ASSERT_TRUE(1);
    DF_ASSERT_TRUE(42);
    DF_ASSERT_TRUE(-1);
}

/* Tests for DF_ASSERT_FALSE */
static void framework_test_assert_false(void) {
    DF_ASSERT_FALSE(0);
}

/* Tests for DF_ASSERT_NULL */
static void framework_test_assert_null(void) {
    void *p = NULL;
    DF_ASSERT_NULL(p);
}

/* Tests for DF_ASSERT_NOT_NULL */
static void framework_test_assert_not_null(void) {
    int x = 42;
    DF_ASSERT_NOT_NULL(&x);
}

/* Tests for DF_ASSERT_STR_EQ */
static void framework_test_assert_str_eq(void) {
    DF_ASSERT_STR_EQ("hello", "hello");
    DF_ASSERT_STR_EQ("", "");
    DF_ASSERT_STR_EQ("test", "test");
}

/* Tests for shadowing - subsequent asserts should be skipped after first fail */
static void framework_test_shadowing_after_fail(void) {
    DF_ASSERT_EQ(10, 20);
    DF_ASSERT_EQ(30, 40);
    DF_ASSERT_EQ(50, 60);
}

/* Tests for multiple asserts with one failing */
static void framework_test_pass_then_fail(void) {
    DF_ASSERT_EQ(1, 1);
    DF_ASSERT_EQ(2, 2);
    DF_ASSERT_EQ(3, 10);
    DF_ASSERT_EQ(4, 4);
}

/* Tests for DF_ASSERT_MEM_EQ */
static void framework_test_assert_mem_eq(void) {
    char buf1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    char buf2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    char buf3[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    DF_ASSERT_MEM_EQ(buf1, buf2, 10);
    DF_ASSERT_MEM_EQ(buf1, buf3, 9);
}

/* Tests for DF_ASSERT_BOOL_EQ */
static void framework_test_assert_bool_eq(void) {
    DF_ASSERT_BOOL_EQ(1, 1);
    DF_ASSERT_BOOL_EQ(0, 0);
    DF_ASSERT_BOOL_EQ(1, 42);
    DF_ASSERT_BOOL_EQ(100, 1);
}

/* Tests for DF_ASSERT_RECT_EQ */
static void framework_test_assert_rect_eq(void) {
    RECT r1 = {0, 0, 10, 20};
    RECT r2 = {0, 0, 10, 20};
    RECT r3 = {0, 0, 10, 21};
    DF_ASSERT_RECT_EQ(r1, r2);
    DF_ASSERT_FALSE(r1.lf == r3.lf && r1.tp == r3.tp && r1.rt == r3.rt && r1.bt == r3.bt);
}

/* Tests for DF_ASSERT_RECT_VALID */
static void framework_test_assert_rect_valid(void) {
    RECT valid = {0, 0, 10, 20};
    RECT zero = {0, 0, 0, 0};
    DF_ASSERT_RECT_VALID(valid);
    DF_ASSERT_RECT_VALID(zero);
}

/* Tests for multiple asserts - last one fails */
static void framework_test_multiple_asserts_same_line(void) {
    int x = 5;
    int y = 10;
    DF_ASSERT_EQ(x, 5);
    DF_ASSERT_EQ(y, 10);
    DF_ASSERT_EQ(x, y);
}

/* Tests for negation style asserts */
static void framework_test_negation_asserts(void) {
    int a = 5;
    int b = 0;
    DF_ASSERT_FALSE(a == b);
    DF_ASSERT_TRUE(a != b);
    DF_ASSERT_FALSE(a == 0);
    DF_ASSERT_TRUE(b == 0);
}

/* Tests for pointer comparison */
static void framework_test_ptr_comparison(void) {
    int val = 42;
    int *ptr1 = &val;
    int *ptr2 = &val;
    int *ptr3 = NULL;
    DF_ASSERT_EQ((intptr_t)ptr1, (intptr_t)ptr2);
    DF_ASSERT_NE((intptr_t)ptr1, (intptr_t)ptr3);
    DF_ASSERT_NULL(ptr3);
    DF_ASSERT_NOT_NULL(ptr1);
}

/* Tests for df_test_get_report */
static void framework_test_get_report_exists(void) {
    DF_TEST_REPORT *report = df_test_get_report();
    DF_ASSERT_NOT_NULL(report);
}

/* Tests for df_test_get_exit_code */
static void framework_test_get_exit_code_logic(void) {
    DF_TEST_REPORT *r = df_test_get_report();
    int expected = (r->failed > 0) ? 1 : 0;
    DF_ASSERT_INT_EQ(df_test_get_exit_code(), expected);
}

/* Tests for complex assertions */
static void framework_test_complex_assertions(void) {
    int a = 100;
    int b = 200;
    int c = 300;
    DF_ASSERT_TRUE(a < b);
    DF_ASSERT_TRUE(b < c);
    DF_ASSERT_FALSE(a > b);
    DF_ASSERT_EQ(a + b, 300);
    DF_ASSERT_EQ(b * 2, 400);
    DF_ASSERT_EQ(c - b, 100);
    DF_ASSERT_NE(a, b);
    DF_ASSERT_INT_NE(a, b);
}

/* Tests for zero and negative values */
static void framework_test_zero_and_negative(void) {
    DF_ASSERT_INT_EQ(0, 0);
    DF_ASSERT_INT_EQ(-1, -1);
    DF_ASSERT_INT_EQ(-100, -100);
    DF_ASSERT_TRUE(0 == 0);
    DF_ASSERT_FALSE(0 != 0);
    DF_ASSERT_TRUE(-1 < 0);
}

DF_TEST_SUITE(df_test_framework_tests)
    DF_TEST_CASE_XFAIL(framework_test_fail)
    DF_TEST_CASE(framework_test_assert_eq)
    DF_TEST_CASE(framework_test_assert_ne)
    DF_TEST_CASE(framework_test_assert_int_eq)
    DF_TEST_CASE(framework_test_assert_int_ne)
    DF_TEST_CASE(framework_test_assert_true)
    DF_TEST_CASE(framework_test_assert_false)
    DF_TEST_CASE(framework_test_assert_null)
    DF_TEST_CASE(framework_test_assert_not_null)
    DF_TEST_CASE(framework_test_assert_str_eq)
    DF_TEST_CASE_XFAIL(framework_test_shadowing_after_fail)
    DF_TEST_CASE_XFAIL(framework_test_pass_then_fail)
    DF_TEST_CASE(framework_test_assert_mem_eq)
    DF_TEST_CASE(framework_test_assert_bool_eq)
    DF_TEST_CASE(framework_test_assert_rect_eq)
    DF_TEST_CASE(framework_test_assert_rect_valid)
    DF_TEST_CASE_XFAIL(framework_test_multiple_asserts_same_line)
    DF_TEST_CASE(framework_test_negation_asserts)
    DF_TEST_CASE(framework_test_ptr_comparison)
    DF_TEST_CASE(framework_test_get_report_exists)
    DF_TEST_CASE(framework_test_get_exit_code_logic)
    DF_TEST_CASE(framework_test_complex_assertions)
    DF_TEST_CASE(framework_test_zero_and_negative)
DF_TEST_SUITE_END(df_test_framework_tests)
