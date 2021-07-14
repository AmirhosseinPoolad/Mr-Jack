#ifndef MAP_H
#define MAP_H

#define MAP_START_X 190
#define MAP_START_Y 90
#define TILE_HEIGHT 140
#define TILE_WIDTH 140
#include "Renderable.h"
#include "SDL.h"

struct Map
{
    struct Renderable mapObj;
    struct Renderable susObject;
    SDL_Point coordinates;
    int susIndex;
    int isShowingSuspect;
};

struct MapData //for loading and saving
{
    int isRandom;
    int susIndex[9];
    int isShowingSuspect[9];
    enum Orientation orientation[9];
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

void SetupRandomMap(struct node **head, SDL_Renderer *rend);
void SetupMap(struct node **head, SDL_Renderer *rend, const struct MapData *data);
void RenderMap(struct node **head, SDL_Renderer *rend);
struct node *GetTileFromCoordinates(struct node **head, int x, int y);
struct node *GetTileFromScreenCoordinates(struct node **head, int screenX, int screenY);
struct node *GetTileFromSusIndex(struct node **head, int susIndex);
void SwapNodes(struct node **head, struct node *node1, struct node *node2);
void SwapTiles(struct node *node1, struct node *node2);
struct MapData GetDataFromMap(struct node **head);

#endif