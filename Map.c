#include <stdlib.h>
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

struct node *newNode(struct GameObject obj)
{
    struct node *nn = malloc(sizeof(struct node));
    nn->next = NULL;
    nn->obj = obj;
    return nn;
}

int pushEnd(struct GameObject obj, struct node **head)
{
    if (*head == NULL)
    {
        *head = newNode(obj);
        return 0;
    }
    struct node *current;
    for (current = *head; current->next != NULL; current = current->next)
        ;
    current->next = newNode(obj);
}

void SetupMap(struct node **head, SDL_Renderer *rend)
{
    if (*head != NULL) //the map is already setup
        return;        //we don't need to do anything else
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            struct GameObject obj;
            SetupGameObject(&obj, rend, "assets/wall.jpg", MapUpdate,
                            MAP_START_X + (j * TILE_WIDTH), MAP_START_Y + (i * TILE_HEIGHT),
                            TILE_WIDTH, TILE_HEIGHT, rand() % 4);
            pushEnd(obj, head);
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
        GORender(&(current->obj), rend);
    }
}

struct node *GetCoordinates(struct node **head, int x, int y)
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