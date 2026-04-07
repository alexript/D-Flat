#include <stdlib.h>
#include <time.h>
#include "tccwin32.h"

HANDLE hConsoleInput = NULL;
HANDLE hConsoleOutput = NULL;
DWORD dwOriginalConsoleMode = 0;
int consoleWidth = 80;
int consoleHeight = 25;
int currentCursorX = 0;
int currentCursorY = 0;
int cursorVisible = TRUE;
int cursorType = 0;
int mouseX = 0;
int mouseY = 0;
int mouseButtons = 0;
int mouseVisible = FALSE;
unsigned int videoMode = 3;
unsigned char textAttribute = 0x07;

#define CURSOR_STACK_SIZE 10
static int cursorStack[CURSOR_STACK_SIZE];
static int cursorStackTop = 0;

static CHAR_INFO *screenBuffer = NULL;
static int screenBufferSize = 0;

static WIN32_FIND_DATAW currentFindData;
static HANDLE currentFindHandle = INVALID_HANDLE_VALUE;

static unsigned char keyQueue[64];
static int keyQueueHead = 0;
static int keyQueueTail = 0;

static int shiftState = 0;

int InitWin32Console(void)
{
    hConsoleInput = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_EXISTING, 0, NULL);
    hConsoleOutput = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                OPEN_EXISTING, 0, NULL);
    
    if (hConsoleInput == INVALID_HANDLE_VALUE || hConsoleOutput == INVALID_HANDLE_VALUE) {
        return -1;
    }
    
    GetConsoleMode(hConsoleInput, &dwOriginalConsoleMode);
    SetConsoleMode(hConsoleInput, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
    
    SetConsoleOutputCP(866);
    SetConsoleCP(866);
    
    GetConsoleSize(&consoleWidth, &consoleHeight);
    
    screenBufferSize = consoleWidth * consoleHeight * sizeof(CHAR_INFO);
    screenBuffer = (CHAR_INFO *)calloc(screenBufferSize, 1);
    if (!screenBuffer) {
        return -1;
    }
    
    videoMode = 3;
    textAttribute = 0x07;
    
    return 0;
}

void CloseWin32Console(void)
{
    if (screenBuffer) {
        free(screenBuffer);
        screenBuffer = NULL;
    }
    
    if (hConsoleInput != INVALID_HANDLE_VALUE) {
        SetConsoleMode(hConsoleInput, dwOriginalConsoleMode);
        CloseHandle(hConsoleInput);
    }
    if (hConsoleOutput != INVALID_HANDLE_VALUE) {
        CloseHandle(hConsoleOutput);
    }
    
    hConsoleInput = INVALID_HANDLE_VALUE;
    hConsoleOutput = INVALID_HANDLE_VALUE;
}

void GetConsoleSize(int *width, int *height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)) {
        *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *width = 80;
        *height = 25;
    }
    consoleWidth = *width;
    consoleHeight = *height;
}

void SetConsoleSize(int width, int height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT sr;
    
    if (!GetConsoleScreenBufferInfo(hConsoleOutput, &csbi))
        return;
    
    sr.Left = 0;
    sr.Top = 0;
    sr.Right = width - 1;
    sr.Bottom = height - 1;
    
    SetConsoleWindowInfo(hConsoleOutput, TRUE, &sr);
    consoleWidth = width;
    consoleHeight = height;
}

void Win32_getvideo(void *buf, int size)
{
    COORD bufferSize = {consoleWidth, consoleHeight};
    COORD bufferCoord = {0, 0};
    SMALL_RECT readRegion = {0, 0, consoleWidth - 1, consoleHeight - 1};
    CHAR_INFO *ci = (CHAR_INFO *)buf;
    int i;
    
    if (!screenBuffer || !buf)
        return;
    
    ReadConsoleOutput(hConsoleOutput, screenBuffer, bufferSize, bufferCoord, &readRegion);
    
    for (i = 0; i < consoleWidth * consoleHeight && i < size / 2; i++) {
        ((char *)buf)[i * 2] = screenBuffer[i].Char.AsciiChar;
        ((char *)buf)[i * 2 + 1] = (char)screenBuffer[i].Attributes;
    }
}

