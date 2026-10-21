#include "graphics.h"

#include "conio.h"

#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <queue>
#include <string>
#include <vector>

namespace {

constexpr int kScale = 2;
constexpr double kPi = 3.14159265358979323846;

struct Rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Classic 16-color EGA palette used by graphics.h.
constexpr Rgb kPalette[16] = {
    {0x00, 0x00, 0x00}, {0x00, 0x00, 0xAA}, {0x00, 0xAA, 0x00}, {0x00, 0xAA, 0xAA},
    {0xAA, 0x00, 0x00}, {0xAA, 0x00, 0xAA}, {0xAA, 0x55, 0x00}, {0xAA, 0xAA, 0xAA},
    {0x55, 0x55, 0x55}, {0x55, 0x55, 0xFF}, {0x55, 0xFF, 0x55}, {0x55, 0xFF, 0xFF},
    {0xFF, 0x55, 0x55}, {0xFF, 0x55, 0xFF}, {0xFF, 0xFF, 0x55}, {0xFF, 0xFF, 0xFF},
};

// clang-format off
// 5x7 glyphs for ASCII 32..90. Bit 4 is the leftmost pixel. Lowercase is drawn as uppercase,
// matching the original BGI default font.
static const unsigned char kFont[59][7] = {
    { 0,  0,  0,  0,  0,  0,  0},  //
    { 0,  0,  0,  0,  0,  0,  0},  // !
    { 0,  0,  0,  0,  0,  0,  0},  // "
    { 0,  0,  0,  0,  0,  0,  0},  // #
    { 0,  0,  0,  0,  0,  0,  0},  // $
    { 0,  0,  0,  0,  0,  0,  0},  // %
    { 0,  0,  0,  0,  0,  0,  0},  // &
    { 0,  0,  0,  0,  0,  0,  0},  // '
    { 0,  0,  0,  0,  0,  0,  0},  // (
    { 0,  0,  0,  0,  0,  0,  0},  // )
    { 0,  0,  0,  0,  0,  0,  0},  // *
    { 0,  0,  0,  0,  0,  0,  0},  // +
    { 0,  0,  0,  0,  0,  0,  0},  // ,
    { 0,  0,  0,  0,  0,  0,  0},  // -
    { 0,  0,  0,  0,  0,  0,  0},  // .
    { 0,  0,  0,  0,  0,  0,  0},  // /
    {14, 17, 19, 21, 25, 17, 14},  // 0
    { 4, 12,  4,  4,  4,  4, 14},  // 1
    {14, 17,  1,  2,  4,  8, 31},  // 2
    {30,  1,  1, 14,  1,  1, 30},  // 3
    {18, 18, 18, 31,  2,  2,  2},  // 4
    {31, 16, 30,  1,  1, 17, 14},  // 5
    {14, 16, 16, 30, 17, 17, 14},  // 6
    {31,  1,  2,  4,  8,  8,  8},  // 7
    {14, 17, 17, 14, 17, 17, 14},  // 8
    {14, 17, 17, 15,  1,  1, 14},  // 9
    { 0,  4,  4,  0,  4,  4,  0},  // :
    { 0,  0,  0,  0,  0,  0,  0},  // ;
    { 0,  0,  0,  0,  0,  0,  0},  // <
    { 0,  0,  0,  0,  0,  0,  0},  // =
    { 0,  0,  0,  0,  0,  0,  0},  // >
    { 0,  0,  0,  0,  0,  0,  0},  // ?
    { 0,  0,  0,  0,  0,  0,  0},  // @
    {14, 17, 17, 31, 17, 17, 17},  // A
    {30, 17, 17, 30, 17, 17, 30},  // B
    {15, 16, 16, 16, 16, 16, 15},  // C
    {30, 17, 17, 17, 17, 17, 30},  // D
    {31, 16, 16, 30, 16, 16, 31},  // E
    {31, 16, 16, 30, 16, 16, 16},  // F
    {14, 17, 16, 23, 17, 17, 14},  // G
    {17, 17, 17, 31, 17, 17, 17},  // H
    {14,  4,  4,  4,  4,  4, 14},  // I
    { 7,  2,  2,  2, 18, 18, 12},  // J
    {17, 18, 20, 24, 20, 18, 17},  // K
    {16, 16, 16, 16, 16, 16, 31},  // L
    {17, 27, 21, 21, 17, 17, 17},  // M
    {17, 25, 21, 19, 17, 17, 17},  // N
    {14, 17, 17, 17, 17, 17, 14},  // O
    {30, 17, 17, 30, 16, 16, 16},  // P
    {14, 17, 17, 17, 21, 18, 13},  // Q
    {30, 17, 17, 30, 20, 18, 17},  // R
    {15, 16, 16, 14,  1,  1, 30},  // S
    {31,  4,  4,  4,  4,  4,  4},  // T
    {17, 17, 17, 17, 17, 17, 14},  // U
    {17, 17, 17, 17, 17, 10,  4},  // V
    {17, 17, 17, 21, 21, 27, 17},  // W
    {17, 17, 10,  4, 10, 17, 17},  // X
    {17, 17, 10,  4,  4,  4,  4},  // Y
    {31,  1,  2,  4,  8, 16, 31}   // Z
};
// clang-format on

SDL_Window* gWindow = nullptr;
SDL_Surface* gScaled = nullptr;
int gWidth = 0;
int gHeight = 0;
std::vector<uint8_t> gPixels;
int gColor = WHITE;
int gFill = WHITE;
bool gClosed = false;
std::queue<int> gKeys;
std::string gDumpDir;
int gFrame = 0;
Uint32 gMapped[16] = {};

const unsigned char* glyph_for(unsigned char ch) {
    static const unsigned char kBlank[7] = {0, 0, 0, 0, 0, 0, 0};
    if (ch >= 'a' && ch <= 'z') {
        ch = static_cast<unsigned char>(ch - 'a' + 'A');
    }
    if (ch < 32 || ch > 90) {
        return kBlank;
    }
    return kFont[ch - 32];
}

void pump() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gClosed = true;
        } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
            gClosed = true;
        } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
            int ch = 0;
            switch (event.key.keysym.sym) {
            case SDLK_w:
            case SDLK_UP:
                ch = 'w';
                break;
            case SDLK_a:
            case SDLK_LEFT:
                ch = 'a';
                break;
            case SDLK_s:
            case SDLK_DOWN:
                ch = 's';
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                ch = 'd';
                break;
            case SDLK_ESCAPE:
                ch = 27;
                break;
            default:
                break;
            }
            if (ch != 0) {
                gKeys.push(ch);
            }
        }
    }
}

