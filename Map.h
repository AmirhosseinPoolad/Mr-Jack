#ifndef MAP_H
#define MAP_H

#include "GameObject.h"

struct node
{
    struct GameObject obj;
    struct node *next;
};

struct node *newNode(struct GameObject obj);
int pushEnd(struct GameObject obj, struct node **head);

void SetupMap(struct node **head, SDL_Renderer *rend);
void RenderMap(struct node **head, SDL_Renderer *rend);
struct node *GetFromCoordinates(struct node **head, int x, int y);
void FindCoordinates(struct node **head, struct node *input, int *x, int *y); //TODO

#endif