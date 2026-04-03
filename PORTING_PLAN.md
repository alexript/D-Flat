# План портирования D-Flat на TinyCC (x64 Windows)

## Формат отслеживания прогресса

| Символ | Статус |
|--------|--------|
| `[ ]`  | Не начато |
| `[~]`  | В процессе |
| `[x]`  | Выполнено |
| `[-]`  | Отменено |

**Структура задачи:**
```
- [x] Описание задачи | ДД.ММ.ГГГГ | Комментарий о выполнении
```

**Пример:**
```
- [x] Создать console.h | 15.03.2025 | Базовый интерфейс PAL реализован
```

## Обзор

Цель: полноценный функциональный порт библиотеки D-Flat DOS TUI на TinyCC для x64 Windows с использованием Windows Console API.

**Архитектурное решение**: Platform Abstraction Layer (PAL) обеспечивает возможность добавления Unix-поддержки без изменения API D-Flat.

## Порядок выполнения фаз

```
Фаза 1 → Фаза 2 → Фаза 3 → Фаза 4 → Фаза 5 → Фаза 6 → Фаза 7 → Фаза 8
```

**Детали порядка:**

1. **Фаза 1** (Юнит-тестирование): Создание тестовой инфраструктуры ДО начала портирования
2. **Фаза 2** (Инфраструктура совместимости): Создаётся папка `tinycc/` с временными файлами для портирования
3. **Фаза 3** (PAL): Создаётся уровень абстракции в корне проекта (console.h) и папках win32/, dos/
4. **Фаза 4** (Модификация заголовков): dflat.h, system.h, video.h адаптируются под TinyCC
5. **Фаза 5** (Модификация исходников): message.c, video.c, console.c, mouse.c, direct.c изменяются для Win32
6. **Фаза 6** (Makefile): Создаётся makefile.tinycc
7. **Фаза 7** (Тестирование): Компиляция и функциональное тестирование
8. **Фаза 8** (Unix-порт): Создаётся папка unix/ с ncurses-реализацией

**Рефакторинг после Фазы 7:** Файл `tinycc/tccwin32.c` перемещается в `win32/console_win32.c`, папка `tinycc/` удаляется.

## Требования

- Компилятор: `c:\tcc\tcc.exe`
- Утилита make: `c:\tcc\make.exe`
- Все фичи: `BUILD_FULL_DFLAT`
- Кодировка: OEM (CP866)
  - Вывод: `SetConsoleOutputCP(866)` и `SetConsoleCP(866)` в `InitWin32Console()`
  - Ввод: может потребоваться преобразование раскладки (RU → EN) для служебных клавиш
- Размер консоли: адаптивный
- Поддержка мыши: полная
- Справка: использовать существующий `memopad.hlp`

## Требования к тестированию

**Фаза 1 (Юнит-тестирование) является обязательной и выполняется ПЕРВЫМИ.**

- Создать header-only библиотеку `df_test.h` для юнит-тестирования
- Все задачи должны сопровождаться юнит-тестами
- Принцип TDD: сначала пишется тест, затем код, удовлетворяющий тесту
- **Корректное выполнение тестов — обязательное условие завершения каждой задачи**
- Каждая подзадача в плане должна иметь соответствующий тест
- Тесты должны обеспечивать покрытие ключевых функций:
  - Функции video.c: getvideo, storevideo, movefromscreen, movetoscreen
  - Функции console.c: charavail, readychar, shiftkey, gotoxy
  - Функции message.c: инициализация, диспетчеризация сообщений
  - Функции mouse.c: позиция мыши, кнопки
  - Функции direct.c: файловые операции

### Структура тестов

```
tests/
├── df_test.h              # Header-only библиотека тестирования
├── test_video.c           # Тесты video.c
├── test_console.c         # Тесты console.c
├── test_message.c         # Тесты message.c
├── test_mouse.c           # Тесты mouse.c
├── test_direct.c          # Тесты direct.c
├── test_rect.c            # Тесты rect.c
└── test_window.c          # Тесты window.c
```

### Критерии приёмки задачи

- [ ] Написан юнит-тест для реализуемой функции
- [ ] Тест компилируется без ошибок
- [ ] Тест проходит успешно
- [ ] Код соответствует требованиям теста

## Структура проекта

```
C:\Workspace\Personal\D-Flat/
├── console.h            # Platform Abstraction Layer (PAL) - Фаза 3
├── win32/               # Win32 реализация PAL (Фаза 3)
│   └── console_win32.c  # Win32 Console API реализация
├── unix/                # Unix реализация PAL (Фаза 8)
│   └── console_unix.c   # ncurses реализация
├── dos/                 # DOS реализация PAL (существующий код)
│   └── console_dos.c    # Обёртка над BIOS-вызовами
├── tinycc/              # Временная папка (до завершения портирования)
│   ├── tcccompat.h      # Макросы совместимости TinyCC
│   ├── tccwin32.h       # Заголовок Win32-обёрток
│   └── tccwin32.c       # Реализация DOS-функций через Win32 API
├── makefile.tinycc         # Файл сборки для TinyCC
├── makefile             # Универсальный Makefile (Фаза 8+)
└── [оригинальные файлы]
```

### Время жизни папок в процессе реализации плана

| Папка | Время жизни | Примечание |
|-------|-------------|------------|
| `tinycc/` | Фазы 2-7 | Создаётся в Фазе 2. Удаляется после Фазы 7. Содержит промежуточный код для TinyCC. |
| `win32/` | Фазы 3 и далее | Создаётся в Фазе 3 (PAL). Используется в финальном порте. |
| `dos/` | Постоянно | Создаётся в Фазе 0 для сохранения оригинального кода. |
| `unix/` | Фаза 8+ | Создаётся в Фазе 8. Используется для Unix-порта. |

**Порядок создания реализаций:**

1. **Фаза 2** (2.1-2.3): Создать `tinycc/tcccompat.h`, `tinycc/tccwin32.h`, `tinycc/tccwin32.c` — основная реализация для портирования
2. **Фаза 3** (3.1-3.6): Создать PAL (`console.h`, `win32/console_win32.c`, `dos/console_dos.c`) — для унификации интерфейса
3. После **Фазы 7**: Рефакторинг `tinycc/tccwin32.c` → `win32/console_win32.c`, удаление `tinycc/`

**Обоснование**: Сначала создаётся рабочая реализация в `tinycc/` для проверки работоспособности порта. Затем создаётся PAL для унификации. После отладки рабочая реализация рефакторится в чистую PAL-реализацию `win32/console_win32.c`.

## Фаза 3: Platform Abstraction Layer (PAL)

**Требования к каждой задаче:**
- Сначала написать юнит-тест
- Проверить, что тест не проходит (красный)
- Реализовать функциональность
- Проверить, что тест проходит (зелёный)
- Рефакторинг при необходимости

### Концепция

Вводится уровень абстракции между D-Flat и платформенно-специфичным кодом. Это позволяет:
- Добавить поддержку Unix без изменения API D-Flat
- Легко переключаться между реализациями (Win32, Unix, ANSI-only терминалы)
- Максимизировать переиспользование кода между платформами

### Архитектура PAL

```
┌─────────────────────────────────────────────────────────────┐
│                    D-Flat Core (ядро)                       │
│  window.c, message.c, video.c, mouse.c, console.c, ...     │
└─────────────────────────────────────────────────────────────┘
                            ↓ ↑
┌─────────────────────────────────────────────────────────────┐
│              Console Abstraction Layer (console.h)          │
│         Платформенно-независимый интерфейс (PAL)           │
└─────────────────────────────────────────────────────────────┘
                            ↓ ↑
┌──────────────┬──────────────┬──────────────┬───────────────┐
│ win32/       │ unix/        │ ansi/        │ dos/          │
│ win_console.c│ unix_term.c  │ ansi_term.c  │ bios_console.c│
└──────────────┴──────────────┴──────────────┴───────────────┘
```

### console.h - Платформенно-независимый интерфейс

Создать единый заголовочный файл с абстракцией:

```c
// console.h - Платформенно-независимый консольный интерфейс

#ifndef CONSOLE_ABSTRACTION_H
#define CONSOLE_ABSTRACTION_H

// Тип платформы
typedef enum {
    PLATFORM_DOS,       // Оригинальный DOS с BIOS
    PLATFORM_WIN32,     // Windows Console API
    PLATFORM_UNIX,      // Unix (ncurses/termcap)
    PLATFORM_ANSI,      // ANSI escape sequences only
    PLATFORM_UNKNOWN
} PLATFORM_TYPE;

// Инициализация консоли
int Console_Init(void);
void Console_Close(void);

// Размер экрана
void Console_GetSize(int *width, int *height);

// Видео-операции
void Console_ReadScreen(int x, int y, int w, int h, void *buf);
void Console_WriteScreen(int x, int y, int w, int h, void *buf);

// Символы
int Console_GetChar(int x, int y);
void Console_PutChar(int x, int y, int ch, int attr);

// Курсор
void Console_SetCursor(int x, int y);
void Console_GetCursor(int *x, int *y);
void Console_HideCursor(void);
void Console_ShowCursor(void);
void Console_SetCursorType(int type);  // 0=normal, 1=block, 2=underline

// Цвета
void Console_SetTextAttribute(int attr);
int Console_GetTextAttribute(void);

// Клавиатура
int Console_KbHit(void);
int Console_GetKey(void);
int Console_GetShiftState(void);

// Мышь
int Console_MouseSupported(void);
void Console_GetMousePos(int *x, int *y, int *buttons);
void Console_SetMousePos(int x, int y);
void Console_ShowMouse(void);
void Console_HideMouse(void);

// Таймер
unsigned long Console_GetTickCount(void);

// Файловые операции
long Console_FindFirst(const char *pattern, void *ffblk);
int Console_FindNext(void *ffblk);
void Console_FindClose(void *ffblk);
char Console_GetCurrentDrive(void);
int Console_SetCurrentDrive(char drive);
char *Console_GetCurrentDir(char drive, char *path);
int Console_ChangeDir(const char *path);

// Утилиты
void Console_Beep(void);
void Console_SetVideoMode(int mode);  // 25/43/50 строк
int Console_GetVideoMode(void);

#endif
```

### Выбор платформы при компиляции

```c
// В dflat.h - выбор реализации
#if defined(__TINYC__) || defined(_WIN32)
    #define CONSOLE_IMPLEMENTATION win32
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)
    #define CONSOLE_IMPLEMENTATION unix
#elif defined(TURBOC) || defined(BCPP)
    #define CONSOLE_IMPLEMENTATION dos
#else
    #define CONSOLE_IMPLEMENTATION dos  // default
#endif

// console.c - переключатель реализации (используется после Фазы 0)
// ПРИМЕЧАНИЕ: include .c файлов используется для упрощения сборки в единый объектный файл.
// Альтернатива - компилировать console_win32.c отдельно и линковать.
#if CONSOLE_IMPLEMENTATION == win32
    #include "win32/console_win32.c"
#elif CONSOLE_IMPLEMENTATION == unix
    #include "unix/console_unix.c"
#elif CONSOLE_IMPLEMENTATION == ansi
    #include "ansi/console_ansi.c"
#else
    #include "dos/console_dos.c"
#endif
```

### Преимущества PAL для Unix-портирования

| Компонент | Изменения для Unix |
|-----------|-------------------|
| `video.c` | Не требует изменений - использует `Console_ReadScreen/WriteScreen` |
| `message.c` | Только `Console_GetTickCount()` - универсально |
| `console.c` | Полностью заменяется на Unix-реализацию |
| `mouse.c` | Заменить `int86(MOUSE)` → `Console_GetMousePos()` |
| `system.h` | `SCREENWIDTH`/`SCREENHEIGHT` через `Console_GetSize()` |

### Структура Unix-реализации

```
unix/
├── unixcons.h      # Заголовок Unix-реализации
├── console_unix.c  # Основная реализация (ncurses)
├── console_ansi.c  # Fallback: pure ANSI (без ncurses)
└── Makefile.unix   # Сборка для Unix
```

### Подзадачи для PAL

#### 3.1. console.h

Создать файл `console.h` в корне проекта:

```
Файл: console.h (~150 строк)
Содержит:
- PLATFORM_TYPE enum: PLATFORM_DOS, PLATFORM_WIN32, PLATFORM_UNIX, PLATFORM_ANSI, PLATFORM_UNKNOWN
- Все прототипы функций Console_*()
- Include guards: #ifndef CONSOLE_ABSTRACTION_H / #define CONSOLE_ABSTRACTION_H / #endif

Конкретные функции:
- Console_Init()           - инициализация консоли
- Console_Close()          - завершение работы
- Console_GetSize()        - получить размер экрана
- Console_ReadScreen()     - чтение области экрана
- Console_WriteScreen()    - запись области экрана
- Console_GetChar()        - получить символ в позиции
- Console_PutChar()        - записать символ в позицию
- Console_SetCursor()      - установить позицию курсора
- Console_GetCursor()      - получить позицию курсора
- Console_HideCursor()     - скрыть курсор
- Console_ShowCursor()     - показать курсор
- Console_SetCursorType()  - тип курсора (0/1/2)
- Console_SetTextAttribute() - установить атрибут
- Console_GetTextAttribute() - получить атрибут
- Console_KbHit()          - проверить наличие ввода
- Console_GetKey()         - получить код клавиши
- Console_GetShiftState()  - состояние Shift/Ctrl/Alt
- Console_MouseSupported() - поддержка мыши
- Console_GetMousePos()    - позиция мыши
- Console_SetMousePos()    - установить позицию мыши
- Console_ShowMouse()      - показать мышь
- Console_HideMouse()      - скрыть мышь
- Console_GetTickCount()   - время в миллисекундах
- Console_FindFirst()      - поиск файлов (первый)
- Console_FindNext()       - поиск файлов (следующий)
- Console_FindClose()      - закрыть поиск
- Console_GetCurrentDrive() - получить текущий диск
- Console_SetCurrentDrive() - установить текущий диск
- Console_GetCurrentDir()  - получить текущий каталог
- Console_ChangeDir()      - сменить каталог
- Console_Beep()           - звуковой сигнал
- Console_SetVideoMode()   - установить видео режим
- Console_GetVideoMode()   - получить видео режим
```

#### 3.2. DOS-реализация (dos/console_dos.c)

Создать папку `dos/` и файл `dos/console_dos.c`:

```
Файл: dos/console_dos.c (~200 строк)
Содержит:
- #include "console.h"
- Функции, вызывающие существующий BIOS-код из video.c, console.c

Реализация функций:
- Console_Init()     -> вызвать существующую инициализацию
- Console_Close()    -> восстановить видео режим
- Console_GetSize()  -> вернуть SCREENWIDTH, SCREENHEIGHT
- Console_ReadScreen() -> вызвать getvideo()
- Console_WriteScreen() -> вызвать storevideo()
- Console_GetChar()  -> прочитать из видеопамяти напрямую
- Console_PutChar()  -> записать в видеопамять напрямую
- Console_SetCursor() -> вызвать gotoxy()
- Console_GetCursor() -> вызвать wherex(), wherey()
- Console_HideCursor() -> вызвать hidecursor()
- Console_ShowCursor() -> вызвать unhidecursor()
- Console_KbHit()    -> вызвать charavail()
- Console_GetKey()   -> вызвать readychar()
- Console_GetShiftState() -> вызвать shiftkey()
- Console_GetTickCount() -> прочитать из 0x046C
- Console_Beep()     -> вызвать beep()
- Console_FindFirst() -> вызвать findfirst()
- Console_FindNext() -> вызвать findnext()
- Console_ChangeDir() -> вызвать chdir()
- и др.
```

#### 3.3. Win32-реализация (win32/console_win32.c)

Создать папку `win32/` и файл `win32/console_win32.c`:

```
Файл: win32/console_win32.c (~400 строк)
Содержит:
- #include "console.h"
- #include <windows.h>
- Win32 Console API реализация

Реализация - полностью аналогично tinycc/tccwin32.c:
- Использует GetStdHandle(), SetConsoleCursorPosition()
- Использует ReadConsoleOutput(), WriteConsoleOutput()
- Использует ReadConsoleInput(), PeekConsoleInput()
- Использует GetTickCount()
- Использует FindFirstFile(), FindNextFile()

Примечание: Файл создаётся как "чистая" PAL-реализация,
используется в Фазе 5 после рефакторинга из tinycc/tccwin32.c
```

#### 3.4. Unix-реализация (unix/console_unix.c)

Создать папку `unix/` и файл `unix/console_unix.c` (Фаза 8):

```
Файл: unix/console_unix.c (~500 строк)
Содержит:
- #include "console.h"
- ncurses реализация

Реализация:
- initscr(), endwin() для инициализации/завершения
- getmaxyx() для размера экрана
- mvwinch(), mvwaddch() для чтения/записи
- wgetch() для клавиатуры
- getmouse() для мыши (если поддерживается)
```

#### 3.5. Интеграция с D-Flat

**Выполняется после Фазы 7**, после рефакторинга tinycc → win32.

Модификации в существующих файлах:

```
dflat.h - добавить (после рефакторинга):
  #if defined(__TINYC__) || defined(_WIN32)
      #include "console.h"
      #define CONSOLE_IMPLEMENTATION win32
  #elif defined(__unix__) || defined(__linux__)
      #include "console.h"
      #define CONSOLE_IMPLEMENTATION unix
  #else
      #define CONSOLE_IMPLEMENTATION dos
  #endif

system.h - добавить:
  #ifdef CONSOLE_IMPLEMENTATION
      #undef SCREENWIDTH
      #undef SCREENHEIGHT
      #define SCREENWIDTH  (consoleWidth)
      #define SCREENHEIGHT (consoleHeight)
      extern int consoleWidth;
      extern int consoleHeight;
  #endif

video.h - добавить:
  #ifdef CONSOLE_IMPLEMENTATION
      // Использовать Console_ReadScreen/Console_WriteScreen
  #endif

message.c - изменить:
  // Заменить direct hardware access на Console_GetTickCount()
```

#### 3.6. Структура PAL после завершения Фазы 3

```
console.h        - интерфейс (единый для всех платформ)
    ↓
dos/console_dos.c    - BIOS-вызовы (существующий код)
win32/console_win32.c - Win32 API (новая реализация)
unix/console_unix.c  - ncurses (Фаза 8)
```

---

## Фаза 2: Инфраструктура совместимости

**Требования к каждой задаче:**
- Сначала написать юнит-тест
- Проверить, что тест не проходит (красный)
- Реализовать функциональность
- Проверить, что тест проходит (зелёный)
- Рефакторинг при необходимости

### 2.1. tinycc/tcccompat.h

Создать файл `tinycc/tcccompat.h` со следующим содержимым:

#### Раздел 1: Макросы памяти (указатели far/near)
```c
#ifndef TCCCOMPAT_H
#define TCCCOMPAT_H

#define far
#define near
#define _far
#define _near
#define huge
#define far16
#define near16
#define __far
#define __near

#define MK_FP(seg, off)  ((void *)(((unsigned long)(seg) << 16) | (unsigned)(off)))
#define FP_SEG(fp)       ((unsigned)((unsigned long)(fp) >> 16))
#define FP_OFF(fp)       ((unsigned)(fp))

#define ptr_to_linear(x) ((unsigned long)(x))
#define linear_to_ptr(x) ((void *)(x))
```

