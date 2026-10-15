#include "snake_logic.hpp"

#include <cstdlib>

void clear_segments(Snake* list, int cap) {
    for (int i = 0; i < cap; ++i) {
        list[i].x = -1;
        list[i].y = -1;
        list[i].dir = DIR_NONE;
    }
}

void initsnake(Snake* snake) {
    clear_segments(snake, SNAKE_CAP);
    // Three segments, heading right from the middle of the board.
    // The original set the tail in main() after initsnake(); same result.
    snake[0].x = 9;
    snake[0].y = 9;
    snake[0].dir = DIR_RIGHT;
    snake[1].x = 8;
    snake[1].y = 9;
    snake[1].dir = DIR_RIGHT;
    snake[2].x = 7;
    snake[2].y = 9;
    snake[2].dir = DIR_RIGHT;
}

int segment_count(const Snake* snake) {
    int i = 0;
    for (; i < SNAKE_CAP && snake[i].dir != DIR_NONE; ++i) {
    }
    return i;
}

void move_segment(Snake* segment) {
    switch (segment->dir) {
    case DIR_RIGHT:
        ++segment->x;
        break;
    case DIR_DOWN:
        ++segment->y;
        break;
    case DIR_LEFT:
        --segment->x;
        break;
    case DIR_UP:
        --segment->y;
        break;
    default:
        break;
    }
}

void addfruit(Snake* fruit) {
    int i = 0;
    for (; i < FRUIT_CAP && fruit[i].dir != DIR_NONE; ++i) {
    }
    // Keep a sentinel slot so the scan loops can stop.
    if (i + 1 >= FRUIT_CAP) {
        return;
    }
    // The board is 0 .. GRID-1. The old `(rand() % max) + 1` could land on
    // column `max`, which is already outside the wall.
    fruit[i].x = std::rand() % GRID;
    fruit[i].y = std::rand() % GRID;
    fruit[i].dir = FRUIT_HERE;
    fruit[i + 1].x = -1;
    fruit[i + 1].y = -1;
    fruit[i + 1].dir = DIR_NONE;
}

void addsnake(Snake* snake) {
    int i = 0;
    for (; i < SNAKE_CAP && snake[i].dir != DIR_NONE; ++i) {
    }
    if (i == 0 || i >= SNAKE_CAP) {
        return;
    }
    const int dir = snake[i - 1].dir;
    snake[i].dir = dir;
    snake[i].x = snake[i - 1].x;
    snake[i].y = snake[i - 1].y;
    // Grow one step behind the tail, opposite its direction of travel.
    switch (dir) {
    case DIR_UP:
        ++snake[i].y;
        break;
    case DIR_RIGHT:
        --snake[i].x;
        break;
    case DIR_DOWN:
        --snake[i].y;
        break;
    case DIR_LEFT:
        ++snake[i].x;
        break;
    default:
        break;
    }
    if (i + 1 < SNAKE_CAP) {
        snake[i + 1].x = -1;
        snake[i + 1].y = -1;
        snake[i + 1].dir = DIR_NONE;
    }
}

bool eatfruitcheck(Snake* snake, Snake* fruit) {
    for (int i = 0; i < FRUIT_CAP && fruit[i].dir != DIR_NONE; ++i) {
        // The original compared fruit[i].y with snake[i].y, so only fruit 0
        // was checked against the head. Every fruit is the head's meal.
        if (fruit[i].x == snake[0].x && fruit[i].y == snake[0].y) {
            for (int j = i; j + 1 < FRUIT_CAP && fruit[j].dir != DIR_NONE; ++j) {
                fruit[j] = fruit[j + 1];
            }
            addsnake(snake);
            return true;
        }
    }
    return false;
}

bool islose(const Snake* snake) {
    for (int i = 0; i < SNAKE_CAP && snake[i].dir != DIR_NONE; ++i) {
        if (snake[i].x < 0 || snake[i].y < 0 || snake[i].x > GRID - 1 || snake[i].y > GRID - 1) {
            return true;
        }
    }
    return false;
}

bool iswin(const Snake* snake) {
    return segment_count(snake) > WIN_LENGTH;
}

bool iseatself(const Snake* snake) {
    for (int i = 1; i < SNAKE_CAP && snake[i].dir != DIR_NONE; ++i) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return true;
        }
    }
    return false;
}

void apply_turn_cache(Snake* snake, Snake* cache) {
    // Each cached turn is a cell and a direction. A segment that is standing
    // on that cell adopts the direction. When the tail reaches it, the turn
    // has propagated through the whole snake and the entry is dropped.
    for (int i = 0; i < SNAKE_CAP && snake[i].x != -1; ++i) {
        int j = 0;
        while (j < CACHE_CAP && cache[j].x != -1) {
            if (snake[i].x != cache[j].x || snake[i].y != cache[j].y) {
                ++j;
                continue;
            }
            snake[i].dir = cache[j].dir;
            const bool tail = (i + 1 >= SNAKE_CAP) || snake[i + 1].x == -1;
            if (!tail) {
                ++j;
                continue;
            }
            int k = j;
            for (; k + 1 < CACHE_CAP && cache[k].x != -1; ++k) {
                cache[k] = cache[k + 1];
            }
            if (k < CACHE_CAP && cache[k].x != -1) {
                cache[k].x = -1;
                cache[k].y = -1;
                cache[k].dir = DIR_NONE;
            }
            // The next entry shifted into j. Check it before moving on.
        }
    }
}

bool queue_turn(Snake* snake, Snake* cache, int key) {
    int dir = 0;
    switch (key) {
    case 'w':
    case 'W':
        dir = DIR_UP;
        break;
    case 'a':
    case 'A':
        dir = DIR_LEFT;
        break;
    case 's':
    case 'S':
        dir = DIR_DOWN;
        break;
    case 'd':
    case 'D':
        dir = DIR_RIGHT;
        break;
    default:
        return false;
    }

    int latest = snake[0].dir;
    int i = 0;
    for (; i < CACHE_CAP && cache[i].x != -1; ++i) {
        latest = cache[i].dir;
    }
    // The original only compared against the head's current direction, so a
    // second key pressed before the snake moved could queue a reversal.
    if (opposite_dir(latest, dir)) {
        return false;
    }
    // Two keys on the same cell before the next move: keep the later one.
    if (i > 0 && cache[i - 1].x == snake[0].x && cache[i - 1].y == snake[0].y) {
        cache[i - 1].dir = dir;
        return true;
    }
    if (i + 1 >= CACHE_CAP) {
        return false;
    }
    cache[i].x = snake[0].x;
    cache[i].y = snake[0].y;
    cache[i].dir = dir;
    cache[i + 1].x = -1;
    cache[i + 1].y = -1;
    cache[i + 1].dir = DIR_NONE;
    return true;
}

int paint_color() {
    return paint_color_from(std::rand());
}
