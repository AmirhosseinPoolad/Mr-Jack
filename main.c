#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "SDL.h"
#include "GameObject.h"
#include "Map.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void CharUpdate(struct GameObject *obj)
{
  obj->orientation++;
  obj->orientation = obj->orientation % 4;
}

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

  //struct GameObject playa;
  //SetupGameObject(&playa, renderer, "assets/wall.jpg", CharUpdate, 100, 100, 100, 100, UP);
  struct node *mapFront = NULL;
  SetupMap(&mapFront, renderer);
  int isQuit = 0;
  while (!isQuit)
  {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
        isQuit = 1;
    }
    struct node *node = GetCoordinates(&mapFront, rand()%3, rand()%3);
    node->obj.update_function(&(node->obj)); //testing
    SDL_RenderClear(renderer);
    RenderMap(&mapFront, renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(200);
  }
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}