#### Раздел 2: Макросы прерываний
```c
#define interrupt
#define _interrupt
#define __interrupt

#define _dos_entry_t struct __dos_entry_t_placeholder
```

#### Раздел 3: Порт ввода/вывода
```c
#define outp(port, val) do { (void)(port); (void)(val); } while(0)
#define outportw(port, val) do { (void)(port); (void)(val); } while(0)
#define inp(port) 0
#define inportw(port) 0
#define _inp(port) 0
#define _outp(port, val) do { (void)(port); (void)(val); } while(0)
```

#### Раздел 4: Структуры регистров DOS
```c
typedef unsigned char       BYTE;
typedef unsigned int        WORD;
typedef unsigned long       DWORD;
typedef unsigned int        UWORD;
typedef unsigned long       UDWORD;
typedef short               SHORT;
typedef int                 SIGNED;
typedef unsigned char       UCHAR;

typedef union _REGS {
    struct {
        unsigned int ax, bx, cx, dx, si, di, cflag, flags;
    } x;
    struct {
        unsigned char al, ah, bl, bh, cl, ch, dl, dh;
    } h;
} REGS;

typedef struct _SREGS {
    unsigned int es;
    unsigned int cs;
    unsigned int ss;
    unsigned int ds;
} SREGS;

typedef struct _IREGS {
    unsigned long edi;
    unsigned long esi;
    unsigned long ebp;
    unsigned long ebx;
    unsigned long edx;
    unsigned long ecx;
    unsigned long eax;
    unsigned short flags;
    unsigned short es, ds, fs, gs, ip, cs, sp, ss;
} IREGS;

#define _wregs REGS
```

#### Раздел 5: Номера прерываний
Определены для совместимости. Используются в симуляции int86() для определения типа вызова.

```c
#define TIMER       0x08
#define VIDEO       0x10
#define KEYBRD      0x16
#define DOS         0x21
#define CRIT        0x24
#define MOUSE       0x33
#define DIVERR      0x00
#define NMI         0x02
#define PRINTF      0x05
#define BIOSDATA    0x12
#define KBDCODE     0x09
```

#### Раздел 6: Структура ffblk для findfirst/findnext
Базовые поля ffblk в tcccompat.h. Win32-специфичные поля (WIN32_FIND_DATAW, HANDLE) определены в tccwin32.h

```c
struct ffblk {
    unsigned long ff_reserved;
    unsigned long ff_attrib;
    unsigned short ff_ftime;
    unsigned short ff_fdate;
    unsigned long ff_fsize;
    char ff_name[13];
    /* Win32 extended data - определены в tccwin32.h при #include <windows.h> */
};

#define FA_RDONLY   0x01
#define FA_HIDDEN   0x02
#define FA_SYSTEM   0x04
#define FA_LABEL    0x08
#define FA_DIREC    0x10
#define FA_ARCH     0x20

#define _A_RDONLY   FA_RDONLY
#define _A_HIDDEN   FA_HIDDEN
#define _A_SYSTEM   FA_SYSTEM
#define _A_LABEL    FA_LABEL
#define _A_DIREC    FA_DIREC
#define _A_ARCH     FA_ARCH
```

#### Раздел 7: Структуры видеопамяти
```c
typedef struct text_info {
    unsigned char curx;
    unsigned char cury;
    unsigned char screenwidth;
    unsigned char screenheight;
    unsigned char currmode;
    unsigned char attribute;
    unsigned char normattr;
    unsigned char colorstate;
    unsigned char cpass;
    unsigned long winleft;
    unsigned long wintop;
    unsigned long winright;
    unsigned long winbottom;
} text_info;

struct country {
    int co_date;
    char co_curr[5];
    char co_thsep[2];
    char co_desep[2];
    char co_dtsep[2];
    char co_tmsep[2];
    char co_currstr[3];
    char co_right;
    char co_decimals;
    char co_leadbr[2];
    char co_negpos;
    char co_negsign;
    char co_lstrike;
    unsigned int co_time;
    unsigned int co_case;
    unsigned int co_sort;
    unsigned int co_code;
};

struct dosdate_t {
    unsigned char da_year;
    unsigned char da_month;
    unsigned char da_day;
    unsigned char da_dow;
};

struct dostime_t {
    unsigned char dt_hour;
    unsigned char dt_min;
    unsigned char dt_sec;
    unsigned char dt_hsec;
};
```

#### Раздел 8: Переменные клавиатуры
```c
extern unsigned char keyportvalue;
#define KEYBOARDPORT 0x60

#define KB_SHIFT     0x01
#define KB_CTRL      0x04
#define KB_ALT       0x08
#define KB_SCROLL    0x10
#define KB_NUMLOCK   0x20
#define KB_CAPSLOCK  0x40
#define KB_INSLOCK   0x80
```

#### Раздел 9: Определения peek/poke
```c
#define peek(seg, off)       (*((unsigned int *)(((seg) << 16) | (off))))
#define pokeb(seg, off, val) (*((unsigned char *)(((seg) << 16) | (off))) = (val))
#define pokew(seg, off, val) (*((unsigned int *)(((seg) << 16) | (off))) = (val))
#define _peek(seg, off)      (*((unsigned int *)(((seg) << 16) | (off))))
#define _pokeb(seg, off, val) (*((unsigned char *)(((seg) << 16) | (off))) = (val))
#define _pokew(seg, off, val) (*((unsigned int *)(((seg) << 16) | (off))) = (val))
```

#### Раздел 10: Размеры экрана (будут переопределены)
```c
#ifndef SCREENWIDTH
#define SCREENWIDTH  80
#endif

#ifndef SCREENHEIGHT
#define SCREENHEIGHT 25
#endif
```

#### Раздел 11: Цвета (DOS атрибуты)
```c
#define BLACK        0
#define BLUE         1
#define GREEN        2
#define CYAN         3
#define RED          4
#define MAGENTA      5
#define BROWN        6
#define LIGHTGRAY    7
#define DARKGRAY     8
#define LIGHTBLUE    9
#define LIGHTGREEN   10
#define LIGHTCYAN    11
#define LIGHTRED     12
#define LIGHTMAGENTA 13
#define YELLOW       14
#define WHITE        15

#define BLINK        0x80
#define UNDERLINE    0x01
#define REVERSE      0x70

/* Атрибуты цвета для комбинаций foreground/background */
#define attr(fg, bg) (((bg) << 4) | (fg))
```

#### Раздел 12: Видео режимы
```c
#define LASTMODE     -1
#define BW40         0
#define C40          1
#define BW80         2
#define C80          3
#define MONO         7
#define C4350        8

#define _LASTMODE    LASTMODE
#define _BW40        BW40
#define _C40         C40
#define _BW80        BW80
#define _C80         C80
#define _MONO        MONO
#define _C4350       C4350
```

#### Раздел 13: Определения getvect/setvect
```c
typedef void (*interrupt_handler_t)(void);

#define getvect(intno)       (NULL)
#define setvect(intno, func) do { (void)(intno); (void)(func); } while(0)
```

#### Раздел 14: Типы D-Flat
```c
#ifndef BOOL
typedef enum { FALSE = 0, TRUE = 1 } BOOL;
#endif
typedef long PARAM;

#ifndef NULL
#define NULL ((void *)0)
#endif

#define farpascal
#define _farpascal far
#define cdecl
#define _cdecl
```

#endif /* TCCCOMPAT_H */

**Примечание**: `BOOL` определяется в dflat.h:45. Добавлен `#ifndef BOOL` для предотвращения конфликта.
```

### 2.2. tinycc/tccwin32.h

Объявления всех Win32-функций и внешние переменные для совместимости.

Создать файл `tinycc/tccwin32.h` со следующим содержимым:

#### Включаемые заголовки
```c
#ifndef TCCWIN32_H
#define TCCWIN32_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
```

#### Константы
```c
#define KEYBOARDPORT 0x60
#define CGA_ADDRESS 0xB800
#define MONO_ADDRESS 0xB000
```

#### Глобальные переменные
```c
extern HANDLE hConsoleInput;          // Дескриптор ввода консоли
extern HANDLE hConsoleOutput;         // Дескриптор вывода консоли
extern DWORD dwOriginalConsoleMode;   // Оригинальный режим консоли
extern int consoleWidth;              // Текущая ширина консоли
extern int consoleHeight;             // Текущая высота консоли
extern int currentCursorX;            // Текущая позиция курсора X
extern int currentCursorY;            // Текущая позиция курсора Y
extern int cursorVisible;             // Флаг видимости курсора
extern int cursorType;                // Тип курсора (0=normal, 1=block, 2=underline)
extern int mouseX;                    // Позиция мыши X
extern int mouseY;                    // Позиция мыши Y
extern int mouseButtons;              // Состояние кнопок мыши
extern int mouseVisible;              // Флаг видимости мыши
extern unsigned int videoMode;        // Текущий видео режим (текстовый)
extern unsigned char textAttribute;   // Текущий текстовый атрибут
```

#### Прототипы функций инициализации
```c
int InitWin32Console(void);           // Инициализация Win32 консоли
void CloseWin32Console(void);         // Закрытие и восстановление консоли
void GetConsoleSize(int *width, int *height);  // Получение размера окна
void SetConsoleSize(int width, int height);    // Установка размера окна
```

#### Прототипы видео-функций
```c
void Win32_getvideo(void *buf, int size);      // Чтение экрана
void Win32_storevideo(void *buf, int size);    // Запись экрана
int Win32_GetVideoChar(int x, int y);          // Получить символ в позиции
void Win32_PutVideoChar(int x, int y, int ch, int attr);  // Записать символ
int Win32_isEGA(void);                         // Проверка EGA (всегда FALSE)
int Win32_isVGA(void);                         // Проверка VGA (всегда FALSE)
void Win32_Set25(void);                        // Установить 25 строк
void Win32_Set43(void);                        // Установить 43 строки
void Win32_Set50(void);                        // Установить 50 строк
void Win32_SetTextAttribute(int attr);         // Установить атрибут текста
int Win32_GetTextAttribute(void);              // Получить текущий атрибут
```

#### Прототипы курсор-функций
```c
void Win32_cursor(int x, int y);               // Установить позицию курсора
void Win32_savecursor(void);                   // Сохранить позицию курсора
void Win32_restorecursor(void);                // Восстановить позицию курсора
void Win32_hidecursor(void);                   // Скрыть курсор
void Win32_unhidecursor(void);                 // Показать курсор
void Win32_set_cursor_type(int type);          // Установить тип курсора
void Win32_curr_cursor(int *x, int *y);        // Получить текущую позицию
void Win32_hidecursor(void);                   // Скрыть курсор
void Win32_showcursor(void);                   // Показать курсор
```

#### Прототипы клавиатурных функций
```c
int Win32_keyhit(void);                        // Проверка наличия нажатия
int Win32_getkey(void);                        // Получить код клавиши
int Win32_getshift(void);                      // Получить состояние Shift/Alt/Ctrl
int Win32_bioskey(int cmd);                    // Функция BIOS для клавиатуры
void Win32_beep(void);                         // Звуковой сигнал
```

#### Прототипы мышиных функций
```c
int Win32_mouse_installed(void);               // Проверка мыши (всегда TRUE)
int Win32_mousebuttons(void);                  // Получить количество кнопок
void Win32_get_mouseposition(int *x, int *y, int *buttons);  // Позиция мыши
void Win32_set_mouseposition(int x, int y);   // Установить позицию мыши
void Win32_show_mousecursor(void);             // Показать курсор мыши
void Win32_hide_mousecursor(void);             // Скрыть курсор мыши
void Win32_resetmouse(void);                   // Сброс мыши
void Win32_set_mousetravel(int x1, int y1, int x2, int y2);  // Границы движения
int Win32_button_releases(void);               // Проверка отпускания кнопки
```

#### Прототипы файловых функций
```c
long Win32_findfirst(const char *pattern, void *ffblk);  // Поиск первого файла
int Win32_findnext(void *ffblk);               // Поиск следующего файла
void Win32_findclose(void *ffblk);             // Закрыть поиск
char Win32_getdisk(void);                      // Получить текущий диск
int Win32_setdisk(char drive);                 // Установить текущий диск
char *Win32_getcwd(char drive, char *path);    // Получить текущий каталог
int Win32_chdir(const char *path);             // Сменить каталог
```

#### Расширение ffblk для Win32
```c
// Win32-специфичное расширение - хранится как статические переменные в tccwin32.c
// (не в struct ffblk для совместимости с DOS-версией)
static WIN32_FIND_DATAW g_FindData;
static HANDLE g_FindHandle;
```

#### Прототипы таймерных функций
```c
unsigned long Win32_GetTickCount(void);        // Получить время в мс
void Win32_sleep(unsigned int ms);             // Задержка в мс
time_t Win32_time(time_t *timer);              // Получить время системы
struct tm *Win32_localtime(const time_t *timer);  // Локальное время
```

#### Прототипы прерываний
```c
int Win32_int86(int intno, union REGS *in, union REGS *out);      // Общий int86
int Win32_int86x(int intno, union REGS *in, union REGS *out,     // int86 с сегментами
                 struct SREGS *sreg);
void *Win32_getvect(int intno);              // Получить вектор (заглушка)
void Win32_setvect(int intno, void *handler); // Установить вектор (заглушка)
```

#### Прототипы вспомогательных функций
```c
void Win32_setvideomode(int mode);           // Установить видео режим
int Win32_getvideomode(void);                // Получить видео режим
void Win32_PollInput(void);                  // Опрос ввода консоли
int ScanCodeToKey(int scan, int alt);        // Преобразование скан-кода
int AltConvert(int key);                     // Преобразование Alt-символов

#endif /* TCCWIN32_H */
```

### 2.3. tinycc/tccwin32.c

Основной файл реализации DOS-функций через Win32 Console API.

Создать файл `tinycc/tccwin32.c` со следующей структурой:

#### Раздел 1: Глобальные переменные (объявления)
```c
HANDLE hConsoleInput = NULL;
HANDLE hConsoleOutput = NULL;
DWORD dwOriginalConsoleMode = 0;
int consoleWidth = 80;
int consoleHeight = 25;
int currentCursorX = 0;
int currentCursorY = 0;
int cursorVisible = TRUE;
int cursorType = 0;  // 0=normal, 1=block, 2=underline
int mouseX = 0;
int mouseY = 0;
int mouseButtons = 0;
int mouseVisible = FALSE;
unsigned int videoMode = 3;  // Текстовый режим 80x25
unsigned char textAttribute = 0x07;  // Светло-серый на чёрном

// Стек для сохранения позиции курсора
#define CURSOR_STACK_SIZE 10
static int cursorStack[CURSOR_STACK_SIZE];
static int cursorStackTop = 0;

// Буфер для чтения/записи экрана
static CHAR_INFO *screenBuffer = NULL;
static int screenBufferSize = 0;
```

#### Раздел 2: Инициализация Win32 консоли
```
Функция: InitWin32Console()
- Получить дескрипторы: GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE)
- Сохранить оригинальный режим: GetConsoleMode(hConsoleInput, &dwOriginalConsoleMode)
- Установить режим консоли: ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS
- Установить кодовую страницу: SetConsoleOutputCP(866), SetConsoleCP(866)
- Получить размер: GetConsoleScreenBufferInfo()
- Выделить буфер: calloc(80 * 25 * 2, sizeof(CHAR_INFO))
- Инициализировать: videoMode = 3, textAttribute = 0x07
```

#### Раздел 3: Видео-подсистема
```
Функция: Win32_getvideo(void *buf, int size)
- Проверить буфер screenBuffer
- Вызвать ReadConsoleOutput(hConsoleOutput, screenBuffer, {80, 25}, {0, 0}, &sr)
- Скопировать данные в пользовательский буфер (символы + атрибуты)

Функция: Win32_storevideo(void *buf, int size)
- Скопировать данные в screenBuffer
- Вызвать WriteConsoleOutput(hConsoleOutput, screenBuffer, {80, 25}, {0, 0}, &sr)

Функция: Win32_GetVideoChar(int x, int y)
- COORD pos = {x, y}
- ReadConsoleOutputCharacter(hConsoleOutput, &ch, 1, pos, &written)
- ReadConsoleOutputAttribute(hConsoleOutput, &attr, 1, pos, &written)
- Вернуть (attr << 8) | ch

Функция: Win32_PutVideoChar(int x, int y, int ch, int attr)
- COORD pos = {x, y}
- WriteConsoleOutputCharacter(hConsoleOutput, &ch, 1, pos, &written)
- WriteConsoleOutputAttribute(hConsoleOutput, &attr, 1, pos, &written)

Функция: Win32_isEGA(void) - вернуть FALSE
Функция: Win32_isVGA(void) - вернуть FALSE
Функция: Win32_Set25(void) - SetConsoleWindowInfo(TRUE, {0,0,79,24})
Функция: Win32_Set43(void) - SetConsoleWindowInfo(TRUE, {0,0,79,42})
Функция: Win32_Set50(void) - SetConsoleWindowInfo(TRUE, {0,0,79,49})
Функция: Win32_SetTextAttribute(int attr) - textAttribute = (unsigned char)attr
Функция: Win32_GetTextAttribute(void) - вернуть textAttribute
```

#### Раздел 4: Курсор
```
Функция: Win32_cursor(int x, int y)
- COORD pos = {x, y}
- SetConsoleCursorPosition(hConsoleOutput, pos)
- currentCursorX = x, currentCursorY = y

Функция: Win32_savecursor(void)
- cursorStack[cursorStackTop++] = currentCursorY * 80 + currentCursorX

Функция: Win32_restorecursor(void)
- pos = cursorStack[--cursorStackTop]
- currentCursorY = pos / 80, currentCursorX = pos % 80
- Win32_cursor(currentCursorX, currentCursorY)

Функция: Win32_hidecursor(void)
- CONSOLE_CURSOR_INFO cci = {100, FALSE}
- SetConsoleCursorInfo(hConsoleOutput, &cci)
- cursorVisible = FALSE

Функция: Win32_unhidecursor(void)
- CONSOLE_CURSOR_INFO cci = {cursorType == 2 ? 100 : 50, TRUE}
- SetConsoleCursorInfo(hConsoleOutput, &cci)
- cursorVisible = TRUE

Функция: Win32_set_cursor_type(int type)
- cursorType = type
- CONSOLE_CURSOR_INFO cci
- cci.dwSize = (type == 2) ? 100 : 50
- cci.bVisible = cursorVisible
- SetConsoleCursorInfo(hConsoleOutput, &cci)

Функция: Win32_curr_cursor(int *x, int *y)
- CONSOLE_SCREEN_BUFFER_INFO csbi
- GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)
- *x = csbi.dwCursorPosition.X
- *y = csbi.dwCursorPosition.Y