void Win32_storevideo(void *buf, int size)
{
    COORD bufferSize = {consoleWidth, consoleHeight};
    COORD bufferCoord = {0, 0};
    SMALL_RECT writeRegion = {0, 0, consoleWidth - 1, consoleHeight - 1};
    int i;
    
    if (!screenBuffer || !buf)
        return;
    
    for (i = 0; i < consoleWidth * consoleHeight && i < size / 2; i++) {
        screenBuffer[i].Char.AsciiChar = ((char *)buf)[i * 2];
        screenBuffer[i].Attributes = (unsigned char)((char *)buf)[i * 2 + 1];
    }
    
    WriteConsoleOutput(hConsoleOutput, screenBuffer, bufferSize, bufferCoord, &writeRegion);
}

int Win32_GetVideoChar(int x, int y)
{
    COORD pos = {x, y};
    char ch;
    WORD attr;
    DWORD written;
    
    if (x < 0 || x >= consoleWidth || y < 0 || y >= consoleHeight)
        return 0;
    
    ReadConsoleOutputCharacter(hConsoleOutput, &ch, 1, pos, &written);
    ReadConsoleOutputAttribute(hConsoleOutput, &attr, 1, pos, &written);
    
    return (attr << 8) | (unsigned char)ch;
}

void Win32_PutVideoChar(int x, int y, int ch, int attr)
{
    COORD pos = {x, y};
    char c = (char)ch;
    WORD a = (WORD)attr;
    DWORD written;
    
    if (x < 0 || x >= consoleWidth || y < 0 || y >= consoleHeight)
        return;
    
    WriteConsoleOutputCharacter(hConsoleOutput, &c, 1, pos, &written);
    WriteConsoleOutputAttribute(hConsoleOutput, &a, 1, pos, &written);
}

int Win32_isEGA(void)
{
    return FALSE;
}

int Win32_isVGA(void)
{
    return FALSE;
}

void Win32_Set25(void)
{
    SetConsoleSize(80, 25);
    videoMode = 3;
}

void Win32_Set43(void)
{
    SetConsoleSize(80, 43);
    videoMode = 0x11;
}

void Win32_Set50(void)
{
    SetConsoleSize(80, 50);
    videoMode = 0x12;
}

void Win32_SetTextAttribute(int attr)
{
    textAttribute = (unsigned char)attr;
}

int Win32_GetTextAttribute(void)
{
    return textAttribute;
}

void Win32_cursor(int x, int y)
{
    COORD pos = {x, y};
    SetConsoleCursorPosition(hConsoleOutput, pos);
    currentCursorX = x;
    currentCursorY = y;
}

void Win32_savecursor(void)
{
    if (cursorStackTop < CURSOR_STACK_SIZE) {
        cursorStack[cursorStackTop++] = currentCursorY * 80 + currentCursorX;
    }
}

void Win32_restorecursor(void)
{
    if (cursorStackTop > 0) {
        int pos = cursorStack[--cursorStackTop];
        currentCursorY = pos / 80;
        currentCursorX = pos % 80;
        Win32_cursor(currentCursorX, currentCursorY);
    }
}

void Win32_hidecursor(void)
{
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = 100;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(hConsoleOutput, &cci);
    cursorVisible = FALSE;
}

void Win32_unhidecursor(void)
{
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = (cursorType == 2) ? 100 : 50;
    cci.bVisible = TRUE;
    SetConsoleCursorInfo(hConsoleOutput, &cci);
    cursorVisible = TRUE;
}

void Win32_set_cursor_type(int type)
{
    CONSOLE_CURSOR_INFO cci;
    cursorType = type;
    cci.dwSize = (type == 2) ? 100 : 50;
    cci.bVisible = cursorVisible;
    SetConsoleCursorInfo(hConsoleOutput, &cci);
}

void Win32_curr_cursor(int *x, int *y)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)) {
        *x = csbi.dwCursorPosition.X;
        *y = csbi.dwCursorPosition.Y;
    } else {
        *x = currentCursorX;
        *y = currentCursorY;
    }
}

void Win32_showcursor(void)
{
    Win32_unhidecursor();
}

