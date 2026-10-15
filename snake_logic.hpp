#pragma once

// Rules for the snake practical.
//
// The 2013 source named the cell size `div` and the grid size `max`. Those
// identifiers are macros in the C library and on Windows, so the sizes live
// here as CELL and GRID.

constexpr int CELL = 20;
constexpr int GRID = 15;

constexpr int SNAKE_CAP = 50;
constexpr int FRUIT_CAP = 20;
constexpr int CACHE_CAP = 100;

// The original win check was `i > 10` after walking the segment list.
constexpr int WIN_LENGTH = 10;

constexpr int FRUIT_SCORE = 50;
constexpr int FAST_SCORE = 900;
constexpr int FAST_DELAY_MS = 100;
constexpr int BASE_DELAY_MS = 1000;

// Screen y grows downward, so DIR_DOWN increases y.
constexpr int DIR_RIGHT = 1;
constexpr int DIR_DOWN = 2;
constexpr int DIR_LEFT = 3;
constexpr int DIR_UP = 4;
constexpr int DIR_NONE = -1;

// A fruit that is still on the board. Empty slots use DIR_NONE.
constexpr int FRUIT_HERE = 0;

struct Snake {
    int x;
    int y;
    int dir;
};

inline bool opposite_dir(int a, int b) {
    return (a == DIR_LEFT && b == DIR_RIGHT) || (a == DIR_RIGHT && b == DIR_LEFT) ||
           (a == DIR_UP && b == DIR_DOWN) || (a == DIR_DOWN && b == DIR_UP);
}

// Original timing: delay(1000 - score), unless the score is past 900.
inline int frame_delay_ms(int score) {
    if (score > FAST_SCORE) {
        return FAST_DELAY_MS;
    }
    return BASE_DELAY_MS - score;
}

// Body and leaf colors are rand() % 15, skipping black (the background).
// The head used to skip blue instead, which is why it sometimes vanished.
inline int paint_color_from(int roll) {
    int color = roll % 15;
    if (color == 0) {
        ++color;
    }
    return color;
}

// Original spawn roll: one chance in nine (`rand() % 9 == 7`).
inline bool should_spawn_fruit(int roll) {
    return roll % 9 == 7;
}

void clear_segments(Snake* list, int cap);
void initsnake(Snake* snake);
int segment_count(const Snake* snake);
void move_segment(Snake* segment);
void addfruit(Snake* fruit);
void addsnake(Snake* snake);
bool eatfruitcheck(Snake* snake, Snake* fruit);
bool islose(const Snake* snake);
bool iswin(const Snake* snake);
bool iseatself(const Snake* snake);
void apply_turn_cache(Snake* snake, Snake* cache);
bool queue_turn(Snake* snake, Snake* cache, int key);
int paint_color();