Функция: Win32_showcursor(void) - вызвать Win32_unhidecursor()
Функция: Win32_hidecursor(void) - вызвать Win32_hidecursor() (дубликат для совместимости)
```

#### Раздел 5: Клавиатура
```
Функция: Win32_keyhit(void)
- DWORD events = 0
- GetNumberOfConsoleInputEvents(hConsoleInput, &events)
- вернуть events > 0

Функция: Win32_getkey(void)
- Вызвать Win32_PollInput() для обработки событий
- Если есть события KEY_EVENT в очереди, извлечь и преобразовать
- Преобразование: ScanCodeToKey(scanCode, (extended ? 0x80 : 0))

Функция: Win32_getshift(void)
- Вернуть: (GetKeyState(VK_SHIFT) & 0x80 ? 1 : 0) |
           (GetKeyState(VK_CONTROL) & 0x80 ? 4 : 0) |
           (GetKeyState(VK_MENU) & 0x80 ? 8 : 0) |
           (GetKeyState(VK_CAPITAL) & 0x1 ? 0x40 : 0)

Функция: Win32_bioskey(int cmd)
- cmd == 0: вернуть Win32_getkey()
- cmd == 1: вернуть Win32_keyhit() ? Win32_getkey() : 0
- cmd == 2: вернуть Win32_getshift()

Функция: Win32_beep(void)
- MessageBeep(MB_OK) или Beep(800, 200)

Функция: Win32_PollInput(void)
- INPUT_RECORD ir[32]
- DWORD count
- PeekConsoleInput(hConsoleInput, ir, 32, &count)
- Для каждого события:
  - MOUSE_EVENT: обновить mouseX, mouseY, mouseButtons
  - KEY_EVENT: добавить в очередь клавиш (если KeyDown)
  - WINDOW_BUFFER_SIZE_EVENT: обновить consoleWidth, consoleHeight
- Вернуть количество обработанных событий
```

#### Раздел 6: Мышь
```
Функция: Win32_mouse_installed(void)
- вернуть TRUE (Win32 консоль всегда поддерживает мышь)

Функция: Win32_mousebuttons(void)
- вернуть 2 (левая + правая кнопка)

Функция: Win32_get_mouseposition(int *x, int *y, int *buttons)
- Координаты мыши обновляются в Win32_PollInput() при обработке событий MOUSE_EVENT
- *x = mouseX (текущая позиция X в символах)
- *y = mouseY (текущая позиция Y в символах)
- *buttons = mouseButtons (состояние кнопок из очереди событий)

Реализация Win32_PollInput():
- При получении MOUSE_EVENT: преобразовать координаты из пикселей в символы
- Вычислить размер символа: csbi.dwSize.X / (csbi.srWindow.Right - csbi.srWindow.Left + 1)
- mouseX = (Event.MousePosition.X - csbi.srWindow.Left) / charWidth
- mouseY = (Event.MousePosition.Y - csbi.srWindow.Top) / charHeight

Функция: Win32_set_mouseposition(int x, int y)
- COORD pos = {x + csbi.srWindow.Left, y + csbi.srWindow.Top}
- SetConsoleCursorPosition(hConsoleOutput, pos)
- mouseX = x, mouseY = y

Функция: Win32_show_mousecursor(void)
- mouseVisible = TRUE
- (Визуальный курсор мыши - это позиция курсора консоли)

Функция: Win32_hide_mousecursor(void)
- mouseVisible = FALSE

Функция: Win32_resetmouse(void)
- mouseX = mouseY = mouseButtons = 0

Функция: Win32_set_mousetravel(int x1, int y1, int x2, int y2)
- Установить границы для мыши (проверка в set_mouseposition)

Функция: Win32_button_releases(void)
- Вернуть состояние отпускания кнопок из очереди событий
```

#### Раздел 7: Таймер и время
```
Функция: Win32_GetTickCount(void)
- вернуть GetTickCount() (миллисекунды с запуска системы)

Функция: Win32_sleep(unsigned int ms)
- Sleep(ms)

Функция: Win32_time(time_t *timer)
- вернуть time(timer)

Функция: Win32_localtime(const time_t *timer)
- вернуть localtime(timer)
```

#### Раздел 8: Файловые операции

**Структуры данных:**

Структура `ffblk` (определена в tcccompat.h) содержит базовые поля:
```c
struct ffblk {
    unsigned long ff_reserved;
    unsigned long ff_attrib;
    unsigned short ff_ftime;
    unsigned short ff_fdate;
    unsigned long ff_fsize;
    char ff_name[13];
};
```

Для Win32 используется расширение через union или отдельное хранение WIN32_FIND_DATA:
```c
// В tccwin32.c - глобальная переменная для хранения Win32-данных
static WIN32_FIND_DATAW currentFindData;
static HANDLE currentFindHandle = INVALID_HANDLE_VALUE;
```

**Реализация:**

```
Функция: Win32_findfirst(const char *pattern, void *ffblk)
- Сохранить WIN32_FIND_DATAW в currentFindData
- currentFindHandle = FindFirstFileW(pattern, &currentFindData)
- Скопировать данные из currentFindData в ffblk (ff_attrib, fsize, ftime, fdate, name)
- вернуть (currentFindHandle == INVALID_HANDLE_VALUE) ? -1 : 0

Функция: Win32_findnext(void *ffblk)
- result = FindNextFileW(currentFindHandle, &currentFindData)
- Скопировать данные из currentFindData в ffblk
- вернуть result ? 0 : -1

Функция: Win32_findclose(void *ffblk)
- FindClose(currentFindHandle)
- currentFindHandle = INVALID_HANDLE_VALUE

Функция: Win32_getdisk(void)
- char cwd[3] = "C:"
- GetCurrentDirectory(3, cwd)
- вернуть cwd[0] - 'A'

Функция: Win32_setdisk(char drive)
- char path[4] = "X:\\"
- path[0] = drive + 'A'
- вернуть SetCurrentDirectory(path) ? 0 : 1

Функция: Win32_getcwd(char drive, char *path)
- Если drive != 0: получить текущий диск, сформировать путь
- Иначе: GetCurrentDirectory(MAX_PATH, path)
- вернуть path

Функция: Win32_chdir(const char *path)
- вернуть SetCurrentDirectory(path) ? 0 : -1
```

#### Раздел 9: Прерывания (int86)
```
Функция: Win32_int86(int intno, union REGS *in, union REGS *out)
- switch(intno):
  - 0x10 (VIDEO): обработать через video API
  - 0x16 (KEYBOARD): обработать через keyboard API
  - 0x21 (DOS): обработать через DOS API
  - 0x33 (MOUSE): обработать через mouse API
  - default: memset(out, 0, sizeof(*out)), return 0

Функция: Win32_int86x(...)
- Аналогично int86, но игнорировать struct SREGS (flat memory модель)

Функция: Win32_getvect(int intno)
- вернуть NULL (прерывания не используются в Win32)

Функция: Win32_setvect(int intno, void *handler)
- Ничего не делать (прерывания не используются в Win32)
```

#### Раздел 10: Вспомогательные функции
```
Функция: Win32_setvideomode(int mode)
- videoMode = mode
- Вызвать SetConsoleWindowInfo для установки размера (25/43/50)

Функция: Win32_getvideomode(void)
- вернуть videoMode

Функция: ScanCodeToKey(int scan, int alt)
- Статическая таблица: скан-код → виртуальный код
- Обработка расширенных кодов (0xE0 prefix)
- Обработка Alt: (alt << 8) | символ

Функция: AltConvert(int key)
- Статическая таблица Alt+цифры → символ (как в оригинальном DOS)
```

#### Раздел 11: Завершение работы
```
Функция: CloseWin32Console(void)
- Восстановить курсор: ShowCursor(TRUE)
- Восстановить режим консоли: SetConsoleMode(hConsoleInput, dwOriginalConsoleMode)
- Освободить буфер: free(screenBuffer)
- Закрыть дескрипторы: CloseHandle(hConsoleInput), CloseHandle(hConsoleOutput)
```

#### Видео-система

| Функция DOS | Win32 API |
|-------------|-----------|
| `getvideo()` | `ReadConsoleOutputCharacter` / `ReadConsoleOutputAttribute` |
| `storevideo()` | `WriteConsoleOutputCharacter` / `WriteConsoleOutputAttribute` |
| `GetVideoChar()` | `ReadConsoleOutput` |
| `PutVideoChar()` | `WriteConsoleOutput` |
| `isEGA()` / `isVGA()` | Всегда FALSE |
| `Set25()` / `Set43()` / `Set50()` | `SetConsoleWindowInfo` |

#### Курсор

| Функция DOS | Win32 API |
|-------------|-----------|
| `cursor()` | `SetConsoleCursorPosition` |
| `savecursor()` / `restorecursor()` | Стек позиций |
| `hidecursor()` / `unhidecursor()` | `CONSOLE_CURSOR_INFO` |
| `set_cursor_type()` | `CONSOLE_CURSOR_INFO` |

#### Клавиатура

| Функция DOS | Win32 API |
|-------------|-----------|
| `getkey()` | `ReadConsoleInput` с преобразованием в скан-коды |
| `keyhit()` | `PeekConsoleInput` |
| `getshift()` | `GetKeyState` |
| `beep()` | `MessageBeep` |

#### Мышь

| Функция DOS | Win32 API |
|-------------|-----------|
| `mouse_installed()` | Проверка `ENABLE_MOUSE_INPUT` (fallback: FALSE) |
| `mousebuttons()` | Состояние из `ReadConsoleInput` |
| `get_mouseposition()` | Преобразование из пикселей в символы |
| `set_mouseposition()` | `SetConsoleCursorPosition` |
| `resetmouse()` | Инициализация |
| `set_mousetravel()` | `SetConsoleWindowInfo` |

**Fallback для мыши**: Если `ENABLE_MOUSE_INPUT` не установлен, `Win32_mouse_installed()` возвращает FALSE. D-Flat корректно работает с клавиатурой без мыши (штатный режим).

**Проверка поддержки мыши**: Флаг `ENABLE_MOUSE_INPUT` устанавливается в `InitWin32Console()`. Проверить можно вызовом `GetConsoleMode(hConsoleInput, &mode)` и проверкой бита `ENABLE_MOUSE_INPUT`.

#### Таймер

Вместо прерываний использовать `GetTickCount64()` для отслеживания времени.

**Таймеры определены в message.c:49-51:**
```c
static volatile int doubletimer = -1;
static volatile int delaytimer  = -1;
static volatile int clocktimer  = -1;
```

**Используются через макросы из system.h:83-88:**
```c
#define timed_out(timer)       (timer==0)
#define set_timer(timer, secs) timer=(secs)*182/10+1
#define disable_timer(timer)   timer = -1
#define timer_running(timer)   (timer > 0)
#define countdown(timer)       --timer
```

**Замена на polling в dispatch_message():**
```c
#ifdef __TINYC__
    static DWORD lastTick = 0;
    DWORD now = GetTickCount();
    if (now != lastTick) {
        lastTick = now;
        if (timer_running(doubletimer)) countdown(doubletimer);
        if (timer_running(delaytimer))  countdown(delaytimer);
        if (timer_running(clocktimer))  countdown(clocktimer);
    }
    #endif
```

Используется `DWORD` и `GetTickCount()` (32-бит), достаточно для систем работающих менее 49 дней. Альтернатива - `GetTickCount64()` для длительных сессий.

#### Файловые операции

| Функция DOS | Win32 API |
|-------------|-----------|
| `findfirst()` / `findnext()` | `FindFirstFile` / `FindNextFile` |
| `fnmerge()` / `fnsplit()` | `_makepath` / `_splitpath` |
| `getdisk()` / `setdisk()` | `GetCurrentDirectory` / `SetCurrentDirectory` |

#### Универсальный int86()

```c
int int86(int intno, union REGS *in, union REGS *out) {
    switch(intno) {
        case 0x10: return int86_video(in, out);
        case 0x16: return int86_keyboard(in, out);
        case 0x21: return int86_dos(in, out);
        case 0x33: return int86_mouse(in, out);
        default: memset(out, 0, sizeof(*out)); return 0;
    }
}
```

**Поддерживаемые функции DOS (int 0x21):**

| AH | Функция | Реализация |
|----|---------|------------|
| 0x4E | findfirst() | Win32_findfirst() |
| 0x4F | findnext() | Win32_findnext() |
| 0x3D | open file | fopen() |
| 0x3F | read file | fread() |
| 0x40 | write file | fwrite() |
| 0x3E | close file | fclose() |
| 0x47 | getcwd() | Win32_getcwd() |
| 0x0F | setdisk() | Win32_setdisk() |
| 0x19 | getdisk() | Win32_getdisk() |
| 0x3B | chdir() | Win32_chdir() |
| 0x44 | ioctl | заглушка (возвращает 0) |
| 0x54 | get verify flag | возвращает 0 |
| 0x30 | get DOS version | возвращает 5.0 |

**Пример реализации int86_dos():**
```c
int int86_dos(union REGS *in, union REGS *out) {
    switch(in->x.ah) {
        case 0x4E:  // findfirst
            return Win32_findfirst((char *)in->x.dx, (void *)in->x.di);
        case 0x4F:  // findnext
            return Win32_findnext((void *)in->x.di);
        case 0x3D:  // open
        case 0x3F:  // read
        case 0x40:  // write
        case 0x3E:  // close
            // Использовать stdio
        default:
            memset(out, 0, sizeof(*out));
            return 0;
    }
}
```

## Фаза 4: Модификация заголовков

**Требования к каждой задаче:**
- Сначала написать юнит-тест
- Проверить, что тест не проходит (красный)
- Реализовать функциональность
- Проверить, что тест проходит (зелёный)
- Рефакторинг при необходимости

### 4.1. dflat.h

Добавить после существующих `#ifdef` блоков секцию для TinyCC:

```c
#elif defined(__TINYC__) || defined(_WIN32)
    #include "tinycc/tcccompat.h"
    #include <windows.h>
```

Конкретные изменения в dflat.h:
- [ ] Добавить проверку `#if defined(__TINYC__)` после `#ifdef __SMALLER_C__` |  | 
- [ ] Включить `#include "tinycc/tcccompat.h"` |  | 
- [ ] Включить `#include <windows.h>` для Win32 API |  | 
- [ ] Добавить `#define __TINYC__` в makefile.tinycc если не определён |  | 
- [ ] Проверить отсутствие конфликтов: `__declspec(dllimport)` не должен конфликтовать с `far` |  | 
- [ ] Добавить `#ifndef BOOL` guard вокруг определения BOOL в dflat.h:45 |  | 
  - Решение: изменить `typedef enum {FALSE, TRUE} BOOL;` на:
    ```c
    #ifndef BOOL
    typedef enum {FALSE, TRUE} BOOL;
    #endif
    ``` |  | 

### 4.2. system.h

Добавить секцию `#ifdef __TINYC__` с переопределениями:

```c
#ifdef __TINYC__
    #define outp(port, val)       /* nothing - Win32 uses API */
    #define inp(port)             0
    #define getvect(intno)        NULL
    #define setvect(intno, func)  /* nothing */
    #define SCREENWIDTH   (consoleWidth)
    #define SCREENHEIGHT  (consoleHeight)
    #define keyportvalue  (0)
    
    extern int consoleWidth;
    extern int consoleHeight;
#else
    #ifdef __SMALLER_C__
        static volatile unsigned char far *clk = (void far *)0x0000046CL;
    #else
        static volatile int far *clk = (void far *)0x0000046CL;
    #endif
#endif
```

**Примечание**: Переменная `clk` (указатель на адрес BIOS-таймера 0x046C) используется только в DOS-реализации. Для TinyCC таймер реализуется через polling в `dispatch_message()` с использованием `GetTickCount()`.

Конкретные изменения в system.h:
- [ ] Найти `static volatile int far *clk` (строка ~40) |  | 
- [ ] Обернуть в `#ifndef __TINYC__` |  | 
- [ ] Добавить `#ifdef __TINYC__` секцию с:
  - `extern int consoleWidth;` |  | 
  - `extern int consoleHeight;` |  | 
  - `#define SCREENWIDTH (consoleWidth)` |  | 
  - `#define SCREENHEIGHT (consoleHeight)` |  | 
  - `#define outp(port, val)` - пустой макрос |  | 
  - `#define inp(port)` - возвращает 0 |  | 
  - `#define getvect(intno)` - возвращает NULL |  | 
  - `#define setvect(intno, func)` - пустой макрос |  | 
  - `#define keyportvalue` - убрать обращение к порту |  | 

### 4.3. video.h

Условная компиляция для `void far *` → `void *`:

```c
#ifndef VIDEO_H
#define VIDEO_H

/* Типы указателей - зависят от модели памяти */
#ifdef __TINYC__
    /* Для Win32 используется Win32 API, video_address не нужен */
    #define video_address  ((void *)0)
    typedef void *VIDEOPTR;
    typedef unsigned short VIDEOTYPE;
#else
    #if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
        #define video_address  ((void far *)0xB8000000L)
    #else
        #define video_address  ((void near *)0xB8000000L)
    #endif
    typedef void far *VIDEOPTR;
    typedef unsigned short far *VIDEOTYPE;
#endif
```

Конкретные изменения в video.h:
- [ ] Добавить `#ifdef __TINYC__` секцию:
  - `#define video_address ((void *)0)` - не используется, Win32 API |  | 
  - `typedef void *VIDEOPTR;` |  | 
  - `typedef unsigned short VIDEOTYPE;` |  | 
  - Для Win32 используется Win32 API (ReadConsoleOutput/WriteConsoleOutput), видеопамять не нужна |  | 
- [ ] Сохранить оригинальные определения в `#else` секции |  | 
- [ ] Проверить использование `void far *` в файлах:
  - video.c: функции `getvideo()`, `storevideo()`, `movefromscreen()`, `movetoscreen()` |  | 
  - Заменить `void far *` на `void *` при `#ifdef __TINYC__` |  | 

### 4.4. mouse.h

Проверить и модифицировать для TinyCC:

```c
#ifdef __TINYC__
    /* Win32 Console API мышь */
    extern int mouse_installed(void);
    extern void get_mouseposition(int *x, int *y, int *buttons);
    extern void set_mouseposition(int x, int y);
    /* ... остальные прототипы ... */
#endif
```

Конкретные изменения в mouse.h:
- [ ] Проверить все объявления с `far` |  | 
- [ ] Добавить `#ifdef __TINYC__` для переопределения типов |  | 
- [ ] Убедиться, что прототипы совместимы с tccwin32.h |  | 

## Фаза 5: Модификация исходников

**Требования к каждой задаче:**
- Сначала написать юнит-тест
- Проверить, что тест не проходит (красный)
- Реализовать функциональность
- Проверить, что тест проходит (зелёный)
- Рефакторинг при необходимости

### 5.1. message.c

Конкретные изменения:

1. **Обернуть обработчики прерываний:**
```c
#ifndef __TINYC__
static void interrupt far newtimer(void)
{
    /* ... оригинальный код обработчика таймера ... */
}
#endif
```