void point_on_arc(int cx, int cy, int radius, int angle, int& x, int& y) {
    const double rad = angle * kPi / 180.0;
    x = cx + static_cast<int>(std::lround(radius * std::cos(rad)));
    y = cy - static_cast<int>(std::lround(radius * std::sin(rad)));
}

} // namespace

void putpixel(int x, int y, int color) {
    if (x < 0 || y < 0 || x >= gWidth || y >= gHeight) {
        return;
    }
    gPixels[static_cast<size_t>(y * gWidth + x)] = static_cast<uint8_t>(color & 15);
}

int getpixel(int x, int y) {
    if (x < 0 || y < 0 || x >= gWidth || y >= gHeight) {
        return BLACK;
    }
    return gPixels[static_cast<size_t>(y * gWidth + x)];
}

void setcolor(int color) {
    gColor = color & 15;
}

void setfillstyle(int pattern, int color) {
    (void)pattern;
    gFill = color & 15;
}

void line(int x0, int y0, int x1, int y1) {
    const int dx = std::abs(x1 - x0);
    const int dy = std::abs(y1 - y0);
    const int sx = x0 < x1 ? 1 : -1;
    const int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    while (true) {
        putpixel(x0, y0, gColor);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        const int e2 = 2 * err;
        // A diagonal step leaves a corner gap that 4-connected flood fill
        // walks through. Plotting both orthogonal neighbors closes it.
        if (e2 > -dy && e2 < dx) {
            putpixel(x0 + sx, y0, gColor);
            putpixel(x0, y0 + sy, gColor);
        }
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void rectangle(int x1, int y1, int x2, int y2) {
    line(x1, y1, x2, y1);
    line(x2, y1, x2, y2);
    line(x2, y2, x1, y2);
    line(x1, y2, x1, y1);
}

void arc(int x, int y, int startAngle, int endAngle, int radius) {
    if (radius < 0) {
        return;
    }
    if (endAngle < startAngle) {
        endAngle += 360;
    }
    int px = 0;
    int py = 0;
    point_on_arc(x, y, radius, startAngle, px, py);
    putpixel(px, py, gColor);
    for (int angle = startAngle + 1; angle <= endAngle; ++angle) {
        int nx = 0;
        int ny = 0;
        point_on_arc(x, y, radius, angle, nx, ny);
        line(px, py, nx, ny);
        px = nx;
        py = ny;
    }
}

void circle(int x, int y, int radius) {
    arc(x, y, 0, 360, radius);
}

void floodfill(int x, int y, int border) {
    if (x < 0 || y < 0 || x >= gWidth || y >= gHeight) {
        return;
    }
    border &= 15;
    const uint8_t wall = static_cast<uint8_t>(border);
    const uint8_t fill = static_cast<uint8_t>(gFill);
    const size_t start = static_cast<size_t>(y * gWidth + x);
    if (gPixels[start] == wall || gPixels[start] == fill) {
        return;
    }
    std::vector<int> stack;
    stack.push_back(static_cast<int>(start));
    while (!stack.empty()) {
        const int index = stack.back();
        stack.pop_back();
        if (gPixels[static_cast<size_t>(index)] == wall ||
            gPixels[static_cast<size_t>(index)] == fill) {
            continue;
        }
        gPixels[static_cast<size_t>(index)] = fill;
        const int px = index % gWidth;
        const int py = index / gWidth;
        if (px > 0) {
            stack.push_back(index - 1);
        }
        if (px + 1 < gWidth) {
            stack.push_back(index + 1);
        }
        if (py > 0) {
            stack.push_back(index - gWidth);
        }
        if (py + 1 < gHeight) {
            stack.push_back(index + gWidth);
        }
    }
}

void outtextxy(int x, int y, const char* text) {
    if (text == nullptr) {
        return;
    }
    int cursor = x;
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(text); *p != 0; ++p) {
        const unsigned char* glyph = glyph_for(*p);
        for (int row = 0; row < 7; ++row) {
            const unsigned char bits = glyph[row];
            for (int col = 0; col < 5; ++col) {
                if ((bits & (1u << (4 - col))) != 0) {
                    putpixel(cursor + col, y + row, gColor);
                }
            }
        }
        cursor += 6;
    }
}

int textwidth(const char* text) {
    if (text == nullptr || text[0] == '\0') {
        return 0;
    }
    return static_cast<int>(std::strlen(text)) * 6 - 1;
}

void cleardevice() {
    std::fill(gPixels.begin(), gPixels.end(), static_cast<uint8_t>(BLACK));
}

void refresh() {
    if (gScaled == nullptr || gWindow == nullptr) {
        return;
    }
    if (SDL_MUSTLOCK(gScaled)) {
        SDL_LockSurface(gScaled);
    }
    for (int y = 0; y < gHeight; ++y) {
        for (int x = 0; x < gWidth; ++x) {
            const uint8_t index = gPixels[static_cast<size_t>(y * gWidth + x)];
            const Uint32 pixel = gMapped[index];
            for (int dy = 0; dy < kScale; ++dy) {
                auto* row = reinterpret_cast<Uint32*>(static_cast<Uint8*>(gScaled->pixels) +
                                                      static_cast<size_t>(y * kScale + dy) *
                                                          static_cast<size_t>(gScaled->pitch));
                for (int dx = 0; dx < kScale; ++dx) {
                    row[x * kScale + dx] = pixel;
                }
            }
        }
    }
    if (SDL_MUSTLOCK(gScaled)) {
        SDL_UnlockSurface(gScaled);
    }
    SDL_Surface* screen = SDL_GetWindowSurface(gWindow);
    if (screen != nullptr) {
        SDL_BlitSurface(gScaled, nullptr, screen, nullptr);
        SDL_UpdateWindowSurface(gWindow);
    }
    if (!gDumpDir.empty()) {
        char name[32];
        std::snprintf(name, sizeof(name), "frame_%04d.bmp", gFrame++);
        const std::filesystem::path path = std::filesystem::path(gDumpDir) / name;
        if (SDL_SaveBMP(gScaled, path.string().c_str()) != 0) {
            std::fprintf(stderr, "Could not write %s: %s\n", path.string().c_str(), SDL_GetError());
        }
    }
}

void delay(int milliseconds) {
    if (milliseconds < 0) {
        milliseconds = 0;
    }
    refresh();
    const Uint32 start = SDL_GetTicks();
    while (!gClosed && SDL_GetTicks() - start < static_cast<Uint32>(milliseconds)) {
        pump();
        const Uint32 elapsed = SDL_GetTicks() - start;
        const Uint32 left = static_cast<Uint32>(milliseconds) - elapsed;
        SDL_Delay(left > 10 ? 10 : left);
    }
    pump();
}

void initwindow(int width, int height, const char* title) {
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        std::exit(1);
    }
    gWidth = width;
    gHeight = height;
    gPixels.assign(static_cast<size_t>(width * height), 0);
    gColor = WHITE;
    gFill = WHITE;
    gClosed = false;
    gWindow =
        SDL_CreateWindow(title != nullptr ? title : "Snake", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, width * kScale, height * kScale, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::fprintf(stderr,
                     "Could not open a window (%s).\n"
                     "A desktop session is required to play. Headless smoke test:\n"
                     "  SDL_VIDEODRIVER=dummy ./SnakeGame --smoke\n",
                     SDL_GetError());
        std::exit(1);
    }
    gScaled = SDL_CreateRGBSurfaceWithFormat(0, width * kScale, height * kScale, 32,
                                             SDL_PIXELFORMAT_RGB888);
    if (gScaled == nullptr) {
        std::fprintf(stderr, "Could not create the frame buffer: %s\n", SDL_GetError());
        std::exit(1);
    }
    for (int i = 0; i < 16; ++i) {
        gMapped[i] = SDL_MapRGB(gScaled->format, kPalette[i].r, kPalette[i].g, kPalette[i].b);
    }
    const char* dump = std::getenv("SNAKE_DUMP_FRAMES");
    if (dump != nullptr && dump[0] != '\0') {
        try {
            std::filesystem::create_directories(dump);
            gDumpDir = dump;
            gFrame = 0;
        } catch (const std::exception& ex) {
            std::fprintf(stderr, "Could not create frame directory %s: %s\n", dump, ex.what());
            gDumpDir.clear();
        }
    }
    SDL_RaiseWindow(gWindow);
    cleardevice();
}

void closegraph() {
    gDumpDir.clear();
    gPixels.clear();
    gKeys = std::queue<int>();
    if (gScaled != nullptr) {
        SDL_FreeSurface(gScaled);
        gScaled = nullptr;
    }
    if (gWindow != nullptr) {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }
    if (SDL_WasInit(SDL_INIT_VIDEO) != 0) {
        SDL_Quit();
    }
    gWidth = 0;
    gHeight = 0;
}

int kbhit() {
    pump();
    return (!gKeys.empty() || gClosed) ? 1 : 0;
}

int getch() {
    pump();
    while (gKeys.empty() && !gClosed) {
        SDL_Delay(10);
        pump();
    }
    if (gKeys.empty()) {
        return 0;
    }
    const int ch = gKeys.front();
    gKeys.pop();
    return ch;
}

void pushch(int ch) {
    if (ch != 0) {
        gKeys.push(ch);
    }
}

int windowclosed() {
    return gClosed ? 1 : 0;
}
