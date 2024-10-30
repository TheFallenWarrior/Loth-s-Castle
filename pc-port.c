/*
	Loth's Castle
	Copyright 2024 TheFallenWarrior
	
	Dirty frontend that makes the LC's cc65 source code compatible with
	raylib. This does the bare minimum to to make the game work and doesn't
	actually reimplement conio.h or joystick.h.
*/

#include <raylib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

RenderTexture2D renderTarget;
Rectangle       renderSourceRec;
Rectangle       renderDestRec;
Texture         nescii;
Image           icon;

uint16_t monitorWidth, monitorHeight;

uint8_t cursorX, cursorY;
uint8_t reversedText;
uint8_t consoleBuffer[CONSOLE_WIDTH*CONSOLE_HEIGHT];

// Initialize game window and raylib context
void init(){
	clrscr();
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Loth's Castle");
	nescii = LoadTexture("tileset.png");
	renderTarget = LoadRenderTexture(
		VIRTUAL_SCREEN_WIDTH,
		VIRTUAL_SCREEN_HEIGHT
	);
	renderSourceRec = (Rectangle){
		0,
		0,
		(float)renderTarget.texture.width,
		-(float)renderTarget.texture.height
	};
	renderDestRec = (Rectangle){
		0,
		0,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	};
	reversedText = 0;
	SetTargetFPS(60);
	
	#ifndef __EMSCRIPTEN__
		icon = LoadImage("icon.png");
		monitorWidth = GetMonitorWidth(GetCurrentMonitor());
		monitorHeight = GetMonitorHeight(GetCurrentMonitor());
		SetWindowIcon(icon);
		SetExitKey(KEY_F4);
	#endif
}

// Toggle fullscreen with forced integer scaling
void toggleFullscreenIntegerScale(){
	#ifndef __EMSCRIPTEN__
		if(IsWindowFullscreen()){
			ToggleFullscreen();
			renderDestRec = (Rectangle){
				0,
				0,
				WINDOW_WIDTH,
				WINDOW_HEIGHT
			};
			SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		} else{
			int x = monitorHeight/VIRTUAL_SCREEN_HEIGHT;
			renderDestRec = (Rectangle){
				0,
				0,
				VIRTUAL_SCREEN_WIDTH*x,
				VIRTUAL_SCREEN_HEIGHT*x
			};
			SetWindowSize(monitorWidth, monitorHeight);
			ToggleFullscreen();
		}
	#endif
}

// Render one frame of the conio's console
void renderScreen(){
	if(WindowShouldClose()){
		if(IsWindowFullscreen()) ToggleFullscreen();
		CloseWindow();
		exit(0);
	}
	if(IsKeyPressed(KEY_F11)){
		toggleFullscreenIntegerScale();
	}

	BeginTextureMode(renderTarget);
	ClearBackground(BLACK);
	for(int i=0;i<(CONSOLE_WIDTH*CONSOLE_HEIGHT);i++){
		DrawTextureRec(
			nescii,
			(Rectangle){
				(consoleBuffer[i]%16)*8,
				(int)(consoleBuffer[i]/16)*8,
				8,
				8
			},
			(Vector2){
				(i%CONSOLE_WIDTH)*8,
				(int)(i/CONSOLE_WIDTH)*8
			},
			LIGHTGRAY
		);
	}
	EndTextureMode();

	BeginDrawing();
	ClearBackground(BLACK);
	DrawTexturePro(
		renderTarget.texture,
		renderSourceRec,
		renderDestRec,
			(Vector2){
			(renderDestRec.width-GetRenderWidth())/2,
			(renderDestRec.height-GetRenderHeight())/2
		},
		0,
		WHITE
	);
	EndDrawing();
}

// Poll game input and return it in NES D-pad format
uint8_t joy_read(uint8_t r){
	r = 0;

	if(IsKeyDown(KEY_UP))    r |= JOY_UP_MASK;
	if(IsKeyDown(KEY_DOWN))  r |= JOY_DOWN_MASK;
	if(IsKeyDown(KEY_LEFT))  r |= JOY_LEFT_MASK;
	if(IsKeyDown(KEY_RIGHT)) r |= JOY_RIGHT_MASK;

	if(IsKeyDown(KEY_C))     r |= JOY_BTN_A_MASK;
	if(IsKeyDown(KEY_X))     r |= JOY_BTN_B_MASK;
	if(IsKeyDown(KEY_SPACE)) r |= JOY_SELECT_MASK;
	if(IsKeyDown(KEY_ENTER)) r |= JOY_START_MASK;

	return r;
}

// Clear screen and set cursor to the top-left
void clrscr(){
	cursorX = cursorY = 0;
	memset(consoleBuffer, ' ', CONSOLE_WIDTH*CONSOLE_HEIGHT);
}

// Make sure the cursor is within screen bounds
void adjustxy(){
	cursorY = (cursorY + cursorX/CONSOLE_WIDTH)%CONSOLE_HEIGHT;
	cursorX %= CONSOLE_WIDTH;
}

// Set cursor position to x, y
void gotoxy(uint8_t x, uint8_t y){
	cursorX = x;
	cursorY = y;
	adjustxy();
}

// Analogous to putc(c);
void cputc(uint8_t c){
	if(c == '\r') cursorX = 0;
	else if(c == '\n') cursorY++;
	else{
		if(reversedText) c ^= 0x80;
		consoleBuffer[cursorX + cursorY*CONSOLE_WIDTH] = c;
		cursorX++;
	}
	adjustxy();
}

// Equivalent to gotoxy(x, y); cputc(c);
void cputcxy(uint8_t x, uint8_t y, uint8_t c){
	gotoxy(x, y);
	cputc(c);
}

// Analogous to puts(str);
void cputs(const char *str){
	for(int i=0;str[i];i++) cputc(str[i]);
}

// Equivalent to gotoxy(x, y); cputs(str);
void cputsxy(uint8_t x, uint8_t y, const char *str){
	gotoxy(x, y);
	cputs(str);
}

// Write k spaces at x, y
void cclearxy(uint8_t x, uint8_t y, uint8_t k){
	gotoxy(x, y);
	for(;k;k--) cputc(' ');
}

// Draws a horizontal line of length k at x, y
void chlinexy(uint8_t x, uint8_t y, uint8_t k){
	gotoxy(x, y);
	for(;k;k--) cputc('\x0b');
}

// Draws a vertical line of length k at x, y
void cvlinexy(uint8_t x, uint8_t y, uint8_t k){
	gotoxy(x, y);
	for(int i=0;i<k;i++){
		cputcxy(x, y+i, '\x0e');
	}
}

// Analogous to vprintf(str, arg);
void vcprintf(const char *str, va_list arg){
	char tmp[CONSOLE_WIDTH*CONSOLE_HEIGHT];

	vsnprintf(tmp, CONSOLE_WIDTH*CONSOLE_HEIGHT, str, arg);
	cputs(tmp);
}

// Analogous to printf(str, ...);
void cprintf(const char *str, ...){
	va_list args;
	va_start(args, str);

	vcprintf(str, args);

	va_end(args);
}

// Enable/disable reverse character display
void revers(uint8_t r){
	reversedText = !!r;
}