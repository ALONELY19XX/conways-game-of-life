#include "game_of_life.h"


auto main() -> int {
   GameOfLife game{};

   // Spawn some patterns on the grid
   game.spawn_acorn(50, 50);
   game.spawn_acorn(70, 30);
   game.spawn_block(40, 40);
   game.spawn_boat(10, 80);
   game.spawn_loaf(80, 80);
   game.spawn_glider(10, 10);
   game.spawn_glider(60, 90);
   game.spawn_glider(75, 75);
   game.spawn_glider(20, 50);
   game.spawn_beehive(30, 75);
   game.spawn_beehive(50, 90);
   game.spawn_beehive(90, 10);
   game.spawn_r_pentomino(25, 80);
   game.spawn_pulsar(15, 55);
   game.spawn_pulsar(85, 85);

   if (game.init()) {
      game.run();
   }

   return 0;
}
