/*
 * pisalec.c
 *
 *  Created on: Feb 16, 2026
 *      Author: domenic
 */

#include "main.h"

#define X_AMOUNT 24
#define Y_AMOUNT 12

struct Pastir {
    int x;
    int y;
    int head;
    struct Pastir* paster;
} typedef Pastirnik;

struct Hrana {
    int x;
    int y;
} typedef Fruit;

/*
    Move below code back into picasso if it doesnt work
*/
uint32_t xLength, yLength;
int segmentX, segmentY;

void DestroyPastir(Pastirnik* pastirStart) {
    Pastirnik* copyPastirCurrent = pastirStart;
    Pastirnik* copyPastirFuture = copyPastirCurrent->paster;

    while (copyPastirCurrent != NULL) {
        free(copyPastirCurrent);
        copyPastirCurrent = copyPastirFuture;
        if (copyPastirCurrent != NULL) {
            copyPastirFuture = copyPastirCurrent->paster;
        }
    } 
}


int EatingFruit(Pastirnik* pastorStart, Fruit loop) {
    Pastirnik* copirnik = pastorStart;
    while(copirnik != NULL) {
        if (copirnik->x == loop.x && copirnik->y == loop.y) {
            return 1;
        }
        copirnik = copirnik->paster;
    }
    return 0;
}



void DrawSnakeField() {
    int currentX = 0;
    int currentY = 0;

    for (int i = 0; i < X_AMOUNT; i++) {
        BSP_LCD_DrawVLine(0, currentX, 0, yLength, UTIL_LCD_COLOR_BLACK);
        currentX += segmentX;
    }

    for (int i = 0; i < Y_AMOUNT; i++) {
        BSP_LCD_DrawHLine(0, 0, currentY, xLength, UTIL_LCD_COLOR_BLACK);
        currentY += segmentY;
    }
}

void DrawSegmentAt(int x, int y, uint32_t color) {
    int locationX = segmentX * x;
    int locationY = segmentY * y;

    BSP_LCD_FillRect(0, locationX, locationY, segmentX, segmentY, color);
}

void DrawGame(Pastirnik* pastirBegin, Fruit* loop) {
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
    
    DrawSegmentAt(loop->x, loop->y, UTIL_LCD_COLOR_ST_GREEN_DARK);

    Pastirnik* copyPastor = pastirBegin;
    while (copyPastor != NULL) {
        DrawSegmentAt(copyPastor->x, copyPastor->y, UTIL_LCD_COLOR_ORANGE);
        copyPastor = copyPastor->paster;
    }
    
    DrawSnakeField();
}

int isOver(Pastirnik* pastirnik) {
    Pastirnik* headOfPastirnik = pastirnik;
    while (headOfPastirnik->paster != NULL) {
        headOfPastirnik = headOfPastirnik->paster;
    }

    Pastirnik* copyOfPastirnik = pastirnik;
    while (copyOfPastirnik->paster != NULL) {
        if (copyOfPastirnik->x == headOfPastirnik->x && copyOfPastirnik->y == headOfPastirnik->y) {
            return 1;
        }
        copyOfPastirnik = copyOfPastirnik->paster;
    }
    return 0;
}

void TouchHandling(Pastirnik* pastir, TS_State_t* touchpadState) {
    BSP_TS_GetState(0, touchpadState);
    if (touchpadState->TouchDetected) {
        Pastirnik* endOfPastir = pastir;
        while (endOfPastir->paster != NULL) {
            endOfPastir = endOfPastir->paster;
        }
        if (endOfPastir->head == 1 || endOfPastir->head == 3) {
            if (endOfPastir->y*segmentY < touchpadState->TouchY) {
                endOfPastir->head = 4;
            } else {
                endOfPastir->head = 2;
            }
        } else {
            if (endOfPastir->x*segmentX < touchpadState->TouchX) {
                endOfPastir->head = 1;
            } else {
                endOfPastir->head = 3;
            }
        }
    }
}

void Movement(Pastirnik* beginOfPastir, Pastirnik* endOfPastirnik) {

	endOfPastirnik->paster = beginOfPastir;
    beginOfPastir->paster = NULL;

    int glavica = endOfPastirnik->head;
    endOfPastirnik->head = 0;
	beginOfPastir->head = glavica;

    switch (glavica) {
    case 1:
        if (endOfPastirnik->x+1 == X_AMOUNT) {
            beginOfPastir->x = 0;
        } else {
            beginOfPastir->x = endOfPastirnik->x+1;
        }
        beginOfPastir->y = endOfPastirnik->y;
        break;
    case 2:
        beginOfPastir->x = endOfPastirnik->x;
        if (endOfPastirnik->y-1 < 0) {
            beginOfPastir->y = Y_AMOUNT-1;
        } else {
        	beginOfPastir->y = endOfPastirnik->y-1;
        }
        break;
    case 3:
        if (endOfPastirnik->x-1 < 0) {
            beginOfPastir->x = X_AMOUNT-1;
        } else {
            beginOfPastir->x = endOfPastirnik->x-1;
        }
        beginOfPastir->y = endOfPastirnik->y;
        break;
    case 4:
        beginOfPastir->x = endOfPastirnik->x;
        if (endOfPastirnik->y+1 == Y_AMOUNT) {
            beginOfPastir->y = 0;
        } else {
            beginOfPastir->y = endOfPastirnik->y+1;
        }
        break;
    
    default:
        break;
    }

}

void GameCycle(Pastirnik** pastirnik, Fruit* loop) {
    Pastirnik* beginOfPastir = *pastirnik;

    Pastirnik* endOfPastirnik = *pastirnik;
    while (endOfPastirnik->paster != NULL) {
        endOfPastirnik = endOfPastirnik->paster;
    }

    if (EatingFruit(*pastirnik, *loop) == 1) {
        loop->x = rand() % X_AMOUNT;
        loop->y = rand() % Y_AMOUNT;

        Pastirnik* rojenec = malloc(sizeof(Pastirnik));

        Movement(rojenec, endOfPastirnik);
        return;
    }
    if (beginOfPastir->paster != NULL) {
        *pastirnik = beginOfPastir->paster;
    }    
    Movement(beginOfPastir, endOfPastirnik);
}

void Picasso() {
    BSP_LCD_GetXSize(0, &xLength);
    BSP_LCD_GetYSize(0, &yLength);

    segmentX = xLength / X_AMOUNT;
    segmentY = yLength / Y_AMOUNT;

    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetFont(&Font24);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *) "Worked", CENTER_MODE);

    TS_State_t state;

    srand(time(NULL));

    Pastirnik* pastorStart = malloc(sizeof(Pastirnik));
    pastorStart->x = 2;
    pastorStart->y = 2;
    pastorStart->head = 1;
    pastorStart->paster = NULL;

    Fruit loop;
    loop.x = rand() % X_AMOUNT;
    loop.y = rand() % Y_AMOUNT;

    while (1) {
        if(isOver(pastorStart)) {
            UTIL_LCD_SetFont(&Font24);
            UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKRED);
            UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
            UTIL_LCD_DisplayStringAt(0, 250, (uint8_t *) "GAME OVER", CENTER_MODE);
            HAL_Delay(5000);
            break;
        }
        TouchHandling(pastorStart, &state);
        GameCycle(&pastorStart, &loop);
        DrawGame(pastorStart, &loop);
        HAL_Delay(1000);
    }

}
