#include <stdio.h>
#include "SDL.h"
#include "GameObject.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void CharUpdate(struct GameObject *obj)
{
  printf("yo\n");
}

void EnemyUpdate(struct GameObject *obj)
{
  printf("no\n");
}

int main(int argc, char *argv[])
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow(
      "Mr-Jack",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      0);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawColor(renderer, 255, 200, 100, SDL_ALPHA_OPAQUE);

  struct GameObject playa;
  SetupGameObject(&playa, renderer, "assets/wall.jpg", CharUpdate, 100, 100, 100, 100);
  playa.update_function(&playa);

  SDL_RenderClear(renderer);
  GORender(&playa,renderer);
  SDL_RenderPresent(renderer);

  SDL_Delay(3000);

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}