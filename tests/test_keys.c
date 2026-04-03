/* test_keys.c - Unit tests for keys.c */

#include "df_test.h"
#include "test_dflat.h"

/* Test the keys array structure */

static void test_keys_array_not_null(void) {
    DF_ASSERT_NOT_NULL(keys);
}

static void test_keys_f1_exists(void) {
    int found = 0;
    for (int i = 0; keys[i].keylabel != NULL; i++) {
        if (keys[i].keycode == F1) {
            found = 1;
            DF_ASSERT_STR_EQ(keys[i].keylabel, "F1");
            break;
        }
    }
    DF_ASSERT_TRUE(found);
}

static void test_keys_f10_exists(void) {
    int found = 0;
    for (int i = 0; keys[i].keylabel != NULL; i++) {
        if (keys[i].keycode == F10) {
            found = 1;
            DF_ASSERT_STR_EQ(keys[i].keylabel, "F10");
            break;
        }
    }
    DF_ASSERT_TRUE(found);
}

static void test_keys_ctrl_keys(void) {
    int found = 0;
    for (int i = 0; keys[i].keylabel != NULL; i++) {
        if (keys[i].keycode == CTRL_F1) {
            found = 1;
            DF_ASSERT_STR_EQ(keys[i].keylabel, "Ctrl+F1");
            break;
        }
    }
    DF_ASSERT_TRUE(found);
}

static void test_keys_alt_keys(void) {
    int found = 0;
    for (int i = 0; keys[i].keylabel != NULL; i++) {
        if (keys[i].keycode == ALT_F1) {
            found = 1;
            DF_ASSERT_STR_EQ(keys[i].keylabel, "Alt+F1");
            break;
        }
    }
    DF_ASSERT_TRUE(found);
}

static void test_keys_arrow_keys(void) {
    int found_up = 0, found_dn = 0, found_home = 0, found_end = 0;
    for (int i = 0; keys[i].keylabel != NULL; i++) {
        if (keys[i].keycode == UP) found_up = 1;
        if (keys[i].keycode == DN) found_dn = 1;
        if (keys[i].keycode == HOME) found_home = 1;
        if (keys[i].keycode == END) found_end = 1;
    }
    DF_ASSERT_TRUE(found_up);
    DF_ASSERT_TRUE(found_dn);
    DF_ASSERT_TRUE(found_home);
    DF_ASSERT_TRUE(found_end);
}

static void test_keys_alt_a_to_z(void) {
    int found = 0;
    for (int i = 0; keys[i].keylabel != NULL; i++) {
        if (keys[i].keycode == ALT_A) {
            found = 1;
            DF_ASSERT_STR_EQ(keys[i].keylabel, "Alt+A");
            break;
        }
    }
    DF_ASSERT_TRUE(found);
}

static void test_keys_ctrl_a_to_z(void) {
    DF_ASSERT_INT_EQ(CTRL_A, 1);
    DF_ASSERT_INT_EQ(CTRL_B, 2);
    DF_ASSERT_INT_EQ(CTRL_C, 3);
    DF_ASSERT_INT_EQ(CTRL_Z, 26);
}

static void test_keys_special_key_offsets(void) {
    DF_ASSERT_INT_EQ(F1, 187 + 0x1000);
    DF_ASSERT_INT_EQ(F10, 196 + 0x1000);
    DF_ASSERT_INT_EQ(UP, 200 + 0x1000);
}

static void test_keys_shift_flags(void) {
    DF_ASSERT_INT_EQ(RIGHTSHIFT, 0x01);
    DF_ASSERT_INT_EQ(LEFTSHIFT, 0x02);
    DF_ASSERT_INT_EQ(CTRLKEY, 0x04);
    DF_ASSERT_INT_EQ(ALTKEY, 0x08);
    DF_ASSERT_INT_EQ(SCROLLLOCK, 0x10);
    DF_ASSERT_INT_EQ(NUMLOCK, 0x20);
    DF_ASSERT_INT_EQ(CAPSLOCK, 0x40);
    DF_ASSERT_INT_EQ(INSERTKEY, 0x80);
}

DF_TEST_SUITE(keys_tests)
    DF_TEST_CASE(test_keys_array_not_null)
    DF_TEST_CASE(test_keys_f1_exists)
    DF_TEST_CASE(test_keys_f10_exists)
    DF_TEST_CASE(test_keys_ctrl_keys)
    DF_TEST_CASE(test_keys_alt_keys)
    DF_TEST_CASE(test_keys_arrow_keys)
    DF_TEST_CASE(test_keys_alt_a_to_z)
    DF_TEST_CASE(test_keys_ctrl_a_to_z)
    DF_TEST_CASE(test_keys_special_key_offsets)
    DF_TEST_CASE(test_keys_shift_flags)
DF_TEST_SUITE_END(keys_tests)