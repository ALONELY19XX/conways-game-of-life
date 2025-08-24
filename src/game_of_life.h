#pragma once

#include <array>
#include <span>

#include <SDL3/SDL.h>


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
      bool is_alive;
   };

   GameOfLife();
   ~GameOfLife();

   auto init() -> bool;
   auto run() -> void;
   auto cleanup() -> void;

   auto spawn_glider(int gridX, int gridY) -> void;
   auto spawn_gosper_glider_gun(int gridX, int gridY) -> void;
   auto spawn_pulsar(int gridX, int gridY) -> void;
   auto spawn_block(int gridX, int gridY) -> void;
   auto spawn_beehive(int gridX, int gridY) -> void;
   auto spawn_blinker(int gridX, int gridY) -> void;
   auto spawn_toad(int gridX, int gridY) -> void;
   auto spawn_loaf(int gridX, int gridY) -> void;
   auto spawn_boat(int gridX, int gridY) -> void;
   auto spawn_beacon(int gridX, int gridY) -> void;
   auto spawn_r_pentomino(int gridX, int gridY) -> void;
   auto spawn_acorn(int gridX, int gridY) -> void;

private:
   SDL_Window* window;
   SDL_Renderer* renderer;

   std::array<Cell, GRID_SIZE> front_grid;
   std::array<Cell, GRID_SIZE> back_grid;

   bool is_running;

   uint64_t last_update_time;

   auto update() -> void;
   auto render() const -> void;
   auto handle_events() -> void;
   auto update_grid() -> void;
   auto swap_grids() -> void;
   auto wait_remaining_frame_time() const -> void;
   auto spawn(std::span<const std::pair<int, int>> pattern, int gridX, int gridY) -> void;
   [[nodiscard]]
   auto count_alive_neighbors(int gridX, int gridY) const -> int;
};
