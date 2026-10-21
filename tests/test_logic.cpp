#include "snake_logic.hpp"

#include <cstdio>
#include <cstdlib>

namespace {

int gFailures = 0;

void expect(bool cond, const char* expr, const char* file, int line) {
    if (!cond) {
        std::fprintf(stderr, "FAIL %s:%d: %s\n", file, line, expr);
        ++gFailures;
    }
}

#define EXPECT(cond) expect(static_cast<bool>(cond), #cond, __FILE__, __LINE__)

void move_all(Snake* snake) {
    for (int i = 0; i < SNAKE_CAP && snake[i].x != -1; ++i) {
        move_segment(&snake[i]);
    }
}

void test_start_and_motion() {
    Snake snake[SNAKE_CAP];
    initsnake(snake);
    EXPECT(segment_count(snake) == 3);
    EXPECT(snake[0].x == 9 && snake[0].y == 9 && snake[0].dir == DIR_RIGHT);
    EXPECT(snake[1].x == 8 && snake[2].x == 7);
    EXPECT(snake[3].dir == DIR_NONE);
    EXPECT(!islose(snake));
    EXPECT(!iseatself(snake));
    EXPECT(!iswin(snake));

    Snake step = snake[0];
    move_segment(&step);
    EXPECT(step.x == 10 && step.y == 9);
    step.dir = DIR_UP;
    move_segment(&step);
    EXPECT(step.x == 10 && step.y == 8);
    step.dir = DIR_LEFT;
    move_segment(&step);
    EXPECT(step.x == 9 && step.y == 8);
    step.dir = DIR_DOWN;
    move_segment(&step);
    EXPECT(step.x == 9 && step.y == 9);
}

void test_walls() {
    Snake snake[SNAKE_CAP];
    initsnake(snake);
    // The 2016 fix treats 0 and GRID-1 as inside, and GRID as a crash.
    snake[0].x = 0;
    snake[0].y = 0;
    snake[1].x = GRID - 1;
    snake[1].y = GRID - 1;
    EXPECT(!islose(snake));

    snake[2].x = GRID;
    EXPECT(islose(snake));
    snake[2].x = -1;
    EXPECT(islose(snake));
    snake[2].x = 7;
    snake[2].y = GRID;
    EXPECT(islose(snake));

    initsnake(snake);
    for (int step = 0; step < 5; ++step) {
        move_all(snake);
        EXPECT(!islose(snake));
    }
    move_all(snake);
    EXPECT(snake[0].x == GRID);
    EXPECT(islose(snake));
}

void test_self_and_win() {
    Snake snake[SNAKE_CAP];
    initsnake(snake);
    snake[2].x = snake[0].x;
    snake[2].y = snake[0].y;
    EXPECT(iseatself(snake));

    clear_segments(snake, SNAKE_CAP);
    for (int i = 0; i < WIN_LENGTH; ++i) {
        snake[i].x = i;
        snake[i].y = 0;
        snake[i].dir = DIR_RIGHT;
    }
    EXPECT(segment_count(snake) == WIN_LENGTH);
    EXPECT(!iswin(snake));
    snake[WIN_LENGTH].x = WIN_LENGTH;
    snake[WIN_LENGTH].y = 0;
    snake[WIN_LENGTH].dir = DIR_RIGHT;
    EXPECT(iswin(snake));
}

void test_fruit_spawn_and_eat() {
    std::srand(1);
    Snake fruit[FRUIT_CAP];
    clear_segments(fruit, FRUIT_CAP);
    for (int n = 0; n < 40; ++n) {
        clear_segments(fruit, FRUIT_CAP);
        addfruit(fruit);
        EXPECT(fruit[0].dir == FRUIT_HERE);
        EXPECT(fruit[0].x >= 0 && fruit[0].x < GRID);
        EXPECT(fruit[0].y >= 0 && fruit[0].y < GRID);
        EXPECT(fruit[1].dir == DIR_NONE);
    }

    clear_segments(fruit, FRUIT_CAP);
    for (int n = 0; n < FRUIT_CAP + 10; ++n) {
        addfruit(fruit);
    }
    int live = 0;
    for (int i = 0; i < FRUIT_CAP; ++i) {
        if (fruit[i].dir != DIR_NONE) {
            ++live;
            EXPECT(fruit[i].x >= 0 && fruit[i].x < GRID);
            EXPECT(fruit[i].y >= 0 && fruit[i].y < GRID);
        }
    }
    EXPECT(live == FRUIT_CAP - 1);
    EXPECT(fruit[FRUIT_CAP - 1].dir == DIR_NONE);

    Snake snake[SNAKE_CAP];
    initsnake(snake);
    clear_segments(fruit, FRUIT_CAP);
    fruit[0].x = 1;
    fruit[0].y = 1;
    fruit[0].dir = FRUIT_HERE;
    EXPECT(!eatfruitcheck(snake, fruit));
    EXPECT(segment_count(snake) == 3);

    fruit[0].x = 9;
    fruit[0].y = 9;
    EXPECT(eatfruitcheck(snake, fruit));
    EXPECT(segment_count(snake) == 4);
    EXPECT(snake[3].x == 6 && snake[3].y == 9 && snake[3].dir == DIR_RIGHT);
    EXPECT(fruit[0].dir == DIR_NONE);

    // Regression: fruit[1] used to be compared with snake[1], not the head.
    initsnake(snake);
    snake[1].y = 8;
    clear_segments(fruit, FRUIT_CAP);
    fruit[0].x = 1;
    fruit[0].y = 1;
    fruit[0].dir = FRUIT_HERE;
    fruit[1].x = snake[0].x;
    fruit[1].y = snake[0].y;
    fruit[1].dir = FRUIT_HERE;
    EXPECT(eatfruitcheck(snake, fruit));
    EXPECT(segment_count(snake) == 4);
    EXPECT(fruit[0].x == 1 && fruit[0].y == 1);
    EXPECT(fruit[1].dir == DIR_NONE);
}

void test_turn_cache() {
    Snake snake[SNAKE_CAP];
    Snake cache[CACHE_CAP];
    initsnake(snake);
    clear_segments(cache, CACHE_CAP);

    EXPECT(!queue_turn(snake, cache, 'a'));
    EXPECT(cache[0].x == -1);
    EXPECT(!queue_turn(snake, cache, 'x'));

    EXPECT(queue_turn(snake, cache, 'd'));
    EXPECT(cache[0].dir == DIR_RIGHT);
    EXPECT(queue_turn(snake, cache, 'w'));
    EXPECT(cache[0].x == 9 && cache[0].y == 9 && cache[0].dir == DIR_UP);
    EXPECT(cache[1].x == -1);

    clear_segments(cache, CACHE_CAP);
    EXPECT(queue_turn(snake, cache, 'w'));
    EXPECT(!queue_turn(snake, cache, 's'));
    EXPECT(cache[0].dir == DIR_UP);

    clear_segments(cache, CACHE_CAP);
    EXPECT(queue_turn(snake, cache, 'W'));
    EXPECT(cache[0].dir == DIR_UP);

    // Head (9,9) turns up. The turn reaches the tail two moves later.
    initsnake(snake);
    clear_segments(cache, CACHE_CAP);
    EXPECT(queue_turn(snake, cache, 'w'));
    apply_turn_cache(snake, cache);
    EXPECT(snake[0].dir == DIR_UP);
    EXPECT(cache[0].x == 9);
    move_all(snake);
    EXPECT(snake[0].x == 9 && snake[0].y == 8);
    EXPECT(snake[1].x == 9 && snake[1].y == 9);

    apply_turn_cache(snake, cache);
    EXPECT(snake[1].dir == DIR_UP);
    EXPECT(cache[0].x == 9);
    move_all(snake);
    EXPECT(snake[2].x == 9 && snake[2].y == 9);

    apply_turn_cache(snake, cache);
    EXPECT(snake[2].dir == DIR_UP);
    EXPECT(cache[0].x == -1);
}

void test_rules_helpers() {
    EXPECT(paint_color_from(0) == 1);
    EXPECT(paint_color_from(1) == 1);
    EXPECT(paint_color_from(14) == 14);
    EXPECT(paint_color_from(15) == 1);
    for (int roll = 0; roll < 30; ++roll) {
        const int color = paint_color_from(roll);
        EXPECT(color >= 1 && color <= 14);
    }
    EXPECT(should_spawn_fruit(7));
    EXPECT(should_spawn_fruit(16));
    EXPECT(!should_spawn_fruit(0));
    EXPECT(!should_spawn_fruit(8));

    EXPECT(frame_delay_ms(0) == 1000);
    EXPECT(frame_delay_ms(50) == 950);
    EXPECT(frame_delay_ms(400) == 600);
    EXPECT(frame_delay_ms(900) == 100);
    EXPECT(frame_delay_ms(901) == 100);
    EXPECT(opposite_dir(DIR_LEFT, DIR_RIGHT));
    EXPECT(opposite_dir(DIR_UP, DIR_DOWN));
    EXPECT(!opposite_dir(DIR_UP, DIR_LEFT));
}

} // namespace

int main() {
    test_start_and_motion();
    test_walls();
    test_self_and_win();
    test_fruit_spawn_and_eat();
    test_turn_cache();
    test_rules_helpers();
    if (gFailures != 0) {
        std::fprintf(stderr, "%d assertion(s) failed\n", gFailures);
        return 1;
    }
    std::printf("logic tests passed\n");
    return 0;
}
