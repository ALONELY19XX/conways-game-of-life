#include <print>
#include <iostream>

#include "game_of_life.h"


int main() {
   std::println("** Conway's Game of Life **");

   GameOfLife game{};
   // Gosper Glider Gun (left side)
   game.spawn_gosper_glider_gun(5, 20);

   // Some gliders
   game.spawn_glider(70, 10);
   game.spawn_glider(80, 30);
   game.spawn_glider(60, 60);

   // Pulsars for visual interest
   game.spawn_pulsar(40, 40);
   game.spawn_pulsar(20, 70);
   game.spawn_pulsar(70, 70);

   if (game.init()) {
      game.run();
   }

   return 0;
}
