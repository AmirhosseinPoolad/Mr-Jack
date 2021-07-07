#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "SDL.h"
#include "GameObject.h"
#include "Map.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char *argv[])
{
  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow(
      "Mr-Jack",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      0);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawColor(renderer, 255, 250, 250, SDL_ALPHA_OPAQUE);

  struct node *map = NULL;
  SetupMap(&map, renderer);
  int isQuit = 0;
  while (!isQuit)
  {
    SDL_Event e;
    SDL_Point mousePos;
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
        isQuit = 1;
      if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)))
      {
        SDL_GetMouseState(&mousePos.x, &mousePos.y);
        struct node *tile = GetTileFromScreenCoordinates(&map, mousePos.x, mousePos.y);
        if (tile != NULL)
          tile->map.Update(&(tile->map.mapObj));
      }
    }
    SDL_RenderClear(renderer);
    RenderMap(&map, renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(200);
  }
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}