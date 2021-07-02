#include <stdlib.h>
#include <stdio.h>
#include "GameObject.h"
#include "Map.h"

#define MAP_START_X 100
#define MAP_START_Y 60
#define TILE_HEIGHT 100
#define TILE_WIDTH 100

void MapUpdate(struct GameObject *obj)
{
    obj->orientation++;
    obj->orientation = (obj->orientation) % 4;
}

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

void SetupMap(struct node **head, SDL_Renderer *rend)
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
            struct GameObject mapObj;
            SetupGameObjectWithTexture(&mapObj, rend, mapTex, MapUpdate,
                                       MAP_START_X + (j * TILE_WIDTH), MAP_START_Y + (i * TILE_HEIGHT),
                                       TILE_WIDTH, TILE_HEIGHT, rand() % 4);

            struct GameObject susObj;

            while (1)
            {
                int index = rand() % 9;
                if (!textures[index].isUsed)
                {
                    SetupGameObjectWithTexture(&susObj, rend, textures[index].tex, NULL,
                                               MAP_START_X + (j * TILE_WIDTH) + (TILE_WIDTH / 3),
                                               MAP_START_Y + (i * TILE_HEIGHT) + (TILE_HEIGHT / 3),
                                               TILE_WIDTH / 3, TILE_HEIGHT / 3, UP);
                    textures[index].isUsed = 1;
                    break;
                }
            }
            struct Map map;
            map.mapObj = mapObj;
            map.susObject = susObj;
            map.isShowingSuspect = 1;
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

struct node *GetFromCoordinates(struct node **head, int x, int y)
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

void FindCoordinates(struct node **head, struct node *input, int *x, int *y)
{
    int count = 0;
    struct node *current = *head;
    while (1)
    {
        if (current == input)
            break;
        count++;
    }
    *y = count / 3;
    *x = count % 3;
}