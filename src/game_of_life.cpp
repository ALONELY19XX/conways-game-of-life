#include <print>
#include <utility>
#include <span>

#include "game_of_life.h"


GameOfLife::GameOfLife() :
   window{nullptr}, renderer{nullptr},
   front_grid{}, back_grid{},
   is_running{false},
   last_update_time{0} {
   front_grid.fill({.is_alive = false});
   back_grid.fill({.is_alive = false});
}


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

   is_running = true;
   last_update_time = SDL_GetTicks();

   return true;
}


/**
 * Game loop
 */
void GameOfLife::run() {
   while (is_running) {
      handle_events();
      update();
      render();

      wait_remaining_frame_time();
   }
}


/**
 * Event polling and handling
 */
void GameOfLife::handle_events() {
   SDL_Event event;
   while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
         is_running = false;
         break;
      }
   }
}


/**
 * Update game state in roughly `FRAME_TIME` millisecond steps
 */
void GameOfLife::update() {
   const uint64_t current_time = SDL_GetTicks();
   const uint64_t delta_time = current_time - last_update_time;

   if (delta_time >= FRAME_TIME) {
      update_grid();
      last_update_time = current_time;
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
         if (front_grid[x + y * GRID_COLS].is_alive) {
            SDL_FRect rect{
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
void GameOfLife::wait_remaining_frame_time() const {
   const uint64_t current_time{SDL_GetTicks()};
   const uint64_t delta_time{current_time - last_update_time};

   if (delta_time < FRAME_TIME) {
      SDL_Delay(FRAME_TIME - delta_time);
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
void GameOfLife::swap_grids() {
   std::swap(front_grid, back_grid);
}


/**
 * Count living neighbors by checking all surrounding cells
 */
int GameOfLife::count_alive_neighbors(int gridX, int gridY) const {
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

         if (front_grid[tx + ty * GRID_COLS].is_alive) {
            count++;
         }
      }
   }

   return count;
}


/**
 * Calculate the next generation of game state by applying Game of Life ruleset to each cell individually
 */
void GameOfLife::update_grid() {
   for (int y = 0; y < GRID_ROWS; ++y) {
      for (int x = 0; x < GRID_COLS; ++x) {
         const int neighbors = count_alive_neighbors(x, y);
         const bool is_alive = front_grid[x + y * GRID_COLS].is_alive;

         if (is_alive) {
            // Live cell survives with 2 or 3 neighbors
            back_grid[x + y * GRID_COLS].is_alive = (neighbors == 2 || neighbors == 3);
         }
         else {
            // Dead cell becomes alive with exactly 3 neighbors
            back_grid[x + y * GRID_COLS].is_alive = (neighbors == 3);
         }
      }
   }

   swap_grids();
}


/**
 * Spawn a given pattern on a specific position on the grid
 */
void GameOfLife::spawn(std::span<const std::pair<int, int>> pattern, int gridX, int gridY) {
   for (auto [x, y] : pattern) {
      const int tx = gridX + x;
      const int ty = gridY + y;
      if (tx >= 0 && tx < GRID_COLS && ty >= 0 && ty < GRID_ROWS) {
         front_grid[tx + ty * GRID_COLS].is_alive = true;
      }
   }
}


/**
 * Spawn Glider pattern into the grid
 */
void GameOfLife::spawn_glider(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 5> pattern{
      {{1, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Gosper Glider Gun pattern into the grid
 */
void GameOfLife::spawn_gosper_glider_gun(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 36> pattern{
      {
         {24, 0}, {22, 1}, {24, 1}, {12, 2}, {13, 2}, {20, 2},
         {21, 2}, {34, 2}, {35, 2}, {11, 3}, {15, 3}, {20, 3},
         {21, 3}, {34, 3}, {35, 3}, {0, 4}, {1, 4}, {10, 4},
         {16, 4}, {20, 4}, {21, 4}, {0, 5}, {1, 5}, {10, 5},
         {14, 5}, {16, 5}, {17, 5}, {22, 5}, {24, 5}, {10, 6},
         {16, 6}, {24, 6}, {11, 7}, {15, 7}, {12, 8}, {13, 8}
      }
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Pulsar pattern into the grid
 */
void GameOfLife::spawn_pulsar(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 48> pattern{
      {
         {2, 0}, {3, 0}, {4, 0}, {8, 0}, {9, 0}, {10, 0},
         {0, 2}, {5, 2}, {7, 2}, {12, 2}, {0, 3}, {5, 3},
         {7, 3}, {12, 3}, {0, 4}, {5, 4}, {7, 4}, {12, 4},
         {2, 5}, {3, 5}, {4, 5}, {8, 5}, {9, 5}, {10, 5},
         {2, 7}, {3, 7}, {4, 7}, {8, 7}, {9, 7}, {10, 7},
         {0, 8}, {5, 8}, {7, 8}, {12, 8}, {0, 9}, {5, 9},
         {7, 9}, {12, 9}, {0, 10}, {5, 10}, {7, 10}, {12, 10},
         {2, 12}, {3, 12}, {4, 12}, {8, 12}, {9, 12}, {10, 12}
      }
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Block pattern into the grid
 */
void GameOfLife::spawn_block(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 4> pattern{
      {{0, 0}, {1, 0}, {0, 1}, {1, 1}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Beehive pattern into the grid
 */
void GameOfLife::spawn_beehive(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 6> pattern{
      {{1, 0}, {2, 0}, {0, 1}, {3, 1}, {1, 2}, {2, 2}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Blinker pattern into the grid
 */
void GameOfLife::spawn_blinker(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 3> pattern{
      {{0, 0}, {1, 0}, {2, 0}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Toad pattern into the grid
 */
void GameOfLife::spawn_toad(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 6> pattern{
      {{1, 0}, {2, 0}, {3, 0}, {0, 1}, {1, 1}, {2, 1}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Loaf pattern into the grid
 */
void GameOfLife::spawn_loaf(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 7> pattern{
      {{1, 0}, {2, 0}, {0, 1}, {3, 1}, {1, 2}, {3, 2}, {2, 3}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Boat pattern into the grid
 */
void GameOfLife::spawn_boat(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 5> pattern{
      {{0, 0}, {1, 0}, {0, 1}, {2, 1}, {1, 2}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Beacon pattern into the grid
 */
void GameOfLife::spawn_beacon(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 6> pattern{
      {{0, 0}, {1, 0}, {0, 1}, {3, 2}, {2, 3}, {3, 3}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn R-Pentomino pattern into the grid
 */
void GameOfLife::spawn_r_pentomino(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 5> pattern{
      {{1, 0}, {2, 0}, {0, 1}, {1, 1}, {1, 2}}
   };

   spawn(pattern, gridX, gridY);
}


/**
 * Spawn Acorn pattern into the grid
 */
void GameOfLife::spawn_acorn(int gridX, int gridY) {
   constexpr std::array<std::pair<int, int>, 7> pattern{
      {{1, 0}, {3, 1}, {0, 2}, {1, 2}, {4, 2}, {5, 2}, {6, 2}}
   };

   spawn(pattern, gridX, gridY);
}
