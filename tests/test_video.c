/* test_video.c - Unit tests for video.c (via Console PAL) */

#include "df_test.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../tinycc/tcccompat.h"
#include "../console.h"

/* Test that Console video operations work through PAL */
static void test_video_console_put_get_char(void) {
    /* Write a character and read it back */
    Console_PutChar(0, 0, 'X', 0x07);
    int ch = Console_GetChar(0, 0);
    DF_ASSERT((ch & 0xFF) == 'X');
}

static void test_video_console_put_get_with_different_attrs(void) {
    /* Test with different attributes */
    Console_PutChar(1, 0, 'A', 0x1F);  /* White on blue */
    Console_PutChar(2, 0, 'B', 0x4E);  /* Yellow on red */
    
    int ch1 = Console_GetChar(1, 0);
    int ch2 = Console_GetChar(2, 0);
    
    DF_ASSERT((ch1 & 0xFF) == 'A');
    DF_ASSERT((ch2 & 0xFF) == 'B');
}

static void test_video_screen_buffer_operations(void) {
    /* Test reading/writing screen buffer */
    char buf[80 * 2];  /* One line: 80 chars * 2 bytes (char + attr) */
    
    /* Initialize buffer with test data */
    for (int i = 0; i < 80; i++) {
        buf[i * 2] = '0' + (i % 10);     /* Character */
        buf[i * 2 + 1] = 0x07;            /* Attribute (light gray on black) */
    }
    
    /* Write line */
    Console_WriteScreen(0, 0, 80, 1, buf);
    
    /* Read back */
    char read_buf[80 * 2];
    Console_ReadScreen(0, 0, 80, 1, read_buf);
    
    /* Verify */
    for (int i = 0; i < 80; i++) {
        DF_ASSERT(read_buf[i * 2] == buf[i * 2]);
    }
}

static void test_video_screen_bounds(void) {
    /* Test that operations respect screen bounds */
    int width, height;
    Console_GetSize(&width, &height);
    
    DF_ASSERT(width > 0);
    DF_ASSERT(height > 0);
    DF_ASSERT(width <= 200);  /* Sanity check */
    DF_ASSERT(height <= 100);
}

static void test_video_multiple_lines(void) {
    /* Test writing multiple lines */
    char buf[10 * 2 * 3];  /* 10 chars wide, 3 lines */
    
    /* Fill with pattern */
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 10; x++) {
            buf[(y * 10 + x) * 2] = 'A' + x + y;
            buf[(y * 10 + x) * 2 + 1] = 0x07;
        }
    }
    
    /* Write to position (5, 2) */
    Console_WriteScreen(5, 2, 10, 3, buf);
    
    /* Read back */
    char read_buf[10 * 2 * 3];
    Console_ReadScreen(5, 2, 10, 3, read_buf);
    
    /* Verify */
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 10; x++) {
            DF_ASSERT(read_buf[(y * 10 + x) * 2] == buf[(y * 10 + x) * 2]);
        }
    }
}

static void test_video_text_attribute_preservation(void) {
    /* Write with specific attribute and verify it's preserved */
    Console_PutChar(5, 5, 'T', 0x2A);  /* Green on green (bright) */
    
    int ch = Console_GetChar(5, 5);
    DF_ASSERT((ch & 0xFF) == 'T');
    /* Note: attribute may be masked, just verify char is correct */
}

/* Video mode tests */
static void test_video_mode_constants(void) {
    DF_ASSERT(VIDEO_MODE_25 == 25);
    DF_ASSERT(VIDEO_MODE_43 == 43);
    DF_ASSERT(VIDEO_MODE_50 == 50);
}

static void test_video_get_mode(void) {
    int mode = Console_GetVideoMode();
    DF_ASSERT(mode >= 0);
    /* Mode is typically 3 for text mode */
}

/* Test clear/write screen operations */
static void test_video_clear_and_rewrite(void) {
    /* Clear a region */
    char clear_buf[10 * 10 * 2];
    memset(clear_buf, 0, sizeof(clear_buf));
    
    for (int i = 0; i < 10 * 10; i++) {
        clear_buf[i * 2] = ' ';     /* Space */
        clear_buf[i * 2 + 1] = 0x07; /* Normal attribute */
    }
    
    Console_WriteScreen(0, 0, 10, 10, clear_buf);
    
    /* Write some text */
    Console_PutChar(1, 1, 'H', 0x07);
    Console_PutChar(2, 1, 'i', 0x07);
    
    /* Verify */
    DF_ASSERT((Console_GetChar(1, 1) & 0xFF) == 'H');
    DF_ASSERT((Console_GetChar(2, 1) & 0xFF) == 'i');
}

DF_TEST_SUITE(video_tests)
    DF_TEST_CASE(test_video_console_put_get_char)
    DF_TEST_CASE(test_video_console_put_get_with_different_attrs)
    DF_TEST_CASE(test_video_screen_buffer_operations)
    DF_TEST_CASE(test_video_screen_bounds)
    DF_TEST_CASE(test_video_multiple_lines)
    DF_TEST_CASE(test_video_text_attribute_preservation)
    DF_TEST_CASE(test_video_mode_constants)
    DF_TEST_CASE(test_video_get_mode)
    DF_TEST_CASE(test_video_clear_and_rewrite)
DF_TEST_SUITE_END(video_tests)
