/*
	Loth's Castle
	Copyright 2025 TheFallenWarrior
*/

#ifndef LC_PC_PORT_H
#define LC_PC_PORT_H

#include <stdint.h>
#include <stdarg.h>

#define JOY_1           0

#define JOY_UP_MASK     0x10
#define JOY_DOWN_MASK   0x20
#define JOY_LEFT_MASK   0x40
#define JOY_RIGHT_MASK  0x80
#define JOY_BTN_A_MASK  0x01
#define JOY_BTN_B_MASK  0x02
#define JOY_SELECT_MASK 0x04
#define JOY_START_MASK  0x08

#define JOY_UP(v)       ((v) & JOY_UP_MASK)
#define JOY_DOWN(v)     ((v) & JOY_DOWN_MASK)
#define JOY_LEFT(v)     ((v) & JOY_LEFT_MASK)
#define JOY_RIGHT(v)    ((v) & JOY_RIGHT_MASK)
#define JOY_BTN_A(v)    ((v) & JOY_BTN_A_MASK)
#define JOY_BTN_B(v)    ((v) & JOY_BTN_B_MASK)
#define JOY_SELECT(v)   ((v) & JOY_SELECT_MASK)
#define JOY_START(v)    ((v) & JOY_START_MASK)

#define CONSOLE_WIDTH   32
#define CONSOLE_HEIGHT  28

#define VIRTUAL_SCREEN_WIDTH  (CONSOLE_WIDTH *8)
#define VIRTUAL_SCREEN_HEIGHT (CONSOLE_HEIGHT*8)
#define WINDOW_SCALE    2

#define WINDOW_WIDTH    (VIRTUAL_SCREEN_WIDTH *WINDOW_SCALE)
#define WINDOW_HEIGHT   (VIRTUAL_SCREEN_HEIGHT*WINDOW_SCALE)

void init();
void renderScreen();
uint8_t joy_read(uint8_t);
void clrscr();
void gotoxy(uint8_t, uint8_t);
void cputc(uint8_t);
void cputcxy(uint8_t, uint8_t, uint8_t);
void cputs(const char*);
void cputsxy(uint8_t, uint8_t, const char*);
void cclearxy(uint8_t, uint8_t, uint8_t);
void chlinexy(uint8_t, uint8_t, uint8_t);
void cvlinexy(uint8_t, uint8_t, uint8_t);
void vcprintf(const char*, va_list);
void cprintf(const char*, ...);
void revers(uint8_t);

#endif