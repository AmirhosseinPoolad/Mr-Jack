#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "SDL.h"
#include "GameObject.h"
#include "Map.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

enum GameState //TODO: Implement these
{
  MAIN_MENU,
  LOAD_MENU,
  QUIT,
  GAME,
  PAUSE
};

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
  int state;
  struct node *tiles[2] = {NULL, NULL};
  while (!isQuit)
  {
    //1. Handle Input
    SDL_Event e;
    SDL_Point mousePos;
    int mouseDown = 0;
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
        isQuit = 1;
      if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)))
      {
        SDL_GetMouseState(&mousePos.x, &mousePos.y);
        mouseDown = 1;
        /*struct node *tile = GetTileFromScreenCoordinates(&map, mousePos.x, mousePos.y);
        if (tile != NULL)
          tile->map.Update(&(tile->map.mapObj));*/
      }
    }
    //2. Game Logic
    if (mouseDown)
    {
      if (tiles[0] == NULL)
      {
        tiles[0] = GetTileFromScreenCoordinates(&map, mousePos.x, mousePos.y);
      }
      else if ((tiles[0] != NULL) && (tiles[1] == NULL))
      {
        tiles[1] = GetTileFromScreenCoordinates(&map, mousePos.x, mousePos.y);
      }
    }
    if ((tiles[0] != NULL) && (tiles[1] != NULL))
    {
      //tiles[0]->map.Update(&(tiles[0]->map.mapObj));
      //tiles[1]->map.Update(&(tiles[1]->map.mapObj));
      SwapNodes(&map,tiles[0],tiles[1]);
      tiles[0] = NULL;
      tiles[1] = NULL;
    }
    //3. Render
    SDL_RenderClear(renderer);
    RenderMap(&map, renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}