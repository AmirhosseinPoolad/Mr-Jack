#ifndef MAP_H
#define MAP_H

#include "GameObject.h"

struct Map
{
    struct GameObject mapObj;
    struct GameObject susObject;
    int isShowingSuspect;
};

struct node
{
    struct Map map;
    struct node *next;
};

struct SuspectTexture
{
    SDL_Texture *tex;
    int isUsed;
};

struct node *newNode(struct Map map);
int pushEnd(struct Map map, struct node **head);

void SetupMap(struct node **head, SDL_Renderer *rend);
void RenderMap(struct node **head, SDL_Renderer *rend);
struct node *GetFromCoordinates(struct node **head, int x, int y);
void FindCoordinates(struct node **head, struct node *input, int *x, int *y); //TODO

#endif