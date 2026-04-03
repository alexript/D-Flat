/* win32/console_win32.c - Win32 PAL implementation
 * 
 * This file provides the Console_* interface using Win32 Console API.
 * It wraps the functions from tinycc/tccwin32.c.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "../console.h"
#include "../tinycc/tcccompat.h"
#include "../tinycc/tccwin32.h"

int ConsoleWin32_Init(void) {
    return InitWin32Console();
}

void ConsoleWin32_Close(void) {
    CloseWin32Console();
}

void ConsoleWin32_GetSize(int *width, int *height) {
    GetConsoleSize(width, height);
}

void ConsoleWin32_ReadScreen(int x, int y, int w, int h, void *buf) {
    (void)x; (void)y; (void)w; (void)h;
    Win32_getvideo(buf, w * h * 2);
}

void ConsoleWin32_WriteScreen(int x, int y, int w, int h, void *buf) {
    (void)x; (void)y; (void)w; (void)h;
    Win32_storevideo(buf, w * h * 2);
}

int ConsoleWin32_GetChar(int x, int y) {
    return Win32_GetVideoChar(x, y);
}

void ConsoleWin32_PutChar(int x, int y, int ch, int attr) {
    Win32_PutVideoChar(x, y, ch, attr);
}

void ConsoleWin32_SetCursor(int x, int y) {
    Win32_cursor(x, y);
}

void ConsoleWin32_GetCursor(int *x, int *y) {
    Win32_curr_cursor(x, y);
}

void ConsoleWin32_HideCursor(void) {
    Win32_hidecursor();
}

void ConsoleWin32_ShowCursor(void) {
    Win32_showcursor();
}

void ConsoleWin32_SetCursorType(int type) {
    Win32_set_cursor_type(type);
}

void ConsoleWin32_SetTextAttribute(int attr) {
    Win32_SetTextAttribute(attr);
}

int ConsoleWin32_GetTextAttribute(void) {
    return Win32_GetTextAttribute();
}

int ConsoleWin32_KbHit(void) {
    return Win32_keyhit();
}

int ConsoleWin32_GetKey(void) {
    return Win32_getkey();
}

int ConsoleWin32_GetShiftState(void) {
    return Win32_getshift();
}

int ConsoleWin32_MouseSupported(void) {
    return Win32_mouse_installed();
}

void ConsoleWin32_GetMousePos(int *x, int *y, int *buttons) {
    Win32_get_mouseposition(x, y, buttons);
}

void ConsoleWin32_SetMousePos(int x, int y) {
    Win32_set_mouseposition(x, y);
}

void ConsoleWin32_ShowMouse(void) {
    Win32_show_mousecursor();
}

void ConsoleWin32_HideMouse(void) {
    Win32_hide_mousecursor();
}

unsigned long ConsoleWin32_GetTickCount(void) {
    return Win32_GetTickCount();
}

long ConsoleWin32_FindFirst(const char *pattern, struct ffblk *ffblk) {
    return Win32_findfirst(pattern, ffblk);
}

int ConsoleWin32_FindNext(struct ffblk *ffblk) {
    return Win32_findnext(ffblk);
}

void ConsoleWin32_FindClose(struct ffblk *ffblk) {
    Win32_findclose(ffblk);
}

char ConsoleWin32_GetCurrentDrive(void) {
    return Win32_getdisk();
}

int ConsoleWin32_SetCurrentDrive(char drive) {
    return Win32_setdisk(drive);
}

char *ConsoleWin32_GetCurrentDir(char drive, char *path) {
    return Win32_getcwd(drive, path);
}

int ConsoleWin32_ChangeDir(const char *path) {
    return Win32_chdir(path);
}

void ConsoleWin32_SetVideoMode(int mode) {
    Win32_setvideomode(mode);
}

int ConsoleWin32_GetVideoMode(void) {
    return Win32_getvideomode();
}

void ConsoleWin32_Beep(void) {
    Win32_beep();
}