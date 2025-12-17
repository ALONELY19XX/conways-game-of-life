#include <print>
#include <utility>
#include <span>
#include <vector>

#include "GameOfLife.h"


GameOfLife::GameOfLife() :
   window{nullptr},
   renderer{nullptr},
   frontGrid{},
   backGrid{},
   running{false},
   lastUpdateTime{0} {}


GameOfLife::~GameOfLife() {
   cleanup();
}


/**
 * Initialization of Video Subsystem, as well as Window and Renderer.
 * Return value indicates success/failure of initialization.
 */
bool GameOfLife::init() {
   if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::println(stderr, "[ERROR] Failed to initialize SDL: {}", SDL_GetError());
      return false;
   }

   if (!SDL_CreateWindowAndRenderer(
      "Conway's Game of Life",
      WINDOW_WIDTH, WINDOW_HEIGHT,
      0,
      &window, &renderer
   )) {
      std::println(stderr, "[ERROR] Failed to initialize SDL Window and Renderer: {}", SDL_GetError());
      return false;
   }

   running = true;
   lastUpdateTime = SDL_GetTicks();

   return true;
}


/**
 * Game loop
 */
void GameOfLife::run() {
   while (running) {
      handleEvents();
      update();
      render();

      waitRemainingFrameTime();
   }
}


/**
 * Event polling and handling
 */
void GameOfLife::handleEvents() {
   SDL_Event event;
   while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
         running = false;
         break;
      }
   }
}


/**
 * Update game state in roughly `FRAME_TIME` millisecond steps
 */
void GameOfLife::update() {
   const uint64_t currentTime = SDL_GetTicks();
   const uint64_t deltaTime = currentTime - lastUpdateTime;

   if (deltaTime >= FRAME_TIME) {
      updateGrid();
      lastUpdateTime = currentTime;
   }
}


/**
 * Render current generation/state of the game
 */
void GameOfLife::render() const {
   // Black background
   SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
   SDL_RenderClear(renderer);

   // Living cells will be drawn as white squares
   SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

   for (int y = 0; y < GRID_ROWS; ++y) {
      for (int x = 0; x < GRID_COLS; ++x) {
         if (frontGrid[x + y * GRID_COLS].alive) {
            auto rect = SDL_FRect{
               .x = static_cast<float>(x * CELL_SIZE),
               .y = static_cast<float>(y * CELL_SIZE),
               .w = static_cast<float>(CELL_SIZE - 1),
               .h = static_cast<float>(CELL_SIZE - 1)
            };
            SDL_RenderFillRect(renderer, &rect);
         }
      }
   }

   SDL_RenderPresent(renderer);
}


/**
 * To ensure fixed/stable update frequency, we will wait at the end of each tick
 * in case the current iteration finished faster than the target frame time (likely).
 */
void GameOfLife::waitRemainingFrameTime() const {
   const uint64_t currentTime = SDL_GetTicks();
   const uint64_t deltaTime = currentTime - lastUpdateTime;

   if (deltaTime < FRAME_TIME) {
      SDL_Delay(FRAME_TIME - deltaTime);
   }
}


/**
 * Cleanup SDL3-related resources
 */
void GameOfLife::cleanup() {
   if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderer = nullptr;
   }

   if (window) {
      SDL_DestroyWindow(window);
      window = nullptr;
   }

   SDL_Quit();
}

/**
 * Swap front and back grid
 */
void GameOfLife::swapGrids() {
   std::swap(frontGrid, backGrid);
}


/**
 * Count living neighbors by checking all surrounding cells
 */
int GameOfLife::countAliveNeighbors(int gridX, int gridY) const {
   int count = 0;

   for (int y = -1; y <= 1; ++y) {
      for (int x = -1; x <= 1; ++x) {
         // Skip the cell itself
         if (x == 0 && y == 0) {
            continue;
         }

         int tx = gridX + x;
         int ty = gridY + y;

         // Wrap around the grid horizontally (wrap left to right)
         if (tx < 0) {
            tx = GRID_COLS - 1;
         }

         // Wrap around the grid horizontally (wrap right to left)
         if (tx >= GRID_COLS) {
            tx = 0;
         }

         // Wrap around the grid vertically (wrap top to bottom)
         if (ty < 0) {
            ty = GRID_ROWS - 1;
         }

         // Wrap around the grid vertically (wrap bottom to top)
         if (ty >= GRID_ROWS) {
            ty = 0;
         }

         if (frontGrid[tx + ty * GRID_COLS].alive) {
            count++;
         }
      }
   }

   return count;
}


