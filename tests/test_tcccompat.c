/* test_tcccompat.c - Unit tests for tinycc compatibility layer */

#include "df_test.h"

#define far
#define near
#include <windows.h>
#include "../tinycc/tcccompat.h"

typedef union {
    struct {
        unsigned int ax, bx, cx, dx, si, di, cflag, flags;
    } x;
    struct {
        unsigned char al, ah, bl, bh, cl, ch, dl, dh;
    } h;
} TEST_REGS;

static void test_tcccompat_far_near(void) {
    void *ptr = NULL;
    (void)ptr;
    DF_ASSERT_NULL(ptr);
}

static void test_tcccompat_mk_fp(void) {
    DF_ASSERT_EQ(C80, 3);
    DF_ASSERT_EQ(SCREENWIDTH, 80);
}

static void test_tcccompat_regs_union(void) {
    TEST_REGS r;
    r.x.ax = 0x1234;
    DF_ASSERT_EQ(r.h.al, 0x34);
    DF_ASSERT_EQ(r.h.ah, 0x12);
}

static void test_tcccompat_ffblk_struct(void) {
    struct ffblk fb;
    fb.ff_attrib = FA_RDONLY;
    fb.ff_fsize = 1000;
    DF_ASSERT_EQ(fb.ff_attrib, 1);
    DF_ASSERT_EQ(fb.ff_fsize, 1000);
}

static void test_tcccompat_colors(void) {
    DF_ASSERT_EQ(BLACK, 0);
    DF_ASSERT_EQ(WHITE, 15);
    DF_ASSERT_EQ(BLINK, 0x80);
    DF_ASSERT_EQ(attr(WHITE, BLACK), 0x0F);
}

static void test_tcccompat_video_modes(void) {
    DF_ASSERT_EQ(C80, 3);
    DF_ASSERT_EQ(C4350, 8);
}

static void test_tcccompat_bool_enum(void) {
    BOOL b = TRUE;
    DF_ASSERT_EQ(b, TRUE);
    b = FALSE;
    DF_ASSERT_EQ(b, FALSE);
}

static void test_tcccompat_null_defined(void) {
    void *p = NULL;
    DF_ASSERT_NULL(p);
}

static void test_tcccompat_param_type(void) {
    PARAM p = 0x12345678;
    DF_ASSERT_EQ(p, 0x12345678);
}

DF_TEST_SUITE(tcccompat_tests)
    DF_TEST_CASE(test_tcccompat_far_near)
    DF_TEST_CASE(test_tcccompat_mk_fp)
    DF_TEST_CASE(test_tcccompat_regs_union)
    DF_TEST_CASE(test_tcccompat_ffblk_struct)
    DF_TEST_CASE(test_tcccompat_colors)
    DF_TEST_CASE(test_tcccompat_video_modes)
    DF_TEST_CASE(test_tcccompat_bool_enum)
    DF_TEST_CASE(test_tcccompat_null_defined)
    DF_TEST_CASE(test_tcccompat_param_type)
DF_TEST_SUITE_END(tcccompat_tests)