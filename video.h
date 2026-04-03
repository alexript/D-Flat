/* ---------------- video.h ----------------- */

#ifndef VIDEO_H
#define VIDEO_H

#include "rect.h"

#ifdef __TINYC__
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

void getvideo(RECT, VIDEOPTR);
void storevideo(RECT, VIDEOPTR);
extern unsigned video_mode;
extern unsigned video_page;
void wputch(WINDOW, int, int, int);
unsigned int GetVideoChar(int, int);
void PutVideoChar(int, int, int);
void get_videomode(void);
void wputs(WINDOW, void *, int, int);
void scroll_window(WINDOW, RECT, int);

#define clr(fg,bg) ((fg)|((bg)<<4))
#define vad(x,y) ((y)*(SCREENWIDTH*2)+(x)*2)
#define ismono() (video_mode == 7)
#define istext() (video_mode < 4)
#define videochar(x,y) (GetVideoChar(x,y) & 255)

#endif