2. **Найти функцию `init_messages()` и модифицировать:**
   - Удалить/обернуть: `setvect(TIMER, newtimer)`
   - Удалить/обернуть: `setvect(KEYBRD, newkeyboard)`
   - Добавить: вызов `InitWin32Console()` из tccwin32.c
   - Добавить: инициализацию таймеров через `GetTickCount()`

3. **Модифицировать `dispatch_message()` (или `collect_events()`):**
   - Заменить проверку `timer_tick` на:
   ```c
   #ifdef __TINYC__
       static DWORD lastTick = 0;
       DWORD now = GetTickCount();
       if (now != lastTick) {
           lastTick = now;
           /* обработка таймеров */
           if (timer_running(doubletimer)) countdown(doubletimer);
           if (timer_running(delaytimer)) countdown(delaytimer);
           /* ... */
       }
   #else
       /* оригинальный код с прерываниями */
   #endif
   ```

4. **Удалить/обернуть мониторинг клавиатуры:**
   - Удалить: `keyportvalue = inp(KEYBOARDPORT)`
   - Заменить на: вызов `PollInput()` из tccwin32.c

5. **Конец работы:**
   - Добавить вызов `CloseWin32Console()` при выходе
   - **Расположение**: В `message.c`, функция `CloseApplication()` или в `memopad.c`, функция `main()` перед `return 0`
   - **Пример:**
     ```c
     // В memopad.c, в конце main()
     #ifdef __TINYC__
         CloseWin32Console();
     #endif
         return exitcode;
     ```

Конкретные функции для поиска и изменения (использовать grep):
- `static void interrupt far newtimer()` в message.c
- `init_messages()` - убрать setvect, добавить Win32 инициализацию
- `dispatch_message()` или `collect_events()` - добавить опрос GetTickCount()
- `keyportvalue = inp(KEYBOARDPORT)` - удалить мониторинг клавиатуры

### 5.2. video.c

**Функции `movefromscreen()` и `movetoscreen()`** — это отдельные функции (не синонимы), используемые для побайтового копирования областей экрана. Они отличаются от `getvideo()`/`storevideo()` тем, что работают с произвольными смещениями.

**Сигнатуры функций в video.c:**
```c
void movefromscreen(void *bf, int offset, int len);  // Из экрана в буфер
void movetoscreen(void *bf, int offset, int len);    // Из буфера на экран
```

Конкретные изменения:

1. **Добавить макрос FAR_PTR:**
```c
#ifdef __TINYC__
    #define FAR_PTR(addr, type) ((type)(addr))
#else
    #define FAR_PTR(addr, type) ((type)(addr))
#endif
```

2. **Заменить видео-операции на Win32:**
   - В `getvideo()`:
   ```c
   #ifdef __TINYC__
       Win32_getvideo(buf, size);
   #else
       /* оригинальный код с far pointers */
   #endif
   ```
   - В `storevideo()`: аналогично

3. **Удалить/заменить CGA-функции:**
   - `vpeek()` - не требуется (flat memory)
   - `vpoke()` - не требуется
   - `waitforretrace()` - не требуется

4. **Найти и изменить (использовать grep):**
   - `void getvideo(RECT rc, void far *buf)` → добавить `#ifdef __TINYC__` обёртку
   - `void storevideo(RECT rc, void far *buf)` → добавить `#ifdef __TINYC__` обёртку
   - `void movefromscreen(void *bf, int offset, int len)` → заменить на Win32_getvideo с учётом offset
   - `void movetoscreen(void *bf, int offset, int len)` → заменить на Win32_storevideo с учётом offset
   - `vpeek()` / `vpoke()` → обернуть в `#ifdef __TINYC__` (не требуются)
   - `waitforretrace()` → обернуть в `#ifdef __TINYC__` (не требуется)

### 5.3. console.c

Конкретные изменения:

1. **Обернуть обращение к часам:**
```c
#ifndef __TINYC__
    static volatile int far *clk = (void far *)0x0000046CL;
#endif
```

2. **Найти и модифицировать функции:**
   - `int charavail(void)` → заменить int86(KEYBRD, ...) на Win32_keyhit()
   - `int readychar(void)` → заменить на Win32_getkey()
   - `int shiftkey(void)` → заменить на Win32_getshift()
   - `void setcursortype(int type)` → заменить на Win32_set_cursor_type()
   - `void gotoxy(int x, int y)` → заменить на Win32_cursor()

3. **Удалить неиспользуемые функции:**
   - Удалить: обращения к портам `outp()`, `inp()`

### 5.4. mouse.c

Конкретные изменения:

1. **Заменить int86(MOUSE, ...) вызовы:**
   - Найти: `int86(MOUSE, &inregs, &outregs)`
   - Заменить на вызовы функций из tccwin32.c:
     - `mouse_installed()` → Win32_mouse_installed()
     - `mousebuttons()` → Win32_mousebuttons()
     - `get_mouseposition()` → Win32_get_mouseposition()
     - `set_mouseposition()` → Win32_set_mouseposition()
     - `resetmouse()` → Win32_resetmouse()

2. **Пример замены:**
   - Было:
   ```c
   inregs.x.ax = 0;
   int86(MOUSE, &inregs, &outregs);
   return outregs.x.ax;
   ```
   - Стало:
   ```c
   #ifdef __TINYC__
       return Win32_mouse_installed();
   #else
       inregs.x.ax = 0;
       int86(MOUSE, &inregs, &outregs);
       return outregs.x.ax;
   #endif
   ```

### 5.5. direct.c

Конкретные изменения:

1. **Проверить использование int86(DOS, ...):**
   - Найти все вызовы `int86(0x21, ...)`
   - Заменить на Win32 API:
     - `findfirst()` → Win32_findfirst()
     - `findnext()` → Win32_findnext()
     - `getdisk()` → Win32_getdisk()
     - `setdisk()` → Win32_setdisk()
     - `getcwd()` → Win32_getcwd()
     - `chdir()` → Win32_chdir()

2. **Пример замены:**
   - Найти: `int86(0x21, &inregs, &outregs)` для функций 0x4E, 0x4F (findfirst/findnext)
   - Заменить на вызовы Win32_findfirst/Win32_findnext

### 5.6. keys.c

Конкретные изменения:

1. **Обработка скан-кодов:**
   - Функция `AltConvert()` - оставить как есть (таблица статическая)
   - Функции преобразования скан-кодов - проверить совместимость

2. **Клавиатурные функции:**
   - Обычно не требует изменений - работает через video.c и console.c

### 5.7. Вспомогательные изменения

Проверить и при необходимости модифицировать:
- [ ] dfalloc.c - проверить `far` и `near` макросы |  | 
- [ ] log.c - проверить файловые операции |  | 
- [ ] editor.c - проверить работу с курсором и экраном |  | 

### Таблица соответствия функций для замены

| Оригинальная функция | Файл | Замена для TinyCC |
|---------------------|------|-------------------|
| `setvect(TIMER, newtimer)` | message.c | удалить |
| `setvect(KEYBRD, newkeyboard)` | message.c | удалить |
| `keyportvalue = inp(0x60)` | message.c | удалить |
| `getvideo()` | video.c | Win32_getvideo() |
| `storevideo()` | video.c | Win32_storevideo() |
| `charavail()` | console.c | Win32_keyhit() |
| `readychar()` | console.c | Win32_getkey() |
| `gotoxy()` | console.c | Win32_cursor() |
| `int86(0x33, ...)` | mouse.c | Win32_mouse_*() |
| `int86(0x21, ...)` | direct.c | Win32_findfirst/findnext и др. |

## Фаза 6: Makefile

**Требования к каждой задаче:**
- Сначала написать юнит-тест
- Проверить, что тест не проходит (красный)
- Реализовать функциональность
- Проверить, что тест проходит (зелёный)
- Рефакторинг при необходимости

### makefile.tinycc

Создать файл `makefile.tinycc` со следующим содержимым:

```makefile
# =============================================================================
# D-Flat Makefile for TinyCC (x64 Windows)
# =============================================================================

# Компилятор и пути
CC = c:\tcc\tcc.exe
MAKE = c:\tcc\make.exe

# Каталоги
TINYCC_DIR = tinycc
BUILD_DIR = build

# Флаги компиляции
# -I.          : включаемые файлы в текущем каталоге
# -I./tinycc   : включаемые файлы из папки tinycc
# -DBUILD_FULL_DFLAT : включить все функции
# -D__TINYC__  : определить макрос для TinyCC
# -Wall        : все предупреждения
# -g           : отладочная информация (опционально)
# -O2          : оптимизация (опционально)
CFLAGS = -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall

# Флаги линкера
# -lkernel32   : основные функции Windows
# -luser32     : функции пользовательского интерфейса Windows
LDFLAGS = -lkernel32 -luser32

# =============================================================================
# Исходные файлы библиотеки (проверено 03.04.2026)
# Примечание: Исключены файлы memopad.c, huffc.c, fixhelp.c (приложения/утилиты)
# Также исключены: scdos.c, htree.c (не требуются для порта)
# Примечание: menus.c и dialogs.c включены в APP_SRCS, так как используются memopad
# =============================================================================

LIB_SRCS = window.c video.c message.c mouse.c console.c \
           textbox.c listbox.c normal.c config.c menu.c \
           menubar.c popdown.c rect.c applicat.c keys.c \
           sysmenu.c editbox.c dialbox.c button.c fileopen.c \
           msgbox.c helpbox.c log.c lists.c statbar.c \
           decomp.c combobox.c pictbox.c calendar.c barchart.c \
           clipbord.c search.c dfalloc.c checkbox.c text.c \
           radio.c box.c spinbutt.c watch.c slidebox.c \
           direct.c editor.c $(TINYCC_DIR)/tccwin32.c

# Объектные файлы библиотеки
LIB_OBJS = $(LIB_SRCS:.c=.obj)

# =============================================================================
# Исходные файлы приложения memopad
# =============================================================================

APP_SRCS = memopad.c dialogs.c menus.c

# Объектные файлы приложения
APP_OBJS = $(APP_SRCS:.c=.obj)

# =============================================================================
# Все объектные файлы
# =============================================================================

ALL_OBJS = $(LIB_OBJS) $(APP_OBJS)

# =============================================================================
# Цели сборки
# =============================================================================

.PHONY: all clean distclean test lib app

# Основная цель - собрать memopad.exe
all: memopad.exe
	@echo.
	@echo ========================================
	@echo Build complete: memopad.exe
	@echo ========================================

# Собрать только библиотеку
lib: dflat.lib

# Собрать только приложение
app: memopad.exe

# memopad.exe - основной исполняемый файл
memopad.exe: $(ALL_OBJS)
	$(CC) -o $@ $(ALL_OBJS) $(LDFLAGS)
	@echo Built: $@

# dflat.lib - статическая библиотека (опционально)
dflat.lib: $(LIB_OBJS)
	$(CC) -ar cr $@ $(LIB_OBJS)
	@echo Built: $@

# =============================================================================
# Правила компиляции
# =============================================================================

# Компиляция .c в .obj
%.obj: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo Compiled: $<

# =============================================================================
# Очистка
# =============================================================================

clean:
	@if exist *.obj del /Q *.obj
	@if exist $(TINYCC_DIR)\*.obj del /Q $(TINYCC_DIR)\*.obj
	@if exist memopad.exe del /Q memopad.exe
	@if exist dflat.lib del /Q dflat.lib
	@echo Cleaned

distclean: clean
	@if exist build rmdir /S /Q build
	@echo Distclean

# =============================================================================
# Запуск приложения (для тестирования)
# =============================================================================

run: memopad.exe
	memopad.exe

# =============================================================================
# Зависимости (автогенерация)
# =============================================================================
# Workflow:
# 1. Первый запуск: make без зависимостей (возможны проблемы с порядком)
# 2. Запуск: make depend - генерирует Makefile.tinycc.dep
# 3. Последующие запуски: make использует зависимости из .dep файла

depend:
	$(CC) -MM $(CFLAGS) $(LIB_SRCS) $(APP_SRCS) > Makefile.tinycc.dep

# Включить зависимости (если файл существует)
-include Makefile.tinycc.dep
```

### Альтернативная минимальная версия makefile.tinycc

```makefile
# Minimal makefile for D-Flat + TinyCC

CC = c:\tcc\tcc.exe
CFLAGS = -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall
LDFLAGS = -lkernel32 -luser32

# Примечание: актуальный список файлов см. в секции "Исходные файлы библиотеки" (выше)
LIB_SRCS = window.c video.c message.c mouse.c console.c \
           textbox.c listbox.c normal.c config.c menu.c \
           menubar.c popdown.c rect.c applicat.c keys.c \
           sysmenu.c editbox.c dialbox.c button.c fileopen.c \
           msgbox.c helpbox.c log.c lists.c statbar.c \
           decomp.c combobox.c pictbox.c calendar.c barchart.c \
           clipbord.c search.c dfalloc.c checkbox.c text.c \
           radio.c box.c spinbutt.c watch.c slidebox.c \
           direct.c editor.c tinycc/tccwin32.c

APP_SRCS = memopad.c dialogs.c menus.c

LIB_OBJS = $(LIB_SRCS:.c=.obj)
APP_OBJS = $(APP_SRCS:.c=.obj)

.PHONY: all clean

all: memopad.exe

memopad.exe: $(LIB_OBJS) $(APP_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.obj: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

tinycc\tccwin32.obj: tinycc\tccwin32.c tinycc\tccwin32.h tinycc\tcccompat.h
	$(CC) $(CFLAGS) -c -o $@ tinycc\tccwin32.c

clean:
	del /Q *.obj tinycc\*.obj memopad.exe 2>nul
```

### Конкретные задачи для makefile.tinycc

- [ ] Создать файл `makefile.tinycc` в корне проекта |  | 
- [ ] Установить `CC = c:\tcc\tcc.exe` |  | 
- [ ] Определить `CFLAGS`:
  - `-I.` - текущий каталог |  | 
  - `-I./tinycc` - каталог tinycc |  | 
  - `-DBUILD_FULL_DFLAT` - все функции |  | 
  - `-D__TINYC__` - макрос TinyCC |  | 
  - `-Wall` - предупреждения |  | 
- [ ] Определить `LDFLAGS = -lkernel32 -luser32` |  | 
- [ ] Определить `LIB_SRCS` - все файлы библиотеки |  | 
- [ ] Определить `APP_SRCS` - memopad.c, dialogs.c, menus.c |  | 
- [ ] Создать правила компиляции: `%.obj: %.c` |  | 
- [ ] Создать правило линковки: `memopad.exe` |  | 
- [ ] Создать цель `clean` |  | 
- [ ] Добавитьtinycc/tccwin32.c в конец LIB_SRCS |  | 

## Фаза 7: Тестирование

### 7.1. Компиляция

Конкретные шаги:

1. **Проверить наличие компилятора:**
   ```
   c:\tcc\tcc.exe --version
   ```

2. **Скомпилировать каждый модуль по отдельности (для отладки):**
   ```
   tcc -c -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall window.c
   tcc -c -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall video.c
   tcc -c -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall message.c
   tcc -c -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall mouse.c
   tcc -c -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall console.c
   tcc -c -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall rect.c
   tcc -c -I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall keys.c
   ... (повторить для всех LIB_SRCS)
   ```

3. **Проверить отсутствие предупреждений:**
   - Предупреждения о несовместимости типов исправить
   - Предупреждения о неиспользуемых функциях - игнорировать

4. **Исправить типичные ошибки:**
   - `undefined reference to ...` - добавить прототип в заголовочный файл
   - `conflicting types` - проверить объявления функций
   - `redefinition` - добавить `#ifndef` guards

### 7.2. Линковка

1. **Собрать dflat.lib (статическая библиотека):**
   ```
   tcc -ar cr dflat.lib *.obj
   ```

2. **Слинковать memopad.exe:**
   ```
   tcc -o memopad.exe memopad.obj dialogs.obj menus.obj dflat.lib -lkernel32 -luser32
   ```

3. **Проверить зависимости:**
   ```
   dumpbin /dependents memopad.exe
   ```
   Ожидаемый вывод:
   - kernel32.dll
   - user32.dll
   - msvcrt.dll (если есть)

### 7.3. Функциональное тестирование

Запустить `memopad.exe` и проверить:

#### 7.3.1. Запуск приложения
- [ ] Приложение запускается без ошибок |  | 
- [ ] Отображается главное окно memopad |  | 
- [ ] Нет сообщений "Program Error" или "Access Violation" |  | 

#### 7.3.2. Файловые операции
- [ ] **New (Ctrl+N)**: создание нового файла |  | 
- [ ] **Open (F3)**: открытие диалога выбора файла |  | 
  - [ ] Отображается список файлов |  | 
  - [ ] Можно выбрать файл мышью или клавишами |  | 
  - [ ] Enter открывает файл |  | 
- [ ] **Save (F2)**: сохранение текущего файла |  | 
- [ ] **Save As**: сохранение с новым именем |  | 
- [ ] **Exit (Alt+F4)**: выход из приложения |  | 
  - [ ] При несохранённых изменениях - запрос подтверждения |  | 

#### 7.3.3. Редактирование текста
- [ ] **Ввод текста**: символы появляются в документе |  | 
- [ ] **Backspace**: удаление символа слева от курсора |  | 
- [ ] **Delete**: удаление символа под курсором |  | 
- [ ] **Enter**: перевод строки |  | 
- [ ] **Стрелки**: перемещение курсора |  | 
- [ ] **Home/End**: перемещение в начало/конец строки |  | 
- [ ] **Ctrl+Home**: перемещение в начало документа |  | 
- [ ] **Ctrl+End**: перемещение в конец документа |  | 
- [ ] **Page Up/Down**: перемещение по страницам |  | 
- [ ] **Insert**: переключение режима вставка/замена |  | 
- [ ] **Tab**: вставка табуляции (4 или 8 пробелов) |  | 

#### 7.3.4. Буфер обмена
- [ ] **Cut (Shift+Del)**: вырезать выделенный текст |  | 
- [ ] **Copy (Ctrl+Ins)**: копировать выделенный текст |  | 
- [ ] **Paste (Shift+Ins)**: вставить текст из буфера |  | 
- [ ] **Delete**: удалить выделенный текст без копирования |  | 

#### 7.3.5. Поиск и замена
- [ ] **Find (Ctrl+F)**: диалог поиска |  | 
  - [ ] Ввод искомого текста |  | 
  - [ ] Поиск Next/Previous |  | 
  - [ ] Закрытие диалога Escape |  | 
- [ ] **Replace (Ctrl+H)**: диалог замены |  | 
  - [ ] Ввод текста для поиска и замены |  | 
  - [ ] Replace, Replace All |  | 
  - [ ] Отмена |  | 

#### 7.3.6. Меню
- [ ] Главное меню отображается (File, Edit, Search, Options, Help) |  | 
- [ ] **Alt+буква**: активация пункта меню |  | 
- [ ] **F10**: активация меню |  | 
- [ ] **Стрелки**: навигация по меню |  | 
- [ ] **Enter**: выбор пункта меню |  | 
- [ ] **Escape**: закрытие меню без выбора |  | 

