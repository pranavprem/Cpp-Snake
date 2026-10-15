# AGENTS.md

Notes for changing this repository. The game is a 2013–2016 computer-graphics practical. Keep its rules and its picture. `README.md` is the explanation for humans. This file is how to work on the code.

## Layout

| File | Role |
| --- | --- |
| `Snake.cpp` | The loop, keyboard handling, and the `--demo` driver. |
| `snake_logic.hpp`, `snake_logic.cpp` | Board, movement, fruit, win/lose, the turn cache. No SDL. |
| `snake_draw.hpp`, `snake_draw.cpp` | Painting. This is the original picture: inset body squares, a head circle on the trailing edge, a leaf from two arcs. |
| `compat/graphics.h`, `compat/graphics.cpp` | The Borland / WinBGIm calls the game uses, implemented with SDL2. |
| `compat/conio.h` | `kbhit`, `getch`, `pushch`, `windowclosed`. Implemented in `graphics.cpp`. |
| `tests/test_logic.cpp` | Rules. No display. |
| `tests/test_graphics.cpp` | The drawing subset, under `SDL_VIDEODRIVER=dummy`. |

`SnakeGame.dev` is the original Dev-C++ project. Do not try to build it.

## Commands

```bash
make            # ./SnakeGame
make test       # logic + graphics + --smoke + a 5-frame demo
make run
make demo
make format     # clang-format -i, using .clang-format
make check-format
make clean
```

CI (`.github/workflows/ci.yml`) is Ubuntu 24.04, g++ in C++17 mode, `-Wall -Wextra -Wpedantic -Werror`, then `make test`. It installs `g++`, `make`, `pkg-config`, `libsdl2-dev`, and `clang-format`. The format check expects clang-format 18, which is what wrote the current sources.

Headless:

```bash
SDL_VIDEODRIVER=dummy ./SnakeGame --smoke
SDL_VIDEODRIVER=dummy SNAKE_FAST=1 ./SnakeGame --demo --frames 20 --seed 1
```

`--seed 1` is deterministic and, given enough frames, wins. A real launch calls `srand(time)` unless `--seed` is passed.

## Conventions

- C++17. Do not add a dependency for tests or for the font.
- The public rule functions keep the original names: `initsnake`, `addfruit`, `addsnake`, `eatfruitcheck`, `islose`, `iswin`, `iseatself`.
- The cell size and grid size were macros named `div` and `max`. They are `CELL` (20) and `GRID` (15). Do not bring the macros back.
- The original `move` is `move_segment`, so it does not collide with `std::move`. There is no `using namespace std`.
- Directions are `DIR_RIGHT=1`, `DIR_DOWN=2`, `DIR_LEFT=3`, `DIR_UP=4`. Y grows downward.
- Empty slots are `dir == DIR_NONE` (-1) and `x == -1`. Some loops stop on `dir`, some on `x`. Always set both, and `y`.
- A leaf that is on the board has `dir == FRUIT_HERE` (0).
- Format with the repo's `.clang-format` before finishing. Pointer stars stay left (`Snake* snake`).
- Do not add a license file. Do not commit `SnakeGame`, the test binaries, `*.o`, `*.exe`, or BMP dumps.

## Rules worth not rediscovering

- Win when `segment_count > WIN_LENGTH` (`WIN_LENGTH` is 10). Start length is 3, so the winning length is 11 and the winning score is 400.
- `frame_delay_ms` is the original formula: `1000 - score`, or `100` when `score > 900`.
- `should_spawn_fruit` is `roll % 9 == 7`.
- `paint_color_from` is `roll % 15`, and 0 becomes 1. Body, head, and leaf all use it. White (15) is reserved for text. Black is the background. The flicker is the original look, not a bug.
- Eating calls `addsnake`, which places the new segment one step behind the tail. If that cell is off the board, `islose` is true on the same frame. That is old behavior.
- Fruit coordinates are `rand() % GRID`, so they stay in `0 .. GRID-1`. They are allowed to land on the snake.
- `queue_turn` rejects the opposite of the latest direction, including a turn that is queued but not yet applied. A second key on the same cell before the next move replaces that entry instead of stacking two turns on one cell.
- `apply_turn_cache` copies the direction onto every segment currently on the cached cell, and deletes the entry only when the tail is the segment that reached it.

`rand` is global. Drawing consumes numbers before the spawn roll, because that is the order in `run_frame` (draw the snake, maybe spawn, draw the leaves). Do not reshuffle that if you care about `--seed` matching old traces.

## Graphics subset

`initwindow` opens an SDL window at 2× the logical size and scales with nearest neighbor. `getpixel` and every drawing call use logical pixels. `refresh` uploads the frame. `delay` refreshes, then pumps events while it waits.

Angles match BGI: 0 is 3 o'clock, counterclockwise, 90 is toward the top of the screen.

Flood fill is 4-connected and stops at the border color. Diagonal Bresenham steps also plot both side pixels, so a one-pixel corner cannot leak the fill. `tests/test_graphics.cpp` checks that a leaf's green pixels stay inside its cell. If you change `line` or `arc`, run that test and look at a dumped frame.

`outtextxy` draws a 5×7 font and maps `a-z` to `A-Z`. `textwidth` is `6 * characters - 1`.

`initwindow` exits the process if SDL cannot open a window. The error tells you to use `SDL_VIDEODRIVER=dummy` for the smoke test.

`SNAKE_DUMP_FRAMES` is read at `initwindow`. Frames are `frame_NNNN.bmp` in that directory, at the scaled 600×600 size.

On Windows the system has its own `conio.h`. The Makefile puts `-Icompat` first so this repo's header wins. Keep it first.

## Debug switches

| Switch | Effect |
| --- | --- |
| `SNAKE_FAST=1` | Delay capped at 40ms. Default timing is unchanged. |
| `SNAKE_GRID=1` | Gray grid, the debug drawing that was commented out in the loop. |
| `SNAKE_TRACE=1` | One stderr line per frame. |
| `SNAKE_DUMP_FRAMES` | BMP per `refresh`. |
| `--demo` | Wall-avoiding driver that also turns toward the nearest leaf when that turn is safe. It is not part of the original game. |

## Do not casually

- Replace the turn cache with a list of positions. The cache is the design.
- Smooth the flicker, add a menu, or change the win length or the delay formula as a drive-by.
- Reintroduce committed binaries. The 2016 `SnakeGame.exe` was a 32-bit Windows build and is only in git history.
- Add a `LICENSE` file. That choice is the owner's.