/**
 * Calculate the next generation of game state by applying Game of Life ruleset to each cell individually
 */
void GameOfLife::updateGrid() {
   for (int y = 0; y < GRID_ROWS; ++y) {
      for (int x = 0; x < GRID_COLS; ++x) {
         const int neighbors = countAliveNeighbors(x, y);
         const bool alive = frontGrid[x + y * GRID_COLS].alive;

         if (alive) {
            // Live cell survives with 2 or 3 neighbors
            backGrid[x + y * GRID_COLS].alive = (neighbors == 2 || neighbors == 3);
         }
         else {
            // Dead cell becomes alive with exactly 3 neighbors
            backGrid[x + y * GRID_COLS].alive = (neighbors == 3);
         }
      }
   }

   swapGrids();
}


/**
 * Get pattern coordinates/points for a given pattern
 */
std::vector<std::pair<int, int>> GameOfLife::getPatternCoords(Pattern pattern) {
   switch (pattern) {
   case Pattern::Glider:
      return {{1, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2}};
   case Pattern::GosperGliderGun:
      return {
         {24, 0}, {22, 1}, {24, 1}, {12, 2}, {13, 2}, {20, 2},
         {21, 2}, {34, 2}, {35, 2}, {11, 3}, {15, 3}, {20, 3},
         {21, 3}, {34, 3}, {35, 3}, {0, 4}, {1, 4}, {10, 4},
         {16, 4}, {20, 4}, {21, 4}, {0, 5}, {1, 5}, {10, 5},
         {14, 5}, {16, 5}, {17, 5}, {22, 5}, {24, 5}, {10, 6},
         {16, 6}, {24, 6}, {11, 7}, {15, 7}, {12, 8}, {13, 8}
      };
   case Pattern::Pulsar:
      return {
         {2, 0}, {3, 0}, {4, 0}, {8, 0}, {9, 0}, {10, 0},
         {0, 2}, {5, 2}, {7, 2}, {12, 2}, {0, 3}, {5, 3},
         {7, 3}, {12, 3}, {0, 4}, {5, 4}, {7, 4}, {12, 4},
         {2, 5}, {3, 5}, {4, 5}, {8, 5}, {9, 5}, {10, 5},
         {2, 7}, {3, 7}, {4, 7}, {8, 7}, {9, 7}, {10, 7},
         {0, 8}, {5, 8}, {7, 8}, {12, 8}, {0, 9}, {5, 9},
         {7, 9}, {12, 9}, {0, 10}, {5, 10}, {7, 10}, {12, 10},
         {2, 12}, {3, 12}, {4, 12}, {8, 12}, {9, 12}, {10, 12}
      };
   case Pattern::Block:
      return {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
   case Pattern::Beehive:
      return {{1, 0}, {2, 0}, {0, 1}, {3, 1}, {1, 2}, {2, 2}};
   case Pattern::Blinker:
      return {{0, 0}, {1, 0}, {2, 0}};
   case Pattern::Toad:
      return {{1, 0}, {2, 0}, {3, 0}, {0, 1}, {1, 1}, {2, 1}};
   case Pattern::Loaf:
      return {{1, 0}, {2, 0}, {0, 1}, {3, 1}, {1, 2}, {3, 2}, {2, 3}};
   case Pattern::Boat:
      return {{0, 0}, {1, 0}, {0, 1}, {2, 1}, {1, 2}};
   case Pattern::Beacon:
      return {{0, 0}, {1, 0}, {0, 1}, {3, 2}, {2, 3}, {3, 3}};
   case Pattern::RPentomino:
      return {{1, 0}, {2, 0}, {0, 1}, {1, 1}, {1, 2}};
   case Pattern::Acorn:
      return {{1, 0}, {3, 1}, {0, 2}, {1, 2}, {4, 2}, {5, 2}, {6, 2}};
   }

   std::unreachable();
}


/**
 * Spawn a given pattern on a specific position on the grid
 */
void GameOfLife::spawn(Pattern pattern, int gridX, int gridY) {
   auto coords = getPatternCoords(pattern);

   for (auto [x, y] : coords) {
      const int tx = gridX + x;
      const int ty = gridY + y;
      if (tx >= 0 && tx < GRID_COLS && ty >= 0 && ty < GRID_ROWS) {
         frontGrid[tx + ty * GRID_COLS].alive = true;
      }
   }
}
