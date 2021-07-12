#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "SDL.h"
#include "Renderable.h"
#include "Map.h"
#include "Game.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define AT_INITPOS_X 25
#define AT_INITPOS_Y 500
enum GameState //TODO: Implement these
{
    MAIN_MENU,
    LOAD_MENU,
    QUIT,
    GAME,
    PAUSE
};

enum PlayState
{
    DEAL_TOKEN,    //deal sction tokens
    SELECT_TOKEN,  //select action tokens
    REMOVE_TOKENS, //remove sus tokens TODO:IMPLEMENT THIS
    TILE_SWAP,
    TILE_ROTATE,
    HOLMES_MOVE,
    WATSON_MOVE,
    TOBY_MOVE,
    ANY_MOVE
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

void SelectToken(int *state, struct Renderable **clickedToken, struct Renderable *zerothToken);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Mr-Jack",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 255, 250, 250, SDL_ALPHA_OPAQUE);

    struct node *map = NULL;
    SetupMap(&map, renderer);

    SDL_Point DTPositions[12]; //positions for detective tokens around the board
                               //starts from top left and goes clockwise
    SetupDTPositions(DTPositions);
    struct Renderable holmes, watson, toby; //pos: 11,3,7
    SetupRenderableFromPath(&holmes, renderer, "assets/detective_tokens/Holmes.jpg",
                            DTPositions[11].x, DTPositions[11].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, UP);
    SetupRenderableFromPath(&watson, renderer, "assets/detective_tokens/Watson.jpg",
                            DTPositions[3].x, DTPositions[3].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, UP);
    SetupRenderableFromPath(&toby, renderer, "assets/detective_tokens/Toby.jpg",
                            DTPositions[7].x, DTPositions[7].y, TILE_WIDTH / 3, TILE_HEIGHT / 3, UP);
    struct Renderable actionTokens[8];
    SetupRenderableFromPath(&actionTokens[0], renderer, "assets/action_tokens/0.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y, TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
    SetupRenderableFromPath(&actionTokens[1], renderer, "assets/action_tokens/1.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y, TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);

    SetupRenderableFromPath(&actionTokens[2], renderer, "assets/action_tokens/2.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
    SetupRenderableFromPath(&actionTokens[3], renderer, "assets/action_tokens/3.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);

    SetupRenderableFromPath(&actionTokens[4], renderer, "assets/action_tokens/4.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + 2 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
    SetupRenderableFromPath(&actionTokens[5], renderer, "assets/action_tokens/5.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + 2 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);

    SetupRenderableFromPath(&actionTokens[6], renderer, "assets/action_tokens/6.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + 3 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
    SetupRenderableFromPath(&actionTokens[7], renderer, "assets/action_tokens/7.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + 3 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);

    struct Renderable confirmButton;
    SetupRenderableFromPath(&confirmButton, renderer, "assets/confirm.png",
                            AT_INITPOS_X, AT_INITPOS_Y - (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, UP);
    struct Renderable *activeATokens[4];

    struct Renderable jTurn, wTurn;
    SetupRenderableFromPath(&jTurn, renderer, "assets/jturn.png",
                            5, 5, 80, 40, UP);
    SetupRenderableFromPath(&wTurn, renderer, "assets/wturn.png",
                            5, 5, 80, 40, UP);

    int turn = 0; //0:watson, 1:mr jack
    int round = 1;
    int isQuit = 0;
    int tokensSelected = 0;
    int gameState;
    int playState = DEAL_TOKEN;
    while (!isQuit)
    {
        //1. Handle Input
        SDL_Event e;
        SDL_Point mousePos;
        int mouseDown = 0;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                isQuit = 1;
            if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)))
            {
                SDL_GetMouseState(&mousePos.x, &mousePos.y);
                mouseDown = 1;
            }
        }
        //2. Game Logic
        switch (playState)
        {
        case HOLMES_MOVE:
            //int a = 0;
            if (MoveDT(mouseDown, mousePos, &confirmButton, &holmes, DTPositions) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        case TOBY_MOVE:
            //int a = 0;
            if (MoveDT(mouseDown, mousePos, &confirmButton, &toby, DTPositions) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        case WATSON_MOVE:
            //int a = 0;
            if (MoveDT(mouseDown, mousePos, &confirmButton, &watson, DTPositions) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        case ANY_MOVE:
            if (MoveAny(mouseDown, mousePos, &confirmButton, &holmes, &watson, &toby, DTPositions, turn) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        case TILE_SWAP:
            if (ClickSwap(mouseDown, mousePos, &map) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        case TILE_ROTATE:
            if (ClickRotate(mouseDown, mousePos, &map, &confirmButton) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        case DEAL_TOKEN:
            for (int i = 0; i < 4; i++)
            {
                int random = rand() % 2;
                activeATokens[i] = &actionTokens[(2 * i) + random];
            }
            playState = SELECT_TOKEN;
            break;
        case SELECT_TOKEN:
            if (tokensSelected == 4)
            {
                playState = REMOVE_TOKENS;
                tokensSelected = 0;
            }

            if (round % 2 == 1) //odd round, watson starts
            {
                if (!turn) //watson's turn
                {
                    if (tokensSelected == 1)
                    {
                        turn = !turn;
                    }
                    if (mouseDown)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            if ((activeATokens[i] != NULL) && SDL_PointInRect(&mousePos, &(activeATokens[i]->rect)))
                            {
                                SelectToken(&playState, &activeATokens[i], &actionTokens[0]);
                                tokensSelected++;
                            }
                        }
                    }
                }
                if (turn) //jack's turn
                {
                    if (tokensSelected == 3)
                    {
                        turn = !turn;
                    }
                    if (mouseDown)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            if ((activeATokens[i] != NULL) && SDL_PointInRect(&mousePos, &(activeATokens[i]->rect)))
                            {
                                SelectToken(&playState, &activeATokens[i], &actionTokens[0]);
                                tokensSelected++;
                            }
                        }
                    }
                }
            }
            if (round % 2 == 0) //even round, jack starts
            {
                if (turn) //jack's turn
                {
                    if (tokensSelected == 1)
                    {
                        turn = !turn;
                    }
                    if (mouseDown)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            if ((activeATokens[i] != NULL) && SDL_PointInRect(&mousePos, &(activeATokens[i]->rect)))
                            {
                                SelectToken(&playState, &activeATokens[i], &actionTokens[0]);
                                tokensSelected++;
                            }
                        }
                    }
                }
                if (!turn) //watson's turn
                {
                    if (tokensSelected == 3)
                    {
                        turn = !turn;
                    }
                    if (mouseDown)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            if ((activeATokens[i] != NULL) && SDL_PointInRect(&mousePos, &(activeATokens[i]->rect)))
                            {
                                SelectToken(&playState, &activeATokens[i], &actionTokens[0]);
                                tokensSelected++;
                            }
                        }
                    }
                }
            }
            break;
        case REMOVE_TOKENS:
            playState = DEAL_TOKEN;
            round++;
            turn = !turn;
        default:
            if (ClickSwap(mouseDown, mousePos, &map) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        }
        //3. Render
        SDL_RenderClear(renderer);

        RenderMap(&map, renderer);
        RenderDT(DTPositions, &holmes, &watson, &toby, renderer);
        for (int i = 0; i < 4; i++)
        {
            GORender(activeATokens[i], renderer);
        }
        if (playState == TILE_ROTATE || playState == HOLMES_MOVE || playState == WATSON_MOVE || playState == TOBY_MOVE || playState == ANY_MOVE)
            GORender(&confirmButton, renderer);
        if (!turn)
            GORender(&wTurn, renderer);
        else
            GORender(&jTurn, renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}

void SelectToken(int *state, struct Renderable **clickedToken, struct Renderable *zerothToken)
{
    int index = (*clickedToken - zerothToken);
    switch (index)
    {
    case AT_SUSPECT:
        break;
    case AT_HOLMES:
        *state = HOLMES_MOVE;
        break;
    case AT_WATSON:
        *state = WATSON_MOVE;
        break;
    case AT_TOBY:
        *state = TOBY_MOVE;
        break;
    case AT_ROTATE1:
        *state = TILE_ROTATE;
        break;
    case AT_SWAP:
        *state = TILE_SWAP;
        break;
    case AT_ROTATE2:
        *state = TILE_ROTATE;
        break;
    case AT_ANY:
        *state = ANY_MOVE;
        break;
    default:
        break;
    }
    *clickedToken = NULL;
}