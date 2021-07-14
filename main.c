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
enum AppState //TODO: Implement these
{
    MAIN_MENU,
    LOAD_MENU,
    QUIT,
    GAME,
    PAUSE
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

void SelectToken(int *state, struct Renderable **clickedToken, struct Renderable *zerothToken);
void SeeTokens(struct Renderable *DT, SDL_Point DTPositions[12], struct node **map, SDL_Point seenTiles[9], int *size);

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
    struct MapData mapData;
    mapData.isRandom = 1;

    SDL_Point DTPositions[12]; //positions for detective tokens around the board
                               //starts from top left and goes clockwise
    SetupDTPositions(DTPositions);
    struct Renderable holmes, watson, toby;
    struct Renderable actionTokens[8];
    SetupRenderableFromPath(&actionTokens[0], renderer, "assets/action_tokens/0.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y, TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);
    SetupRenderableFromPath(&actionTokens[1], renderer, "assets/action_tokens/1.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y, TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);

    SetupRenderableFromPath(&actionTokens[2], renderer, "assets/action_tokens/2.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);
    SetupRenderableFromPath(&actionTokens[3], renderer, "assets/action_tokens/3.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);

    SetupRenderableFromPath(&actionTokens[4], renderer, "assets/action_tokens/4.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + 2 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);
    SetupRenderableFromPath(&actionTokens[5], renderer, "assets/action_tokens/5.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + 2 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);

    SetupRenderableFromPath(&actionTokens[6], renderer, "assets/action_tokens/6.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + 3 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);
    SetupRenderableFromPath(&actionTokens[7], renderer, "assets/action_tokens/7.jpg",
                            AT_INITPOS_X, AT_INITPOS_Y + 3 * (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);

    struct Renderable confirmButton;
    SetupRenderableFromPath(&confirmButton, renderer, "assets/confirm.png",
                            AT_INITPOS_X, AT_INITPOS_Y - (TILE_HEIGHT / 2), TILE_WIDTH / 2, TILE_HEIGHT / 2, DOWN);
    struct Renderable *activeATokens[4] = {NULL, NULL, NULL, NULL};

    struct Renderable jTurn, wTurn, jWin, hWin;
    SetupRenderableFromPath(&jTurn, renderer, "assets/jturn.png",
                            5, 5, 80, 40, DOWN);
    SetupRenderableFromPath(&wTurn, renderer, "assets/wturn.png",
                            5, 5, 80, 40, DOWN);
    SetupRenderableFromPath(&jWin, renderer, "assets/jwins.png",
                            200, 200, 400, 400, DOWN);
    SetupRenderableFromPath(&hWin, renderer, "assets/hwins.png",
                            200, 200, 400, 400, DOWN);

    int characters[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8}; //these are the suspect cards
    for (int i = 9 - 1; i > 0; i--)                  //we shuffle them
    {
        int j = rand() % (i + 1);
        int tmp = characters[i];
        characters[i] = characters[j];
        characters[j] = tmp;
    }
    struct Renderable susCard;
    struct Renderable jack; //UNCOMMENT TO SEE JACK

    int jackIndex = characters[0]; //and the first card is jack's identity
    int susIndex = 1;              //we use this index to access the cards. value of 1 means we never see the first card again.
    int turn = 0;                  //0:watson, 1:mr jack
    char address[50];
    sprintf(address, "assets/char_cards/char_%d.jpg", jackIndex);
    SetupRenderableFromPath(&jack, renderer, address, 573, 452, 222, 343, DOWN); //UNCOMMENT TO SEE JACK
    int round = 1;
    int isQuit = 0;
    int tokensSelected = 0;
    int gameState;
    int playState = REVEAL_JACK;
    struct GameState initState;// = {mapData, 11, 3, 7, {-1, -1, -1, -1}, NULL, characters[0], 1, 0, 1, 0, REVEAL_JACK};
    initState.mData = mapData;
    initState.holmesPos = 11;
    initState.watsonPos = 3;
    initState.tobyPos = 7;
    for(int i = 0;i<4;i++)
    {
        initState.ActiveTokensIndex[i] = -1;
    }
    for(int i = 0;i<9;i++)
    {
        initState.characterCards[i] = characters[i];
    }
    initState.jackIndex = characters[0];
    initState.susIndex = 1;
    initState.turn = 0;
    initState.round = 1;
    initState.tokensSelected = 0;
    initState.playState = REVEAL_JACK;
    SetupGame(initState, &map, &holmes, &watson, &toby, activeATokens, characters, &jackIndex, &susIndex, &turn, &round, &tokensSelected, &playState, renderer, DTPositions, actionTokens);
    int timer = 0, lastFrame = 0, deltaTime, isFirstTime = 1;
    while (!isQuit)
    {
        //0. Timing calculations
        int ticks = SDL_GetTicks();
        deltaTime = ticks - lastFrame;
        lastFrame = ticks;
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
        case REVEAL_JACK:
        {
            timer += deltaTime;
            if (timer >= 2000) //stay in this state for a while second
            {
                playState = DEAL_TOKEN;
                timer = 0;
            }
        }
        case HOLMES_MOVE:
        {
            if (MoveDT(mouseDown, mousePos, &confirmButton, &holmes, DTPositions) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        }
        case TOBY_MOVE:
        {
            if (MoveDT(mouseDown, mousePos, &confirmButton, &toby, DTPositions) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        }
        case WATSON_MOVE:
        {
            if (MoveDT(mouseDown, mousePos, &confirmButton, &watson, DTPositions) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        }
        case ANY_MOVE:
        {
            if (MoveAny(mouseDown, mousePos, &confirmButton, &holmes, &watson, &toby, DTPositions, turn) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        }
        case TILE_SWAP:
        {
            if (ClickSwap(mouseDown, mousePos, &map) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        }
        case TILE_ROTATE:
        {
            if (ClickRotate(mouseDown, mousePos, &map, &confirmButton) == 1)
            {
                playState = SELECT_TOKEN;
            }
            break;
        }
        case DEAL_TOKEN:
        {
            for (int i = 0; i < 4; i++)
            {
                int random = rand() % 2;
                activeATokens[i] = &actionTokens[(2 * i) + random];
            }
            playState = SELECT_TOKEN;
            break;
        }
        case SELECT_TOKEN:
        {
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
        }
        case AT_SUSPECT_REVEAL:
        {
            if (turn) //jack picked this token, just remove a card from character cards
            {
                susIndex++;
                playState = SELECT_TOKEN;
            }
            if (!turn) //watson picked this token, reveal the first character chard
            {
                timer += deltaTime;
                int index = characters[susIndex];
                struct node *tile = GetTileFromSusIndex(&map, index);
                //if(tile->map.isShowingSuspect != 0){
                if (isFirstTime)
                {
                    char address[50];
                    sprintf(address, "assets/char_cards/char_%d.jpg", index);
                    SetupRenderableFromPath(&susCard, renderer, address, 573, 452, 222, 343, DOWN);
                    isFirstTime = 0;
                }
                if (timer >= 1500) // >1.5 seconds
                {
                    tile->map.isShowingSuspect = 0;
                    susIndex++;
                    timer = 0;
                    isFirstTime = 1;
                    FreeRenderable(&susCard);
                    playState = SELECT_TOKEN;
                }
            }

            break;
        }
        case REMOVE_TOKENS:
        {
            //1. find and remove tokens from map
            playState = DEAL_TOKEN;
            SDL_Point seenTiles[9];
            int size = 0;
            int isJackSeen = 0;
            SeeTokens(&holmes, DTPositions, &map, seenTiles, &size);
            SeeTokens(&watson, DTPositions, &map, seenTiles, &size);
            SeeTokens(&toby, DTPositions, &map, seenTiles, &size);
            for (int i = 0; i < size; i++)
            {
                if ((GetTileFromCoordinates(&map, seenTiles[i].x, seenTiles[i].y)->map.susIndex) == jackIndex)
                    isJackSeen = 1;
            }
            if (isJackSeen == 0)
            {
                for (int i = 0; i < size; i++)
                {
                    (GetTileFromCoordinates(&map, seenTiles[i].x, seenTiles[i].y)->map.isShowingSuspect) = 0;
                }
            }
            else
            {
                struct node *current;
                for (current = map; current != NULL; current = current->next)
                {
                    current->map.isShowingSuspect = 0;
                }
                for (int i = 0; i < size; i++)
                {
                    (GetTileFromCoordinates(&map, seenTiles[i].x, seenTiles[i].y)->map.isShowingSuspect) = 1;
                }
            }
            // 2. See if anyone won
            int visibleTokens = 0;
            struct node *current;
            for (current = map; current != NULL; current = current->next)
            {
                if (current->map.isShowingSuspect == 1)
                    visibleTokens++;
            }
            if (visibleTokens <= 1)
            {
                playState = HOLMES_WIN;
            }
            if (round == 8 && visibleTokens > 1) //final round just finished
            {
                playState = JACK_WIN;
            }
            round++;
            turn = !turn;
            break;
        }
        case JACK_WIN:
        case HOLMES_WIN:
            break;
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
        if (playState == AT_SUSPECT_REVEAL && !isFirstTime)
            GORender(&susCard, renderer);
        if (playState == HOLMES_WIN)
            GORender(&hWin, renderer);
        if (playState == JACK_WIN)
            GORender(&jWin, renderer);
        if (playState == REVEAL_JACK)
            GORender(&jack, renderer); //UNCOMMENT TO SEE JACK
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
        *state = AT_SUSPECT_REVEAL;
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

void SeeTokens(struct Renderable *DT, SDL_Point DTPositions[12], struct node **map, SDL_Point seenTiles[9], int *size)
{
    int index = GetDTIndex(DTPositions, DT);
    int x, y;
    x = MapXCoordsFromDTIndex(index);
    y = MapXCoordsFromDTIndex(11 - index);
    struct node *startingTile = GetTileFromCoordinates(map, x, y);
    if (index <= 2)
    {
        if (startingTile->map.mapObj.orientation != UP)
            AddToVisiblesList(DOWN, startingTile, map, seenTiles, size);
    }
    else if (index <= 5)
    {
        if (startingTile->map.mapObj.orientation != RIGHT)
            AddToVisiblesList(LEFT, startingTile, map, seenTiles, size);
    }
    else if (index <= 8)
    {
        if (startingTile->map.mapObj.orientation != DOWN)
            AddToVisiblesList(UP, startingTile, map, seenTiles, size);
    }
    else if (index <= 11)
    {
        if (startingTile->map.mapObj.orientation != LEFT)
            AddToVisiblesList(RIGHT, startingTile, map, seenTiles, size);
    }
}