#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Renderable.h"

void ClickSwap(int mouseDown, SDL_Point mousePos, struct node **map);
void ClickRotate(SDL_Point mousePos, struct node **map);
void SetupDTPositions(SDL_Point DTPositions[12]);
void IncrementDTPos(SDL_Point DTPositions[12], struct Renderable *dt, int amount);
int GetDTIndex(SDL_Point DTPositions[12], struct Renderable *dt);
int IncrementDTIndex(int index, int amount);
void RenderDT(SDL_Point DTPositions[12], struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby, SDL_Renderer *renderer);

#endif