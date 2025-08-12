#include "game_of_life.h"


int main() {
   GameOfLife game{};

   // Spawn some patterns on the grid
   game.spawn_gosper_glider_gun(5, 20);
   game.spawn_glider(30, 10);
   game.spawn_glider(30, 35);
   game.spawn_glider(20, 70);
   game.spawn_pulsar(25, 80);
   game.spawn_pulsar(20, 90);
   game.spawn_pulsar(15, 70);

   if (game.init()) {
      game.run();
   }

   return 0;
}
