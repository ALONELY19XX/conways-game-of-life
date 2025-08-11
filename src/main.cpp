#include <print>
#include <iostream>

#include <SDL3/SDL.h>

int main() {
   std::println("** Conway's Game of Life **");

   if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::println(stderr, "Failed to initialize SDL {}", SDL_GetError());
      return 1;
   }

   SDL_Window* window;
   SDL_Renderer* renderer;

   if (!SDL_CreateWindowAndRenderer(
      "Conway's Game of Life",
      640, 480,
      0,
      &window, &renderer
   )) {
      std::println(stderr, "Failed to initialize SDL Window and Renderer {}", SDL_GetError());
      return 1;
   }

   bool running = true;

   while (running) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         if (event.type == SDL_EVENT_QUIT) {
            running = false;
         }
      }

      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);

      SDL_RenderPresent(renderer);

      SDL_Delay(1000 / 30);
   }

   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
