#pragma once

#include <array>
#include <span>
#include <vector>

#include <SDL3/SDL.h>


enum class Pattern {
   Glider,
   GosperGliderGun,
   Pulsar,
   Block,
   Beehive,
   Blinker,
   Toad,
   Loaf,
   Boat,
   Beacon,
   RPentomino,
   Acorn
};


class GameOfLife {
public:
   static constexpr int GRID_COLS{100};
   static constexpr int GRID_ROWS{100};
   static constexpr int GRID_SIZE{GRID_COLS * GRID_ROWS};
   static constexpr int CELL_SIZE{10};
   static constexpr int WINDOW_WIDTH{GRID_COLS * CELL_SIZE};
   static constexpr int WINDOW_HEIGHT{GRID_ROWS * CELL_SIZE};

   static constexpr int TARGET_FPS{30};
   static constexpr int FRAME_TIME{1000 / TARGET_FPS};

   struct Cell {
      bool alive;
   };

   GameOfLife();
   ~GameOfLife();

   bool init();
   void run();
   void cleanup();

   void spawn(Pattern pattern, int gridX, int gridY);

private:
   SDL_Window* window;
   SDL_Renderer* renderer;

   std::array<Cell, GRID_SIZE> frontGrid;
   std::array<Cell, GRID_SIZE> backGrid;

   bool running;

   uint64_t lastUpdateTime;

   void update();
   void render() const;
   void handleEvents();
   void updateGrid();
   void swapGrids();
   void waitRemainingFrameTime() const;
   [[nodiscard]] int countAliveNeighbors(int gridX, int gridY) const;

   static std::vector<std::pair<int, int>> getPatternCoords(Pattern pattern);
};
