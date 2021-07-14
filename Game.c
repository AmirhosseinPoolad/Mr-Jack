#include "Game.h"
#include "Map.h"
#include "Renderable.h"
#include "math.h"
#include "stdio.h"

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
        else
            activeATokens[i] = NULL;
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

struct GameState ReadGameStateFromFile(char *address)
{
    struct GameState gameState;
    FILE *fp = fopen(address, "rb");
    fread(&gameState, sizeof(struct GameState), 1, fp);
    fclose(fp);
    return gameState;
}
void WriteGameStateToFile(char *address, struct GameState gameState)
{
    FILE *fp = fopen(address, "wb");
    fwrite(&gameState, sizeof(struct GameState), 1, fp);
    fclose(fp);
}

void RunGame(char *address)
{
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
    struct Renderable susCard, jack;

    int jackIndex; //and the first card is jack's identity
    int susIndex;  //we use this index to access the cards. value of 1 means we never see the first card again.
    int turn;      //0:watson, 1:mr jack
    int round, isQuit = 0, tokensSelected, gameState, playState;
    struct GameState initState;
    if (address == NULL)
    {
        struct MapData mapData;
        mapData.isRandom = 1;
        initState.mData = mapData;
        initState.holmesPos = 11;
        initState.watsonPos = 3;
        initState.tobyPos = 7;
        for (int i = 0; i < 4; i++)
        {
            initState.ActiveTokensIndex[i] = -1;
        }
        for (int i = 0; i < 9; i++)
        {
            initState.characterCards[i] = characters[i];
        }
        initState.jackIndex = characters[0];
        initState.susIndex = 1;
        initState.turn = 0;
        initState.round = 1;
        initState.tokensSelected = 0;
        initState.playState = REVEAL_JACK;
    }
    /* this makes the initial game state
    initState.mData = mapData;
    initState.holmesPos = 11;
    initState.watsonPos = 3;
    initState.tobyPos = 7;
    for (int i = 0; i < 4; i++)
    {
        initState.ActiveTokensIndex[i] = -1;
    }
    for (int i = 0; i < 9; i++)
    {
        initState.characterCards[i] = characters[i];
    }
    initState.jackIndex = characters[0];
    initState.susIndex = 1;
    initState.turn = 0;
    initState.round = 1;
    initState.tokensSelected = 0;
    initState.playState = REVEAL_JACK;
    WriteGameStateToFile("save.bin",initState);*/
    else
    {
        initState = ReadGameStateFromFile(address);
    }
    SetupGame(initState, &map, &holmes, &watson, &toby, activeATokens, characters, &jackIndex, &susIndex, &turn, &round, &tokensSelected, &playState, renderer, DTPositions, actionTokens);
    if (playState == REVEAL_JACK)
    {
        char address[50];
        sprintf(address, "assets/char_cards/char_%d.jpg", jackIndex);
        SetupRenderableFromPath(&jack, renderer, address, 573, 452, 222, 343, DOWN);
    }
    int timer = 0, lastFrame = 0, deltaTime, isFirstTime = 1;
    int prevState = playState, pauseState = MAIN, quitPause = 0;
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
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    if (playState != PAUSE_MENU)
                    {
                        prevState = playState;
                        playState = PAUSE_MENU;
                        pauseState = MAIN;
                        quitPause = 0;
                    }
                }
            }
            if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)))
            {
                SDL_GetMouseState(&mousePos.x, &mousePos.y);
                mouseDown = 1;
            }
        }
        //2. Game Logic
        switch (playState)
        {
        case PAUSE_MENU:
        {
            while (!quitPause)
            {
                switch (pauseState)
                {
                case MAIN:
                    printf("1.Resume\n2.Save\n3.Quit\n");
                    int input;
                    scanf("%d", &input);
                    while (input != 1 && input != 2 && input != 3)
                    {
                        printf("Invalid input, try again\n");
                        scanf("%d", &input);
                    }
                    switch (input)
                    {
                    case 1:
                        pauseState = RESUME;
                        break;
                    case 2:
                        pauseState = SAVE;
                        break;
                    case 3:
                        pauseState = QUIT_PLAY;
                        break;
                    }
                    break;
                case RESUME:
                    playState = prevState;
                    quitPause = 1;
                    break;
                case SAVE:
                {
                    printf("Input name of save file with .bin\n");
                    char address[256];
                    scanf("%s", address);
                    struct MapData mData = GetDataFromMap(&map);
                    struct GameState gState;
                    gState.mData = mData;
                    gState.holmesPos = GetDTIndex(DTPositions, &holmes);
                    gState.watsonPos = GetDTIndex(DTPositions, &watson);
                    gState.tobyPos = GetDTIndex(DTPositions, &toby);
                    for (int i = 0; i < 4; i++)
                    {
                        if (activeATokens[i] == NULL)
                        {
                            gState.ActiveTokensIndex[i] = -1;
                        }
                        else
                        {
                            gState.ActiveTokensIndex[i] = activeATokens[i] - &(actionTokens[0]);
                        }
                    }
                    for (int i = 0; i < 9; i++)
                    {
                        gState.characterCards[i] = characters[i];
                    }
                    gState.jackIndex = jackIndex;
                    gState.susIndex = susIndex;
                    gState.turn = turn;
                    gState.round = round;
                    gState.tokensSelected = tokensSelected;
                    gState.playState = prevState;
                    WriteGameStateToFile(address, gState);
                    pauseState = MAIN;
                    break;
                }
                case QUIT_PLAY:
                    playState = prevState;
                    isQuit = 1;
                    quitPause = 1;
                    break;
                }
            }

            break;
        }
        case REVEAL_JACK:
        {
            timer += deltaTime;
            if (timer >= 2000) //stay in this state for a while second
            {
                playState = DEAL_TOKEN;
                FreeRenderable(&jack);
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
            GORender(&jack, renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    //Game is over, need to clean up
    FreeRenderable(&holmes);
    FreeRenderable(&watson);
    FreeRenderable(&toby);
    for (int i = 0; i < 8; i++)
    {
        FreeRenderable(&actionTokens[i]);
    }
    FreeRenderable(&jTurn);
    FreeRenderable(&jWin);
    FreeRenderable(&wTurn);
    FreeRenderable(&hWin);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}