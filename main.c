#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "SDL.h"
#include "Renderable.h"
#include "Map.h"
#include "Game.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define AT_INITPOS_X 25
#define AT_INITPOS_Y 500
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

enum ActionTokens
{
  AT_SUSPECT, //removes a card from sus cards
  AT_HOLMES,  //moves holmes 1 or 2 tiles clockwise
  AT_TOBY,    //same
  AT_WATSON,  //same
  AT_ROTATE1, //rotate 90 or 180 degrees
  AT_SWAP,    //swap two tiles
  AT_ROTATE2,
  AT_ANY //move any of the detective 1 tiles clockwise
};

void SelectToken(int *state, struct Renderable *clickedToken, struct Renderable *zerothToken);

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
  int a = 0; //TODO: TOKEN DEALING
  struct Renderable actionTokens[8];
  SetupRenderableFromPath(&actionTokens[0], renderer, "assets/action_tokens/0.jpg",
                          AT_INITPOS_X, AT_INITPOS_Y, TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
  SetupRenderableFromPath(&actionTokens[1], renderer, "assets/action_tokens/1.jpg",
                          AT_INITPOS_X, AT_INITPOS_Y, TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);

  SetupRenderableFromPath(&actionTokens[2], renderer, "assets/action_tokens/2.jpg",
                          AT_INITPOS_X, AT_INITPOS_Y + (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
  SetupRenderableFromPath(&actionTokens[3], renderer, "assets/action_tokens/3.jpg",
                          AT_INITPOS_X, AT_INITPOS_Y + (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);

  SetupRenderableFromPath(&actionTokens[4], renderer, "assets/action_tokens/4.jpg",
                          AT_INITPOS_X, AT_INITPOS_Y + 2 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
  SetupRenderableFromPath(&actionTokens[5], renderer, "assets/action_tokens/5.jpg",
                          AT_INITPOS_X, AT_INITPOS_Y + 2 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);

  SetupRenderableFromPath(&actionTokens[6], renderer, "assets/action_tokens/6.jpg",
                          AT_INITPOS_X, AT_INITPOS_Y + 3 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
  SetupRenderableFromPath(&actionTokens[7], renderer, "assets/action_tokens/7.jpg",
                          AT_INITPOS_X, AT_INITPOS_Y + 3 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);

  struct Renderable *activeATokens[4];

  int isQuit = 0;
  int gameState;
  int playState = DEAL_TOKEN;
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
    case DEAL_TOKEN:
      for (int i = 0; i < 4; i++)
      {
        int random = rand() % 2;
        activeATokens[i] = &actionTokens[(2 * i) + random];
      }
      playState = SELECT_TOKEN;
      break;
    case SELECT_TOKEN:
      if (mouseDown)
      {
        for (int i; i < 4; i++)
        {
          if (SDL_PointInRect(&mousePos, &(activeATokens[i]->rect)))
          {
            SelectToken(&playState, activeATokens[i], &actionTokens[0]);
          }
        }
      }
      break;
    default:
      ClickSwap(mouseDown, mousePos, &map);
      break;
    }
    //3. Render
    SDL_RenderClear(renderer);

    RenderMap(&map, renderer);
    RenderDT(DTPositions, &holmes, &watson, &toby, renderer);
    for (int i = 0; i < 4; i++)
    {
      GORender(activeATokens[i], renderer);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}

void SelectToken(int *state, struct Renderable *clickedToken, struct Renderable *zerothToken)
{
  int index = (clickedToken - zerothToken);
  switch (index)
  {
  case AT_SUSPECT:
    break;
  case AT_HOLMES:
    *state = HOLMES_MOVE;
    break;
  case AT_WATSON:
    *state = WATSON_MOVE;
    break;
  case AT_TOBY:
    *state = HOLMES_MOVE;
    break;
  case AT_ROTATE1:
    *state = TILE_ROTATE;
    break;
  case AT_SWAP:
    *state = TILE_SWAP;
    break;
  case AT_ROTATE2:
    *state = TILE_ROTATE;
    break;
  case AT_ANY:
    *state = HOLMES_MOVE;
    break;
  default:
    break;
  }
}