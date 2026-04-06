/* test_tccwin32.c - Unit tests for tinycc tccwin32 layer */

#include "df_test.h"

#define far
#define near
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../tinycc/tccwin32.h"
#undef far
#undef near

#include "../tinycc/tcccompat.h"

static void test_tccwin32_global_vars(void) {
    DF_ASSERT(consoleWidth > 0);
    DF_ASSERT(consoleHeight > 0);
    DF_ASSERT_EQ(videoMode, 3);
    DF_ASSERT_EQ(textAttribute, 0x07);
}

static void test_tccwin32_mouse_installed(void) {
    int result = Win32_mouse_installed();
    DF_ASSERT(result == TRUE || result == FALSE);
}

static void test_tccwin32_mousebuttons(void) {
    int buttons = Win32_mousebuttons();
    DF_ASSERT(buttons >= 0);
    DF_ASSERT(buttons <= 3);
}

static void test_tccwin32_getdisk(void) {
    char disk = Win32_getdisk();
    DF_ASSERT(disk >= 0);
    DF_ASSERT(disk < 26);
}

static void test_tccwin32_getvideomode(void) {
    int mode = Win32_getvideomode();
    DF_ASSERT(mode >= 0);
}

static void test_tccwin32_getvect(void) {
    void *vec = Win32_getvect(0x10);
    DF_ASSERT_NULL(vec);
}

static void test_tccwin32_setvect(void) {
    Win32_setvect(0x10, NULL);
    DF_ASSERT(1);
}

static void test_tccwin32_tickcount(void) {
    unsigned long tc1 = Win32_GetTickCount();
    Sleep(10);
    unsigned long tc2 = Win32_GetTickCount();
    DF_ASSERT(tc2 >= tc1);
}

static void test_tccwin32_time(void) {
    time_t t = Win32_time(NULL);
    DF_ASSERT(t > 0);
}

static void test_tccwin32_beep(void) {
    Win32_beep();
    DF_ASSERT(1);
}

static void test_tccwin32_colors(void) {
    DF_ASSERT_EQ(BLACK, 0);
    DF_ASSERT_EQ(WHITE, 15);
}

static void test_tccwin32_ffblk(void) {
    struct ffblk fb;
    fb.ff_attrib = FA_RDONLY;
    fb.ff_fsize = 1000;
    DF_ASSERT_EQ(fb.ff_attrib, 1);
    DF_ASSERT_EQ(fb.ff_fsize, 1000);
}

DF_TEST_SUITE(tccwin32_tests)
    DF_TEST_CASE(test_tccwin32_global_vars)
    DF_TEST_CASE(test_tccwin32_mouse_installed)
    DF_TEST_CASE(test_tccwin32_mousebuttons)
    DF_TEST_CASE(test_tccwin32_getdisk)
    DF_TEST_CASE(test_tccwin32_getvideomode)
    DF_TEST_CASE(test_tccwin32_getvect)
    DF_TEST_CASE(test_tccwin32_setvect)
    DF_TEST_CASE(test_tccwin32_tickcount)
    DF_TEST_CASE(test_tccwin32_time)
    DF_TEST_CASE(test_tccwin32_beep)
    DF_TEST_CASE(test_tccwin32_colors)
    DF_TEST_CASE(test_tccwin32_ffblk)
DF_TEST_SUITE_END(tccwin32_tests)