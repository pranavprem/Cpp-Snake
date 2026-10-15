#pragma once

// Keyboard helpers the original program took from conio.h (kbhit/getch).
// They read the SDL window opened by initwindow(), not the terminal.
// Arrow keys arrive as the matching WASD letter. Esc is 27.

int kbhit();
int getch();
void pushch(int ch);
int windowclosed();
