#include "Game.h"
#include "Map.h"
#include "Renderable.h"
#include "math.h"

int MoveDT(int mouseDown, SDL_Point mousePos, struct Renderable *confirmButton, struct Renderable *dt, SDL_Point DTPositions[12])
{
    static int times = 0;
    if (mouseDown)
    {
        if (SDL_PointInRect(&mousePos, &(confirmButton->rect)) && times >= 1)
        {
            times = 0;
            return 1;
        }
        if (times <= 1 && SDL_PointInRect(&mousePos, &(dt->rect)))
        {
            IncrementDTPos(DTPositions, dt, 1);
            times++;
        }
    }
    return 0;
}

int MoveAny(int mouseDown, SDL_Point mousePos, struct Renderable *confirmButton,
            struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby,
            SDL_Point DTPositions[12], int turn)
{
    static struct Renderable *selectedDetective = NULL;
    static int times = 0;
    if (mouseDown)
    {
        if (!turn) //watson's turn, has to move
        {
            if (SDL_PointInRect(&mousePos, &(confirmButton->rect)) && times == 1)
            {
                times = 0;
                return 1;
            }
            if (SDL_PointInRect(&mousePos, &(holmes->rect)) && times == 0)
            {
                IncrementDTPos(DTPositions, holmes, 1);
                times++;
                return 0;
            }
            if (SDL_PointInRect(&mousePos, &(watson->rect)) && times == 0)
            {
                IncrementDTPos(DTPositions, watson, 1);
                times++;
                return 0;
            }
            if (SDL_PointInRect(&mousePos, &(toby->rect)) && times == 0)
            {
                IncrementDTPos(DTPositions, toby, 1);
                times++;
                return 0;
            }
        }
        if (turn) //jack's move, doesn't have to move
        {
            if (SDL_PointInRect(&mousePos, &(confirmButton->rect)))
            {
                times = 0;
                return 1;
            }
            if (SDL_PointInRect(&mousePos, &(holmes->rect)) && times == 0)
            {
                IncrementDTPos(DTPositions, holmes, 1);
                times++;
                return 0;
            }
            if (SDL_PointInRect(&mousePos, &(watson->rect)) && times == 0)
            {
                IncrementDTPos(DTPositions, watson, 1);
                times++;
                return 0;
            }
            if (SDL_PointInRect(&mousePos, &(toby->rect)) && times == 0)
            {
                IncrementDTPos(DTPositions, toby, 1);
                times++;
                return 0;
            }
        }
    }
    return 0;
}

int ClickSwap(int mouseDown, SDL_Point mousePos, struct node **map)
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
        return 1;
    }
    return 0;
}

