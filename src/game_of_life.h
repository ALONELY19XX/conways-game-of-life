#pragma once

#include <array>
#include <SDL3/SDL.h>


class GameOfLife {
public:
   static constexpr int GRID_COLS = 100;
   static constexpr int GRID_ROWS = 100;
   static constexpr int CELL_SIZE = 10;
   static constexpr int TOTAL_CELLS = GRID_COLS * GRID_ROWS;
   static constexpr int WINDOW_WIDTH = GRID_COLS * CELL_SIZE;
   static constexpr int WINDOW_HEIGHT = GRID_ROWS * CELL_SIZE;

   static constexpr int TARGET_FPS = 30;
   static constexpr int FRAME_TIME = 1000 / TARGET_FPS;

   GameOfLife();
   ~GameOfLife();

   bool init();
   void run();
   void cleanup();

   void spawn_glider(int x, int y);
   void spawn_gosper_glider_gun(int startX, int startY);
   void spawn_pulsar(int startX, int startY);

private:
   SDL_Window* window;
   SDL_Renderer* renderer;

   std::array<bool, TOTAL_CELLS> current_grid;
   std::array<bool, TOTAL_CELLS> next_grid;

   bool is_running;

   uint64_t last_update_time;


   void update();
   void render();
   void handle_events();
   void update_grid();
   void swap_grids();
   int count_alive_neighbors(int x, int y);
   void wait_remaining_frame_time() const;
};
