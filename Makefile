# Build Pranav's Snake against SDL2. The graphics.h subset lives in compat/.

CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Icompat -I.
CLANG_FORMAT ?= clang-format

ifeq (,$(shell pkg-config --exists sdl2 && echo ok))
$(error SDL2 development files not found. On Debian/Ubuntu: sudo apt install g++ make pkg-config libsdl2-dev)
endif

SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS := $(shell pkg-config --libs sdl2)

BIN := SnakeGame
TEST_LOGIC := tests/test_logic
TEST_GFX := tests/test_graphics

LOGIC_SRC := snake_logic.cpp
DRAW_SRC := snake_draw.cpp
GFX_SRC := compat/graphics.cpp
GAME_SRC := Snake.cpp $(DRAW_SRC) $(LOGIC_SRC) $(GFX_SRC)

FORMAT_FILES := Snake.cpp snake_logic.cpp snake_logic.hpp snake_draw.cpp snake_draw.hpp \
	compat/graphics.cpp compat/graphics.h compat/conio.h \
	tests/test_logic.cpp tests/test_graphics.cpp

.PHONY: all test clean format check-format run demo

all: $(BIN)

$(BIN): $(GAME_SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) -o $@ $(GAME_SRC) $(SDL_LIBS)

$(TEST_LOGIC): tests/test_logic.cpp $(LOGIC_SRC) snake_logic.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ tests/test_logic.cpp $(LOGIC_SRC)

$(TEST_GFX): tests/test_graphics.cpp $(DRAW_SRC) $(LOGIC_SRC) $(GFX_SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) -o $@ tests/test_graphics.cpp $(DRAW_SRC) $(LOGIC_SRC) $(GFX_SRC) $(SDL_LIBS)

test: $(TEST_LOGIC) $(TEST_GFX) $(BIN)
	./$(TEST_LOGIC)
	SDL_VIDEODRIVER=dummy ./$(TEST_GFX)
	SDL_VIDEODRIVER=dummy ./$(BIN) --smoke
	SDL_VIDEODRIVER=dummy SNAKE_FAST=1 ./$(BIN) --demo --frames 5 --seed 1

run: $(BIN)
	./$(BIN)

demo: $(BIN)
	./$(BIN) --demo

format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)

check-format:
	$(CLANG_FORMAT) --dry-run --Werror $(FORMAT_FILES)

clean:
	rm -f $(BIN) $(TEST_LOGIC) $(TEST_GFX)
