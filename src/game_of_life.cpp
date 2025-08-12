#include <print>
#include <utility>
#include <vector>

#include "game_of_life.h"


GameOfLife::GameOfLife() :
   window{nullptr},
   renderer{nullptr},
   current_grid{},
   next_grid{},
   is_running{false},
   last_update_time{0} {}


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
 * If the last recorded update timestamp lies at least `FRAME_TIME` milliseconds in the past,
 * we update it by setting it to the current (frame) timestamp.
 */
void GameOfLife::update() {
   const uint64_t current_time = SDL_GetTicks();
   const uint64_t delta_time = current_time - last_update_time;

   if (delta_time >= FRAME_TIME) {
      update_grid();
      last_update_time = current_time;
   }
}


void GameOfLife::render() {
   SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
   SDL_RenderClear(renderer);

   SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

   for (int y{0}; y < GRID_ROWS; ++y) {
      for (int x{0}; x < GRID_COLS; ++x) {
         if (current_grid[x + y * GRID_COLS]) {
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

void GameOfLife::swap_grids() {
   std::swap(current_grid, next_grid);
}

int GameOfLife::count_alive_neighbors(int x, int y) {
   int count = 0;

   for (int dy = -1; dy <= 1; ++dy) {
      for (int dx = -1; dx <= 1; ++dx) {
         if (dx == 0 && dy == 0) continue; // Skip the cell itself

         int nx = x + dx;
         int ny = y + dy;

         // Wrap around (toroidal topology)
         if (nx < 0) nx = GRID_COLS - 1;
         if (nx >= GRID_COLS) nx = 0;
         if (ny < 0) ny = GRID_ROWS - 1;
         if (ny >= GRID_ROWS) ny = 0;

         if (current_grid[nx + ny * GRID_COLS]) {
            count++;
         }
      }
   }

   return count;
}


void GameOfLife::update_grid() {
   // Calculate next generation
   for (int y = 0; y < GRID_ROWS; ++y) {
      for (int x = 0; x < GRID_COLS; ++x) {
         const int neighbors = count_alive_neighbors(x, y);
         const bool current_state = current_grid[x + y * GRID_COLS];

         // Conway's Game of Life rules
         if (current_state) {
            // Live cell survives with 2 or 3 neighbors
            next_grid[x + y * GRID_COLS] = (neighbors == 2 || neighbors == 3);
         }
         else {
            // Dead cell becomes alive with exactly 3 neighbors
            next_grid[x + y * GRID_COLS] = (neighbors == 3);
         }
      }
   }

   swap_grids();
}


void GameOfLife::spawn_glider(int x, int y) {
   std::vector<std::pair<int, int>> pattern{
      {1, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2}
   };

   for (auto [dx, dy] : pattern) {
      int xPos = x + dx;
      int yPos = y + dy;
      if (xPos >= 0 && xPos < GRID_COLS && yPos >= 0 && yPos < GRID_ROWS) {
         current_grid[xPos + yPos * GRID_COLS] = true;
      }
   }
}

void GameOfLife::spawn_gosper_glider_gun(int startX, int startY) {
   std::vector<std::pair<int, int>> pattern = {
      {24, 0}, {22, 1}, {24, 1}, {12, 2}, {13, 2}, {20, 2}, {21, 2}, {34, 2}, {35, 2},
      {11, 3}, {15, 3}, {20, 3}, {21, 3}, {34, 3}, {35, 3}, {0, 4}, {1, 4}, {10, 4},
      {16, 4}, {20, 4}, {21, 4}, {0, 5}, {1, 5}, {10, 5}, {14, 5}, {16, 5}, {17, 5},
      {22, 5}, {24, 5}, {10, 6}, {16, 6}, {24, 6}, {11, 7}, {15, 7}, {12, 8}, {13, 8}
   };

   for (auto [dx, dy] : pattern) {
      int x = startX + dx;
      int y = startY + dy;
      if (x >= 0 && x < GRID_COLS && y >= 0 && y < GRID_ROWS) {
         current_grid[x + y * GRID_COLS] = true;
      }
   }
}

void GameOfLife::spawn_pulsar(int startX, int startY) {
   std::vector<std::pair<int, int>> pattern = {
      {2, 0}, {3, 0}, {4, 0}, {8, 0}, {9, 0}, {10, 0},
      {0, 2}, {5, 2}, {7, 2}, {12, 2},
      {0, 3}, {5, 3}, {7, 3}, {12, 3},
      {0, 4}, {5, 4}, {7, 4}, {12, 4},
      {2, 5}, {3, 5}, {4, 5}, {8, 5}, {9, 5}, {10, 5},
      {2, 7}, {3, 7}, {4, 7}, {8, 7}, {9, 7}, {10, 7},
      {0, 8}, {5, 8}, {7, 8}, {12, 8},
      {0, 9}, {5, 9}, {7, 9}, {12, 9},
      {0, 10}, {5, 10}, {7, 10}, {12, 10},
      {2, 12}, {3, 12}, {4, 12}, {8, 12}, {9, 12}, {10, 12}
   };

   for (auto [dx, dy] : pattern) {
      int x = startX + dx;
      int y = startY + dy;
      if (x >= 0 && x < GRID_COLS && y >= 0 && y < GRID_ROWS) {
         current_grid[x + y * GRID_COLS] = true;
      }
   }
}
