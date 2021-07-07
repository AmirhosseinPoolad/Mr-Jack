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

void ClickSwap(int mouseDown, SDL_Point mousePos, struct node **map);
void ClickRotate(SDL_Point mousePos, struct node **map);

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
      }
    }
    //2. Game Logic
    ClickSwap(mouseDown, mousePos, &map);
    /*if (mouseDown)
      ClickRotate(mousePos, &map);*/
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

void ClickSwap(int mouseDown, SDL_Point mousePos, struct node **map)
{
  static struct node *tiles[2] = {NULL, NULL};
  if (mouseDown)
  {
    if (tiles[0] == NULL)
    {
      tiles[0] = GetTileFromScreenCoordinates(map, mousePos.x, mousePos.y);
    }
    else if ((tiles[0] != NULL) && (tiles[1] == NULL))
    {
      tiles[1] = GetTileFromScreenCoordinates(map, mousePos.x, mousePos.y);
    }
  }
  if ((tiles[0] != NULL) && (tiles[1] != NULL))
  {
    SwapNodes(map, tiles[0], tiles[1]);
    tiles[0] = NULL;
    tiles[1] = NULL;
  }
}

void ClickRotate(SDL_Point mousePos, struct node **map)
{
  struct node *tile = GetTileFromScreenCoordinates(map, mousePos.x, mousePos.y);
  if (tile == NULL)
    return;
  if (mousePos.x >= tile->map.mapObj.rect.x + (tile->map.mapObj.rect.w / 2)) //clicked right half, cw
    tile->map.mapObj.orientation = (tile->map.mapObj.orientation + 1) % 4;
  else //clicked left half, ccw
  {
    tile->map.mapObj.orientation = (tile->map.mapObj.orientation - 1);
    if (tile->map.mapObj.orientation >= 4)
      tile->map.mapObj.orientation = RIGHT; //c doesn't do modular arithmetic correctly so we need to handle negative numbers manually
  }
}