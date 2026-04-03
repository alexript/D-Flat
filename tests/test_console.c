/* test_console.c - Tests for Console Abstraction Layer (console.h) */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../tinycc/tcccompat.h"
#include "../console.h"
#include "df_test.h"

static int consoleInitialized = 0;

static void console_setup(void) {
    if (!consoleInitialized) {
        consoleInitialized = 1;
    }
}

static void console_teardown(void) {
}

/* Platform type tests */
static void test_platform_type_enum_exists(void) {
    PLATFORM_TYPE pt = PLATFORM_DOS;
    DF_ASSERT_INT_EQ(pt, 0);
    pt = PLATFORM_WIN32;
    DF_ASSERT_INT_EQ(pt, 1);
    pt = PLATFORM_UNIX;
    DF_ASSERT_INT_EQ(pt, 2);
    pt = PLATFORM_ANSI;
    DF_ASSERT_INT_EQ(pt, 3);
    pt = PLATFORM_UNKNOWN;
    DF_ASSERT_INT_EQ(pt, 4);
}

/* Size retrieval tests */
static void test_console_getsize_basic(void) {
    int width = -1;
    int height = -1;
    Console_GetSize(&width, &height);
    DF_ASSERT(width > 0);
    DF_ASSERT(height > 0);
    DF_ASSERT(width <= 80);
    DF_ASSERT(height <= 50);
}

/* Cursor tests */
static void test_console_set_cursor_basic(void) {
    Console_SetCursor(0, 0);
    int x = -1, y = -1;
    Console_GetCursor(&x, &y);
    DF_ASSERT_INT_EQ(x, 0);
    DF_ASSERT_INT_EQ(y, 0);
}

static void test_console_cursor_range(void) {
    Console_SetCursor(10, 5);
    int x = -1, y = -1;
    Console_GetCursor(&x, &y);
    DF_ASSERT_INT_EQ(x, 10);
    DF_ASSERT_INT_EQ(y, 5);
}

/* Cursor visibility tests */
static void test_console_hide_show_cursor(void) {
    Console_ShowCursor();
    Console_HideCursor();
    Console_ShowCursor();
}

/* Text attribute tests */
static void test_console_set_get_attribute(void) {
    Console_SetTextAttribute(0x07);
    int attr = Console_GetTextAttribute();
    DF_ASSERT_INT_EQ(attr, 0x07);
    
    Console_SetTextAttribute(0x1F);
    attr = Console_GetTextAttribute();
    DF_ASSERT_INT_EQ(attr, 0x1F);
}

/* Keyboard tests */
static void test_console_kbhit_basic(void) {
    int result = Console_KbHit();
    DF_ASSERT(result == 0 || result == 1);
}

/* Video mode tests */
static void test_console_get_videomode(void) {
    int mode = Console_GetVideoMode();
    DF_ASSERT(mode >= 0);
}

/* Timer tests */
static void test_console_get_tick_count_positive(void) {
    unsigned long t1 = Console_GetTickCount();
    DF_ASSERT(t1 > 0);
}

static void test_console_tick_count_increases(void) {
    unsigned long t1 = Console_GetTickCount();
    Sleep(10);
    unsigned long t2 = Console_GetTickCount();
    DF_ASSERT(t2 >= t1);
}

/* Video operations - basic buffer tests */
static void test_console_read_screen_null_buffer(void) {
    char buf[80 * 25 * 2];
    Console_ReadScreen(0, 0, 80, 25, buf);
}

static void test_console_write_screen_basic(void) {
    char buf[80 * 25 * 2];
    memset(buf, 0, sizeof(buf));
    Console_WriteScreen(0, 0, 80, 25, buf);
}

/* Char operations */
static void test_console_put_char_basic(void) {
    Console_PutChar(0, 0, 'A', 0x07);
    int ch = Console_GetChar(0, 0);
    DF_ASSERT_INT_EQ(ch & 0xFF, 'A');
}

/* Beep test */
static void test_console_beep(void) {
    Console_Beep();
}

/* Drive operations */
static void test_console_get_current_drive(void) {
    char drive = Console_GetCurrentDrive();
    DF_ASSERT(drive >= 0);
    DF_ASSERT(drive < 26);
}

/* Directory operations */
static void test_console_get_current_dir(void) {
    char path[MAX_PATH];
    char *result = Console_GetCurrentDir(0, path);
    DF_ASSERT_NOT_NULL(result);
    DF_ASSERT(result[0] != '\0');
}

/* Change directory - restore original */
static void test_console_change_dir_and_back(void) {
    char origPath[MAX_PATH];
    Console_GetCurrentDir(0, origPath);
    Console_ChangeDir(origPath);
}

DF_TEST_SUITE(console_tests)

    DF_TEST_CASE(test_platform_type_enum_exists)
    DF_TEST_CASE(test_console_getsize_basic)
    DF_TEST_CASE(test_console_set_cursor_basic)
    DF_TEST_CASE(test_console_cursor_range)
    DF_TEST_CASE(test_console_hide_show_cursor)
    DF_TEST_CASE(test_console_set_get_attribute)
    DF_TEST_CASE(test_console_kbhit_basic)
    DF_TEST_CASE(test_console_get_videomode)
    DF_TEST_CASE(test_console_get_tick_count_positive)
    DF_TEST_CASE(test_console_tick_count_increases)
    DF_TEST_CASE(test_console_read_screen_null_buffer)
    DF_TEST_CASE(test_console_write_screen_basic)
    DF_TEST_CASE(test_console_put_char_basic)
    DF_TEST_CASE(test_console_beep)
    DF_TEST_CASE(test_console_get_current_drive)
    DF_TEST_CASE(test_console_get_current_dir)
    DF_TEST_CASE(test_console_change_dir_and_back)

DF_TEST_SUITE_END(console_tests)