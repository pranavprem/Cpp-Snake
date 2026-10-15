#pragma once

// Subset of Borland graphics.h / WinBGIm used by this game, drawn with SDL2.
//
// Angles match BGI: 0 degrees is 3 o'clock, and the arc runs counterclockwise
// with 90 degrees toward the top of the screen. Coordinates are pixels in a
// buffer of the size passed to initwindow(); the window itself is scaled up
// with nearest-neighbor so the chunky pixels stay visible.
//
// Set SNAKE_DUMP_FRAMES to a directory and every refresh() writes a BMP there.

constexpr int BLACK = 0;
constexpr int BLUE = 1;
constexpr int GREEN = 2;
constexpr int CYAN = 3;
constexpr int RED = 4;
constexpr int MAGENTA = 5;
constexpr int BROWN = 6;
constexpr int LIGHTGRAY = 7;
constexpr int DARKGRAY = 8;
constexpr int LIGHTBLUE = 9;
constexpr int LIGHTGREEN = 10;
constexpr int LIGHTCYAN = 11;
constexpr int LIGHTRED = 12;
constexpr int LIGHTMAGENTA = 13;
constexpr int YELLOW = 14;
constexpr int WHITE = 15;

constexpr int SOLID_FILL = 1;
constexpr int DETECT = 0;

void initwindow(int width, int height, const char* title);
void closegraph();
void cleardevice();
void refresh();
void delay(int milliseconds);

void setcolor(int color);
void setfillstyle(int pattern, int color);
int getpixel(int x, int y);
void putpixel(int x, int y, int color);

void line(int x1, int y1, int x2, int y2);
void rectangle(int x1, int y1, int x2, int y2);
void circle(int x, int y, int radius);
void arc(int x, int y, int startAngle, int endAngle, int radius);
void floodfill(int x, int y, int border);

void outtextxy(int x, int y, const char* text);
int textwidth(const char* text);
