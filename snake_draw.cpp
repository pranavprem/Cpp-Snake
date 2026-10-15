#include "snake_draw.hpp"

#include "graphics.h"

#include <cstdio>

void shade(int x, int y) {
    const int color = paint_color();
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    rectangle(x * CELL + CELL / 10, y * CELL + CELL / 10, (x + 1) * CELL - CELL / 10,
              (y + 1) * CELL - CELL / 10);
    floodfill(x * CELL + CELL / 2, y * CELL + CELL / 2, color);
}

void draw_leaf(int cellX, int cellY, int color) {
    // Two quarter-circles, one from each corner, make the leaf.
    // The original README called this a vegan snake: it eats leaves.
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    arc(cellX * CELL, (cellY + 1) * CELL, 0, 90, CELL);
    arc(cellX * CELL + CELL, cellY * CELL, 180, 270, CELL);
    floodfill(cellX * CELL + CELL / 2, cellY * CELL + CELL / 2, color);
}

void draw_head(int cellX, int cellY, int dir, int color) {
    // The head is a circle centered on the trailing edge of its cell.
    // That offset is what makes the direction readable.
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    int cx = cellX * CELL + CELL / 2;
    int cy = cellY * CELL + CELL / 2;
    switch (dir) {
    case DIR_UP:
        cx = cellX * CELL + CELL / 2;
        cy = (cellY + 1) * CELL;
        break;
    case DIR_RIGHT:
        cx = cellX * CELL;
        cy = cellY * CELL + CELL / 2;
        break;
    case DIR_DOWN:
        cx = cellX * CELL + CELL / 2;
        cy = cellY * CELL;
        break;
    case DIR_LEFT:
        cx = (cellX + 1) * CELL;
        cy = cellY * CELL + CELL / 2;
        break;
    default:
        break;
    }
    circle(cx, cy, CELL / 2);
    // The geometric center sits on the window edge for a head in the last
    // row or column. Step inside so the fill still lands on a real pixel.
    int fx = cx;
    int fy = cy;
    if (fx < 0) {
        fx = 0;
    }
    if (fy < 0) {
        fy = 0;
    }
    if (fx >= CELL * GRID) {
        fx = CELL * GRID - 1;
    }
    if (fy >= CELL * GRID) {
        fy = CELL * GRID - 1;
    }
    floodfill(fx, fy, color);
}

void drawfruit(const Snake* fruit) {
    for (int i = 0; i < FRUIT_CAP && fruit[i].dir != DIR_NONE; ++i) {
        draw_leaf(fruit[i].x, fruit[i].y, paint_color());
    }
}

void drawsnake(const Snake* snake) {
    draw_head(snake[0].x, snake[0].y, snake[0].dir, paint_color());
    for (int i = 1; i < SNAKE_CAP && snake[i].x != -1; ++i) {
        shade(snake[i].x, snake[i].y);
    }
}

void draw_grid() {
    setcolor(DARKGRAY);
    const int extent = CELL * GRID;
    for (int i = 0; i < extent; i += CELL) {
        line(i, 0, i, extent);
        line(0, i, extent, i);
    }
}

void draw_label(int x, int y, const char* text) {
    setcolor(BLACK);
    outtextxy(x + 1, y + 1, text);
    setcolor(WHITE);
    outtextxy(x, y, text);
}

void draw_score(int score) {
    char text[32];
    std::snprintf(text, sizeof(text), "Score: %d", score);
    draw_label(CELL, CELL, text);
}

void draw_banner(const char* text) {
    const int width = textwidth(text);
    const int x = (CELL * GRID - width) / 2;
    const int y = (CELL * GRID - 7) / 2;
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, DARKGRAY);
    rectangle(x - 8, y - 6, x + width + 8, y + 13);
    floodfill(x, y, WHITE);
    draw_label(x, y, text);
}