#### 7.3.7. Мышь
- [ ] **Левый клик**: позиционирование курсора |  | 
- [ ] **Двойной клик**: выделение слова |  | 
- [ ] **Перетаскивание**: выделение текста |  | 
- [ ] **Правая кнопка**: контекстное меню (если поддерживается) |  | 
- [ ] Мышь работает в диалоговых окнах |  | 

#### 7.3.8. Клавиатура
- [ ] **Горячие клавиши** работают: |  | 
  - Ctrl+N, Ctrl+O, Ctrl+S, Ctrl+F, Ctrl+H |  | 
  - F1-F10 |  | 
  - Alt+буква |  | 
  - Shift+стрелки (выделение) |  | 
- [ ] **NumLock** не влияет на стрелки |  | 
- [ ] **CapsLock** переключает регистр правильно |  | 

#### 7.3.9. Помощь
- [ ] **F1**: открытие справки |  | 
- [ ] Навигация по разделам справки |  | 
- [ ] **Escape**: закрытие справки |  | 
- [ ] **Enter**: переход по ссылке |  | 

#### 7.3.10. Диалоговые окна
- [ ] Модальные диалоги блокируют главное окно |  | 
- [ ] Кнопки работают (OK, Cancel, Yes, No, Help) |  | 
- [ ] Tab переключает между элементами |  | 
- [ ] Enter активирует кнопку по умолчанию |  | 
- [ ] Escape активирует Cancel |  | 
- [ ] Поля ввода работают корректно |  | 

#### 7.3.11. Изменение размера окна консоли
- [ ] При изменении размера окна - перерисовка |  | 
- [ ] SCREENWIDTH/SCREENHEIGHT обновляются динамически |  | 
- [ ] Главное окно не выходит за границы |  | 

### 7.4. Тестирование граничных условий

- [ ] Пустой файл (New, сразу Save) |  | 
- [ ] Файл с очень длинной строкой (>80 символов) |  | 
- [ ] Файл с юникод-символами (если поддерживается) |  | 
- [ ] Максимальный размер файла (ограничение памяти) |  | 
- [ ] Много окон (Window → Cascade, Tile) |  | 
- [ ] Русские буквы в именах файлов |  | 

### 7.5. Проверка утечек памяти

- [ ] Открыть/закрыть много файлов |  | 
- [ ] Много операций создания/удаления окон |  | 
- [ ] Проверить Task Manager - память не растёт бесконечно |  | 

### 7.6. Команда для запуска полного тестирования

```bash
# Компиляция
cd C:\Workspace\Personal\D-Flat
c:\tcc\make.exe -f makefile.tinycc clean all
```

#### Запуск и тестирование

**Важно**: Приложение запускается в Windows-консоли (cmd.exe, PowerShell), **не в DOSBox**.

```cmd
:: Запуск из командной строки Windows
cd C:\Workspace\Personal\D-Flat
memopad.exe
```

**Ожидаемые результаты:**
- Приложение запускается без ошибок
- Отображается текстовый интерфейс с меню (File, Edit, Search, Options, Help)
- Окно memopad с возможностью ввода текста
- Поддержка мыши (при наличии)
- Корректное отображение русских букв (кодовая страница 866)

**Визуальная проверка:**
- Верхняя строка: главное меню (File Edit Search Options Help)
- Ниже: пустое окно редактора с мигающим курсором
- Нижняя строка: строка состояния (номер строки/столбца, режим)

### Критерии успешного завершения Фазы 5

- [ ] Компиляция проходит без ошибок |  | 
- [ ] Линковка создаёт memopad.exe |  | 
- [ ] Приложение запускается без ошибок |  | 
- [ ] Все операции из раздела 5.3 работают корректно |  | 
- [ ] Нет утечек памяти |  | 
- [ ] Приложение корректно завершается |  | 
- [ ] **Все юнит-тесты проходят успешно** |  | 
- [ ] **Покрытие тестами ключевых функций >= 80%** |  | 

## Ключевые технические решения

### Кодировка

```c
SetConsoleOutputCP(866);  // OEM Russian
SetConsoleCP(866);
```

### Размер консоли

```c
GetConsoleScreenBufferInfo(hConsole, &csbi);
SCREENWIDTH = csbi.srWindow.Right - csbi.srWindow.Left + 1;
SCREENHEIGHT = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
```

### Цвета

DOS и Win32 используют одинаковые коды цветов (0-15), преобразование не требуется.

## Полный список DOS-функций для реализации

### video.c зависимости

| Функция/Переменная | Описание | Win32 замена |
|--------------------|----------|--------------|
| `video_address` | Сегмент видеопамяти (0xB800) | Не требуется |
| `movefromscreen()` | Копировать из видеопамяти | `ReadConsoleOutput` |
| `movetoscreen()` | Копировать в видеопамять | `WriteConsoleOutput` |
| `vpeek()` / `vpoke()` |safe peek/poke для CGA | Не требуется |
| `waitforretrace()` | Ожидание обратного хода луча | Не требуется |

### system.h зависимости

| Функция/Макрос | Описание | Win32 замена |
|----------------|----------|--------------|
| `outp(port, val)` | Вывод в порт | Не требуется |
| `inp(port)` | Чтение из порта | Не требуется |
| `keyportvalue` | Порт клавиатуры 0x60 | `ReadConsoleInput` |
| `peek()/pokeb()` | Чтение/запись по адресу | Не требуется (flat memory) |
| `getvect/setvect` | Работа с векторами прерываний | Не требуется |

### message.c зависимости

| Функция/Переменная | Описание | Win32 замена |
|--------------------|----------|--------------|
| `oldtimer` | Старый обработчик таймера | Не требуется |
| `oldkeyboard` | Старый обработчик клавиатуры | Не требуется |
| `newtimer()` | Обработчик прерывания таймера | `GetTickCount64()` в цикле |
| `newkeyboard()` | Обработчик прерывания клавиатуры | `ReadConsoleInput` |
| `newcrit()` | Обработчик критических ошибок | Не требуется |
| `keyportvalue` | Мониторинг клавиатуры | `GetKeyState` |

## Детальные подзадачи

### Фаза 1.1: tinycc/tcccompat.h

**Требования к тестам:** Каждая задача должна иметь соответствующий юнит-тест.

- [ ] Определения `far`, `near`, `interrupt` → пустые макросы |  | 
- [ ] Определения `MK_FP`, `FP_OFF`, `FP_SEG` → flat memory (простые cast-ы) |  | 
- [ ] Структуры `REGS`, `SREGS`, `IREGS` → Win32-совместимые |  | 
- [ ] Структура `ffblk` для findfirst/findnext |  | 
- [ ] Константы: `TIMER=8`, `VIDEO=0x10`, `KEYBRD=0x16`, `DOS=0x21`, `CRIT=0x24`, `MOUSE=0x33` |  | 
- [ ] Определения `outp`, `inp` → возвращают 0 / ничего не делают |  | 
- [ ] Определения `keyportvalue` → переменная для мониторинга |  | 

### Фаза 1.2: tinycc/tccwin32.h

- [ ] Объявления Win32 Console API функций |  | 
- [ ] Глобальные переменные: `HANDLE hConsoleInput`, `HANDLE hConsoleOutput` |  | 
- [ ] Прототипы: `InitWin32Console()`, `PollInput()`, `GetConsoleSize()` |  | 

### Фаза 1.3: tinycc/tccwin32.c

#### Видео-подсистема
- [ ] `InitWin32Console()` - инициализация консоли, настройка CP 866 |  | 
- [ ] `GetConsoleSize()` - получение размера окна консоли |  | 
- [ ] Переопределение `SCREENWIDTH` / `SCREENHEIGHT` - динамическое получение |  | 
- [ ] Реализация `getvideo()` → `ReadConsoleOutputCharacter/Attribute` |  | 
- [ ] Реализация `storevideo()` → `WriteConsoleOutputCharacter/Attribute` |  | 
- [ ] Реализация `GetVideoChar()` / `PutVideoChar()` → прямой доступ к буферу |  | 
- [ ] `isEGA()` / `isVGA()` → возвращают FALSE |  | 
- [ ] `Set25()` / `Set43()` / `Set50()` → `SetConsoleWindowInfo` или игнорировать |  | 

#### Курсор
- [ ] Реализация `cursor()` → `SetConsoleCursorPosition` |  | 
- [ ] Реализация `savecursor()` / `restorecursor()` → стек позиций |  | 
- [ ] Реализация `hidecursor()` / `unhidecursor()` → `CONSOLE_CURSOR_INFO` |  | 
- [ ] Реализация `set_cursor_type()` → `CONSOLE_CURSOR_INFO` |  | 
- [ ] Реализация `curr_cursor()` → `GetConsoleScreenBufferInfo` |  | 

#### Клавиатура
- [ ] Реализация `keyhit()` → `PeekConsoleInput` |  | 
- [ ] Реализация `getkey()` → `ReadConsoleInput` + преобразование скан-кодов |  | 
- [ ] Реализация `getshift()` → `GetKeyState` / `GetAsyncKeyState` |  | 
- [ ] Реализация `bioskey(0)` → `ReadConsoleInput` |  | 
- [ ] Реализация `AltConvert()` - таблица конвертации (оставить как есть) |  | 
- [ ] Реализация `beep()` → `MessageBeep` или `Beep()` |  | 

#### Мышь
- [ ] Реализация `mouse_installed()` → всегда TRUE |  | 
- [ ] Реализация `mousebuttons()` → из `ReadConsoleInput` (MOUSE_EVENT) |  | 
- [ ] Реализация `get_mouseposition()` → из `ReadConsoleInput`, преобразование в символы |  | 
- [ ] Реализация `set_mouseposition()` → `SetConsoleCursorPosition` |  | 
- [ ] Реализация `show_mousecursor()` / `hide_mousecursor()` → отслеживание состояния |  | 
- [ ] Реализация `resetmouse()` → инициализация переменных |  | 
- [ ] Реализация `set_mousetravel()` → проверка границ |  | 
- [ ] Реализация `button_releases()` → отслеживание событий мыши |  | 

#### Таймер
- [ ] Реализация таймеров: `doubletimer`, `delaytimer`, `clocktimer` |  | 
- [ ] Реализация `set_timer()`, `timer_running()`, `countdown()`, `timed_out()` макросов |  | 
- [ ] Модификация `dispatch_message()` для опроса `GetTickCount64()` вместо прерываний |  | 
- [ ] Реализация clock display через `time()` / `localtime()` |  | 

#### Файловые операции
- [ ] Реализация `findfirst()` → `FindFirstFile` |  | 
- [ ] Реализация `findnext()` → `FindNextFile` |  | 
- [ ] Реализация `fnmerge()` / `fnsplit()` → `_makepath` / `_splitpath` |  | 
- [ ] Реализация `getdisk()` / `setdisk()` → `GetCurrentDrive` / `SetCurrentDirectory` |  | 
- [ ] Реализация `getcwd()` / `chdir()` → Win32 API |  | 

#### Прерывания
- [ ] Реализация `int86()` - диспетчер для int 0x10, 0x16, 0x21, 0x33 |  | 
- [ ] Реализация `int86x()` - с сегментными регистрами (упрощённо) |  | 
- [ ] Заглушки для `getvect()` / `setvect()` - не требуются для Win32 |  | 

### Фаза 2: Модификация заголовков

#### dflat.h
- [ ] Добавить `#ifdef __TINYC__` секцию после `#ifdef __SMALLER_C__` |  | 
- [ ] Включить `tinycc/tcccompat.h` и `<windows.h>` |  | 
- [ ] Проверить отсутствие конфликтов с `__declspec(dllimport)` |  | 

#### system.h
- [ ] Добавить `#ifdef __TINYC__` секцию |  | 
- [ ] Переопределить `SCREENWIDTH` / `SCREENHEIGHT` для динамического получения |  | 
- [ ] Убрать `far` из `static volatile int far *clk` |  | 
- [ ] Добавить определения для `outp`, `inp` |  | 

#### video.h
- [ ] Добавить `#ifdef __TINYC__` для замены `void far *` на `void *` |  | 
- [ ] Объявить `movefromscreen()`, `movetoscreen()` если не объявлены |  | 

### Фаза 3: Модификация исходников

#### message.c
- [ ] Обернуть `#ifdef __TINYC__` вокруг `static void interrupt far newtimer()` |  | 
- [ ] Обернуть `#ifdef __TINYC__` вокруг `static void interrupt far newkeyboard()` |  | 
- [ ] Модифицировать `init_messages()` - убрать `setvect()`, использовать Win32 инициализацию |  | 
- [ ] Модифицировать `collect_events()` - добавить опрос `GetTickCount64()` для таймеров |  | 
- [ ] Убрать `keyportvalue = inp(KEYBOARDPORT)` - использовать `GetKeyState` |  | 

#### video.c
- [ ] Добавить `#ifdef __TINYC__` для замены `MK_FP()` на flat pointer |  | 
- [ ] Модифицировать `movefromscreen()` / `movetoscreen()` - использовать Win32 API |  | 
- [ ] Убрать или заменить `vpeek()` / `vpoke()` / `waitforretrace()` - не требуются |  | 

#### console.c
- [ ] Добавить `#ifdef __TINYC__` вокруг `static volatile int far *clk` |  | 
- [ ] Заменить BIOS-вызовы на Win32 API |  | 
- [ ] Реализовать `keyhit()` через `PeekConsoleInput` |  | 

#### mouse.c
- [ ] Заменить `int86(MOUSE, ...)` на Win32 консольный ввод |  | 
- [ ] Модифицировать `mouse_installed()` - использовать `GetNumberOfConsoleInputEvents` |  | 

#### direct.c
- [ ] Проверить использование `int86(DOS, ...)` - заменить на Win32 API |  | 

### Фаза 4: Makefile

- [ ] Создать `makefile.tinycc` |  | 
- [ ] Определить `CC = c:\tcc\tcc.exe` |  | 
- [ ] Определить флаги: `-I. -I./tinycc -DBUILD_FULL_DFLAT -D__TINYC__ -Wall` |  | 
- [ ] Определить линковку: `-lkernel32 -luser32` |  | 
- [ ] Список исходников библиотеки |  | 
- [ ] Список исходников memopad |  | 
- [ ] Правила компиляции и линковки |  | 
- [ ] Цель `clean` |  | 

### Фаза 5: Тестирование

#### Компиляция
- [ ] Скомпилировать все .c файлы по отдельности |  | 
- [ ] Проверить отсутствие предупреждений |  | 
- [ ] Исправить ошибки компиляции |  | 

#### Линковка
- [ ] Собрать dflat.lib (или скомпилировать все в .obj) |  | 
- [ ] Слинковать memopad.exe |  | 

#### Функциональное тестирование
- [ ] Запуск memopad.exe |  | 
- [ ] Проверка: создание нового файла |  | 
- [ ] Проверка: открытие файла |  | 
- [ ] Проверка: редактирование текста (ввод, удаление, курсор) |  | 
- [ ] Проверка: сохранение файла |  | 
- [ ] Проверка: меню File (New, Open, Save, Exit) |  | 
- [ ] Проверка: меню Edit (Cut, Copy, Paste, Find, Replace) |  | 
- [ ] Проверка: мышь (позиционирование, клики) |  | 
- [ ] Проверка: клавиатура (горячие клавиши, Ctrl+...) |  | 
- [ ] Проверка: F1 - помощь |  | 
- [ ] Проверка: диалоговые окна |  | 
- [ ] Проверка: изменение размера окна консоли |  | 

## Зависимости задач

```
Фаза 1: Инфраструктура совместимости (tinycc/tccwin32.c)
               ↓
Фаза 3: PAL (console.h, win32/console_win32.c, dos/console_dos.c)
               ↓
Фаза 2: Модификация заголовков (dflat.h, system.h, video.h)
               ↓
Фаза 3: Модификация исходников (message.c, video.c, console.c, mouse.c, direct.c)
               ↓
Фаза 4: makefile.tinycc
               ↓
Фаза 5: Тестирование
               ↓
Фаза 8: unix/console_unix.c (не требует изменения ядра!)
```

### Схема независимости Unix-порта

```
После Фазы 0 (PAL), Unix-реализация не требует изменения:
- window.c
- message.c  
- video.c (только адаптировать getvideo/storevideo к PAL)
- console.c (полностью заменяется)
- mouse.c (только адаптировать к PAL)
- Все остальные модули ядра D-Flat
```

## Фаза 1: Юнит-тестирование (ОБЯЗАТЕЛЬНО)

**Фаза 1 выполняется ПЕРВОЙ и является обязательной.**

Создание тестовой инфраструктуры до начала портирования обеспечит:
- Возможность проверки корректности при портировании
- Регрессионное тестирование при изменении кода
- Документирование ожидаемого поведения
- TDD-подход: сначала тест, потом код

**Каждая задача последующих фаз должна сопровождаться тестами.**

### Обзор

Создание простой header-only библиотеки для юнит-тестирования и максимальное покрытие кода тестами до начала портирования. Это обеспечит:
- Возможность проверки корректности при портировании
- Регрессионное тестирование при изменении кода
- Документирование ожидаемого поведения

### Особенности проекта для библиотеки

| Характеристика | Влияние на тесты |
|----------------|------------------|
| ~50 .c файлов | Модульные тесты для каждого модуля |
| DOS-специфичный код (int86, BIOS) | Требуется mock-инфраструктура |
| Типы: `BOOL`, `RECT`, `WINDOW`, `PARAM` | Специфичные assert-ы |
| Платформенные макросы: `far`, `near` | Условная компиляция |
| Нет существующих тестов | Начать с нуля |

### Библиотека тестирования (df_test.h)

#### Концепция

Header-only библиотека в одном файле `df_test.h` с минимальным набором макросов и функций:

