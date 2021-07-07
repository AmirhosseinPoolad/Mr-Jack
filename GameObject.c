#include "GameObject.h"

#include "SDL.h"
#include "Map.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//loads a texture from path
SDL_Texture *SetupTexture(char *path, SDL_Renderer *renderer)
{
    int req_format = STBI_rgb_alpha;
    int width, height, orig_format;
    char *data = stbi_load(path, &width, &height, &orig_format, req_format);
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
    //now that the texture is made, we don't need to surface and the pixel data anymore
    SDL_FreeSurface(surf);
    stbi_image_free(data);
    data = NULL;
    surf = NULL;
    return text;
}

//loads an image from textureAddress and sets up a Renderable with the image
void SetupRenderableFromPath(struct Renderable *obj, SDL_Renderer *renderer, char *textureAddress,
                             int x, int y, int w, int h, enum Orientation orientation)
{
    SDL_Texture *text = SetupTexture(textureAddress, renderer);
    obj->texture = text;

    obj->rect.h = h;
    obj->rect.w = w;
    obj->rect.x = x;
    obj->rect.y = y;

    obj->orientation = orientation;
}

void SetupRenderableWithTexture(struct Renderable *obj, SDL_Renderer *renderer, SDL_Texture *texture,
                                int x, int y, int w, int h, enum Orientation orientation)
{
    obj->texture = texture;
    obj->rect.h = h;
    obj->rect.w = w;
    obj->rect.x = x;
    obj->rect.y = y;
    obj->orientation = orientation;
}

void GORender(struct Renderable *obj, SDL_Renderer *renderer) //renders the Renderable
{
    double rotation = obj->orientation * 90;
    SDL_RenderCopyEx(renderer, obj->texture, NULL, &(obj->rect), rotation, NULL, SDL_FLIP_NONE);
}