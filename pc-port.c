/*
	Dirty 'backend' that makes the LC's CC65 source code compatible with
	Raylib. This does the bare minimum to to make the game work, and doesn't
	actually reimplement conio.h or joystick.h behaviours.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <stdint.h>
#include <stdarg.h>

#define JOY_1           0

#define JOY_UP_MASK     0x10
#define JOY_DOWN_MASK   0x20
#define JOY_LEFT_MASK   0x40
#define JOY_RIGHT_MASK  0x80
#define JOY_BTN_1_MASK  0x01
#define JOY_BTN_2_MASK  0x02
#define JOY_BTN_3_MASK  0x04
#define JOY_BTN_4_MASK  0x08

#define JOY_BTN_A_MASK  JOY_BTN_1_MASK
#define JOY_BTN_B_MASK  JOY_BTN_2_MASK
#define JOY_SELECT_MASK JOY_BTN_3_MASK
#define JOY_START_MASK  JOY_BTN_4_MASK

#define JOY_UP(v)       ((v) & JOY_UP_MASK)
#define JOY_DOWN(v)     ((v) & JOY_DOWN_MASK)
#define JOY_LEFT(v)     ((v) & JOY_LEFT_MASK)
#define JOY_RIGHT(v)    ((v) & JOY_RIGHT_MASK)
#define JOY_BTN_A(v)    ((v) & JOY_BTN_A_MASK)
#define JOY_BTN_B(v)    ((v) & JOY_BTN_B_MASK)
#define JOY_SELECT(v)   ((v) & JOY_SELECT_MASK)
#define JOY_START(v)    ((v) & JOY_START_MASK)

#define VIRTUAL_SCREEN_WIDTH  256
#define VIRTUAL_SCREEN_HEIGHT 224
#define SCREEN_SCALE    2

#define SCREEN_WIDTH    256*SCREEN_SCALE
#define SCREEN_HEIGHT   224*SCREEN_SCALE

void init();
uint8_t joy_read(uint8_t);
void clrscr();
void gotoxy(uint8_t, uint8_t);
void joy_uninstall();
void cputc(uint8_t);
void cputcxy(uint8_t, uint8_t, uint8_t);
void cputs(const char*);
void cputsxy(uint8_t, uint8_t, const char*);
void cprintf(const char*, ...);

static RenderTexture2D target;
static Rectangle       sourceRec;
static Rectangle       destRec;
static Texture         nescii;

static uint8_t cursorX, cursorY;
static uint8_t consoleBuffer[32*28];

void init(){
	clrscr();
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Loth's Castle");
	nescii = LoadTexture("nescii.png");
	target = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
	sourceRec = (Rectangle){
		0.0f,
		0.0f,
		(float)target.texture.width,
		-(float)target.texture.height
	};
	destRec = (Rectangle){
		0,
		0,
		SCREEN_WIDTH,
		SCREEN_HEIGHT
	};
	SetTargetFPS(60);
}

// I assume that this function will always be looping inside waitForInput(),
// so I use it to draw the screen as well.
uint8_t joy_read(uint8_t x){
	uint8_t r = 0;

	if(WindowShouldClose()) exit(0);

	BeginTextureMode(target);
	ClearBackground(BLACK);
	for(int i=0;i<(32*28);i++){
		DrawTextureRec(
			nescii,
			(Rectangle){
				(float)(consoleBuffer[i]%16)*8,
				(float)(consoleBuffer[i]>>4)*8,
				(float)(1+consoleBuffer[i]%16)*8,
				(float)(1+(consoleBuffer[i]>>4))*8
			},
			(Vector2){(float)((i%32)*8), (float)((i>>5)*8)},
			LIGHTGRAY
		);
	}
	EndTextureMode();

	BeginDrawing();
	ClearBackground(RED);
	DrawTexturePro(
		target.texture,
		sourceRec,
		destRec, (Vector2){0, 0},
		0,
		WHITE
	);
	EndDrawing();

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

inline void joy_uninstall(){}

void clrscr(){
	cursorX = cursorY = 0;
	memset(consoleBuffer, '\0', 32*28);
}

void adjustxy(){
	if(cursorY >= 28) cursorY %= 28;
	while(cursorX >= 32){
		cursorX -= 32;
		cursorY++;
		if(cursorY >= 28) cursorY %= 28;
	}
}

void gotoxy(uint8_t x, uint8_t y){
	cursorX = x;
	cursorY = y;
	adjustxy();
}

void cputc(uint8_t c){
	if(c == '\r') cursorX = 0;
	else if(c == '\n') cursorY++;
	else{
		consoleBuffer[cursorX + cursorY*32] = c;
		cursorX++;
	}
	adjustxy();
}

void cputcxy(uint8_t x, uint8_t y, uint8_t c){
	gotoxy(x, y);
	cputc(c);
}

void cputs(const char *str){
	for(int i=0;str[i];i++) cputc(str[i]);
}

void cputsxy(uint8_t x, uint8_t y, const char *str){
	gotoxy(x, y);
	cputs(str);
}

void cclearxy(uint8_t x, uint8_t y, uint8_t k){
	gotoxy(x, y);
	for(;k;k--) cputc('\0');
}

void cprintf(const char *str, ...){
	char tmp[768];
	va_list args;
	va_start(args, str);

	vsprintf(tmp, str, args);
	cputs(tmp);

	va_end(args);
}