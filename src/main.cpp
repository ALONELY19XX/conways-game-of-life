#include "GameOfLife.h"

int main() {
   auto game = GameOfLife{};

   // Spawn some patterns on the grid
   game.spawn(Pattern::Acorn, 50, 50);
   game.spawn(Pattern::Acorn, 70, 30);
   game.spawn(Pattern::Block, 40, 40);
   game.spawn(Pattern::Boat, 10, 80);
   game.spawn(Pattern::Loaf, 80, 80);
   game.spawn(Pattern::Glider, 10, 10);
   game.spawn(Pattern::Glider, 60, 90);
   game.spawn(Pattern::Glider, 75, 75);
   game.spawn(Pattern::Glider, 20, 50);
   game.spawn(Pattern::Beehive, 30, 75);
   game.spawn(Pattern::Beehive, 50, 90);
   game.spawn(Pattern::Beehive, 90, 10);
   game.spawn(Pattern::RPentomino, 25, 80);
   game.spawn(Pattern::Pulsar, 15, 55);
   game.spawn(Pattern::Pulsar, 85, 85);

   if (game.init()) {
      game.run();
   }

   return 0;
}
