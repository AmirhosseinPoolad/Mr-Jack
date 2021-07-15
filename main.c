#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "SDL.h"
#include "Renderable.h"
#include "Map.h"
#include "Game.h"

enum AppState //TODO: Implement these
{
    MAIN_MENU,
    LOAD_MENU,
    QUIT,
    GAME
};

int main(int argc, char *argv[])
{
    int appState = MAIN_MENU;
    while (appState != QUIT)
    {
        switch (appState)
        {
        case MAIN_MENU:
        {
            printf("1.Play\n2.Load\n3.quit\n");
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
                appState = GAME;
                break;
            case 2:
                appState = LOAD_MENU;
                break;
            case 3:
                appState = QUIT;
                break;
            }
            break;
        }
        case GAME:
        {
            srand(time(NULL));
            RunGame(NULL);
            appState = MAIN_MENU;
            break;
        }
        case LOAD_MENU:
        {
            printf("Input address of save file\n");
            char address[256];
            scanf("%s", address);
            srand(time(NULL));
            RunGame(address);
            appState = MAIN_MENU;
            break;
        }
        }
    }
    return 0;
}