int ClickRotate(int mouseDown, SDL_Point mousePos, struct node **map, struct Renderable *confirmButton)
{
    static int times = 0;
    static struct node *tile = NULL;
    if (mouseDown)
    {
        if (tile == NULL)
        {
            tile = GetTileFromScreenCoordinates(map, mousePos.x, mousePos.y);
        }
        if (SDL_PointInRect(&mousePos, &(confirmButton->rect)) && times != 0)
        {
            times = 0;
            tile = NULL;
            return 1;
        }
        if ((tile != NULL) && SDL_PointInRect(&mousePos, &(tile->map.mapObj.rect)) && mousePos.x >= tile->map.mapObj.rect.x + (tile->map.mapObj.rect.w / 2)) //clicked right half, cw
        {
            if (times <= 1)
            {
                tile->map.mapObj.orientation = (tile->map.mapObj.orientation + 1) % 4;
                times++;
                return 0;
            }
        }
        else if ((tile != NULL) && SDL_PointInRect(&mousePos, &(tile->map.mapObj.rect))) //clicked left half, ccw
        {
            if (times >= -1)
            {
                tile->map.mapObj.orientation = (tile->map.mapObj.orientation - 1);
                if (tile->map.mapObj.orientation >= 4) //enums are unsigned so we need to check for overflow and not negative numbers
                    tile->map.mapObj.orientation = RIGHT;
                times--;
                return 0;
            }
        }
    }

    return 0;
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
    int hPos, wPos, tPos;
    hPos = GetDTIndex(DTPositions, holmes);
    wPos = GetDTIndex(DTPositions, watson);
    tPos = GetDTIndex(DTPositions, toby);
    if ((hPos == wPos) && (wPos == tPos))
    {
        holmes->rect.x += OFFSET;
        GORender(holmes, renderer);
        holmes->rect.x -= OFFSET;

        GORender(toby, renderer);

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

int MapXCoordsFromDTIndex(int index) // y of index i is equal to x of index 11 - i
{
    if (index == 0 || index >= 8)
        return 0;
    if (index >= 2 && index <= 6)
        return 2;
    if (index == 1 || index == 7)
        return 1;
}

void AddToVisiblesList(enum Orientation direction, struct node *tile, struct node **head, SDL_Point seenTiles[9], int *size)
{
    if (abs(direction - tile->map.mapObj.orientation) == 2)
    {
        return;
    }
    if (tile->map.isShowingSuspect != 0)
    {
        tile->map.isShowingSuspect = 0;
        seenTiles[*size] = tile->map.coordinates;
        *size += 1;
    }
    if (tile->map.mapObj.orientation != direction)
    {
        if (direction == DOWN)
        {
            if (tile->map.coordinates.y != 2)
                AddToVisiblesList(direction, GetTileFromCoordinates(head, tile->map.coordinates.x, tile->map.coordinates.y + 1), head, seenTiles, size);
        }
        if (direction == LEFT)
        {
            if (tile->map.coordinates.x != 0)
                AddToVisiblesList(direction, GetTileFromCoordinates(head, tile->map.coordinates.x - 1, tile->map.coordinates.y), head, seenTiles, size);
        }
        if (direction == UP)
        {
            if (tile->map.coordinates.y != 0)
                AddToVisiblesList(direction, GetTileFromCoordinates(head, tile->map.coordinates.x, tile->map.coordinates.y - 1), head, seenTiles, size);
        }
        if (direction == RIGHT)
        {
            if (tile->map.coordinates.x != 2)
                AddToVisiblesList(direction, GetTileFromCoordinates(head, tile->map.coordinates.x + 1, tile->map.coordinates.y), head, seenTiles, size);
        }
    }
}

void SetupGame(struct GameState gameState, struct node **map,
               struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby,
               struct Renderable *activeATokens[4], int characters[9], int *jackIndex,
               int *susIndex, int *turn, int *round, int *tokensSelected, int *playState,
               SDL_Renderer *renderer, SDL_Point DTPositions[12], struct Renderable actionTokens[8])
{
    SetupMap(map, renderer, &(gameState.mData));
    SetupRenderableFromPath(holmes, renderer, "assets/detective_tokens/Holmes.jpg",
                            DTPositions[gameState.holmesPos].x, DTPositions[gameState.holmesPos].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, DOWN);
    SetupRenderableFromPath(watson, renderer, "assets/detective_tokens/Watson.jpg",
                            DTPositions[gameState.watsonPos].x, DTPositions[gameState.watsonPos].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, DOWN);
    SetupRenderableFromPath(toby, renderer, "assets/detective_tokens/Toby.jpg",
                            DTPositions[gameState.tobyPos].x, DTPositions[gameState.tobyPos].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, DOWN);

    for (int i = 0; i < 4; i++)
    {
        if (gameState.ActiveTokensIndex[i] != -1)
            activeATokens[i] = &(actionTokens[gameState.ActiveTokensIndex[i]]);
    }

    for (int i = 0; i < 9; i++)
    {
        characters[i] = gameState.characterCards[i];
    }

    *jackIndex = gameState.jackIndex;
    *susIndex = gameState.susIndex;
    *turn = gameState.turn;
    *round = gameState.round;
    *tokensSelected = gameState.tokensSelected;
    *playState = gameState.playState;
}