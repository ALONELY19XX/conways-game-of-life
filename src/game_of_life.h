#pragma once

#include <array>
#include <span>

#include <SDL3/SDL.h>


class GameOfLife {
public:
   static constexpr int GRID_COLS = 100;
   static constexpr int GRID_ROWS = 100;
   static constexpr int GRID_SIZE = GRID_COLS * GRID_ROWS;
   static constexpr int CELL_SIZE = 10;
   static constexpr int WINDOW_WIDTH = GRID_COLS * CELL_SIZE;
   static constexpr int WINDOW_HEIGHT = GRID_ROWS * CELL_SIZE;

   static constexpr int TARGET_FPS = 30;
   static constexpr int FRAME_TIME = 1000 / TARGET_FPS;

   struct Cell {
      bool is_alive;
   };

   GameOfLife();
   ~GameOfLife();

   bool init();
   void run();
   void cleanup();

   void spawn_glider(int gridX, int gridY);
   void spawn_gosper_glider_gun(int gridX, int gridY);
   void spawn_pulsar(int gridX, int gridY);
   void spawn_block(int gridX, int gridY);
   void spawn_beehive(int gridX, int gridY);
   void spawn_blinker(int gridX, int gridY);
   void spawn_toad(int gridX, int gridY);
   void spawn_loaf(int gridX, int gridY);
   void spawn_boat(int gridX, int gridY);
   void spawn_beacon(int gridX, int gridY);
   void spawn_r_pentomino(int gridX, int gridY);
   void spawn_acorn(int gridX, int gridY);

private:
   SDL_Window* window;
   SDL_Renderer* renderer;

   std::array<Cell, GRID_SIZE> front_grid;
   std::array<Cell, GRID_SIZE> back_grid;

   bool is_running;

   uint64_t last_update_time;

   void update();
   void render() const;
   void handle_events();
   void update_grid();
   void swap_grids();
   void wait_remaining_frame_time() const;

   int count_alive_neighbors(int gridX, int gridY) const;

   void spawn(std::span<const std::pair<int, int>> pattern, int gridX, int gridY);
};
