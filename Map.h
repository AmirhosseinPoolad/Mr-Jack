#ifndef MAP_H
#define MAP_H

#include "GameObject.h"
#include "SDL.h"

struct Map
{
    struct Renderable mapObj;
    struct Renderable susObject;
    void (*Update)(struct Renderable *self);
    void (*OnMouseDown)(struct Renderable *self);
    SDL_Point coordinates;
    int susIndex;
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
struct node *GetTileFromCoordinates(struct node **head, int x, int y);
struct node *GetTileFromScreenCoordinates(struct node **head, int screenX, int screenY);
void SwapNodes(struct node **head, struct node *node1, struct node *node2);
void SwapTiles(struct node *node1, struct node *node2);

#endif