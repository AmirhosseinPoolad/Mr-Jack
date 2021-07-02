#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
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
    void (*update_function)(struct GameObject *self); //pointer to a generic update function to call every frame
    SDL_Rect rect;                                    //position and size
    SDL_Texture *texture;                             //texture
    enum Orientation orientation;                     //rotation
};

void SetupGameObject(struct GameObject *obj, SDL_Renderer *renderer, char *textureAddress,
                     void (*update_function)(struct GameObject *self),
                     int x, int y, int w, int h, enum Orientation orientation);

void GORender(struct GameObject *obj, SDL_Renderer *renderer);
#endif