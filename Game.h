#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Renderable.h"

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

#endif