#ifndef TCCCOMPAT_H
#define TCCCOMPAT_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

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

#define interrupt
#define _interrupt
#define __interrupt

#define _dos_entry_t struct __dos_entry_t_placeholder

#define outp(port, val) do { (void)(port); (void)(val); } while(0)
#define outportw(port, val) do { (void)(port); (void)(val); } while(0)
#define inp(port) 0
#define inportw(port) 0
#define _inp(port) 0
#define _outp(port, val) do { (void)(port); (void)(val); } while(0)

#ifndef _BASETSD_H
#ifndef _WINDEF_
typedef unsigned char       BYTE;
#ifndef WORD
typedef unsigned int        WORD;
#endif
typedef unsigned long       DWORD;
typedef unsigned int        UWORD;
typedef unsigned long       UDWORD;
typedef short               SHORT;
typedef int                 SIGNED;
typedef unsigned char       UCHAR;
#endif
#endif

#ifndef _WINDEF_
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
#endif

#define _wregs REGS

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

struct ffblk {
    unsigned long ff_reserved;
    unsigned long ff_attrib;
    unsigned short ff_ftime;
    unsigned short ff_fdate;
    unsigned long ff_fsize;
    char ff_name[13];
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

extern unsigned char keyportvalue;
#define KEYBOARDPORT 0x60

#define KB_SHIFT     0x01
#define KB_CTRL      0x04
#define KB_ALT       0x08
#define KB_SCROLL    0x10
#define KB_NUMLOCK   0x20
#define KB_CAPSLOCK  0x40
#define KB_INSLOCK   0x80

#define peek(seg, off)       (*((unsigned int *)(((seg) << 16) | (off))))
#define pokeb(seg, off, val) (*((unsigned char *)(((seg) << 16) | (off))) = (val))
#define pokew(seg, off, val) (*((unsigned int *)(((seg) << 16) | (off))) = (val))
#define _peek(seg, off)      (*((unsigned int *)(((seg) << 16) | (off))))
#define _pokeb(seg, off, val) (*((unsigned char *)(((seg) << 16) | (off))) = (val))
#define _pokew(seg, off, val) (*((unsigned int *)(((seg) << 16) | (off))) = (val))

#ifndef SCREENWIDTH
#define SCREENWIDTH  80
#endif

#ifndef SCREENHEIGHT
#define SCREENHEIGHT 25
#endif

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

#define attr(fg, bg) (((bg) << 4) | (fg))

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

typedef void (*interrupt_handler_t)(void);

#define getvect(intno)       (NULL)
#define setvect(intno, func) do { (void)(intno); (void)(func); } while(0)

#ifndef BOOL
#if defined(_WINDEF_) || defined(_BASETSD_H)
typedef int BOOL;
#else
typedef enum { FALSE = 0, TRUE = 1 } BOOL;
#endif
#endif
typedef long PARAM;

#ifndef NULL
#define NULL ((void *)0)
#endif

#define farpascal
#define _farpascal far
#define cdecl
#define _cdecl

#define MAXPATH      256
#define MAXDRIVE     3
#define MAXDIR       256
#define MAXFILE      256
#define MAXEXT       256

#endif /* TCCCOMPAT_H */