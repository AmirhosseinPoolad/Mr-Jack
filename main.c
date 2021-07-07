#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "SDL.h"
#include "GameObject.h"
#include "Map.h"

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

enum PlayState
{
  DEAL_TOKEN,
  SELECT_TOKEN,
  REMOVE_TOKENS,
  TILE_SWAP,
  TILE_ROTATE
};

void ClickSwap(int mouseDown, SDL_Point mousePos, struct node **map);
void ClickRotate(SDL_Point mousePos, struct node **map);
void SetupDTPositions(SDL_Point DTPositions[12]);
void IncrementDTPos(SDL_Point DTPositions[12], struct Renderable *dt, int amount);

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
  struct Renderable holmes, watson, toby;
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
    GORender(&holmes, renderer);
    GORender(&watson, renderer);
    GORender(&toby, renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(200);
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
    if (tile->map.mapObj.orientation >= 4) //enums are unsigned so we need to check for overflow and not negative numbers
      tile->map.mapObj.orientation = RIGHT;
  }
}

void SetupDTPositions(SDL_Point DTPositions[12])
{
  for (int i = 0; i < 3; i++)
  {
    DTPositions[i].x = MAP_START_X + (i * TILE_WIDTH) + (TILE_WIDTH / 3);
    DTPositions[i].y = MAP_START_Y + (0 * TILE_HEIGHT) + (TILE_HEIGHT / 3) - (TILE_HEIGHT);
  }
  for (int i = 0; i < 3; i++)
  {
    DTPositions[i + 3].x = MAP_START_X + (2 * TILE_WIDTH) + (TILE_WIDTH / 3) + (TILE_WIDTH);
    DTPositions[i + 3].y = MAP_START_Y + (i * TILE_HEIGHT) + (TILE_HEIGHT / 3);
  }
  for (int i = 0; i < 3; i++)
  {
    DTPositions[i + 6].x = MAP_START_X + ((2 - i) * TILE_WIDTH) + (TILE_WIDTH / 3);
    DTPositions[i + 6].y = MAP_START_Y + (2 * TILE_HEIGHT) + (TILE_HEIGHT / 3) + (TILE_HEIGHT);
  }
  for (int i = 0; i < 3; i++)
  {
    DTPositions[i + 9].x = MAP_START_X + (0 * TILE_WIDTH) + (TILE_WIDTH / 3) - (TILE_WIDTH);
    DTPositions[i + 9].y = MAP_START_Y + ((2 - i) * TILE_HEIGHT) + (TILE_HEIGHT / 3);
  }
}

void IncrementDTPos(SDL_Point DTPositions[12], struct Renderable *dt, int amount)
{
  //find the index of dt
  int index = 0;
  SDL_Point pos;
  pos.x = dt->rect.x;
  pos.y = dt->rect.y;
  for (int i = 0; i < 12; i++)
  {
    if ((DTPositions[i].x == pos.x) && (DTPositions[i].y == pos.y))
    {
      index = i;
      break;
    }
  }
  //increment index
  index += amount;
  if (index >= 12)
  {
    index = index % 12;
  }
  else if (index < 0)
  {
    while (index < 0)
    {
      index += 12;
    }
  }
  //apply new index to dt
  dt->rect.x = DTPositions[index].x;
  dt->rect.y = DTPositions[index].y;
}