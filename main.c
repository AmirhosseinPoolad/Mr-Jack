#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "SDL.h"
#include "Renderable.h"
#include "Map.h"
#include "Game.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

enum GameState //TODO: Implement these
{
  MAIN_MENU,
  LOAD_MENU,
  QUIT,
  GAME,
  PAUSE
};

enum PlayState //TODO: And these
{
  DEAL_TOKEN,
  SELECT_TOKEN,
  REMOVE_TOKENS,
  TILE_SWAP,
  TILE_ROTATE,
  HOLMES_MOVE,
  WATSON_MOVE,
  TOBY_MOVE
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

  SDL_Point DTPositions[12]; //positions for detective tokens around the board
                             //starts from top left and goes clockwise
  SetupDTPositions(DTPositions);
  struct Renderable holmes, watson, toby; //pos: 11,3,7
  SetupRenderableFromPath(&holmes, renderer, "assets/detective_tokens/Holmes.jpg",
                          DTPositions[11].x, DTPositions[11].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, UP);
  SetupRenderableFromPath(&watson, renderer, "assets/detective_tokens/Watson.jpg",
                          DTPositions[3].x, DTPositions[3].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, UP);
  SetupRenderableFromPath(&toby, renderer, "assets/detective_tokens/Toby.jpg",
                          DTPositions[7].x, DTPositions[7].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, UP);

  int isQuit = 0;
  int gameState;
  int playState = TILE_ROTATE;
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
    switch (playState)
    {
    case TILE_SWAP:
      ClickSwap(mouseDown, mousePos, &map);
      break;
    case TILE_ROTATE:
      if (mouseDown)
        ClickRotate(mousePos, &map);
      break;
    default:
      ClickSwap(mouseDown, mousePos, &map);
      break;
    }
    //3. Render
    SDL_RenderClear(renderer);

    RenderMap(&map, renderer);
    RenderDT(DTPositions, &holmes, &watson, &toby, renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}