```c
// df_test.h - D-Flat Unit Testing Framework (header-only)

#ifndef DF_TEST_H
#define DF_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

// ============ Конфигурация ============

#ifdef __TINYC__
    #define DF_TEST_EXPORT
#else
    #define DF_TEST_EXPORT
#endif

// ============ Базовые типы ============

typedef void (*DF_TEST_FUNC)(void);
typedef void (*DF_TEST_SETUP)(void);
typedef void (*DF_TEST_TEARDOWN)(void);

typedef struct {
    const char *name;
    DF_TEST_FUNC func;
    int skipped;
} DF_TEST_CASE;

typedef struct {
    const char *name;
    DF_TEST_CASE *tests;
    int test_count;
    int tests_run;
    int tests_passed;
    int tests_failed;
    int tests_skipped;
} DF_TEST_SUITE;

// ============ Результаты тестов ============

typedef struct {
    const char *suite_name;
    const char *test_name;
    const char *file;
    int line;
    const char *failed_expr;
    char message[256];
} DF_TEST_RESULT;

#define DF_TEST_MAX_RESULTS 100

typedef struct {
    DF_TEST_RESULT results[DF_TEST_MAX_RESULTS];
    int count;
    int passed;
    int failed;
} DF_TEST_REPORT;

// ============ Assertions ============

#define DF_ASSERT(expr) \
    do { if (!(expr)) { \
        df_test_assert_fail(#expr, __FILE__, __LINE__); \
    } } while(0)

#define DF_ASSERT_TRUE(expr) DF_ASSERT(expr)
#define DF_ASSERT_FALSE(expr) DF_ASSERT(!(expr))

#define DF_ASSERT_EQ(a, b) \
    do { if ((a) != (b)) { \
        df_test_assert_eq_fail((a), (b), #a, #b, __FILE__, __LINE__); \
    } } while(0)

#define DF_ASSERT_NE(a, b) \
    do { if ((a) == (b)) { \
        df_test_assert_ne_fail((a), (b), #a, #b, __FILE__, __LINE__); \
    } } while(0)

#define DF_ASSERT_NULL(ptr) DF_ASSERT((ptr) == NULL)
#define DF_ASSERT_NOT_NULL(ptr) DF_ASSERT((ptr) != NULL)

#define DF_ASSERT_INT_EQ(a, b) DF_ASSERT_EQ((int)(a), (int)(b))
#define DF_ASSERT_INT_NE(a, b) DF_ASSERT_NE((int)(a), (int)(b))

#define DF_ASSERT_STR_EQ(a, b) \
    do { if (strcmp((a), (b)) != 0) { \
        df_test_assert_str_eq_fail((a), (b), __FILE__, __LINE__); \
    } } while(0)

#define DF_ASSERT_MEM_EQ(a, b, size) \
    do { if (memcmp((a), (b), (size)) != 0) { \
        df_test_assert_mem_eq_fail((a), (b), (size), __FILE__, __LINE__); \
    } } while(0)

// D-Flat специфичные assertions

#define DF_ASSERT_RECT_EQ(r1, r2) do { \
    DF_ASSERT_INT_EQ((r1).lf, (r2).lf); \
    DF_ASSERT_INT_EQ((r1).tp, (r2).tp); \
    DF_ASSERT_INT_EQ((r1).rt, (r2).rt); \
    DF_ASSERT_INT_EQ((r1).bt, (r2).bt); \
} while(0)

#define DF_ASSERT_WNDRECT_EQ(wnd, r) do { \
    DF_ASSERT_INT_EQ((wnd)->rc.lf, (r).lf); \
    DF_ASSERT_INT_EQ((wnd)->rc.tp, (r).tp); \
    DF_ASSERT_INT_EQ((wnd)->rc.rt, (r).rt); \
    DF_ASSERT_INT_EQ((wnd)->rc.bt, (r).bt); \
} while(0)

#define DF_ASSERT_BOOL_EQ(a, b) DF_ASSERT_INT_EQ((a) ? 1 : 0, (b) ? 1 : 0)

#define DF_ASSERT_RECT_VALID(r) do { \
    DF_ASSERT((r).lf >= 0); \
    DF_ASSERT((r).tp >= 0); \
    DF_ASSERT((r).rt >= (r).lf); \
    DF_ASSERT((r).bt >= (r).tp); \
} while(0)

// ============ Тестовые случаи и наборы ============

#define DF_TEST_SUITE(suite_name) \
    static DF_TEST_CASE suite_name##_tests[] = {

#define DF_TEST_CASE(test_name) \
    { #test_name, test_name, 0 },

#define DF_TEST_CASE_SKIP(test_name) \
    { #test_name, test_name, 1 },

#define DF_TEST_SUITE_END(suite_name) \
    }; \
    static DF_TEST_SUITE suite_name##_suite = { \
        #suite_name, \
        suite_name##_tests, \
        sizeof(suite_name##_tests) / sizeof(DF_TEST_CASE), \
        0, 0, 0, 0 \
    }; \
    DF_TEST_SUITE *suite_name(void) { return &suite_name##_suite; }

// ============ Mock-система ============

typedef struct {
    const char *name;
    void *func_ptr;
    void *mock_ptr;
    int call_count;
    int expected_calls;
} DF_MOCK;

typedef struct {
    DF_MOCK mocks[50];
    int mock_count;
    jmp_buf jump_buffer;
    int test_status;
} DF_TEST_CONTEXT;

extern DF_TEST_CONTEXT _df_test_ctx;

#define DF_MOCK_SETUP(mock_name, real_func, mock_func) \
    do { \
        int idx = _df_test_ctx.mock_count++; \
        _df_test_ctx.mocks[idx].name = #mock_name; \
        _df_test_ctx.mocks[idx].func_ptr = (void*)(real_func); \
        _df_test_ctx.mocks[idx].mock_ptr = (void*)(mock_func); \
        _df_test_ctx.mocks[idx].call_count = 0; \
    } while(0)

#define DF_MOCK_EXPECT_CALL(mock_name) \
    do { \
        for (int i = 0; i < _df_test_ctx.mock_count; i++) { \
            if (strcmp(_df_test_ctx.mocks[i].name, #mock_name) == 0) { \
                _df_test_ctx.mocks[i].expected_calls = \
                    _df_test_ctx.mocks[i].call_count + 1; \
                break; \
            } \
        } \
    } while(0)

#define DF_MOCK_VERIFY(mock_name) \
    do { \
        for (int i = 0; i < _df_test_ctx.mock_count; i++) { \
            if (strcmp(_df_test_ctx.mocks[i].name, #mock_name) == 0) { \
                if (_df_test_ctx.mocks[i].expected_calls > 0 && \
                    _df_test_ctx.mocks[i].call_count != \
                    _df_test_ctx.mocks[i].expected_calls) { \
                    DF_ASSERT(0); /* Mock verification failed */ \
                } \
                break; \
            } \
        } \
    } while(0)

// ============ API функций ============

DF_TEST_EXPORT void df_test_init(void);
DF_TEST_EXPORT void df_test_run_suite(DF_TEST_SUITE *suite);
DF_TEST_EXPORT void df_test_run_all(DF_TEST_SUITE **suites, int count);
DF_TEST_EXPORT DF_TEST_REPORT *df_test_get_report(void);
DF_TEST_EXPORT void df_test_print_report(void);
DF_TEST_EXPORT int df_test_get_exit_code(void);

DF_TEST_EXPORT void df_test_assert_fail(const char *expr, 
    const char *file, int line);
DF_TEST_EXPORT void df_test_assert_eq_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line);
DF_TEST_EXPORT void df_test_assert_ne_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line);
DF_TEST_EXPORT void df_test_assert_str_eq_fail(const char *a, const char *b, 
    const char *file, int line);
DF_TEST_EXPORT void df_test_assert_mem_eq_fail(const void *a, const void *b, 
    size_t size, const char *file, int line);

// ============ Stub-функции для DOS-вызовов ============

#ifdef DF_TEST_ENABLE_STUBS

// Video stubs
int stub_getvideo_enabled = 0;
void *stub_video_buffer = NULL;
int stub_screen_width = 80;
int stub_screen_height = 25;

#define DF_STUB_GETVIDEO(buf, size) \
    stub_getvideo_enabled = 1; \
    stub_video_buffer = (buf)

// Keyboard stubs
int stub_keyhit_result = 0;
int stub_getkey_result = 0;

#define DF_STUB_KEYHIT(result) stub_keyhit_result = (result)
#define DF_STUB_GETKEY(result) stub_getkey_result = (result)

// Timer stubs
unsigned long stub_tick_count = 0;

#define DF_STUB_TICKCOUNT(value) stub_tick_count = (value)
#define DF_STUB_ADVANCE_TICK(ms) stub_tick_count += (ms)

// Mouse stubs
int stub_mouse_x = 0;
int stub_mouse_y = 0;
int stub_mouse_buttons = 0;

#define DF_STUB_MOUSE(x, y, buttons) \
    stub_mouse_x = (x); stub_mouse_y = (y); stub_mouse_buttons = (buttons)

#endif /* DF_TEST_ENABLE_STUBS */

#endif /* DF_TEST_H */
```

#### df_test.c - реализация

```c
// df_test.c - реализация тестовой библиотеки

#include "df_test.h"

DF_TEST_CONTEXT _df_test_ctx = {0};
static DF_TEST_REPORT _df_report = {0};

void df_test_init(void) {
    memset(&_df_test_ctx, 0, sizeof(_df_test_ctx));
    memset(&_df_report, 0, sizeof(_df_report));
}

void df_test_assert_fail(const char *expr, const char *file, int line) {
    _df_report.results[_df_report.count].suite_name = "";
    _df_report.results[_df_report.count].test_name = "";
    _df_report.results[_df_report.count].file = file;
    _df_report.results[_df_report.count].line = line;
    _df_report.results[_df_report.count].failed_expr = expr;
    snprintf(_df_report.results[_df_report.count].message, 
        sizeof(_df_report.results[_df_report.count].message),
        "Assertion failed: %s", expr);
    _df_report.count++;
    _df_report.failed++;
    longjmp(_df_test_ctx.jump_buffer, 1);
}

// ... реализации остальных assert функций ...

void df_test_run_suite(DF_TEST_SUITE *suite) {
    printf("Running suite: %s\n", suite->name);
    
    for (int i = 0; i < suite->test_count; i++) {
        DF_TEST_CASE *tc = &suite->tests[i];
        
        if (tc->skipped) {
            suite->tests_skipped++;
            printf("  SKIP: %s\n", tc->name);
            continue;
        }
        
        printf("  RUN: %s ... ", tc->name);
        fflush(stdout);
        
        suite->tests_run++;
        
        if (setjmp(_df_test_ctx.jump_buffer) == 0) {
            tc->func();
            suite->tests_passed++;
            printf("PASS\n");
        } else {
            suite->tests_failed++;
            printf("FAIL\n");
        }
    }
    
    printf("Results: %d/%d passed, %d failed, %d skipped\n",
        suite->tests_passed, suite->tests_run, 
        suite->tests_failed, suite->tests_skipped);
}

DF_TEST_REPORT *df_test_get_report(void) {
    return &_df_report;
}

int df_test_get_exit_code(void) {
    return _df_report.failed > 0 ? 1 : 0;
}
```

### Структура тестов

```
tests/
├── df_test.h              # Header-only библиотека
├── df_test.c              # Реализация (необязательно, можно объединить в .h)
├── test_main.c            # main() - запускает все тесты
├── test_rect.c            # Тесты для rect.c
├── test_window.c          # Тесты для window.c
├── test_message.c         # Тесты для message.c
├── test_video.c           # Тесты для video.c
├── test_keys.c            # Тесты для keys.c
└── Makefile.test          # Сборка тестов
```

### Примеры тестов

#### test_rect.c

```c
#include "dflat.h"
#include "df_test.h"

static void test_rect_width(void) {
    RECT r = {0, 0, 10, 5};
    DF_ASSERT_INT_EQ(RectWidth(r), 11);
}

static void test_rect_height(void) {
    RECT r = {0, 0, 10, 5};
    DF_ASSERT_INT_EQ(RectHeight(r), 6);
}

static void test_rect_inside(void) {
    RECT r = {0, 0, 10, 10};
    DF_ASSERT_TRUE(InsideRect(5, 5, r));
    DF_ASSERT_FALSE(InsideRect(11, 5, r));
    DF_ASSERT_FALSE(InsideRect(5, 11, r));
}

static void test_subrectangle(void) {
    RECT outer = {0, 0, 20, 10};
    RECT inner = {5, 3, 15, 8};
    RECT result = subRectangle(outer, inner);
    
    DF_ASSERT_RECT_VALID(result);
    DF_ASSERT_INT_EQ(result.lf, 5);
    DF_ASSERT_INT_EQ(result.tp, 3);
    DF_ASSERT_INT_EQ(result.rt, 15);
    DF_ASSERT_INT_EQ(result.bt, 8);
}

DF_TEST_SUITE(rect_tests)
    DF_TEST_CASE(test_rect_width)
    DF_TEST_CASE(test_rect_height)
    DF_TEST_CASE(test_rect_inside)
    DF_TEST_CASE(test_subrectangle)
DF_TEST_SUITE_END(rect_tests)
```

#### test_keys.c

```c
#include "dflat.h"
#include "df_test.h"

// Тесты функций преобразования клавиш

static void test_alt_convert(void) {
    // Тест Alt+символ → скан-код
}

static void test_function_keys(void) {
    // Тест F1-F10
}

static void test_ctrl_keys(void) {
    // Тест Ctrl+символ
}

DF_TEST_SUITE(keys_tests)
    DF_TEST_CASE(test_alt_convert)
    DF_TEST_CASE(test_function_keys)
    DF_TEST_CASE(test_ctrl_keys)
DF_TEST_SUITE_END(keys_tests)
```

### Подзадачи

#### Создание библиотеки

- [ ] Создать `tests/df_test.h` - основной заголовочный файл |  | 
  - Определить макросы `DF_ASSERT_*` |  | 
  - Определить структуры `DF_TEST_CASE`, `DF_TEST_SUITE` |  | 
  - Определить макросы для создания тестов |  | 
  - Добавить mock-инфраструктуру |  | 
  - Добавить stub-функции для DOS-вызовов |  | 

- [ ] Создать `tests/df_test.c` - реализация |  | 
  - `df_test_init()` |  | 
  - `df_test_run_suite()` |  | 
  - `df_test_assert_*()` функции |  | 
  - `df_test_print_report()` |  | 

- [ ] Создать `tests/test_main.c` - точка входа |  | 
  - Регистрация всех тестовых наборов |  | 
  - Вызов `df_test_run_all()` |  | 
  - Вывод отчёта и возврат кода завершения |  | 

#### Покрытие тестами

- [ ] **rect.c** - геометрические функции |  | 
  - `test_rect_width`, `test_rect_height` |  | 
  - `test_rect_inside`, `test_rect_valid` |  | 
  - `test_subRectangle`, `test_ClipRectangle` |  | 
  - `test_ClientRect`, `test_RelativeWindowRect` |  | 

- [ ] **keys.c** - обработка клавиш |  | 
  - `test_alt_convert` - таблица Alt-символов |  | 
  - `test_function_keys` - F1-F10 |  | 
  - `test_ctrl_keys` - Ctrl+символ |  | 
  - `test_special_keys` - стрелки, Esc, etc. |  | 

- [ ] **window.c** - управление окнами |  | 
  - `test_window_create` - создание окна |  | 
  - `test_window_destroy` - удаление окна |  | 
  - `test_window_attributes` - атрибуты |  | 
  - `test_window_rect` - размеры и позиции |  | 
  - `test_window_tree` - иерархия parent/child |  | 

- [ ] **message.c** - система сообщений |  | 
  - `test_message_send` - отправка сообщений |  | 
  - `test_message_handlers` - обработчики |  | 
  - `test_timer_functions` - таймеры |  | 

- [ ] **video.c** - видео-операции (со stub-ами) |  | 
  - `test_getvideo`, `test_storevideo` |  | 
  - `test_rect_operations` |  | 

- [ ] **dfalloc.c** - аллокатор памяти |  | 
  - `test_dfmalloc`, `test_dfrealloc` |  | 
  - `test_memory_alignment` |  | 

- [ ] **lists.c** - списки |  | 
  - `test_list_add`, `test_list_remove` |  | 
  - `test_list_find` |  | 

- [ ] **log.c** - логирование |  | 
  - `test_log_messages` |  | 

- [ ] **config.c** - конфигурация |  | 
  - `test_config_load`, `test_config_save` |  | 

#### Сборка тестов

- [ ] Создать `tests/Makefile.test` |  | 
  - Компиляция с `-DDF_TEST_ENABLE_STUBS` |  | 
  - Линковка с тестируемыми модулями |  | 
  - Цель `test` - запуск всех тестов |  | 
  - Цель `clean` - удаление объектных файлов |  | 

```makefile
# tests/Makefile.test

CC = tcc
CFLAGS = -I.. -I. -DDF_TEST_ENABLE_STUB -D__TINYC__

TEST_SRCS = test_main.c test_rect.c test_keys.c test_window.c \
            test_message.c test_dfalloc.c test_lists.c

TEST_OBJS = $(TEST_SRCS:.c=.obj)

LIB_OBJS = ../rect.obj ../keys.obj ../window.obj ../message.obj \
           ../dfalloc.obj ../lists.obj ../log.obj ../config.obj

all: test.exe

test.exe: $(TEST_OBJS) $(LIB_OBJS)
	$(CC) -o $@ $^

%.obj: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: test.exe
	test.exe

clean:
	del *.obj test.exe
```

### Критерии приёмки Фазы -1

- [ ] `df_test.h` компилируется с TinyCC без ошибок |  | 
- [ ] Все тесты запускаются через `test.exe` |  | 
- [ ] Тесты проходят для: rect, keys, window, message, dfalloc, lists |  | 
- [ ] Mock/stub система работает для video.c |  | 
- [ ] Код завершения = 0 при успехе, = 1 при ошибках |  | 

## Критерии приёмки

### Фаза 1 (Инфраструктура)
- [ ] tcccompat.h компилируется без ошибок |  | 
- [ ] Все структуры DOS совместимы с Win32 версиями |  | 
- [ ] tccwin32.c компилируется с Win32 API |  | 

### Фаза 2 (Заголовки)
- [ ] dflat.h корректно включает tinycc файлы при `__TINYC__` |  | 
- [ ] system.h компилируется с переопределёнными макросами |  | 
- [ ] video.h работает с плоской памятью |  | 

### Фаза 3 (Исходники)
- [ ] message.c работает без аппаратных прерываний |  | 
- [ ] video.c использует Win32 Console API |  | 
- [ ] console.c правильно обрабатывает ввод/вывод |  | 
- [ ] mouse.c работает с консольными событиями мыши |  | 

### Фаза 4 (Сборка)
- [ ] makefile.tinycc собирает проект без ошибок |  | 
- [ ] Создаётся memopad.exe |  | 

### Фаза 5 (Тестирование)
- [ ] Приложение запускается без ошибок |  | 
- [ ] Все основные функции работают (см. список выше) |  | 
- [ ] Нет утечек памяти |  | 

## Риски

1. **Таймеры** — требуется полностью переработать message.c
   - Решение: опрос `GetTickCount64()` в цикле обработки сообщений

2. **Синхронизация** — нужно обеспечить корректную работу таймеров без прерываний
   - Решение: проверка `timed_out()` в `collect_events()` на каждой итерации

3. **Размер консоли** — Windows консоль может меняться динамически
   - Решение: переопределить `SCREENWIDTH`/`SCREENHEIGHT` как макросы с вызовом функции

4. **Мышь в консоли** — не все терминалы поддерживают мышь
   - Решение: эмуляция через keyboard (щелчок мыши = Enter на элементе)

5. **Кодировка** — Windows консоль по умолчанию использует CP1251
   - Решение: `SetConsoleOutputCP(866)` и `SetConsoleCP(866)`

6. **Видеопамять** — прямое чтение/запись в видеопамять невозможно
   - Решение: использовать `ReadConsoleOutput` / `WriteConsoleOutput`

## Фаза 9: Поддержка современных терминалов