void Win32_PollInput(void)
{
    INPUT_RECORD ir[32];
    DWORD count = 0;
    
    if (hConsoleInput == INVALID_HANDLE_VALUE)
        return;
    
    PeekConsoleInput(hConsoleInput, ir, 32, &count);
    
    DWORD i;
    for (i = 0; i < count; i++) {
        if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) {
            int scan = ir[i].Event.KeyEvent.wVirtualScanCode;
            int vkey = ir[i].Event.KeyEvent.wVirtualKeyCode;
            int ch = ir[i].Event.KeyEvent.uChar.AsciiChar;
            
            if (keyQueueHead != ((keyQueueTail + 1) % 64)) {
                keyQueue[keyQueueTail] = (unsigned char)((scan << 8) | vkey);
                keyQueueTail = (keyQueueTail + 1) % 64;
            }
            
            ReadConsoleInput(hConsoleInput, ir, 1, &count);
        } else if (ir[i].EventType == MOUSE_EVENT) {
            COORD mc = ir[i].Event.MouseEvent.dwMousePosition;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)) {
                int charWidth = csbi.dwSize.X / (csbi.srWindow.Right - csbi.srWindow.Left + 1);
                int charHeight = csbi.dwSize.Y / (csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
                mouseX = (mc.X - csbi.srWindow.Left) / charWidth;
                mouseY = (mc.Y - csbi.srWindow.Top) / charHeight;
                
                if (ir[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
                    mouseButtons = 1;
                else if (ir[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
                    mouseButtons = 2;
                else
                    mouseButtons = 0;
            }
            ReadConsoleInput(hConsoleInput, ir, 1, &count);
        } else if (ir[i].EventType == WINDOW_BUFFER_SIZE_EVENT) {
            GetConsoleSize(&consoleWidth, &consoleHeight);
            ReadConsoleInput(hConsoleInput, ir, 1, &count);
        } else {
            ReadConsoleInput(hConsoleInput, ir, 1, &count);
        }
    }
}

int Win32_keyhit(void)
{
    Win32_PollInput();
    return keyQueueHead != keyQueueTail;
}

int Win32_getkey(void)
{
    while (!Win32_keyhit()) {
        Sleep(10);
    }
    
    int key = keyQueue[keyQueueHead];
    keyQueueHead = (keyQueueHead + 1) % 64;
    return key;
}

int Win32_getshift(void)
{
    shiftState = 0;
    if (GetKeyState(VK_SHIFT) & 0x80)
        shiftState |= 1;
    if (GetKeyState(VK_CONTROL) & 0x80)
        shiftState |= 4;
    if (GetKeyState(VK_MENU) & 0x80)
        shiftState |= 8;
    if (GetKeyState(VK_CAPITAL) & 1)
        shiftState |= 0x40;
    return shiftState;
}

int Win32_bioskey(int cmd)
{
    switch (cmd) {
        case 0:
            return Win32_getkey();
        case 1:
            return Win32_keyhit() ? Win32_getkey() : 0;
        case 2:
            return Win32_getshift();
    }
    return 0;
}

void Win32_beep(void)
{
    Beep(800, 200);
}

int Win32_mouse_installed(void)
{
    return TRUE;
}

int Win32_mousebuttons(void)
{
    return 2;
}

void Win32_get_mouseposition(int *x, int *y, int *buttons)
{
    Win32_PollInput();
    *x = mouseX;
    *y = mouseY;
    *buttons = mouseButtons;
}

void Win32_set_mouseposition(int x, int y)
{
    COORD pos;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    
    if (!GetConsoleScreenBufferInfo(hConsoleOutput, &csbi))
        return;
    
    pos.X = x + csbi.srWindow.Left;
    pos.Y = y + csbi.srWindow.Top;
    
    SetConsoleCursorPosition(hConsoleOutput, pos);
    mouseX = x;
    mouseY = y;
}

void Win32_show_mousecursor(void)
{
    mouseVisible = TRUE;
}

void Win32_hide_mousecursor(void)
{
    mouseVisible = FALSE;
}

void Win32_resetmouse(void)
{
    mouseX = mouseY = mouseButtons = 0;
}

void Win32_set_mousetravel(int x1, int y1, int x2, int y2)
{
    (void)x1;
    (void)y1;
    (void)x2;
    (void)y2;
}

int Win32_button_releases(void)
{
    return 0;
}

long Win32_findfirst(const char *pattern, void *ffblk)
{
    struct ffblk *fb = (struct ffblk *)ffblk;
    WCHAR wpattern[MAX_PATH];
    int i;
    
    for (i = 0; pattern[i] && i < MAX_PATH - 1; i++) {
        wpattern[i] = pattern[i];
    }
    wpattern[i] = 0;
    
    currentFindHandle = FindFirstFileW(wpattern, &currentFindData);
    
    if (currentFindHandle == INVALID_HANDLE_VALUE) {
        return -1;
    }
    
    fb->ff_attrib = currentFindData.dwFileAttributes;
    fb->ff_fsize = currentFindData.nFileSizeLow;
    
    FILETIME ft = currentFindData.ftLastWriteTime;
    unsigned long dosdate = ((ft.dwHighDateTime >> 16) & 0xFFFF) |
                           ((ft.dwLowDateTime >> 16) & 0xFFFF0000);
    fb->ff_ftime = (unsigned short)(dosdate & 0xFFFF);
    fb->ff_fdate = (unsigned short)((dosdate >> 16) & 0xFFFF);
    
    for (i = 0; currentFindData.cFileName[i] && i < 12; i++) {
        fb->ff_name[i] = currentFindData.cFileName[i];
    }
    fb->ff_name[i] = 0;
    
    return 0;
}

int Win32_findnext(void *ffblk)
{
    struct ffblk *fb = (struct ffblk *)ffblk;
    int i;
    
    if (currentFindHandle == INVALID_HANDLE_VALUE)
        return -1;
    
    if (!FindNextFileW(currentFindHandle, &currentFindData)) {
        return -1;
    }
    
    fb->ff_attrib = currentFindData.dwFileAttributes;
    fb->ff_fsize = currentFindData.nFileSizeLow;
    
    FILETIME ft = currentFindData.ftLastWriteTime;
    unsigned long dosdate = ((ft.dwHighDateTime >> 16) & 0xFFFF) |
                           ((ft.dwLowDateTime >> 16) & 0xFFFF0000);
    fb->ff_ftime = (unsigned short)(dosdate & 0xFFFF);
    fb->ff_fdate = (unsigned short)((dosdate >> 16) & 0xFFFF);
    
    for (i = 0; currentFindData.cFileName[i] && i < 12; i++) {
        fb->ff_name[i] = currentFindData.cFileName[i];
    }
    fb->ff_name[i] = 0;
    
    return 0;
}

void Win32_findclose(void *ffblk)
{
    (void)ffblk;
    if (currentFindHandle != INVALID_HANDLE_VALUE) {
        FindClose(currentFindHandle);
        currentFindHandle = INVALID_HANDLE_VALUE;
    }
}

char Win32_getdisk(void)
{
    char cwd[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, cwd);
    if (cwd[0] >= 'A' && cwd[0] <= 'Z')
        return cwd[0] - 'A';
    return 0;
}

int Win32_setdisk(char drive)
{
    char path[4] = "X:\\";
    path[0] = (char)('A' + drive);
    return SetCurrentDirectory(path) ? 0 : 1;
}

char *Win32_getcwd(char drive, char *path)
{
    if (drive != 0) {
        char drivePath[4] = "X:";
        drivePath[0] = (char)('A' + drive);
        if (GetCurrentDirectory(MAX_PATH, path)) {
            if (path[0] == drivePath[0] || path[0] == (char)('a' + drive))
                return path;
        }
    }
    GetCurrentDirectory(MAX_PATH, path);
    return path;
}

int Win32_chdir(const char *path)
{
    return SetCurrentDirectory(path) ? 0 : -1;
}

unsigned long Win32_GetTickCount(void)
{
    return GetTickCount();
}

void Win32_sleep(unsigned int ms)
{
    Sleep(ms);
}

time_t Win32_time(time_t *timer)
{
    return time(timer);
}

struct tm *Win32_localtime(const time_t *timer)
{
    return localtime(timer);
}

int Win32_int86(int intno, void *in, void *out)
{
    (void)in;
    (void)out;
    
    switch (intno) {
        case VIDEO:
            break;
        case KEYBRD:
            break;
        case DOS:
            break;
        case MOUSE:
            break;
    }
    return 0;
}

int Win32_int86x(int intno, void *in, void *out, void *sreg)
{
    (void)sreg;
    return Win32_int86(intno, in, out);
}

void *Win32_getvect(int intno)
{
    (void)intno;
    return NULL;
}

void Win32_setvect(int intno, void *handler)
{
    (void)intno;
    (void)handler;
}

void Win32_setvideomode(int mode)
{
    videoMode = mode;
    switch (mode) {
        case 3:
        case 2:
        case 1:
        case 0:
            Win32_Set25();
            break;
        case 0x11:
            Win32_Set43();
            break;
        case 0x12:
            Win32_Set50();
            break;
    }
}

int Win32_getvideomode(void)
{
    return videoMode;
}

int ScanCodeToKey(int scan, int alt)
{
    static unsigned char scancode_table[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8, 9,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 13, 0, 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.'
    };
    
    if (scan < sizeof(scancode_table))
        return scancode_table[scan] | (alt & 0x80);
    return 0;
}

int AltConvert(int key)
{
    if ((key & 0xFF00) == 0)
        return 0;
    return key & 0xFF;
}