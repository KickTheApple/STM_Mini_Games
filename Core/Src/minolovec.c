#include "main.h"
#include <time.h>

#define X_AMOUNT 12
#define Y_AMOUNT 8
#define BOMB_COUNT 5

#define TOP_FLAG 1
#define RIGHT_FLAG 2
#define BOTTOM_FLAG 4
#define LEFT_FLAG 8


struct bombar {
    uint8_t isBomb;
    uint8_t isRevealed;
    uint8_t neighbor;
} typedef bombarnik;

extern xLength, yLength;
int segmentMineX, segmentMineY;
int spaceNumber;

void DrawMineField() {
    int currentX = 0;
    int currentY = 0;

    for (int i = 0; i < X_AMOUNT; i++) {
        BSP_LCD_DrawVLine(0, currentX, 0, yLength, UTIL_LCD_COLOR_BLACK);
        currentX += segmentMineX;
    }

    for (int i = 0; i <= Y_AMOUNT; i++) {
        BSP_LCD_DrawHLine(0, 0, currentY, xLength, UTIL_LCD_COLOR_BLACK);
        currentY += segmentMineY;
    }
}

void DrawMineGame(bombarnik bombPolje[Y_AMOUNT][X_AMOUNT]) {
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

    UTIL_LCD_SetFont(&Font12);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ORANGE);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

    int locationX = 0;
    int locationY = 0;
    char buffer[16];

    for (int i = 0; i < Y_AMOUNT; i++) {
        locationX = 0;
        for (int j = 0; j < X_AMOUNT; j++) {
            if (!bombPolje[i][j].isRevealed) {
                locationX += segmentMineX;
                continue;
            }
            if (bombPolje[i][j].isBomb) {
                UTIL_LCD_DrawRect(locationX, locationX, segmentMineX, segmentMineY, UTIL_LCD_COLOR_RED);
                locationX += segmentMineX;
                continue;
            }
            sprintf(buffer, "%d", bombPolje[i][j].neighbor);
            UTIL_LCD_DisplayStringAt(locationX, locationY, (uint8_t*) buffer, LEFT_MODE);
            locationX += segmentMineX;
        }
        locationY += segmentMineY;
    }

    DrawMineField();
}

int isMineOver(bombarnik bombPolje[Y_AMOUNT][X_AMOUNT]) {
    for (int i = 0; i < Y_AMOUNT; i++) {
        for (int j = 0; j < X_AMOUNT; j++) {
            if (bombPolje[i][j].isRevealed && bombPolje[i][j].isBomb) {
                return 1;
            }
        }
    }
    return 0;
}

void revealeration(bombarnik bombPolje[Y_AMOUNT][X_AMOUNT], int x, int y) {
    int xReinterpreted = x / segmentMineX;
    int yReinterpreted = y / segmentMineY;
    if (!bombPolje[yReinterpreted][xReinterpreted].isRevealed) {
        bombPolje[yReinterpreted][xReinterpreted].isRevealed = 1;
    }
}

void TouchMineHandling(bombarnik bombPolje[Y_AMOUNT][X_AMOUNT], TS_State_t* touchpadState, int* gameTracker) {
    BSP_TS_GetState(0, touchpadState);
    if (touchpadState->TouchDetected) {
        revealeration(bombPolje, touchpadState->TouchX, touchpadState->TouchY);
        DrawMineGame(bombPolje);
        if (isMineOver(bombPolje)) {
            UTIL_LCD_SetFont(&Font24);
            UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKRED);
            UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
            UTIL_LCD_DisplayStringAt(0, 250, (uint8_t *) "GAME OVER", CENTER_MODE);

            *gameTracker = 1;
            HAL_Delay(5000);
        }
    }
}

