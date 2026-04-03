/* test_rect.c - Unit tests for rect.c */

#include "df_test.h"
#include "test_dflat.h"

/* Test functions that work without DOS dependencies */

static void test_rect_width(void) {
    RECT r = {0, 0, 10, 5};
    DF_ASSERT_INT_EQ(RectWidth(r), 11);
}

static void test_rect_height(void) {
    RECT r = {0, 0, 10, 5};
    DF_ASSERT_INT_EQ(RectHeight(r), 6);
}

static void test_rect_width_height_different(void) {
    RECT r = {5, 10, 20, 30};
    DF_ASSERT_INT_EQ(RectWidth(r), 16);
    DF_ASSERT_INT_EQ(RectHeight(r), 21);
}

static void test_rect_inside_true(void) {
    RECT r = {0, 0, 10, 10};
    DF_ASSERT_TRUE(InsideRect(5, 5, r));
    DF_ASSERT_TRUE(InsideRect(0, 0, r));
    DF_ASSERT_TRUE(InsideRect(10, 10, r));
}

static void test_rect_inside_false(void) {
    RECT r = {0, 0, 10, 10};
    DF_ASSERT_FALSE(InsideRect(11, 5, r));
    DF_ASSERT_FALSE(InsideRect(5, 11, r));
    DF_ASSERT_FALSE(InsideRect(-1, 5, r));
    DF_ASSERT_FALSE(InsideRect(5, -1, r));
}

static void test_rect_valid_macro(void) {
    RECT r1 = {0, 0, 0, 0};
    DF_ASSERT_FALSE(ValidRect(r1));
    
    RECT r2 = {0, 0, 10, 10};
    DF_ASSERT_TRUE(ValidRect(r2));
}

static void test_subrectangle_overlap(void) {
    RECT outer = {0, 0, 20, 10};
    RECT inner = {5, 3, 15, 8};
    RECT result = subRectangle(outer, inner);
    
    DF_ASSERT_RECT_VALID(result);
    DF_ASSERT_INT_EQ(result.lf, 5);
    DF_ASSERT_INT_EQ(result.tp, 3);
    DF_ASSERT_INT_EQ(result.rt, 15);
    DF_ASSERT_INT_EQ(result.bt, 8);
}

static void test_subrectangle_no_overlap(void) {
    RECT r1 = {0, 0, 10, 10};
    RECT r2 = {20, 20, 30, 30};
    RECT result = subRectangle(r1, r2);
    
    DF_ASSERT_INT_EQ(result.lf, 0);
    DF_ASSERT_INT_EQ(result.tp, 0);
    DF_ASSERT_INT_EQ(result.rt, 0);
    DF_ASSERT_INT_EQ(result.bt, 0);
}

static void test_subrectangle_partial(void) {
    RECT r1 = {0, 0, 10, 10};
    RECT r2 = {5, 5, 20, 20};
    RECT result = subRectangle(r1, r2);
    
    DF_ASSERT_RECT_VALID(result);
    DF_ASSERT_INT_EQ(result.lf, 5);
    DF_ASSERT_INT_EQ(result.tp, 5);
    DF_ASSERT_INT_EQ(result.rt, 10);
    DF_ASSERT_INT_EQ(result.bt, 10);
}

static void test_rect_accessors(void) {
    RECT r;
    RectLeft(r) = 1;
    RectTop(r) = 2;
    RectRight(r) = 3;
    RectBottom(r) = 4;
    
    DF_ASSERT_INT_EQ(RectLeft(r), 1);
    DF_ASSERT_INT_EQ(RectTop(r), 2);
    DF_ASSERT_INT_EQ(RectRight(r), 3);
    DF_ASSERT_INT_EQ(RectBottom(r), 4);
}

DF_TEST_SUITE(rect_tests)
    DF_TEST_CASE(test_rect_width)
    DF_TEST_CASE(test_rect_height)
    DF_TEST_CASE(test_rect_width_height_different)
    DF_TEST_CASE(test_rect_inside_true)
    DF_TEST_CASE(test_rect_inside_false)
    DF_TEST_CASE(test_rect_valid_macro)
    DF_TEST_CASE(test_subrectangle_overlap)
    DF_TEST_CASE(test_subrectangle_no_overlap)
    DF_TEST_CASE(test_subrectangle_partial)
    DF_TEST_CASE(test_rect_accessors)
DF_TEST_SUITE_END(rect_tests)