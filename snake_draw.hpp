#pragma once

#include "snake_logic.hpp"

// Drawing uses the graphics.h subset in compat/. The leaf, the trailing-edge
// head, and the inset body squares are the original picture.

void shade(int x, int y);
void draw_leaf(int cellX, int cellY, int color);
void draw_head(int cellX, int cellY, int dir, int color);
void drawfruit(const Snake* fruit);
void drawsnake(const Snake* snake);
void draw_grid();
void draw_label(int x, int y, const char* text);
void draw_score(int score);
void draw_banner(const char* text);
