#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Renderable.h"

struct GameState
{
    struct MapData mData;
    int holmesPos, watsonPos, tobyPos;
    int ActiveTokensIndex[4];
    int characterCards[9];
    int jackIndex;
    int susIndex; //where we are in the character card array
    int turn;
    int round;
    int tokensSelected;
    int playState;
};

enum PlayState
{
    REVEAL_JACK,
    DEAL_TOKEN,    //deal sction tokens
    SELECT_TOKEN,  //select action tokens
    REMOVE_TOKENS, //remove sus tokens TODO:IMPLEMENT THIS
    TILE_SWAP,
    TILE_ROTATE,
    HOLMES_MOVE,
    WATSON_MOVE,
    TOBY_MOVE,
    ANY_MOVE,
    AT_SUSPECT_REVEAL,
    HOLMES_WIN,
    JACK_WIN
};

enum ActionTokens
{
    AT_SUSPECT, //removes a card from sus cards TODO: IMPLEMENT SUSPECT CARDS
    AT_HOLMES,  //moves holmes 1 or 2 tiles clockwise
    AT_TOBY,    //same
    AT_WATSON,  //same
    AT_ROTATE1, //rotate 90 or 180 degrees
    AT_SWAP,    //swap two tiles
    AT_ROTATE2,
    AT_ANY //move any of the detective 1 tiles clockwise
};

int MoveDT(int mouseDown, SDL_Point mousePos, struct Renderable *confirmButton, struct Renderable *dt, SDL_Point DTPositions[12]);
int MoveAny(int mouseDown, SDL_Point mousePos, struct Renderable *confirmButton,
            struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby,
            SDL_Point DTPositions[12], int turn);
int ClickSwap(int mouseDown, SDL_Point mousePos, struct node **map);
int ClickRotate(int mouseDown, SDL_Point mousePos, struct node **map, struct Renderable *confirmButton);
void SetupDTPositions(SDL_Point DTPositions[12]);
void IncrementDTPos(SDL_Point DTPositions[12], struct Renderable *dt, int amount);
int GetDTIndex(SDL_Point DTPositions[12], struct Renderable *dt);
int IncrementDTIndex(int index, int amount);
void RenderDT(SDL_Point DTPositions[12], struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby, SDL_Renderer *renderer);
int MapXCoordsFromDTIndex(int index);
void AddToVisiblesList(enum Orientation direction, struct node *tile, struct node **head, SDL_Point seenTiles[9], int *size);
void SelectToken(int *state, struct Renderable **clickedToken, struct Renderable *zerothToken);
void SeeTokens(struct Renderable *DT, SDL_Point DTPositions[12], struct node **map, SDL_Point seenTiles[9], int *size);
void SetupGame(struct GameState gameState, struct node **map,
               struct Renderable *holmes, struct Renderable *watson, struct Renderable *toby,
               struct Renderable *activeATokens[4], int characters[9], int *jackIndex,
               int *susIndex, int *turn, int *round, int *tokensSelected, int *playState,
               SDL_Renderer *renderer, SDL_Point DTPositions[12], struct Renderable actionTokens[8]);

struct GameState ReadGameStateFromFile(char *address);

void WriteGameStateToFile(char *address,struct GameState gameState);

#endif