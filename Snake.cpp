// Nokia-style Snake.
//
// Written by Pranav Prem as a computer-graphics practical at Goa College of
// Engineering (started 2013, last touched 2016). The original called Borland
// graphics.h through WinBGIm and Dev-C++. compat/graphics.h implements the
// calls this file needs, so the grid, the turn cache, and the leaf fruit
// are unchanged.
//
// The board is GRID by GRID cells of CELL pixels. Each snake segment stores
// the direction it is currently traveling. A keypress does not turn the
// whole snake at once: it records the head's cell and the new direction in
// a cache. Every segment that lands on that cell picks up the direction,
// and the entry is removed once the tail has passed it.

#include "conio.h"
#include "graphics.h"
#include "snake_draw.hpp"
#include "snake_logic.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

namespace {

enum class Step { Running, Finished, Closed };

bool gShowGrid = false;
bool gFast = false;
bool gTrace = false;

void print_usage(std::FILE* out) {
    std::fputs("Usage: ./SnakeGame [--demo] [--smoke] [--frames N] [--seed N]\n"
               "\n"
               "  --demo      let the snake play itself\n"
               "  --smoke     draw one frame and exit (headless CI)\n"
               "  --frames N  stop after N movement frames\n"
               "  --seed N    random seed (demo defaults to 1)\n"
               "\n"
               "Controls: W A S D or the arrow keys. Esc or the window close "
               "button quits.\n"
               "SNAKE_GRID=1 draws the debug grid. SNAKE_FAST=1 caps the frame "
               "delay at 40ms.\n"
               "SNAKE_DUMP_FRAMES=dir writes each presented frame as a BMP.\n",
               out);
}

int turn_left(int dir) {
    switch (dir) {
    case DIR_RIGHT:
        return DIR_UP;
    case DIR_UP:
        return DIR_LEFT;
    case DIR_LEFT:
        return DIR_DOWN;
    case DIR_DOWN:
        return DIR_RIGHT;
    default:
        return dir;
    }
}

int turn_right(int dir) {
    switch (dir) {
    case DIR_RIGHT:
        return DIR_DOWN;
    case DIR_DOWN:
        return DIR_LEFT;
    case DIR_LEFT:
        return DIR_UP;
    case DIR_UP:
        return DIR_RIGHT;
    default:
        return dir;
    }
}

int key_for(int dir) {
    switch (dir) {
    case DIR_UP:
        return 'w';
    case DIR_LEFT:
        return 'a';
    case DIR_DOWN:
        return 's';
    case DIR_RIGHT:
        return 'd';
    default:
        return 0;
    }
}

void cell_ahead(int x, int y, int dir, int& nx, int& ny) {
    nx = x;
    ny = y;
    switch (dir) {
    case DIR_RIGHT:
        ++nx;
        break;
    case DIR_LEFT:
        --nx;
        break;
    case DIR_DOWN:
        ++ny;
        break;
    case DIR_UP:
        --ny;
        break;
    default:
        break;
    }
}

bool blocked(const Snake* snake, int x, int y) {
    if (x < 0 || y < 0 || x >= GRID || y >= GRID) {
        return true;
    }
    // The tail moves out of its cell this frame, so it is not a wall.
    const int body = segment_count(snake) - 1;
    for (int i = 0; i < body; ++i) {
        if (snake[i].x == x && snake[i].y == y) {
            return true;
        }
    }
    return false;
}

int latest_dir(const Snake* snake, const Snake* cache) {
    int dir = snake[0].dir;
    for (int i = 0; i < CACHE_CAP && cache[i].x != -1; ++i) {
        dir = cache[i].dir;
    }
    return dir;
}

void drive_demo(Snake* snake, Snake* cache, const Snake* fruit) {
    const int dir = latest_dir(snake, cache);
    int nx = 0;
    int ny = 0;
    cell_ahead(snake[0].x, snake[0].y, dir, nx, ny);
    int chosen = dir;
    if (blocked(snake, nx, ny)) {
        const int left = turn_left(dir);
        const int right = turn_right(dir);
        int lx = 0;
        int ly = 0;
        int rx = 0;
        int ry = 0;
        cell_ahead(snake[0].x, snake[0].y, left, lx, ly);
        cell_ahead(snake[0].x, snake[0].y, right, rx, ry);
        if (!blocked(snake, lx, ly)) {
            chosen = left;
        } else if (!blocked(snake, rx, ry)) {
            chosen = right;
        } else {
            return;
        }
    } else {
        int best = -1;
        int bestDist = 0;
        for (int i = 0; i < FRUIT_CAP && fruit[i].dir != DIR_NONE; ++i) {
            const int dist = std::abs(fruit[i].x - snake[0].x) + std::abs(fruit[i].y - snake[0].y);
            if (best < 0 || dist < bestDist) {
                best = i;
                bestDist = dist;
            }
        }
        if (best >= 0) {
            const int fx = fruit[best].x;
            const int fy = fruit[best].y;
            int want = dir;
            if (std::abs(fx - snake[0].x) >= std::abs(fy - snake[0].y) && fx != snake[0].x) {
                want = fx > snake[0].x ? DIR_RIGHT : DIR_LEFT;
            } else if (fy != snake[0].y) {
                want = fy > snake[0].y ? DIR_DOWN : DIR_UP;
            }
            if (want != dir && !opposite_dir(dir, want)) {
                int wx = 0;
                int wy = 0;
                cell_ahead(snake[0].x, snake[0].y, want, wx, wy);
                if (!blocked(snake, wx, wy)) {
                    chosen = want;
                }
            }
        }
    }
    if (chosen != dir) {
        pushch(key_for(chosen));
    }
}

Step run_frame(Snake* snake, Snake* cache, Snake* fruit, int& score, bool demo,
               bool& resultOnScreen) {
    apply_turn_cache(snake, cache);
    cleardevice();
    if (gShowGrid) {
        draw_grid();
    }
    for (int i = 0; i < SNAKE_CAP && snake[i].x != -1; ++i) {
        move_segment(&snake[i]);
    }
    drawsnake(snake);
    if (should_spawn_fruit(std::rand())) {
        addfruit(fruit);
    }
    drawfruit(fruit);
    draw_score(score);
    if (eatfruitcheck(snake, fruit)) {
        score += FRUIT_SCORE;
    }
    if (gTrace) {
        int fruits = 0;
        for (int i = 0; i < FRUIT_CAP && fruit[i].dir != DIR_NONE; ++i) {
            ++fruits;
        }
        std::fprintf(stderr, "head=(%d,%d) dir=%d score=%d len=%d fruits=%d\n", snake[0].x,
                     snake[0].y, snake[0].dir, score, segment_count(snake), fruits);
    }
    if (islose(snake) || iseatself(snake)) {
        draw_banner("LOSER SNAKE");
        refresh();
        resultOnScreen = true;
        return Step::Finished;
    }
    if (iswin(snake)) {
        draw_banner("VICTORIOUS SNAKE");
        refresh();
        resultOnScreen = true;
        return Step::Finished;
    }
    if (demo) {
        drive_demo(snake, cache, fruit);
    }
    int wait = frame_delay_ms(score);
    if (gFast && wait > 40) {
        wait = 40;
    }
    delay(wait);
    if (windowclosed()) {
        return Step::Closed;
    }
    return Step::Running;
}

} // namespace