### Обзор

Поддержка современных терминальных приложений: Microsoft Terminal (Windows Terminal), WezTerm, iTerm2, kitty и других xterm-совместимых эмуляторов.

### Отличия от стандартной консоли Windows

| Характеристика | Windows Console | Microsoft Terminal / WeZTerm |
|----------------|-----------------|------------------------------|
| Цвета | 16 (4-bit) | 24-bit RGB (16M цветов) |
| Кодировка | CP866 / CP1251 | UTF-8 |
| Мышь | Ограниченная | SGR extended mode |
| Графика | Отсутствует | Sixel (WezTerm) |
| Escape-последовательности | Минимальные | Полные ANSI/VT100 |
| Размер буфера | Фиксированный | Динамический |

### 6.1. Создание файлов терминальной абстракции

#### 6.1.1. tinycc/termcaps.h

Создать файл `tinycc/termcaps.h`:

```c
#ifndef TERMCAPS_H
#define TERMCAPS_H

typedef enum {
    TERM_UNKNOWN,
    TERM_WINDOWS_CONSOLE,    // cmd.exe, 16 color mode
    TERM_WINDOWS_TERMINAL,   // Windows Terminal (wt.exe)
    TERM_WEZTERM,            // WezTerm
    TERM_ITERM2,             // iTerm2 (macOS)
    TERM_KITTY,              // kitty terminal
    TERM_XTERM,              // xterm, urxvt, etc.
    TERM_SCREEN,             // GNU screen
    TERM_TMUX                // tmux
} TERMINAL_TYPE;

typedef struct {
    int colors;              // 16, 256, or 16777216 (24-bit)
    int mouse_tracking;      // 0=none, 1=click, 2=SGR
    int utf8;                // Use UTF-8
    int bracketed_paste;     // Bracketed paste mode
    int sixel;               // Sixel graphics (WezTerm only)
    int clear_screen;        // Can clear screen
    int set_cursor;          // Can set cursor position
    int hide_cursor;         // Can hide cursor
} TerminalCapabilities;

extern TERMINAL_TYPE currentTerminalType;
extern TerminalCapabilities terminalCaps;

/* Прототипы функций */
TERMINAL_TYPE DetectTerminal(void);
TerminalCapabilities DetectCapabilities(TERMINAL_TYPE type);
void InitTerminalCapabilities(void);
void SetTerminalFeatures(TERMINAL_TYPE type);

#endif /* TERMCAPS_H */
```

#### 6.1.2. tinycc/terminal.c

Создать файл `tinycc/terminal.c`:

```
Переменные:
- currentTerminalType - глобальная переменная типа терминала
- terminalCaps - глобальная структура возможностей

Функция: DetectTerminal()
  1. Проверить GetEnvironmentVariable("WT_SESSION", ...) > 0
     -> вернуть TERM_WINDOWS_TERMINAL
  2. Проверить GetEnvironmentVariable("TERM_PROGRAM", buf, size)
     если содержит "WezTerm" -> TERM_WEZTERM
     если содержит "iTerm" -> TERM_ITERM2
     если содержит "Apple_Terminal" -> TERM_ITERM2
  3. Проверить GetEnvironmentVariable("TERM", buf, size)
     если начинается с "xterm" -> TERM_XTERM
     если "screen" -> TERM_SCREEN
     если "tmux" -> TERM_TMUX
     если "kitty" -> TERM_KITTY
  4. Вернуть TERM_WINDOWS_CONSOLE по умолчанию

Функция: DetectCapabilities(TERMINAL_TYPE type)
  - Инициализировать caps = {16, 0, 0, 0, 0, 1, 1, 1}
  - Для TERM_WINDOWS_TERMINAL, TERM_WEZTERM, TERM_ITERM2, TERM_KITTY, TERM_XTERM:
    caps = {16777216, 2, 1, 1, (type==TERM_WEZTERM)?1:0, 1, 1, 1}
  - Для TERM_WINDOWS_CONSOLE:
    caps = {16, 1, 0, 0, 0, 1, 1, 1}
  - Вернуть caps

Функция: InitTerminalCapabilities()
  - currentTerminalType = DetectTerminal()
  - terminalCaps = DetectCapabilities(currentTerminalType)

Функция: SetTerminalFeatures(TERMINAL_TYPE type)
  - Вызвать printf("\x1b[?1006h") если caps.mouse_tracking >= 2
  - Вызвать printf("\x1b[?2004h") если caps.bracketed_paste
  - Вызвать SetConsoleOutputCP(CP_UTF8) если caps.utf8
```

#### 6.1.3. tinycc/ansi_output.c

Создать файл `tinycc/ansi_output.c` (~300 строк):

```
Глобальные переменные:
- currentX, currentY - текущая позиция курсора
- currentFgColor, currentBgColor - текущие цвета
- currentAttributes - текущие атрибуты (bold, underline, etc.)

Макросы для escape-последовательностей:
#define ANSI_ESCAPE "\x1b["
#define ANSI_RESET "0m"
#define ANSI_HOME "1;1H"

Функция: ANSI_Init()
  - Вызвать SetTerminalFeatures(DetectTerminal())
  - Инициализировать цвета по умолчанию (светло-серый на чёрном)
  - currentX = currentY = 0

Функция: ANSI_SetCursor(int x, int y)
  - printf("\x1b[%d;%df", y + 1, x + 1)
  - currentX = x, currentY = y

Функция: ANSI_GetCursor(int *x, int *y)
  - *x = currentX, *y = currentY

Функция: ANSI_HideCursor(void)
  - printf("\x1b[?25l")

Функция: ANSI_ShowCursor(void)
  - printf("\x1b[?25h")

Функция: ANSI_SetForeground(int r, int g, int b)
  - printf("\x1b[38;2;%d;%d;%dm", r, g, b)
  - currentFgColor = RGB(r,g,b)

Функция: ANSI_SetBackground(int r, int g, int b)
  - printf("\x1b[48;2;%d;%d;%dm", r, g, b)
  - currentBgColor = RGB(r,g,b)

Функция: ANSI_SetTextAttribute(int attr)
  -解析 attr: foreground = attr & 0x0F, background = (attr >> 4) & 0x0F
  - Если bright: printf("\x1b[1m")
  - Если underline: printf("\x1b[4m")
  - Если reverse: printf("\x1b[7m")
  - Если blink: printf("\x1b[5m")
  - Вызвать ANSI_SetForeground/Background для соответствующих RGB

Функция: ANSI_ClearScreen(void)
  - printf("\x1b[2J")

Функция: ANSI_ClearLine(void)
  - printf("\x1b[2K")

Функция: ANSI_SetBold(void)
  - printf("\x1b[1m")

Функция: ANSI_ResetAttributes(void)
  - printf("\x1b[0m")

Функция: ANSI_EnableMouseTracking(void)
  - printf("\x1b[?1003h")  // Всё движение
  - printf("\x1b[?1006h")  // SGR режим

Функция: ANSI_DisableMouseTracking(void)
  - printf("\x1b[?1003l")
  - printf("\x1b[?1006l")

Функция: ANSI_EnableBracketedPaste(void)
  - printf("\x1b[?2004h")

Функция: ANSI_DisableBracketedPaste(void)
  - printf("\x1b[?2004l")

Функция: ANSI_SetScrollingRegion(int top, int bottom)
  - printf("\x1b[%d;%dr", top + 1, bottom + 1)

Функция: ANSI_ScrollUp(void)
  - printf("\n")

Функция: ANSI_ScrollDown(void)
  - printf("\x1bM")
```

#### 6.1.4. tinycc/term_mouse.c

Создать файл `tinycc/term_mouse.c` для обработки мышиных событий:

```
Глобальные переменные:
- mouseX, mouseY, mouseButtons - позиция и кнопки
- mouseEventQueue[32] - очередь событий
- mouseQueueHead, mouseQueueTail - индексы очереди

Макросы кнопок мыши:
#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_MIDDLE 0x04
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_MOVE   0x20
#define MOUSE_WHEEL_UP      0x40
#define MOUSE_WHEEL_DOWN    0x80

Функция: Mouse_Init()
  - Вызвать ANSI_EnableMouseTracking()
  - Инициализировать очередь: head = tail = 0

Функция: Mouse_Close()
  - Вызвать ANSI_DisableMouseTracking()

Функция: Mouse_ParseSGR(const char *seq)
  - Парсить последовательность: \x1b[<button;x;yM или \x1b[<button;x;ym
  - button: 0=move, 1=left, 2=middle, 3=right, 64=wheel up, 65=wheel down
  - Извлечь x, y (1-based) и преобразовать в 0-based
  - Добавить событие в очередь

Функция: Mouse_GetEvent(int *x, int *y, int *buttons)
  - Если очередь не пуста: извлечь событие, вернуть TRUE
  - Иначе: вернуть FALSE
```

### 6.2. Модификация tinycc/tccwin32.c

Добавить переключение между Win32 Console API и ANSI-режимом:

```
Добавить переменную:
  extern int useAnsiOutput;  // TRUE для современных терминалов

Модифицировать функции вывода:
  Win32_PutChar() -> если useAnsiOutput: вызвать ANSI_PutChar()
  Win32_SetCursor() -> если useAnsiOutput: вызвать ANSI_SetCursor()

Добавить в InitWin32Console():
  // Определить тип терминала
  InitTerminalCapabilities();
  if (terminalCaps.colors >= 256) {
      useAnsiOutput = TRUE;
      ANSI_Init();
      Mouse_Init();
  }
```

### 6.3. Добавить в tinycc/tccwin32.h

```c
/* Терминальные функции */
extern TERMINAL_TYPE currentTerminalType;
extern TerminalCapabilities terminalCaps;
extern int useAnsiOutput;

TERMINAL_TYPE DetectTerminal(void);
TerminalCapabilities DetectCapabilities(TERMINAL_TYPE type);
void InitTerminalCapabilities(void);

/* ANSI вывод */
void ANSI_Init(void);
void ANSI_SetCursor(int x, int y);
void ANSI_GetCursor(int *x, int *y);
void ANSI_HideCursor(void);
void ANSI_ShowCursor(void);
void ANSI_SetForeground(int r, int g, int b);
void ANSI_SetBackground(int r, int g, int b);
void ANSI_SetTextAttribute(int attr);
void ANSI_ClearScreen(void);
void ANSI_ClearLine(void);
void ANSI_PutChar(int ch, int attr);
void ANSI_EnableMouseTracking(void);
void ANSI_DisableMouseTracking(void);
void ANSI_EnableBracketedPaste(void);
void ANSI_DisableBracketedPaste(void);

/* Мышь */
void Mouse_Init(void);
void Mouse_Close(void);
int Mouse_ParseEvent(const char *seq);
int Mouse_GetEvent(int *x, int *y, int *buttons);
```

### 6.4. Константы RGB-цветов для DOS-совместимости

Добавить таблицу преобразования 16 цветов DOS в RGB:

```c
static const RGB_Color DOS_ColorTable[16] = {
    {0, 0, 0},         // BLACK (0)
    {0, 0, 170},       // BLUE (1)
    {0, 170, 0},       // GREEN (2)
    {0, 170, 170},     // CYAN (3)
    {170, 0, 0},       // RED (4)
    {170, 0, 170},     // MAGENTA (5)
    {170, 85, 0},      // BROWN (6)
    {170, 170, 170},   // LIGHTGRAY (7)
    {85, 85, 85},      // DARKGRAY (8)
    {85, 85, 255},     // LIGHTBLUE (9)
    {85, 255, 85},     // LIGHTGREEN (10)
    {85, 255, 255},    // LIGHTCYAN (11)
    {255, 85, 85},     // LIGHTRED (12)
    {255, 85, 255},    // LIGHTMAGENTA (13)
    {255, 255, 85},    // YELLOW (14)
    {255, 255, 255}    // WHITE (15)
};
```

### 6.5. Конфигурация через переменные окружения

Добавить поддержку переменных:

```c
void LoadEnvironmentConfig(void) {
    char envBuf[256];
    
    // DFLAT_TERM=auto|console|ansi
    if (GetEnvironmentVariable("DFLAT_TERM", envBuf, sizeof(envBuf)) > 0) {
        if (strcmp(envBuf, "console") == 0) {
            useAnsiOutput = FALSE;
        } else if (strcmp(envBuf, "ansi") == 0) {
            useAnsiOutput = TRUE;
        } else {  // auto
            InitTerminalCapabilities();
            useAnsiOutput = (terminalCaps.colors > 16);
        }
    }
    
    // DFLAT_COLORS=16|256|16777216
    if (GetEnvironmentVariable("DFLAT_COLORS", envBuf, sizeof(envBuf)) > 0) {
        terminalCaps.colors = atoi(envBuf);
    }
}
```

### 6.6. Таблица соответствия функций Фазы 6

| Создать/Изменить | Файл | Описание |
|-----------------|------|----------|
| Создать | `tinycc/termcaps.h` | Определения типов терминалов |
| Создать | `tinycc/terminal.c` | Определение типа терминала |
| Создать | `tinycc/ansi_output.c` | ANSI escape-последовательности |
| Создать | `tinycc/term_mouse.c` | Обработка мыши SGR |
| Модифицировать | `tinycc/tccwin32.c` | Добавить переключение режимов |
| Модифицировать | `tinycc/tccwin32.h` | Добавить прототипы |

```c
typedef enum {
    TERM_UNKNOWN,
    TERM_WINDOWS_CONSOLE,
    TERM_WINDOWS_TERMINAL,
    TERM_WEZTERM,
    TERM_ITERM2,
    TERM_KITTY,
    TERM_XTERM
} TERMINAL_TYPE;

TERMINAL_TYPE DetectTerminal(void) {
    if (GetEnvironmentVariable("WT_SESSION", NULL, 0) > 0)
        return TERM_WINDOWS_TERMINAL;
    if (GetEnvironmentVariable("TERM_PROGRAM", buf, sizeof(buf))) {
        if (strstr(buf, "WezTerm")) return TERM_WEZTERM;
        if (strstr(buf, "iTerm")) return TERM_ITERM2;
    }
    return TERM_WINDOWS_CONSOLE;
}
```

#### True Color (24-bit RGB)

- [ ] Определить константы цветов в формате RGB |  | 
- [ ] Реализовать escape-последовательности для true color: |  | 
  - Foreground: `\x1b[38;2;R;G;Bm` |  | 
  - Background: `\x1b[48;2;R;G;Bm` |  | 

```c
#define ANSI_FG_RGB "\x1b[38;2;%d;%d;%dm"
#define ANSI_BG_RGB "\x1b[48;2;%d;%d;%dm"
#define ANSI_RESET "\x1b[0m"

// Пример использования:
printf(ANSI_FG_RGB RGB(255, 128, 0) "Orange text" ANSI_RESET);
```

- [ ] Создать таблицу преобразования 16 цветов DOS в RGB для совместимости |  | 
- [ ] Добавить расширенные атрибуты: яркость, полужирный, подчёркивание, курсив |  | 

| DOS атрибут | ANSI код |
|-------------|----------|
| BLACK | 30/40 |
| BLUE | 34/44 |
| GREEN | 32/42 |
| CYAN | 36/46 |
| RED | 31/41 |
| MAGENTA | 35/45 |
| BROWN | 33/43 |
| LIGHTGRAY | 37/47 |
| DARKGRAY | 90/100 |
| LIGHTBLUE | 94/104 |
| LIGHTGREEN | 92/102 |
| LIGHTCYAN | 96/106 |
| LIGHTRED | 91/101 |
| LIGHTMAGENTA | 95/105 |
| YELLOW | 93/103 |
| WHITE | 97/107 |
| BLINK | 5 |
| UNDERLINE | 4 |
| REVERSE | 7 |

#### Расширенные escape-последовательности

- [ ] Реализовать функции вывода ANSI последовательностей |  | 
- [ ] Курсор: `\x1b[Н;Мf` (position), `\x1b[?25h/l` (show/hide) |  | 
- [ ] Очистка: `\x1b[2J` (screen), `\x1b[К` (line) |  | 
- [ ] Режимы: `\x1b[?1003h/l` (mouse tracking), `\x1b[?2004h/l` (bracketed paste) |  | 

```c
void ANSI_SetCursor(int row, int col) {
    printf("\x1b[%d;%df", row + 1, col + 1);
}

void ANSI_HideCursor(void) {
    printf("\x1b[?25l");
}

void ANSI_ShowCursor(void) {
    printf("\x1b[?25h");
}

void ANSI_ClearScreen(void) {
    printf("\x1b[2J");
}
```

#### Мышь (расширенный протокол SGR)

- [ ] Реализовать режим SGR 1006: `\x1b[?1006h` |  | 
- [ ] Формат событий мыши: `\x1b[<button;x;yM` или `\x1b[<button;x;ym` |  | 
- [ ] Поддержка колеса прокрутки (buttons 64-71) |  | 
- [ ] Поддержка перемещения (button 32) |  | 

```c
// Включение расширенного режима мыши:
printf("\x1b[?1006h");  // SGR mode
printf("\x1b[?1003h");  // Track mouse movement
```

#### Unicode и UTF-8

- [ ] Использовать UTF-8 вместо CP866 при обнаружении современного терминала |  | 
- [ ] Широкие символы (wchar_t) для внутреннего представления |  | 
- [ ] Функции конвертации UTF-8 ↔ UTF-16 для Win32 API |  | 

```c
#ifdef UNICODE
#define TEXT(s) L##s
#else
#define TEXT(s) s
#endif

// Определение кодировки
int target_cp = (terminal_type >= TERM_WINDOWS_TERMINAL) ? CP_UTF8 : 866;
```

#### Bracketed Paste Mode

- [ ] Включить: `\x1b[?2004h` |  | 
- [ ] Выключить: `\x1b[2004l` |  | 
- [ ] Обработка входящих данных: `\x1b[200~...~\x1b[201~` |  | 

#### WezTerm-специфичные функции

- [ ] Определение WezTerm через `TERM_PROGRAM=WezTerm` |  | 
- [ ] Sixel графика (опционально): `\x1bP1;1;[q` ... `\x1b\\` |  | 
- [ ] Оптимизация производительности для GPU-рендеринга |  | 

#### Microsoft Terminal-специфичные

- [ ] Определение через `WT_SESSION` |  | 
- [ ] Поддержка ConPTY (Pseudo Console) - автоматически работает |  | 
- [ ] Настройки пользователя через `settings.json` |  | 

#### Режим совместимости

- [ ] Автоматическое определение возможностей терминала |  | 
- [ ] Фоллбек на стандартное Win32 Console API если терминал не поддерживается |  | 
- [ ] Конфигурация через переменные окружения: |  | 
  - `DFLAT_TERM=auto|console|ansi` |  | 
  - `DFLAT_COLORS=16|256|24bit` |  | 

