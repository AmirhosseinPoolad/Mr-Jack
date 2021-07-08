#include "Game.h"
#include "Map.h"
#include "Renderable.h"

void ClickSwap(int mouseDown, SDL_Point mousePos, struct node **map)
{
    static struct node *tiles[2] = {NULL, NULL};
    if (mouseDown)
    {
        if (tiles[0] == NULL)
        {
            tiles[0] = GetTileFromScreenCoordinates(map, mousePos.x, mousePos.y);
        }
        else if ((tiles[0] != NULL) && (tiles[1] == NULL))
        {
            tiles[1] = GetTileFromScreenCoordinates(map, mousePos.x, mousePos.y);
        }
    }
    if ((tiles[0] != NULL) && (tiles[1] != NULL))
    {
        SwapNodes(map, tiles[0], tiles[1]);
        tiles[0] = NULL;
        tiles[1] = NULL;
    }
}

void ClickRotate(SDL_Point mousePos, struct node **map)
{
    struct node *tile = GetTileFromScreenCoordinates(map, mousePos.x, mousePos.y);
    if (tile == NULL)
        return;
    if (mousePos.x >= tile->map.mapObj.rect.x + (tile->map.mapObj.rect.w / 2)) //clicked right half, cw
        tile->map.mapObj.orientation = (tile->map.mapObj.orientation + 1) % 4;
    else //clicked left half, ccw
    {
        tile->map.mapObj.orientation = (tile->map.mapObj.orientation - 1);
        if (tile->map.mapObj.orientation >= 4) //enums are unsigned so we need to check for overflow and not negative numbers
            tile->map.mapObj.orientation = RIGHT;
    }
}

void SetupDTPositions(SDL_Point DTPositions[12])
{
    for (int i = 0; i < 3; i++)
    {
        DTPositions[i].x = MAP_START_X + (i * TILE_WIDTH) + (TILE_WIDTH / 3);
        DTPositions[i].y = MAP_START_Y + (0 * TILE_HEIGHT) + (TILE_HEIGHT / 3) - (TILE_HEIGHT);
    }
    for (int i = 0; i < 3; i++)
    {
        DTPositions[i + 3].x = MAP_START_X + (2 * TILE_WIDTH) + (TILE_WIDTH / 3) + (TILE_WIDTH);
        DTPositions[i + 3].y = MAP_START_Y + (i * TILE_HEIGHT) + (TILE_HEIGHT / 3);
    }
    for (int i = 0; i < 3; i++)
    {
        DTPositions[i + 6].x = MAP_START_X + ((2 - i) * TILE_WIDTH) + (TILE_WIDTH / 3);
        DTPositions[i + 6].y = MAP_START_Y + (2 * TILE_HEIGHT) + (TILE_HEIGHT / 3) + (TILE_HEIGHT);
    }
    for (int i = 0; i < 3; i++)
    {
        DTPositions[i + 9].x = MAP_START_X + (0 * TILE_WIDTH) + (TILE_WIDTH / 3) - (TILE_WIDTH);
        DTPositions[i + 9].y = MAP_START_Y + ((2 - i) * TILE_HEIGHT) + (TILE_HEIGHT / 3);
    }
}

void IncrementDTPos(SDL_Point DTPositions[12], struct Renderable *dt, int amount)
{
    //find the index of dt
    int index = GetDTIndex(DTPositions, dt);
    //increment index
    index = IncrementDTIndex(index, amount);
    //apply new index to dt
    dt->rect.x = DTPositions[index].x;
    dt->rect.y = DTPositions[index].y;
}

int GetDTIndex(SDL_Point DTPositions[12], struct Renderable *dt)
{
    int index = 0;
    SDL_Point pos;
    pos.x = dt->rect.x;
    pos.y = dt->rect.y;
    for (int i = 0; i < 12; i++)
    {
        if ((DTPositions[i].x == pos.x) && (DTPositions[i].y == pos.y))
        {
            index = i;
            break;
        }
    }
    return index;
}

int IncrementDTIndex(int index, int amount)
{
    index += amount;
    if (index >= 12)
    {
        index = index % 12;
    }
    else if (index < 0)
    {
        while (index < 0)
        {
            index += 12;
        }
    }
    return index;
}

void RenderDT(SDL_Point DTPositions[12], struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby, SDL_Renderer *renderer)
{
    int hPos,wPos,tPos;
    hPos = GetDTIndex(DTPositions, holmes);
    wPos = GetDTIndex(DTPositions, watson);
    tPos = GetDTIndex(DTPositions, toby);
    if ((hPos == wPos) && (wPos == tPos))
    {
        holmes->rect.x += OFFSET;
        GORender(holmes, renderer);
        holmes->rect.x -= OFFSET;

        GORender(toby,renderer);

        watson->rect.x -= OFFSET;
        GORender(watson, renderer);
        watson->rect.x += OFFSET;
        return;
    }
    if (hPos == wPos)
    {
        holmes->rect.x += OFFSET;
        GORender(holmes, renderer);
        holmes->rect.x -= OFFSET;
    }
    else if (hPos == tPos)
    {
        holmes->rect.x += OFFSET;
        GORender(holmes, renderer);
        holmes->rect.x -= OFFSET;
    }
    else
    {
        GORender(holmes, renderer);
    }

    if (wPos == tPos)
    {
        watson->rect.x += OFFSET;
        GORender(watson, renderer);
        watson->rect.x -= OFFSET;
    }
    else
    {
        GORender(watson, renderer);
    }

    GORender(toby, renderer);
}