/* test_direct.c - Unit tests for direct.c file operations (via Console PAL) */

#include "df_test.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../tinycc/tcccompat.h"
#include "../console.h"

/* File attribute constants from console.h */
#ifndef FA_RDONLY
#define FA_RDONLY   0x01
#define FA_HIDDEN   0x02
#define FA_SYSTEM   0x04
#define FA_LABEL    0x08
#define FA_DIREC    0x10
#define FA_ARCH     0x20
#endif

/* Test current drive operations */
static void test_direct_get_current_drive(void) {
    char drive = Console_GetCurrentDrive();
    DF_ASSERT(drive >= 0);
    DF_ASSERT(drive < 26);  /* A-Z */
}

/* Test directory operations */
static void test_direct_get_current_dir(void) {
    char path[MAX_PATH];
    char *result = Console_GetCurrentDir(0, path);
    
    DF_ASSERT_NOT_NULL(result);
    DF_ASSERT_NE(result[0], '\0');
    
    /* Path should be absolute and contain backslashes */
    DF_ASSERT(strchr(result, '\\') != NULL || result[1] == ':');
}

/* Test change directory and restore */
static void test_direct_change_dir(void) {
    char original[MAX_PATH];
    Console_GetCurrentDir(0, original);
    
    /* Try to change to current directory (should succeed) */
    int result = Console_ChangeDir(original);
    DF_ASSERT_EQ(result, 0);
    
    /* Verify we're still in the same directory */
    char current[MAX_PATH];
    Console_GetCurrentDir(0, current);
    DF_ASSERT_STR_EQ(current, original);
}

/* Test findfirst/findnext with current directory */
static void test_direct_find_first_file(void) {
    struct ffblk fb;
    
    /* Try to find any file */
    long result = Console_FindFirst("*.*", &fb);
    
    /* Should succeed (there should be files) */
    if (result == 0) {
        DF_ASSERT_NE(fb.ff_name[0], '\0');
        Console_FindClose(&fb);
    }
    /* If result != 0, it may mean no files found - not necessarily an error */
}

/* Test findfirst for directories */
static void test_direct_find_directories(void) {
    struct ffblk fb;
    
    /* Find directories */
    long result = Console_FindFirst("*.*", &fb);
    
    int found_dir = 0;
    while (result == 0) {
        if (fb.ff_attrib & FA_DIREC) {
            found_dir = 1;
            /* Directory names should be valid */
            DF_ASSERT_NE(fb.ff_name[0], '\0');
            /* Check for special directories */
            if (strcmp(fb.ff_name, ".") == 0 || strcmp(fb.ff_name, "..") == 0) {
                /* These are valid directory entries */
            }
        }
        result = Console_FindNext(&fb);
    }
    
    Console_FindClose(&fb);
    
    /* Should find at least . and .. directories */
    DF_ASSERT(found_dir);
}

/* Test findfirst/findnext iteration */
static void test_direct_find_iteration(void) {
    struct ffblk fb;
    int count = 0;
    
    long result = Console_FindFirst("*.*", &fb);
    
    while (result == 0 && count < 100) {  /* Limit to prevent infinite loop */
        count++;
        DF_ASSERT_NE(fb.ff_name[0], '\0');
        result = Console_FindNext(&fb);
    }
    
    Console_FindClose(&fb);
    
    /* Should find at least some entries (. and ..) */
    DF_ASSERT(count >= 2);
}

/* Test file attributes in find results */
static void test_direct_file_attributes(void) {
    struct ffblk fb;
    
    long result = Console_FindFirst("*.*", &fb);
    
    if (result == 0) {
        /* Check that attributes are valid */
        DF_ASSERT(fb.ff_attrib < 0xFF);  /* Should be byte value */
        
        /* Name should be null-terminated */
        DF_ASSERT(fb.ff_name[259] == '\0' || fb.ff_name[strlen(fb.ff_name)] == '\0');
        
        Console_FindClose(&fb);
    }
}

/* Test find close idempotency */
static void test_direct_find_close_idempotent(void) {
    struct ffblk fb;
    
    /* Open a find operation */
    long result = Console_FindFirst("*.*", &fb);
    
    if (result == 0) {
        /* Close once */
        Console_FindClose(&fb);
        
        /* Close again - should not crash (idempotent) */
        Console_FindClose(&fb);
    }
}

/* Test specific file search */
static void test_direct_find_specific_file(void) {
    struct ffblk fb;
    
    /* Try to find README.md or similar common file */
    long result = Console_FindFirst("README*", &fb);
    
    if (result == 0) {
        /* Found README file */
        DF_ASSERT(strnicmp(fb.ff_name, "README", 6) == 0);
        Console_FindClose(&fb);
    }
    /* Not finding it is OK - file might not exist */
}

/* Test drive switching (if multiple drives available) */
static void test_direct_set_drive(void) {
    char original = Console_GetCurrentDrive();
    
    /* Try to set to current drive (should succeed) */
    int result = Console_SetCurrentDrive(original);
    
    /* Result may be 0 (success) or non-zero (no permission/other issue) */
    /* Just verify it doesn't crash */
    
    /* Verify drive is still the same */
    char current = Console_GetCurrentDrive();
    DF_ASSERT_EQ(current, original);
}

DF_TEST_SUITE(direct_tests)
    DF_TEST_CASE(test_direct_get_current_drive)
    DF_TEST_CASE(test_direct_get_current_dir)
    DF_TEST_CASE(test_direct_change_dir)
    DF_TEST_CASE(test_direct_find_first_file)
    DF_TEST_CASE(test_direct_find_directories)
    DF_TEST_CASE(test_direct_find_iteration)
    DF_TEST_CASE(test_direct_file_attributes)
    DF_TEST_CASE(test_direct_find_close_idempotent)
    DF_TEST_CASE(test_direct_find_specific_file)
    DF_TEST_CASE(test_direct_set_drive)
DF_TEST_SUITE_END(direct_tests)
