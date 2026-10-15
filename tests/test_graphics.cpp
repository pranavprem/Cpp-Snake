#include "graphics.h"
#include "snake_draw.hpp"

#include <cstdio>

namespace {

int gFailures = 0;

void expect(bool cond, const char* expr, const char* file, int line) {
    if (!cond) {
        std::fprintf(stderr, "FAIL %s:%d: %s\n", file, line, expr);
        ++gFailures;
    }
}

#define EXPECT(cond) expect(static_cast<bool>(cond), #cond, __FILE__, __LINE__)

constexpr int kSize = CELL * GRID;

int count_color(int color) {
    int n = 0;
    for (int y = 0; y < kSize; ++y) {
        for (int x = 0; x < kSize; ++x) {
            if (getpixel(x, y) == color) {
                ++n;
            }
        }
    }
    return n;
}

} // namespace

int main() {
    initwindow(kSize, kSize, "Snake graphics test");
    cleardevice();
    EXPECT(getpixel(0, 0) == BLACK);

    setcolor(YELLOW);
    line(5, 5, 20, 5);
    EXPECT(getpixel(5, 5) == YELLOW);
    EXPECT(getpixel(12, 5) == YELLOW);
    EXPECT(getpixel(20, 5) == YELLOW);
    EXPECT(getpixel(12, 6) == BLACK);
    line(0, 0, 10, 10);
    EXPECT(getpixel(0, 0) == YELLOW);
    EXPECT(getpixel(10, 10) == YELLOW);

    cleardevice();
    setcolor(RED);
    setfillstyle(SOLID_FILL, RED);
    rectangle(10, 10, 40, 30);
    floodfill(20, 20, RED);
    EXPECT(getpixel(20, 20) == RED);
    EXPECT(getpixel(10, 10) == RED);
    EXPECT(getpixel(9, 20) == BLACK);
    EXPECT(getpixel(41, 20) == BLACK);
    EXPECT(getpixel(0, 0) == BLACK);

    cleardevice();
    setcolor(BLUE);
    setfillstyle(SOLID_FILL, BLUE);
    circle(150, 150, 20);
    floodfill(150, 150, BLUE);
    const int blue = count_color(BLUE);
    std::printf("circle blue pixels: %d\n", blue);
    EXPECT(getpixel(150, 150) == BLUE);
    EXPECT(getpixel(0, 0) == BLACK);
    EXPECT(blue > 100 && blue < 2000);

    cleardevice();
    draw_leaf(5, 5, GREEN);
    int inside = 0;
    int outside = 0;
    for (int y = 0; y < kSize; ++y) {
        for (int x = 0; x < kSize; ++x) {
            if (getpixel(x, y) != GREEN) {
                continue;
            }
            if (x < 5 * CELL || x > 6 * CELL || y < 5 * CELL || y > 6 * CELL) {
                ++outside;
            } else {
                ++inside;
            }
        }
    }
    std::printf("leaf inside=%d outside=%d\n", inside, outside);
    EXPECT(inside > 20);
    EXPECT(outside < 40);
    EXPECT(getpixel(0, 0) == BLACK);

    cleardevice();
    draw_head(9, 9, DIR_RIGHT, LIGHTRED);
    const int head = count_color(LIGHTRED);
    std::printf("head pixels: %d\n", head);
    EXPECT(getpixel(9 * CELL, 9 * CELL + CELL / 2) == LIGHTRED);
    EXPECT(head > 50 && head < 800);
    EXPECT(getpixel(0, 0) == BLACK);

    cleardevice();
    draw_head(GRID - 1, GRID - 1, DIR_UP, LIGHTGREEN);
    EXPECT(count_color(LIGHTGREEN) > 20);
    EXPECT(getpixel(0, 0) == BLACK);

    cleardevice();
    setcolor(WHITE);
    outtextxy(10, 10, "Hi");
    EXPECT(getpixel(10, 10) == WHITE);
    EXPECT(textwidth("HI") == 11);
    EXPECT(textwidth("") == 0);
    EXPECT(textwidth(nullptr) == 0);
    outtextxy(10, 30, nullptr);

    cleardevice();
    draw_banner("LOSER SNAKE");
    const int plaque = count_color(DARKGRAY);
    const int ink = count_color(WHITE);
    std::printf("banner gray=%d white=%d\n", plaque, ink);
    EXPECT(getpixel(0, 0) == BLACK);
    EXPECT(plaque > 50 && plaque < 5000);
    EXPECT(ink > 20);

    cleardevice();
    draw_score(400);
    EXPECT(count_color(WHITE) > 10);

    delay(20);
    refresh();
    closegraph();

    if (gFailures != 0) {
        std::fprintf(stderr, "%d assertion(s) failed\n", gFailures);
        return 1;
    }
    std::printf("graphics tests passed\n");
    return 0;
}
