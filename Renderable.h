#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include "SDL.h"

#define OFFSET 25

enum Orientation
{
    DOWN,
    LEFT,
    UP,
    RIGHT
};

struct Renderable
{
    SDL_Rect rect;                //position and size
    SDL_Texture *texture;         //texture
    enum Orientation orientation; //rotation
};

SDL_Texture *SetupTexture(char *path, SDL_Renderer *renderer);

void SetupRenderableFromPath(struct Renderable *obj, SDL_Renderer *renderer, char *textureAddress,
                             int x, int y, int w, int h, enum Orientation orientation);

void SetupRenderableWithTexture(struct Renderable *obj, SDL_Renderer *renderer, SDL_Texture *texture,
                                int x, int y, int w, int h, enum Orientation orientation);

void GORender(struct Renderable *obj, SDL_Renderer *renderer);
void FreeRenderable(struct Renderable *obj);

#endif