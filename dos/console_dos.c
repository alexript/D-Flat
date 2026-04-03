/* dos/console_dos.c - DOS PAL implementation
 * 
 * This file provides the Console_* interface using existing BIOS functions.
 * It wraps the existing DOS functions in video.c, console.c, etc.
 */

#include "../console.h"

#ifndef CONSOLE_PLATFORM
#define CONSOLE_PLATFORM PLATFORM_DOS
#endif

#if CONSOLE_PLATFORM == PLATFORM_DOS

#include "../video.h"
#include "../console.h"
#include "../bios.h"

int Console_DOS_Init(void) {
    return 0;
}

void Console_DOS_Close(void) {
}

void Console_DOS_GetSize(int *width, int *height) {
    *width = SCREENWIDTH;
    *height = SCREENHEIGHT;
}

void Console_DOS_ReadScreen(int x, int y, int w, int h, void *buf) {
    getvideo(buf, w * h * 2);
    (void)x; (void)y;
}

void Console_DOS_WriteScreen(int x, int y, int w, int h, void *buf) {
    storevideo(buf, w * h * 2);
    (void)x; (void)y;
}

int Console_DOS_GetChar(int x, int y) {
    return GetVideoChar(x, y);
}

void Console_DOS_PutChar(int x, int y, int ch, int attr) {
    PutVideoChar(x, y, ch, attr);
}

void Console_DOS_SetCursor(int x, int y) {
    gotoxy(x + 1, y + 1);
}

void Console_DOS_GetCursor(int *x, int *y) {
    *x = wherex() - 1;
    *y = wherey() - 1;
}

void Console_DOS_HideCursor(void) {
    hidecursor();
}

void Console_DOS_ShowCursor(void) {
    unhidecursor();
}

void Console_DOS_SetCursorType(int type) {
    (void)type;
}

void Console_DOS_SetTextAttribute(int attr) {
    textattr(attr);
}

int Console_DOS_GetTextAttribute(void) {
    return gettextattr();
}

int Console_DOS_KbHit(void) {
    return charavail();
}

int Console_DOS_GetKey(void) {
    return readychar();
}

int Console_DOS_GetShiftState(void) {
    return shiftkey();
}

int Console_DOS_MouseSupported(void) {
    return mouseinstalled();
}

void Console_DOS_GetMousePos(int *x, int *y, int *buttons) {
    getmouseposition(x, y, buttons);
}

void Console_DOS_SetMousePos(int x, int y) {
    setmouseposition(x, y);
}

void Console_DOS_ShowMouse(void) {
    showmousecursor();
}

void Console_DOS_HideMouse(void) {
    hidemousecursor();
}

unsigned long Console_DOS_GetTickCount(void) {
    return gettick();
}

long Console_DOS_FindFirst(const char *pattern, struct ffblk *ffblk) {
    return findfirst(pattern, ffblk, 0);
}

int Console_DOS_FindNext(struct ffblk *ffblk) {
    return findnext(ffblk);
}

void Console_DOS_FindClose(struct ffblk *ffblk) {
    (void)ffblk;
}

char Console_DOS_GetCurrentDrive(void) {
    return getdisk();
}

int Console_DOS_SetCurrentDrive(char drive) {
    return setdisk(drive);
}

char *Console_DOS_GetCurrentDir(char drive, char *path) {
    getcwd(drive, path);
    return path;
}

int Console_DOS_ChangeDir(const char *path) {
    return chdir(path);
}

void Console_DOS_SetVideoMode(int mode) {
    switch(mode) {
        case 25:
            Set25();
            break;
        case 43:
            Set43();
            break;
        case 50:
            Set50();
            break;
    }
}

int Console_DOS_GetVideoMode(void) {
    return getvideomode();
}

void Console_DOS_Beep(void) {
    sound(800);
    delay(200);
    nosound();
}

#endif /* CONSOLE_PLATFORM == PLATFORM_DOS */