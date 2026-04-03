/* console.h - Platform Abstraction Layer (PAL) for D-Flat
 * 
 * This header provides a platform-independent interface for console operations,
 * allowing D-Flat to run on DOS, Win32 (Console API), and Unix (ncurses).
 */

#ifndef CONSOLE_ABSTRACTION_H
#define CONSOLE_ABSTRACTION_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============ Platform Type ============ */

typedef enum {
    PLATFORM_DOS = 0,
    PLATFORM_WIN32 = 1,
    PLATFORM_UNIX = 2,
    PLATFORM_ANSI = 3,
    PLATFORM_UNKNOWN = 4
} PLATFORM_TYPE;

/* ============ Initialization ============ */

int Console_Init(void);
void Console_Close(void);
PLATFORM_TYPE Console_GetPlatformType(void);

/* ============ Screen Size ============ */

void Console_GetSize(int *width, int *height);
int Console_GetWidth(void);
int Console_GetHeight(void);

/* ============ Video Operations ============ */

void Console_ReadScreen(int x, int y, int w, int h, void *buf);
void Console_WriteScreen(int x, int y, int w, int h, void *buf);

int Console_GetChar(int x, int y);
void Console_PutChar(int x, int y, int ch, int attr);

/* ============ Cursor Control ============ */

void Console_SetCursor(int x, int y);
void Console_GetCursor(int *x, int *y);
void Console_HideCursor(void);
void Console_ShowCursor(void);
void Console_SetCursorType(int type);

#define CURSOR_NORMAL  0
#define CURSOR_BLOCK   1
#define CURSOR_UNDERLINE 2

/* ============ Text Attributes ============ */

void Console_SetTextAttribute(int attr);
int Console_GetTextAttribute(void);

/* ============ Keyboard Input ============ */

int Console_KbHit(void);
int Console_GetKey(void);
int Console_GetShiftState(void);

#define KB_SHIFT   0x01
#define KB_CTRL    0x04
#define KB_ALT     0x08

/* ============ Mouse Input ============ */

int Console_MouseSupported(void);
void Console_GetMousePos(int *x, int *y, int *buttons);
void Console_SetMousePos(int x, int y);
void Console_ShowMouse(void);
void Console_HideMouse(void);

#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_MIDDLE 0x04

/* ============ Timer ============ */

unsigned long Console_GetTickCount(void);

/* ============ File Operations (DOS/Windows compatibility) ============ */
/* ffblk is defined in tcccompat.h - don't redefine here */

#ifndef _FFBLK_DEFINED
#define _FFBLK_DEFINED
struct ffblk {
    unsigned long ff_reserved;
    unsigned long ff_attrib;
    unsigned short ff_ftime;
    unsigned short ff_fdate;
    unsigned long ff_fsize;
    char ff_name[260];
};
#endif

long Console_FindFirst(const char *pattern, struct ffblk *ffblk);
int Console_FindNext(struct ffblk *ffblk);
void Console_FindClose(struct ffblk *ffblk);

#define FA_RDONLY   0x01
#define FA_HIDDEN   0x02
#define FA_SYSTEM   0x04
#define FA_LABEL    0x08
#define FA_DIREC    0x10
#define FA_ARCH     0x20

/* ============ Drive/Directory Operations ============ */

char Console_GetCurrentDrive(void);
int Console_SetCurrentDrive(char drive);
char *Console_GetCurrentDir(char drive, char *path);
int Console_ChangeDir(const char *path);

/* ============ Video Modes ============ */

void Console_SetVideoMode(int mode);
int Console_GetVideoMode(void);

#define VIDEO_MODE_25  25
#define VIDEO_MODE_43  43
#define VIDEO_MODE_50  50

/* ============ Sound ============ */

void Console_Beep(void);

/* ============ Platform Detection ============ */

#if defined(__TINYC__) || defined(_WIN32)
    #define CONSOLE_PLATFORM PLATFORM_WIN32
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)
    #define CONSOLE_PLATFORM PLATFORM_UNIX
#elif defined(TURBOC) || defined(BCPP) || defined(__DJGPP__)
    #define CONSOLE_PLATFORM PLATFORM_DOS
#else
    #define CONSOLE_PLATFORM PLATFORM_UNKNOWN
#endif

