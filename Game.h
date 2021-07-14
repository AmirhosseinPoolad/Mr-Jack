#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Renderable.h"

struct GameState
{
    struct MapData mData;
    int holmesPos, watsonPos, tobyPos;
    int ActiveTokensIndex[4];
    int characterCards[9];
    int jackIndex;
    int susIndex; //where we are in the character card array
    int turn;
    int round;
    int tokensSelected;
    int playState;
};

int MoveDT(int mouseDown, SDL_Point mousePos, struct Renderable *confirmButton, struct Renderable *dt, SDL_Point DTPositions[12]);
int MoveAny(int mouseDown, SDL_Point mousePos, struct Renderable *confirmButton,
            struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby,
            SDL_Point DTPositions[12], int turn);
int ClickSwap(int mouseDown, SDL_Point mousePos, struct node **map);
int ClickRotate(int mouseDown, SDL_Point mousePos, struct node **map, struct Renderable *confirmButton);
void SetupDTPositions(SDL_Point DTPositions[12]);
void IncrementDTPos(SDL_Point DTPositions[12], struct Renderable *dt, int amount);
int GetDTIndex(SDL_Point DTPositions[12], struct Renderable *dt);
int IncrementDTIndex(int index, int amount);
void RenderDT(SDL_Point DTPositions[12], struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby, SDL_Renderer *renderer);
int MapXCoordsFromDTIndex(int index);
void AddToVisiblesList(enum Orientation direction, struct node *tile, struct node **head, SDL_Point seenTiles[9], int *size);

void SetupGame(struct GameState gameState, struct node **map,
               struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby,
               struct Renderable *activeATokens[4], int characters[9], int *jackIndex,
               int *susIndex, int *turn, int *round, int *tokensSelected, int *playState,
               SDL_Renderer *renderer, SDL_Point DTPositions[12], struct Renderable actionTokens[8]);
#endif