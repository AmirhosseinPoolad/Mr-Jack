#ifndef GAME_OBJECT
#define GAME_OBJECT
#include "SDL.h"

enum Orientation
{
    UP,
    LEFT,
    DOWN,
    RIGHT
};

struct GameObject
{
    void (*update_function)(struct GameObject *self);
    SDL_Rect rect;        //position and size
    SDL_Texture *texture; //texture
    enum Orientation orientation;
};

void SetupGameObject(struct GameObject *obj, SDL_Renderer *renderer, char *textureAddress,
                     void (*update_function)(struct GameObject *self),
                     int x, int y, int w, int h);

void GORender(struct GameObject *obj, SDL_Renderer *renderer);
#endif