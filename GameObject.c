#include "GameObject.h"
#include "SDL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//loads an image from textureAddress and sets up a GameObject with the image
void SetupGameObject(struct GameObject *obj, SDL_Renderer *renderer, char *textureAddress,
                     void (*update_function)(struct GameObject *self),
                     int x, int y, int w, int h, enum Orientation orientation)
{
    //neccesarry to convert an stbi loaded image into an SDL_Surface
    int req_format = STBI_rgb_alpha;
    int width, height, orig_format;
    char *data = stbi_load(textureAddress, &width, &height, &orig_format, req_format);
    int depth, pitch;
    Uint32 pixel_format;
    if (req_format == STBI_rgb)
    {
        depth = 24;
        pitch = 3 * width; // 3 bytes per pixel * pixels per row
        pixel_format = SDL_PIXELFORMAT_RGB24;
    }
    else
    { // STBI_rgb_alpha (RGBA)
        depth = 32;
        pitch = 4 * width;
        pixel_format = SDL_PIXELFORMAT_RGBA32;
    }
    //create a surface and then a texture from it
    //we use textures for hardware rendering so we don't need the surface afterwards
    SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormatFrom((void *)data, width, height, depth, pitch, pixel_format);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surf);
    obj->texture = text;
    //now that the texture is made, we don't need to surface and the pixel data anymore
    SDL_FreeSurface(surf);
    stbi_image_free(data);
    data = NULL;
    surf = NULL;

    obj->update_function = update_function;
    obj->rect.h = h;
    obj->rect.w = w;
    obj->rect.x = x;
    obj->rect.y = y;

    obj->orientation = orientation;
}

void GORender(struct GameObject *obj, SDL_Renderer *renderer) //renders the GameObject
{
    double rotation = obj->orientation * 90;
    SDL_RenderCopyEx(renderer, obj->texture, NULL, &(obj->rect), rotation, NULL, SDL_FLIP_NONE);
}