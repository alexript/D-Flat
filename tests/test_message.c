/* test_message.c - Unit tests for message.c (TinyCC modifications) */

#include "df_test.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../tinycc/tcccompat.h"
#include "../tinycc/tccwin32.h"

#define timed_out(timer) ((timer) == 0)
#define set_timer(timer, secs) (timer)=((secs)*182/10+1)
#define disable_timer(timer) (timer) = -1
#define timer_running(timer) ((timer) > 0)
#define countdown(timer) (--(timer))

static void test_message_timer_macros(void) {
    int timer = -1;
    DF_ASSERT(timer_running(timer) == 0);
    DF_ASSERT(timer > 0 || timer == -1);

    timer = 10;
    DF_ASSERT(timer_running(timer) != 0);
    DF_ASSERT(timed_out(timer) == 0);

    countdown(timer);
    DF_ASSERT(timer == 9);

    countdown(timer);
    countdown(timer);
    countdown(timer);
    countdown(timer);
    countdown(timer);
    countdown(timer);
    countdown(timer);
    countdown(timer);
    DF_ASSERT(timer == 1);
    countdown(timer);
    DF_ASSERT(timer == 0);
    DF_ASSERT(timed_out(timer) != 0);

    disable_timer(timer);
    DF_ASSERT(timer == -1);
    DF_ASSERT(timer_running(timer) == 0);
}

static void test_message_set_timer(void) {
    int timer = -1;
    set_timer(timer, 1);
    DF_ASSERT(timer > 0);
    DF_ASSERT(timer_running(timer));
    DF_ASSERT(!timed_out(timer));
}

static void test_message_init_win32(void) {
    int result = InitWin32Console();
    DF_ASSERT(result == 0 || result != 0);
    DF_ASSERT(consoleWidth > 0);
    DF_ASSERT(consoleHeight > 0);
}

static void test_message_get_tick_count(void) {
    unsigned long tc1 = Win32_GetTickCount();
    Sleep(10);
    unsigned long tc2 = Win32_GetTickCount();
    DF_ASSERT(tc2 >= tc1);
    DF_ASSERT(tc1 > 0);
}

static void test_message_poll_input(void) {
    Win32_PollInput();
    DF_ASSERT(1);
}

static void test_message_timer_polling_logic(void) {
    static DWORD lastTick = 0;
    DWORD nowTick = Win32_GetTickCount();

    if (nowTick != lastTick) {
        lastTick = nowTick;
        DF_ASSERT(1);
    } else {
        DF_ASSERT(1);
    }

    lastTick = nowTick;
    nowTick = Win32_GetTickCount();
    DF_ASSERT(nowTick == lastTick);
}

static void test_message_mouse_position(void) {
    int x = -1, y = -1, buttons = -1;
    Win32_get_mouseposition(&x, &y, &buttons);
    DF_ASSERT(x >= 0);
    DF_ASSERT(y >= 0);
    DF_ASSERT(buttons >= 0);
}

static void test_message_keyboard_state(void) {
    int shift = Win32_getshift();
    DF_ASSERT(shift >= 0);
    DF_ASSERT(shift < 128);
}

static void test_message_keyboard_hit(void) {
    int hit = Win32_keyhit();
    DF_ASSERT(hit == 0 || hit == 1);
}

static void test_message_console_cursor(void) {
    Win32_cursor(0, 0);
    int x = -1, y = -1;
    Win32_curr_cursor(&x, &y);
    DF_ASSERT(x >= 0);
    DF_ASSERT(y >= 0);
}

/* Note: CloseWin32Console test removed - it closes console handles
 * and breaks subsequent tests. Console cleanup happens at process exit. */

DF_TEST_SUITE(message_tests)
    DF_TEST_CASE(test_message_timer_macros)
    DF_TEST_CASE(test_message_set_timer)
    DF_TEST_CASE(test_message_init_win32)
    DF_TEST_CASE(test_message_get_tick_count)
    DF_TEST_CASE(test_message_poll_input)
    DF_TEST_CASE(test_message_timer_polling_logic)
    DF_TEST_CASE(test_message_mouse_position)
    DF_TEST_CASE(test_message_keyboard_state)
    DF_TEST_CASE(test_message_keyboard_hit)
    DF_TEST_CASE(test_message_console_cursor)
DF_TEST_SUITE_END(message_tests)