void Grid_greeter(bombarnik bombPolje[Y_AMOUNT][X_AMOUNT], bombarnik poljak, int x, int y) {
    if (poljak.isBomb) {
        return;
    }

    int runningCount = 0;
    uint8_t blockFlagger = 0;

    if (x == 0) {
        blockFlagger = blockFlagger | LEFT_FLAG;
    }
    if (y == 0) {
        blockFlagger = blockFlagger | TOP_FLAG;
    }
    if (x == X_AMOUNT-1) {
        blockFlagger = blockFlagger | RIGHT_FLAG;
    }
    if (y == Y_AMOUNT-1) {
        blockFlagger = blockFlagger | BOTTOM_FLAG;
    }

    if (!blockFlagger) {
        runningCount += bombPolje[y-1][x-1].isBomb;
        runningCount += bombPolje[y-1][x].isBomb;
        runningCount += bombPolje[y-1][x+1].isBomb;
        runningCount += bombPolje[y][x-1].isBomb;
        runningCount += bombPolje[y][x+1].isBomb;
        runningCount += bombPolje[y+1][x-1].isBomb;
        runningCount += bombPolje[y+1][x].isBomb;
        runningCount += bombPolje[y+1][x+1].isBomb;
    } else {
        if ((blockFlagger & TOP_FLAG) == blockFlagger) {
            runningCount += bombPolje[y][x-1].isBomb;
            runningCount += bombPolje[y][x+1].isBomb;
            runningCount += bombPolje[y+1][x-1].isBomb;
            runningCount += bombPolje[y+1][x].isBomb;
            runningCount += bombPolje[y+1][x+1].isBomb;
        } else if ((blockFlagger & RIGHT_FLAG) == blockFlagger) {
            runningCount += bombPolje[y-1][x-1].isBomb;
            runningCount += bombPolje[y-1][x].isBomb;
            runningCount += bombPolje[y][x-1].isBomb;
            runningCount += bombPolje[y+1][x-1].isBomb;
            runningCount += bombPolje[y+1][x].isBomb;
        } else if ((blockFlagger & BOTTOM_FLAG) == blockFlagger) {
            runningCount += bombPolje[y-1][x-1].isBomb;
            runningCount += bombPolje[y-1][x].isBomb;
            runningCount += bombPolje[y-1][x+1].isBomb;
            runningCount += bombPolje[y][x-1].isBomb;
            runningCount += bombPolje[y][x+1].isBomb;
        } else if ((blockFlagger & LEFT_FLAG) == blockFlagger) {
            runningCount += bombPolje[y-1][x].isBomb;
            runningCount += bombPolje[y-1][x+1].isBomb;
            runningCount += bombPolje[y][x+1].isBomb;
            runningCount += bombPolje[y+1][x].isBomb;
            runningCount += bombPolje[y+1][x+1].isBomb;
        } else if ((blockFlagger | LEFT_FLAG) == blockFlagger && (blockFlagger | TOP_FLAG) == blockFlagger) {
            runningCount += bombPolje[y][x+1].isBomb;
            runningCount += bombPolje[y+1][x].isBomb;
            runningCount += bombPolje[y+1][x+1].isBomb;
        } else if ((blockFlagger | TOP_FLAG) == blockFlagger && (blockFlagger | RIGHT_FLAG) == blockFlagger) {
            runningCount += bombPolje[y][x-1].isBomb;
            runningCount += bombPolje[y+1][x-1].isBomb;
            runningCount += bombPolje[y+1][x].isBomb;
        } else if ((blockFlagger | RIGHT_FLAG) == blockFlagger && (blockFlagger | BOTTOM_FLAG) == blockFlagger) {
            runningCount += bombPolje[y-1][x-1].isBomb;
            runningCount += bombPolje[y-1][x].isBomb;
            runningCount += bombPolje[y][x-1].isBomb;
        } else if ((blockFlagger | BOTTOM_FLAG) == blockFlagger && (blockFlagger | LEFT_FLAG) == blockFlagger) {
            runningCount += bombPolje[y-1][x].isBomb;
            runningCount += bombPolje[y-1][x+1].isBomb;
            runningCount += bombPolje[y][x+1].isBomb;
        }
    }
    poljak.neighbor = runningCount;
}

void Neighbor_greeter(bombarnik bombPolje[Y_AMOUNT][X_AMOUNT]) {
    for (int i = 0; i < Y_AMOUNT; i++) {
        for (int j = 0; j < X_AMOUNT; j++) {
            Grid_greeter(bombPolje, bombPolje[i][j], j, i);
        }
    }
}

void Bombarnik_randomizer(bombarnik bombPolje[Y_AMOUNT][X_AMOUNT]) {
    bombarnik simplePolje[spaceNumber];
    int positioner = 0;
    for (int i = 0; i < Y_AMOUNT; i++) {
        for (int j = 0; j < X_AMOUNT; j++) {
            simplePolje[positioner] = bombPolje[i][j];
            positioner++;
        }
    }

    int length = X_AMOUNT * Y_AMOUNT;
    int currentLocation = length;

    for (int i = 0; i < length; i++) {
        int j = rand() % currentLocation + i;

        bombarnik holder = simplePolje[i];
        simplePolje[i] = simplePolje[j];
        simplePolje[j] = holder;

        currentLocation--;
    }

    for (int i = 0; i < BOMB_COUNT; i++) {
        simplePolje[i].isBomb = 1;
    }
}

void Bombarnik_starter(bombarnik bombPolje[Y_AMOUNT][X_AMOUNT]) {
    for (int i = 0; i < Y_AMOUNT; i++) {
        for (int j = 0; j < X_AMOUNT; j++) {
            bombPolje[i][j].isBomb = 0;
            bombPolje[i][j].isRevealed = 0;
            bombPolje[i][j].neighbor = 0;
        }
    }
}

void Bombica(int* gameTracker) {
    
    segmentMineX = xLength / X_AMOUNT;
    segmentMineY = yLength / Y_AMOUNT;

    TS_State_t state;

    spaceNumber = X_AMOUNT*Y_AMOUNT;
    bombarnik bombPolje[Y_AMOUNT][X_AMOUNT];

    srand(time(NULL));

    Bombarnik_starter(bombPolje);
    Bombarnik_randomizer(bombPolje);
    Neighbor_greeter(bombPolje);

    while (1) {
        TouchMineHandling(bombPolje, &state, gameTracker);
    }

}