#ifndef TCCWIN32_H
#define TCCWIN32_H

#define _WINDEF_
#include "tcccompat.h"
#undef _WINDEF_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define KEYBOARDPORT 0x60
#define CGA_ADDRESS 0xB800
#define MONO_ADDRESS 0xB000

extern HANDLE hConsoleInput;
extern HANDLE hConsoleOutput;
extern DWORD dwOriginalConsoleMode;
extern int consoleWidth;
extern int consoleHeight;
extern int currentCursorX;
extern int currentCursorY;
extern int cursorVisible;
extern int cursorType;
extern int mouseX;
extern int mouseY;
extern int mouseButtons;
extern int mouseVisible;
extern unsigned int videoMode;
extern unsigned char textAttribute;

int InitWin32Console(void);
void CloseWin32Console(void);
void GetConsoleSize(int *width, int *height);
void SetConsoleSize(int width, int height);

void Win32_getvideo(void *buf, int size);
void Win32_storevideo(void *buf, int size);
int Win32_GetVideoChar(int x, int y);
void Win32_PutVideoChar(int x, int y, int ch, int attr);
int Win32_isEGA(void);
int Win32_isVGA(void);
void Win32_Set25(void);
void Win32_Set43(void);
void Win32_Set50(void);
void Win32_SetTextAttribute(int attr);
int Win32_GetTextAttribute(void);

void Win32_cursor(int x, int y);
void Win32_savecursor(void);
void Win32_restorecursor(void);
void Win32_hidecursor(void);
void Win32_unhidecursor(void);
void Win32_set_cursor_type(int type);
void Win32_curr_cursor(int *x, int *y);
void Win32_showcursor(void);

int Win32_keyhit(void);
int Win32_getkey(void);
int Win32_getshift(void);
int Win32_bioskey(int cmd);
void Win32_beep(void);

int Win32_mouse_installed(void);
int Win32_mousebuttons(void);
void Win32_get_mouseposition(int *x, int *y, int *buttons);
void Win32_set_mouseposition(int x, int y);
void Win32_show_mousecursor(void);
void Win32_hide_mousecursor(void);
void Win32_resetmouse(void);
void Win32_set_mousetravel(int x1, int y1, int x2, int y2);
int Win32_button_releases(void);

long Win32_findfirst(const char *pattern, void *ffblk);
int Win32_findnext(void *ffblk);
void Win32_findclose(void *ffblk);
char Win32_getdisk(void);
int Win32_setdisk(char drive);
char *Win32_getcwd(char drive, char *path);
int Win32_chdir(const char *path);

unsigned long Win32_GetTickCount(void);
void Win32_sleep(unsigned int ms);
time_t Win32_time(time_t *timer);
struct tm *Win32_localtime(const time_t *timer);

int Win32_int86(int intno, void *in, void *out);
int Win32_int86x(int intno, void *in, void *out, void *sreg);
void *Win32_getvect(int intno);
void Win32_setvect(int intno, void *handler);

void Win32_setvideomode(int mode);
int Win32_getvideomode(void);
void Win32_PollInput(void);
int ScanCodeToKey(int scan, int alt);
int AltConvert(int key);

#endif /* TCCWIN32_H */