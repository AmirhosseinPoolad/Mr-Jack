#include "Map.h"
#include <stdlib.h>
#include <stdio.h>
#include "Renderable.h"

struct node *newNode(struct Map map)
{
    struct node *nn = malloc(sizeof(struct node));
    nn->next = NULL;
    nn->map = map;
    return nn;
}

int pushEnd(struct Map map, struct node **head)
{
    if (*head == NULL)
    {
        *head = newNode(map);
        return 0;
    }
    struct node *current;
    for (current = *head; current->next != NULL; current = current->next)
        ;
    current->next = newNode(map);
}

void SetupRandomMap(struct node **head, SDL_Renderer *rend)
{
    if (*head != NULL) //the map is already setup
        return;        //we don't need to do anything else

    struct SuspectTexture textures[9];
    struct SDL_Texture *mapTex = SetupTexture("assets/wall.jpg", rend);
    for (int i = 0; i < 9; i++)
    {
        char address[31];
        sprintf(address, "assets/suspects/suspects_%d.png", i);
        textures[i].tex = SetupTexture(address, rend);
        textures[i].isUsed = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            struct Renderable mapObj;
            SetupRenderableWithTexture(&mapObj, rend, mapTex,
                                       MAP_START_X + (j * TILE_WIDTH), MAP_START_Y + (i * TILE_HEIGHT),
                                       TILE_WIDTH, TILE_HEIGHT, rand() % 4);

            struct Renderable susObj;
            int index;
            while (1)
            {
                index = (rand() % 9);
                if (!textures[index].isUsed)
                {
                    SetupRenderableWithTexture(&susObj, rend, textures[index].tex,
                                               MAP_START_X + (j * TILE_WIDTH) + (TILE_WIDTH / 3),
                                               MAP_START_Y + (i * TILE_HEIGHT) + (TILE_HEIGHT / 3),
                                               TILE_WIDTH / 3, TILE_HEIGHT / 3, DOWN);
                    textures[index].isUsed = 1;
                    break;
                }
            }
            struct Map map;
            map.mapObj = mapObj;
            map.susObject = susObj;
            map.isShowingSuspect = 1;
            map.susIndex = index;
            SDL_Point coords = {j, i};
            map.coordinates = coords;
            pushEnd(map, head);
        }
    }
    GetTileFromCoordinates(head, 0, 0)->map.mapObj.orientation = LEFT;
    GetTileFromCoordinates(head, 2, 0)->map.mapObj.orientation = RIGHT;
    GetTileFromCoordinates(head, 1, 2)->map.mapObj.orientation = DOWN;
}

void SetupMap(struct node **head, SDL_Renderer *rend, const struct MapData *data)
{
    if (data->isRandom)
    {
        SetupRandomMap(head, rend);
        return;
    }

    if (*head != NULL) //the map is already setup
        return;        //we don't need to do anything else

    struct SuspectTexture textures[9];
    struct SDL_Texture *mapTex = SetupTexture("assets/wall.jpg", rend);
    for (int i = 0; i < 9; i++)
    {
        char address[31];
        sprintf(address, "assets/suspects/suspects_%d.png", i);
        textures[i].tex = SetupTexture(address, rend);
        textures[i].isUsed = 0;
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            struct Renderable mapObj;
            SetupRenderableWithTexture(&mapObj, rend, mapTex,
                                       MAP_START_X + (j * TILE_WIDTH), MAP_START_Y + (i * TILE_HEIGHT),
                                       TILE_WIDTH, TILE_HEIGHT, data->orientation[(3 * i) + j]);

            struct Renderable susObj;
            int index = data->susIndex[(3 * i) + j];

            SetupRenderableWithTexture(&susObj, rend, textures[index].tex,
                                       MAP_START_X + (j * TILE_WIDTH) + (TILE_WIDTH / 3),
                                       MAP_START_Y + (i * TILE_HEIGHT) + (TILE_HEIGHT / 3),
                                       TILE_WIDTH / 3, TILE_HEIGHT / 3, DOWN);
            textures[index].isUsed = 1;

            struct Map map;
            map.mapObj = mapObj;
            map.susObject = susObj;
            map.isShowingSuspect = data->isShowingSuspect[(3 * i) + j];
            map.susIndex = index;
            SDL_Point coords = {j, i};
            map.coordinates = coords;
            pushEnd(map, head);
        }
    }
}

void RenderMap(struct node **head, SDL_Renderer *rend)
{
    if (*head == NULL)
    {
        return;
    }
    struct node *current;
    for (current = *head; current != NULL; current = current->next)
    {
        GORender(&(current->map.mapObj), rend);
        if (current->map.isShowingSuspect)
            GORender(&(current->map.susObject), rend);
    }
}

struct node *GetTileFromCoordinates(struct node **head, int x, int y)
{
    //need to do 3y + x next operations
    int count = (3 * y) + x;
    struct node *current = *head;
    for (int i = 0; i < count; i++)
    {
        current = current->next;
    }
    return current;
}

struct node *GetTileFromScreenCoordinates(struct node **head, int screenX, int screenY)
{
    struct node *current;
    SDL_Point coords;
    coords.x = screenX;
    coords.y = screenY;
    for (current = *head; current != NULL; current = current->next)
    {
        if (SDL_PointInRect(&coords, &(current->map.mapObj.rect)))
            return current;
    }
    return NULL;
}

struct node *GetTileFromSusIndex(struct node **head, int susIndex)
{
    struct node *current;
    for (current = *head; current != NULL; current = current->next)
    {
        if (current->map.susIndex == susIndex)
            return current;
    }
    return NULL;
}

void SwapNodes(struct node **head, struct node *node1, struct node *node2)
{
    if (*head == NULL)
    {
        return;
    }

    struct node *prev1 = NULL;
    struct node *curr1 = *head;
    while (curr1 && curr1 != node1)
    {
        prev1 = curr1;
        curr1 = curr1->next;
    }

    struct node *prev2 = NULL;
    struct node *curr2 = *head;
    while (curr2 && curr2 != node2)
    {
        prev2 = curr2;
        curr2 = curr2->next;
    }
    if (curr1 == NULL || curr2 == NULL)
        return;

    if (prev1 == NULL) //node1 is head
        *head = curr2;
    else
        prev1->next = curr2;

    if (prev2 == NULL) //node2 is head
        *head = curr1;
    else
        prev2->next = curr1;

    struct node *tmp = curr2->next;
    curr2->next = curr1->next;
    curr1->next = tmp;
    SwapTiles(curr1, curr2);
}

void SwapTiles(struct node *node1, struct node *node2) //swaps the Renderables' SDL_Rect and Orientation
{
    struct Renderable tmp = node1->map.mapObj;
    node1->map.mapObj.rect = node2->map.mapObj.rect;
    node2->map.mapObj.rect = tmp.rect;

    tmp = node1->map.susObject;
    node1->map.susObject.rect = node2->map.susObject.rect;
    node1->map.susObject.orientation = node2->map.susObject.orientation;
    node2->map.susObject.rect = tmp.rect;
    node2->map.susObject.orientation = tmp.orientation;
}