/* Forward declarations for platform implementations */
#if CONSOLE_PLATFORM == PLATFORM_WIN32
    int ConsoleWin32_Init(void);
    void ConsoleWin32_Close(void);
    void ConsoleWin32_GetSize(int *width, int *height);
    void ConsoleWin32_ReadScreen(int x, int y, int w, int h, void *buf);
    void ConsoleWin32_WriteScreen(int x, int y, int w, int h, void *buf);
    int ConsoleWin32_GetChar(int x, int y);
    void ConsoleWin32_PutChar(int x, int y, int ch, int attr);
    void ConsoleWin32_SetCursor(int x, int y);
    void ConsoleWin32_GetCursor(int *x, int *y);
    void ConsoleWin32_HideCursor(void);
    void ConsoleWin32_ShowCursor(void);
    void ConsoleWin32_SetCursorType(int type);
    void ConsoleWin32_SetTextAttribute(int attr);
    int ConsoleWin32_GetTextAttribute(void);
    int ConsoleWin32_KbHit(void);
    int ConsoleWin32_GetKey(void);
    int ConsoleWin32_GetShiftState(void);
    int ConsoleWin32_MouseSupported(void);
    void ConsoleWin32_GetMousePos(int *x, int *y, int *buttons);
    void ConsoleWin32_SetMousePos(int x, int y);
    void ConsoleWin32_ShowMouse(void);
    void ConsoleWin32_HideMouse(void);
    unsigned long ConsoleWin32_GetTickCount(void);
    long ConsoleWin32_FindFirst(const char *pattern, struct ffblk *ffblk);
    int ConsoleWin32_FindNext(struct ffblk *ffblk);
    void ConsoleWin32_FindClose(struct ffblk *ffblk);
    char ConsoleWin32_GetCurrentDrive(void);
    int ConsoleWin32_SetCurrentDrive(char drive);
    char *ConsoleWin32_GetCurrentDir(char drive, char *path);
    int ConsoleWin32_ChangeDir(const char *path);
    void ConsoleWin32_SetVideoMode(int mode);
    int ConsoleWin32_GetVideoMode(void);
    void ConsoleWin32_Beep(void);
    
    #define Console_Init ConsoleWin32_Init
    #define Console_Close ConsoleWin32_Close
    #define Console_GetSize ConsoleWin32_GetSize
    #define Console_ReadScreen ConsoleWin32_ReadScreen
    #define Console_WriteScreen ConsoleWin32_WriteScreen
    #define Console_GetChar ConsoleWin32_GetChar
    #define Console_PutChar ConsoleWin32_PutChar
    #define Console_SetCursor ConsoleWin32_SetCursor
    #define Console_GetCursor ConsoleWin32_GetCursor
    #define Console_HideCursor ConsoleWin32_HideCursor
    #define Console_ShowCursor ConsoleWin32_ShowCursor
    #define Console_SetCursorType ConsoleWin32_SetCursorType
    #define Console_SetTextAttribute ConsoleWin32_SetTextAttribute
    #define Console_GetTextAttribute ConsoleWin32_GetTextAttribute
    #define Console_KbHit ConsoleWin32_KbHit
    #define Console_GetKey ConsoleWin32_GetKey
    #define Console_GetShiftState ConsoleWin32_GetShiftState
    #define Console_MouseSupported ConsoleWin32_MouseSupported
    #define Console_GetMousePos ConsoleWin32_GetMousePos
    #define Console_SetMousePos ConsoleWin32_SetMousePos
    #define Console_ShowMouse ConsoleWin32_ShowMouse
    #define Console_HideMouse ConsoleWin32_HideMouse
    #define Console_GetTickCount ConsoleWin32_GetTickCount
    #define Console_FindFirst ConsoleWin32_FindFirst
    #define Console_FindNext ConsoleWin32_FindNext
    #define Console_FindClose ConsoleWin32_FindClose
    #define Console_GetCurrentDrive ConsoleWin32_GetCurrentDrive
    #define Console_SetCurrentDrive ConsoleWin32_SetCurrentDrive
    #define Console_GetCurrentDir ConsoleWin32_GetCurrentDir
    #define Console_ChangeDir ConsoleWin32_ChangeDir
    #define Console_SetVideoMode ConsoleWin32_SetVideoMode
    #define Console_GetVideoMode ConsoleWin32_GetVideoMode
    #define Console_Beep ConsoleWin32_Beep

#elif CONSOLE_PLATFORM == PLATFORM_DOS
    /* DOS implementation uses existing BIOS functions */
    #define Console_Init() 0
    #define Console_Close()
    /* Additional DOS macros will be defined in dos/console_dos.h */
#endif

/* Helper functions that call platform implementations */
static inline int Console_GetWidth(void) {
    int w, h;
    Console_GetSize(&w, &h);
    return w;
}

static inline int Console_GetHeight(void) {
    int w, h;
    Console_GetSize(&w, &h);
    return h;
}

static inline PLATFORM_TYPE Console_GetPlatformType(void) {
    return CONSOLE_PLATFORM;
}

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_ABSTRACTION_H */