```c
typedef struct {
    int colors;          // 16, 256, or 16777216
    int mouse_tracking;  // 0=none, 1=click, 2=SGR
    int utf8;           // Use UTF-8
    int bracketed_paste;
    int sixel;          // WezTerm only
} TerminalCapabilities;

TerminalCapabilities DetectCapabilities(TERMINAL_TYPE type) {
    TerminalCapabilities caps = {16, 0, 0, 0, 0};
    
    switch (type) {
        case TERM_WINDOWS_TERMINAL:
        case TERM_WEZTERM:
        case TERM_KITTY:
        case TERM_ITERM2:
            caps.colors = 16777216;
            caps.mouse_tracking = 2;
            caps.utf8 = 1;
            caps.bracketed_paste = 1;
            if (type == TERM_WEZTERM)
                caps.sixel = 1;
            break;
        case TERM_XTERM:
            caps.colors = 16777216;
            caps.mouse_tracking = 2;
            caps.utf8 = 1;
            caps.bracketed_paste = 1;
            break;
        default:
            // Windows Console - use Win32 API
            break;
    }
    return caps;
}
```

### Файлы для модификации

- [ ] `tinycc/termcaps.h` — определение возможностей терминала |  | 
- [ ] `tinycc/ansi_output.c` — вывод ANSI escape-последовательностей |  | 
- [ ] `tinycc/terminal.c` — определение типа терминала и возможностей |  | 
- [ ] `tinycc/tccwin32.c` — добавить переключение между Win32 и ANSI режимами |  | 

### Зависимости

```
DetectTerminal() → LoadTerminalCapabilities() → ANSI_Init()
       ↓
tccwin32.c (основной выбор: Win32 API или ANSI вывод)
       ↓
message.c (обработка мыши в ANSI режиме)
```

## Фаза 10: Unix-портирование (будущая)

### Обзор

После реализации PAL (Фаза 3), добавление Unix-поддержки требует только создания Unix-реализации console abstraction layer без изменения ядра D-Flat.

### Структура Unix-порта

```
unix/
├── console_unix.c      # ncurses реализация (полная функциональность)
├── console_ansi.c      # Pure ANSI fallback (без ncurses)
├── unixcons.h          # Заголовочный файл
├── Makefile.unix       # Сборка для Unix (gcc/clang)
└── dflat.h            # Модифицированный: #define CONSOLE_IMPLEMENTATION unix
```

### Unix-реализация через ncurses

#### Зависимости

- ncursesw (с поддержкой wide characters)
- libpanel (для панелей - опционально)
- POSIX API (терминал, файловая система)

### 7.1. unix/unixcons.h

Создать файл `unix/unixcons.h`:

```c
#ifndef UNIXCONS_H
#define UNIXCONS_H

#include <curses.h>

/* Глобальные переменные */
extern WINDOW *stdscr;
extern SCREEN *screen;
extern int consoleWidth;
extern int consoleHeight;
extern int currentCursorX;
extern int currentCursorY;
extern int cursorVisible;
extern int mouseX;
extern int mouseY;
extern int mouseButtons;

/* Прототипы функций инициализации */
int  UnixConsole_Init(void);
void UnixConsole_Close(void);
void UnixConsole_GetSize(int *width, int *height);

/* Прототипы видео-функций */
void UnixConsole_ReadScreen(int x, int y, int w, int h, void *buf);
void UnixConsole_WriteScreen(int x, int y, int w, int h, void *buf);
int  UnixConsole_GetChar(int x, int y);
void UnixConsole_PutChar(int x, int y, int ch, int attr);

/* Прототипы курсор-функций */
void UnixConsole_SetCursor(int x, int y);
void UnixConsole_GetCursor(int *x, int *y);
void UnixConsole_HideCursor(void);
void UnixConsole_ShowCursor(void);
void UnixConsole_SetCursorType(int type);

/* Прототипы клавиатурных функций */
int  UnixConsole_KbHit(void);
int  UnixConsole_GetKey(void);
int  UnixConsole_GetShiftState(void);

/* Прототипы мышиных функций */
int  UnixConsole_MouseSupported(void);
void UnixConsole_GetMousePos(int *x, int *y, int *buttons);
void UnixConsole_SetMousePos(int x, int y);
void UnixConsole_ShowMouse(void);
void UnixConsole_HideMouse(void);

/* Прототипы таймерных функций */
unsigned long UnixConsole_GetTickCount(void);

/* Прототипы файловых функций */
long UnixConsole_FindFirst(const char *pattern, void *ffblk);
int  UnixConsole_FindNext(void *ffblk);
void UnixConsole_FindClose(void *ffblk);
char UnixConsole_GetCurrentDrive(void);
int  UnixConsole_SetCurrentDrive(char drive);
char *UnixConsole_GetCurrentDir(char drive, char *path);
int  UnixConsole_ChangeDir(const char *path);

/* Прототипы утилит */
void UnixConsole_Beep(void);
void UnixConsole_SetVideoMode(int mode);
int  UnixConsole_GetVideoMode(void);

#endif /* UNIXCONS_H */
```

### 7.2. unix/console_unix.c

Создать файл `unix/console_unix.c` (~500 строк):

#### Раздел 1: Инициализация ncurses
```
Функция: UnixConsole_Init()
- Вызвать initscr() - инициализация ncurses
- Проверить NULL -> ошибка
- Вызвать raw() - режим без буферизации
- Вызвать noecho() - без автоматического эха
- Вызвать keypad(stdscr, TRUE) - расширенные клавиши
- Вызвать nodelay(stdscr, TRUE) - неблокирующий ввод
- Вызвать mousemask(ALL_MOUSE_EVENTS, NULL) - включить мышь
- Получить размер: getmaxyx(stdscr, height, width)
- Инициализировать переменные: cursorVisible = TRUE
```

#### Раздел 2: Завершение
```
Функция: UnixConsole_Close()
- Вызвать endwin() - восстановить терминал
- Если screen != NULL: delscreen(screen)
```

#### Раздел 3: Размер экрана
```
Функция: UnixConsole_GetSize(int *width, int *height)
- getmaxyx(stdscr, *height, *width)
- Обновить глобальные переменные consoleWidth, consoleHeight
```

#### Раздел 4: Видео-операции
```
Функция: UnixConsole_ReadScreen(int x, int y, int w, int h, void *buf)
- for (row = 0; row < h; row++):
    for (col = 0; col < w; col++):
        chtype ch = mvwinch(stdscr, y + row, x + col)
        ((unsigned short *)buf)[row * w + col] = ch

Функция: UnixConsole_WriteScreen(int x, int y, int w, int h, void *buf)
- for (row = 0; row < h; row++):
    for (col = 0; col < w; col++):
        chtype ch = ((unsigned short *)buf)[row * w + col]
        mvwaddch(stdscr, y + row, x + col, ch)
- touchwin(stdscr) -> refresh()
```

#### Раздел 5: Курсор
```
Функция: UnixConsole_SetCursor(int x, int y)
- wmove(stdscr, y, x)
- currentCursorX = x, currentCursorY = y

Функция: UnixConsole_GetCursor(int *x, int *y)
- getyx(stdscr, *y, *x)

Функция: UnixConsole_HideCursor(void)
- curs_set(0)
- cursorVisible = FALSE

Функция: UnixConsole_ShowCursor(void)
- curs_set(1)
- cursorVisible = TRUE
```

#### Раздел 6: Клавиатура
```
Функция: UnixConsole_KbHit(void)
- nodelay(stdscr, TRUE)
- int ch = wgetch(stdscr)
- if (ch != ERR) { ungetch(ch); return TRUE; }
- return FALSE

Функция: UnixConsole_GetKey(void)
- nodelay(stdscr, FALSE)
- int ch = wgetch(stdscr)
- Обработать escape-последовательности (стрелки, F1-F12)
- Обработать meta-клавиши (Alt+...)
- Вернуть виртуальный код (совместимый с DOS)

Функция: UnixConsole_GetShiftState(void)
- Проверить getmods() или состояние клавиш Shift/Ctrl/Alt
- Вернуть битовую маску: SHIFT | CTRL | ALT
```

#### Раздел 7: Мышь
```
Функция: UnixConsole_MouseSupported(void)
- Проверить: return (mousemask(ALL_MOUSE_EVENTS, NULL) != 0)

Функция: UnixConsole_GetMousePos(int *x, int *y, int *buttons)
- MEVENT event
- if (getmouse(&event) == OK):
    *x = event.x, *y = event.y
    *buttons = (event.bstate & BUTTON1_CLICKED ? 1 : 0) |
               (event.bstate & BUTTON2_CLICKED ? 2 : 0) |
               (event.bstate & BUTTON3_CLICKED ? 4 : 0)
```

#### Раздел 8: Таймер
```
Функция: UnixConsole_GetTickCount(void)
- struct timespec ts
- clock_gettime(CLOCK_MONOTONIC, &ts)
- return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000)
```

#### Раздел 9: Файловые операции (POSIX)
```
Функция: UnixConsole_FindFirst(const char *pattern, void *ffblk)
- DIR *dir = opendir(pattern_with_wildcards)
- вернуть (long)dir или -1 при ошибке

Функция: UnixConsole_FindNext(void *ffblk)
- DIR *dir = (DIR *)ffblk
- struct dirent *entry = readdir(dir)
- если entry != NULL: return 0 иначе: return -1

Функция: UnixConsole_FindClose(void *ffblk)
- closedir((DIR *)ffblk)

Функция: UnixConsole_GetCurrentDrive(void)
- Всегда возвращать '/' (Unix использует один корень)

Функция: UnixConsole_ChangeDir(const char *path)
- return chdir(path) == 0 ? 0 : -1

Функция: UnixConsole_GetCurrentDir(char drive, char *path)
- getcwd(path, MAX_PATH)
```

### 7.3. unix/console_ansi.c (Pure ANSI fallback)

Создать файл `unix/console_ansi.c` (~200 строк) для терминалов без ncurses:

```
Функции: аналогично console_unix.c, но вместо ncurses используются:
- tputs(), tgetstr() для терминальных escape-последовательностей
- termcap/terminfo для определения возможностей
- read() для чтения из stdin
```

### 7.4. unix/Makefile.unix

Создать файл `unix/Makefile.unix`:

```makefile
# Unix Makefile for D-Flat
# Компилятор: gcc или clang

CC = gcc
CFLAGS = -Wall -Wextra -I. -I./unix -DBUILD_FULL_DFLAT -D__unix__ -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lncursesw -lpanel -lform

LIB_SRCS = window.c video.c message.c mouse.c console.c \
           textbox.c listbox.c normal.c config.c menu.c \
           menubar.c popdown.c rect.c applicat.c keys.c \
           sysmenu.c editbox.c dialbox.c button.c fileopen.c \
           msgbox.c helpbox.c log.c lists.c statbar.c \
           decomp.c combobox.c pictbox.c calendar.c barchart.c \
           clipbord.c search.c dfalloc.c checkbox.c text.c \
           radio.c box.c spinbutt.c watch.c slidebox.c \
           direct.c editor.c unix/console_unix.c

APP_SRCS = memopad.c dialogs.c menus.c

all: memopad

memopad: $(LIB_SRCS:.c=.o) $(APP_SRCS:.c=.o)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o unix/*.o memopad
```

### 7.5. Модификация dflat.h

Добавить в dflat.h секцию выбора реализации:

```c
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)
    #include "unix/unixcons.h"
    #define CONSOLE_IMPLEMENTATION unix
```

### 7.6. Модификация system.h

Добавить переопределения:

```c
#ifdef __unix__
    #undef SCREENWIDTH
    #undef SCREENHEIGHT
    #define SCREENWIDTH  (consoleWidth)
    #define SCREENHEIGHT (consoleHeight)
    extern int consoleWidth;
    extern int consoleHeight;
#endif
```

### Таблица функций для Unix-порта

| Оригинальная функция | Unix-реализация | Файл |
|---------------------|-----------------|------|
| `InitWin32Console()` | `UnixConsole_Init()` | console_unix.c |
| `CloseWin32Console()` | `UnixConsole_Close()` | console_unix.c |
| `GetConsoleSize()` | `UnixConsole_GetSize()` | console_unix.c |
| `Win32_getvideo()` | `UnixConsole_ReadScreen()` | console_unix.c |
| `Win32_storevideo()` | `UnixConsole_WriteScreen()` | console_unix.c |
| `Win32_cursor()` | `UnixConsole_SetCursor()` | console_unix.c |
| `Win32_keyhit()` | `UnixConsole_KbHit()` | console_unix.c |
| `Win32_getkey()` | `UnixConsole_GetKey()` | console_unix.c |
| `Win32_mouse_*()` | `UnixConsole_GetMousePos()` | console_unix.c |
| `Win32_GetTickCount()` | `UnixConsole_GetTickCount()` | console_unix.c |
| `Win32_findfirst()` | `UnixConsole_FindFirst()` | console_unix.c |
| `Win32_chdir()` | `UnixConsole_ChangeDir()` | console_unix.c |

#### Особенности реализации

```c
// Unix-инициализация
int Console_Init(void) {
    // ncurses инициализация
    SCREEN *screen = newterm(NULL, stdout, stdin);
    set_term(screen);
    raw();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    
    // Определение поддержки мыши
    int has_mouse = (mousemask(ALL_MOUSE_EVENTS, NULL) != 0);
    
    return 0;
}

// Чтение экрана
void Console_ReadScreen(int x, int y, int w, int h, void *buf) {
    chtype *p = (chtype *)buf;
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            *p++ = mvwinch(stdscr, y + j, x + i);
        }
    }
}

// Мышь через ncurses
void Console_GetMousePos(int *x, int *y, int *buttons) {
    MEVENT event;
    if (getmouse(&event) == OK) {
        *x = event.x;
        *y = event.y;
        *buttons = (event.bstate & BUTTON1_CLICKED ? 1 : 0) |
                   (event.bstate & BUTTON3_CLICKED ? 2 : 0);
    } else {
        *buttons = 0;
    }
}
```

### Unix-реализация без ncurses (Pure ANSI)

Для терминалов без ncurses (embedded systems, минимальные окружения):

```
console_ansi.c - минимальная реализация через termcap/terminfo
```

| Функция | ANSI-эквивалент |
|---------|----------------|
| `Console_Init()` | `tput init`, определение `TERM` |
| `Console_SetCursor()` | `\x1b[LINE;COLH` |
| `Console_GetKey()` | `read()` stdin + таблицы scan codes |
| `Console_ReadScreen()` | Не поддерживается (keep it simple) |
| `Console_WriteScreen()` | `\x1b[LINE;COLH` + символы |

### Интеграция Unix с PAL

#### Модификации system.h

```c
// system.h - Unix-специфичные изменения
#ifdef __unix__
    #define CONSOLE_IMPLEMENTATION unix
    // Переопределение макросов
    #undef SCREENWIDTH
    #undef SCREENHEIGHT
    #define SCREENWIDTH  (Console_GetWidth())
    #define SCREENHEIGHT (Console_GetHeight())
#endif
```

#### Модификации video.c

```c
// video.c - использование PAL
#ifdef __unix__
    void getvideo(RECT rc, void far *buf) {
        Console_ReadScreen(rc.lf, rc.tp, RectWidth(rc), RectHeight(rc), buf);
    }
    void storevideo(RECT rc, void far *buf) {
        Console_WriteScreen(rc.lf, rc.tp, RectWidth(rc), RectHeight(rc), buf);
    }
#endif
```

### Файлы для Unix-портирования

#### Новые файлы

- [ ] `unix/unixcons.h` — объявления для Unix |  | 
- [ ] `unix/console_unix.c` — ncurses реализация (~400 строк) |  | 
- [ ] `unix/console_ansi.c` — ANSI fallback (~200 строк) |  | 
- [ ] `unix/Makefile.unix` — gcc/clang сборка |  | 

#### Модифицируемые файлы

- [ ] `dflat.h` — добавить `#elif defined(__unix__)` |  | 
- [ ] `system.h` — переопределить `SCREENWIDTH`/`SCREENHEIGHT` |  | 
- [ ] `video.h` — добавить `#ifdef __unix__` обёртки |  | 
- [ ] Создать универсальный `makefile` для кроссплатформенной сборки (gcc/clang) |  | 
- [ ] Сохранить `makefile.tinycc` для TinyCC |  | 

### Сборка на Unix

```makefile
# Unix Makefile
CC = gcc
CFLAGS = -Wall -Wextra -I. -I./unix -DBUILD_FULL_DFLAT -D__unix__

LIB_SRCS = window.c video.c message.c mouse.c console.c \
           textbox.c listbox.c normal.c config.c menu.c \
           menubar.c popdown.c rect.c applicat.c keys.c \
           sysmenu.c editbox.c dialbox.c button.c fileopen.c \
           msgbox.c helpbox.c log.c lists.c statbar.c \
           decomp.c combobox.c pictbox.c calendar.c barchart.c \
           clipbord.c search.c dfalloc.c checkbox.c text.c \
           radio.c box.c spinbutt.c watch.c slidebox.c \
           direct.c editor.c unix/console_unix.c

APP_SRCS = memopad.c dialogs.c menus.c

LDFLAGS = -lncursesw -lpanel

all: memopad

memopad: $(LIB_SRCS:.c=.o) $(APP_SRCS:.c=.o)
	$(CC) -o $@ $^ $(LDFLAGS)
```

### Кроссплатформенная сборка

Для поддержки Windows и Unix из одного Makefile:

```makefile
# Универсальный Makefile
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    CC = gcc
    LDFLAGS = -lncursesw
    PLATFORM_DEFS = -D__linux__
else ifeq ($(UNAME_S),Darwin)
    CC = clang
    LDFLAGS = -lncursesw
    PLATFORM_DEFS = -D__APPLE__
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    CC = gcc
    PLATFORM_DEFS = -D__MINGW__
    # Windows-specific
endif
```

### Зависимости Unix-порта

```
console.h (из Фазы 0)
        ↓
unix/unixcons.h + console_unix.c
        ↓
dflat.h (platform selection)
        ↓
system.h + video.h (использование PAL)
        ↓
Unix-сборка
```

---

## Успешность

Высокая — Win32 Console API полностью покрывает функциональность DOS BIOS.
Благодаря PAL, Unix-портирование потребует только реализации `console_unix.c` без изменения ядра.

---

## Сводка прогресса

### Фазы

| Фаза | Статус | Дата завершения | Примечания |
|------|--------|-----------------|------------|
| Фаза 1: Юнит-тестирование | [ ] |  |  |
| Фаза 2: PAL (Platform Abstraction Layer) | [ ] |  |  |
| Фаза 3: Инфраструктура совместимости | [ ] |  |  |
| Фаза 4: Модификация заголовков | [ ] |  |  |
| Фаза 5: Модификация исходников | [ ] |  |  |
| Фаза 6: Makefile | [ ] |  |  |
| Фаза 7: Тестирование | [ ] |  |  |
| Фаза 8: Современные терминалы | [ ] |  |  |
| Фаза 9: Unix-портирование | [ ] |  |  |

### Статистика

```
Общее задач: XXX
Выполнено:   XX (XX%)
В процессе:  XX
Не начато:   XX
```

### Недавние изменения

| Дата | Задача | Примечания |
|------|--------|------------|
|  |  |  |