int main(int argc, char** argv) {
    bool smoke = false;
    bool demo = false;
    int maxFrames = -1;
    int seed = 1;
    bool seedSet = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            print_usage(stdout);
            return 0;
        }
        if (std::strcmp(argv[i], "--smoke") == 0) {
            smoke = true;
        } else if (std::strcmp(argv[i], "--demo") == 0) {
            demo = true;
        } else if (std::strcmp(argv[i], "--frames") == 0) {
            if (i + 1 >= argc) {
                print_usage(stderr);
                return 2;
            }
            maxFrames = std::atoi(argv[++i]);
        } else if (std::strcmp(argv[i], "--seed") == 0) {
            if (i + 1 >= argc) {
                print_usage(stderr);
                return 2;
            }
            seed = std::atoi(argv[++i]);
            seedSet = true;
        } else {
            std::fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(stderr);
            return 2;
        }
    }

    if (const char* grid = std::getenv("SNAKE_GRID")) {
        gShowGrid = grid[0] == '1';
    }
    if (const char* fast = std::getenv("SNAKE_FAST")) {
        gFast = fast[0] == '1';
    }
    if (const char* trace = std::getenv("SNAKE_TRACE")) {
        gTrace = trace[0] == '1';
    }

    // The 2016 binary never called srand(), so every run was the same game.
    if (demo && !seedSet) {
        seedSet = true;
        seed = 1;
    }
    if (seedSet) {
        std::srand(static_cast<unsigned>(seed));
    } else {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    Snake snake[SNAKE_CAP];
    Snake cache[CACHE_CAP];
    Snake fruit[FRUIT_CAP];
    initsnake(snake);
    clear_segments(cache, CACHE_CAP);
    clear_segments(fruit, FRUIT_CAP);

    initwindow(CELL * GRID, CELL * GRID, "Pranav's Snake Game");

    if (smoke) {
        cleardevice();
        drawsnake(snake);
        draw_score(0);
        refresh();
        closegraph();
        return 0;
    }

    int score = 0;
    int frames = 0;
    bool resultOnScreen = false;
    Step state = Step::Running;
    while (state == Step::Running) {
        while (state == Step::Running && !kbhit()) {
            state = run_frame(snake, cache, fruit, score, demo, resultOnScreen);
            if (state != Step::Running) {
                break;
            }
            ++frames;
            if (maxFrames > 0 && frames >= maxFrames) {
                state = Step::Finished;
            }
        }
        if (state != Step::Running || windowclosed()) {
            break;
        }
        const int key = getch();
        if (key == 27 || windowclosed()) {
            state = Step::Closed;
            break;
        }
        queue_turn(snake, cache, key);
    }

    if (resultOnScreen && demo && !windowclosed()) {
        int wait = 800;
        if (gFast) {
            wait = 40;
        }
        delay(wait);
    }
    if (resultOnScreen && !demo && !windowclosed()) {
        getch();
    }
    closegraph();
    return 0;
}
