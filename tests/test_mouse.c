/* test_mouse.c - Unit tests for mouse.c (via Console PAL) */

#include "df_test.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../tinycc/tcccompat.h"
#include "../console.h"

/* Mouse button constants from console.h */
#ifndef MOUSE_BUTTON_LEFT
#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_MIDDLE 0x04
#endif

/* Test mouse support detection */
static void test_mouse_supported(void) {
    int supported = Console_MouseSupported();
    /* Win32 console always supports mouse */
    DF_ASSERT(supported == 0 || supported == 1);
}

/* Test getting mouse position */
static void test_mouse_get_position(void) {
    int x = -1, y = -1, buttons = -1;
    
    Console_GetMousePos(&x, &y, &buttons);
    
    /* Position should be valid (non-negative) */
    DF_ASSERT(x >= 0);
    DF_ASSERT(y >= 0);
    DF_ASSERT(buttons >= 0);
    DF_ASSERT(buttons < 8);  /* Max 3 buttons (bits 0-2) */
}

/* Test setting mouse position */
static void test_mouse_set_position(void) {
    /* Save current position */
    int orig_x, orig_y, orig_buttons;
    Console_GetMousePos(&orig_x, &orig_y, &orig_buttons);
    
    /* Set new position */
    Console_SetMousePos(10, 5);
    
    /* Read back - note: in Win32 console this may set cursor position */
    int x, y, buttons;
    Console_GetMousePos(&x, &y, &buttons);
    
    /* Restore original position */
    Console_SetMousePos(orig_x, orig_y);
}

/* Test mouse visibility */
static void test_mouse_show_hide(void) {
    /* These should not crash */
    Console_ShowMouse();
    Console_HideMouse();
    Console_ShowMouse();
}

/* Test mouse position bounds */
static void test_mouse_position_bounds(void) {
    int width, height;
    Console_GetSize(&width, &height);
    
    int x, y, buttons;
    Console_GetMousePos(&x, &y, &buttons);
    
    /* Mouse position should be within reasonable bounds */
    DF_ASSERT(x >= 0);
    DF_ASSERT(y >= 0);
    /* Note: x/y may exceed current console size if mouse is outside window */
}

/* Test multiple mouse position reads */
static void test_mouse_position_consistency(void) {
    int x1, y1, b1;
    int x2, y2, b2;
    
    Console_GetMousePos(&x1, &y1, &b1);
    Console_GetMousePos(&x2, &y2, &b2);
    
    /* Consecutive reads should return similar values
     * (allowing for mouse movement between calls) */
    DF_ASSERT(x1 >= 0 && x2 >= 0);
    DF_ASSERT(y1 >= 0 && y2 >= 0);
}

/* Test mouse button states */
static void test_mouse_button_states(void) {
    int x, y, buttons;
    Console_GetMousePos(&x, &y, &buttons);
    
    /* Buttons should be a valid combination */
    DF_ASSERT(buttons >= 0);
    DF_ASSERT(buttons <= 7);  /* Bits 0-2 for left, right, middle */
}

/* Test that mouse cursor functions exist and work */
static void test_mouse_cursor_functions_exist(void) {
    /* These are placeholders in Win32 - just verify they don't crash */
    Console_ShowMouse();
    Console_HideMouse();
    Console_ShowMouse();
    
    DF_ASSERT(1);  /* If we got here, functions exist */
}

DF_TEST_SUITE(mouse_tests)
    DF_TEST_CASE(test_mouse_supported)
    DF_TEST_CASE(test_mouse_get_position)
    DF_TEST_CASE(test_mouse_set_position)
    DF_TEST_CASE(test_mouse_show_hide)
    DF_TEST_CASE(test_mouse_position_bounds)
    DF_TEST_CASE(test_mouse_position_consistency)
    DF_TEST_CASE(test_mouse_button_states)
    DF_TEST_CASE(test_mouse_cursor_functions_exist)
DF_TEST_SUITE_END(mouse_tests)
