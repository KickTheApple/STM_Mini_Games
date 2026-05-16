#include "main.h"

#define GRID_LENGTH 8

#define EMPTY_SPACE 0
#define BLUE_SPACE 1
#define RED_SPACE 2

struct rock {
    uint8_t player;
    uint8_t is_promoted;
} typedef stoner;

struct move {
    uint8_t moveMode;
    uint8_t startX;
    uint8_t startY;
} typedef mover;

extern xLength, yLength;
int segmentCheckerX, segmentCheckerY;

uint8_t currentPlayer = BLUE_SPACE;

void DrawCheckField() {
    int currentDimension = 0;

    for (int i = 0; i < GRID_LENGTH; i++) {
        BSP_LCD_DrawVLine(0, currentDimension, 0, yLength, UTIL_LCD_COLOR_BLACK);
        currentDimension += segmentCheckerX;
    }

    currentDimension = 0;

    for (int i = 0; i <= GRID_LENGTH; i++) {
        BSP_LCD_DrawHLine(0, 0, currentDimension, xLength, UTIL_LCD_COLOR_BLACK);
        currentDimension += segmentCheckerY;
    }
}

void StoneBoardDraw(stoner stonerBoard[GRID_LENGTH][GRID_LENGTH], mover movingPiece) {
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

    int currentY = 0;
    for (int i = 0; i < GRID_LENGTH; i++) {
        int currentX = 0;
        for (int j = 0; j < GRID_LENGTH; j++) {
            if ((i+j) % 2 == 0) {
                UTIL_LCD_FillRect(currentX, currentY, segmentCheckerX, segmentCheckerY, UTIL_LCD_COLOR_GRAY);
            }
            if (stonerBoard[i][j].player == BLUE_SPACE) {
                UTIL_LCD_FillEllipse(currentX + segmentCheckerX / 2, currentY + segmentCheckerY / 2, segmentCheckerX / 2, segmentCheckerY / 2, UTIL_LCD_COLOR_BLUE);
            }
            if (stonerBoard[i][j].player == RED_SPACE) {
                UTIL_LCD_FillEllipse(currentX + segmentCheckerX / 2, currentY + segmentCheckerY / 2, segmentCheckerX / 2, segmentCheckerY / 2, UTIL_LCD_COLOR_RED);
            }
            currentX += segmentCheckerX;
        }
        currentY += segmentCheckerY;
    }

    DrawCheckField();
}

int IsCheckover(stoner stonerBoard[GRID_LENGTH][GRID_LENGTH]) {
    uint8_t pieceCount = 0;
    for (int i = 0; i < GRID_LENGTH; i++) {
        for (int j = 0; j < GRID_LENGTH; j++) {
            if (!stonerBoard[i][j].player) continue;
            if ((pieceCount & stonerBoard[i][j].player) == 0) {
                pieceCount += stonerBoard[i][j].player;
            }  
        }
    }
    if (pieceCount != BLUE_SPACE + RED_SPACE) {
        return pieceCount;
    }
    return 0;
}

int moveStone(stoner stonerBoard[GRID_LENGTH][GRID_LENGTH], int startX, int startY, int endX, int endY) {
    stonerBoard[endY][endX].is_promoted = stonerBoard[startY][startX].is_promoted;
    stonerBoard[endY][endX].player = stonerBoard[startY][startX].player;
    stonerBoard[startY][startX].is_promoted = 0;
    stonerBoard[startY][startX].player = 0;
    return 0;
}

int isLegalMove2(stoner stonerBoard[GRID_LENGTH][GRID_LENGTH], mover movingPiece, int finalX, int finalY) {
    int xDistance = finalX - movingPiece.startX;
    int yDistance = finalY - movingPiece.startY;

    if (abs(xDistance) != abs(yDistance)) {
        return 0;
    }

    int fullDistance = abs(xDistance);
    if (fullDistance > 2 || fullDistance <= 0) {
        return 0;
    }

    if (!stonerBoard[movingPiece.startY][movingPiece.startX].is_promoted) {
        if (currentPlayer == BLUE_SPACE) {
            if (yDistance <= 0) {
                return 0;
            }
            if (movingPiece.startX + xDistance < 0 || movingPiece.startX + xDistance >= GRID_LENGTH) {
                return 0;
            }
            if (movingPiece.startY + yDistance >= GRID_LENGTH) {
                return 0;
            }
        } else {
            if (yDistance >= 0) {
                return 0;
            }
            if (movingPiece.startX + xDistance < 0 || movingPiece.startX + xDistance >= GRID_LENGTH) {
                return 0;
            }
            if (movingPiece.startY + yDistance < 0) {
                return 0;
            }
        }
    } else {
        if (movingPiece.startX + xDistance < 0 || movingPiece.startX + xDistance >= GRID_LENGTH) {
            return 0;
        }
        if (movingPiece.startY + yDistance < 0 || movingPiece.startY + yDistance >= GRID_LENGTH) {
            return 0;
        }
    }
    if (fullDistance == 1 && !stonerBoard[movingPiece.startY+yDistance][movingPiece.startX+xDistance].player) return 1;
    if (fullDistance == 2 && !stonerBoard[movingPiece.startY+yDistance][movingPiece.startX+xDistance].player && stonerBoard[movingPiece.startY+yDistance-(yDistance/2)][movingPiece.startX+xDistance-(xDistance/2)].player && stonerBoard[movingPiece.startY+yDistance-(yDistance/2)][movingPiece.startX+xDistance-(xDistance/2)].player != currentPlayer) return 1;
    return 0;
}

int isLegalMove(stoner stonerBoard[GRID_LENGTH][GRID_LENGTH], mover movingPiece, int finalX, int finalY) {
    if (!stonerBoard[movingPiece.startY][movingPiece.startX].is_promoted) {
        if (currentPlayer == BLUE_SPACE) {
            if (finalX == movingPiece.startX-1 && finalY == movingPiece.startY+1 && movingPiece.startX != 0 && !stonerBoard[movingPiece.startY+1][movingPiece.startX-1].player) return 1;
            if (finalX == movingPiece.startX-2 && finalY == movingPiece.startY+2 && movingPiece.startX-2 >= 0 && movingPiece.startY+2 <= GRID_LENGTH-1 && stonerBoard[movingPiece.startY+1][movingPiece.startX-1].player && stonerBoard[movingPiece.startY+1][movingPiece.startX-1].player != currentPlayer && !stonerBoard[finalY][finalX].player) return 1;
            if (finalX == movingPiece.startX+1 && finalY == movingPiece.startY+1 && movingPiece.startX != GRID_LENGTH-1 && !stonerBoard[movingPiece.startY+1][movingPiece.startX+1].player) return 1;
            if (finalX == movingPiece.startX+2 && finalY == movingPiece.startY+2 && movingPiece.startX+2 <= GRID_LENGTH-1 && movingPiece.startY+2 <= GRID_LENGTH-1 && stonerBoard[movingPiece.startY+1][movingPiece.startX+1].player && stonerBoard[movingPiece.startY+1][movingPiece.startX+1].player != currentPlayer && !stonerBoard[finalY][finalX].player) return 1;
        } else {
            if (finalX == movingPiece.startX-1 && finalY == movingPiece.startY-1 && movingPiece.startX != 0 && !stonerBoard[movingPiece.startY-1][movingPiece.startX-1].player) return 1;
            if (finalX == movingPiece.startX-2 && finalY == movingPiece.startY-2 && movingPiece.startX-2 >= 0 && movingPiece.startY-2 <= GRID_LENGTH-1 && stonerBoard[movingPiece.startY-1][movingPiece.startX-1].player && stonerBoard[movingPiece.startY-1][movingPiece.startX-1].player != currentPlayer && !stonerBoard[finalY][finalX].player) return 1;
            if (finalX == movingPiece.startX+1 && finalY == movingPiece.startY-1 && movingPiece.startX != GRID_LENGTH-1 && !stonerBoard[movingPiece.startY-1][movingPiece.startX+1].player) return 1;
            if (finalX == movingPiece.startX+2 && finalY == movingPiece.startY-2 && movingPiece.startX+2 <= GRID_LENGTH-1 && movingPiece.startY-2 <= GRID_LENGTH-1 && stonerBoard[movingPiece.startY-1][movingPiece.startX+1].player && stonerBoard[movingPiece.startY-1][movingPiece.startX+1].player != currentPlayer && !stonerBoard[finalY][finalX].player) return 1;
        }
    } else {
        if (finalX == movingPiece.startX-1 && finalY == movingPiece.startY+1 && movingPiece.startX != 0 && !stonerBoard[movingPiece.startY+1][movingPiece.startX-1].player) return 1;
        if (finalX == movingPiece.startX-2 && finalY == movingPiece.startY+2 && movingPiece.startX-2 >= 0 && movingPiece.startY+2 <= GRID_LENGTH-1 && stonerBoard[movingPiece.startY+1][movingPiece.startX-1].player && stonerBoard[movingPiece.startY+1][movingPiece.startX-1].player != currentPlayer && !stonerBoard[finalY][finalX].player) return 1;
        if (finalX == movingPiece.startX+1 && finalY == movingPiece.startY+1 && movingPiece.startX != GRID_LENGTH-1 && !stonerBoard[movingPiece.startY+1][movingPiece.startX+1].player) return 1;
        if (finalX == movingPiece.startX+2 && finalY == movingPiece.startY+2 && movingPiece.startX+2 <= GRID_LENGTH-1 && movingPiece.startY+2 <= GRID_LENGTH-1 && stonerBoard[movingPiece.startY+1][movingPiece.startX+1].player && stonerBoard[movingPiece.startY+1][movingPiece.startX+1].player != currentPlayer && !stonerBoard[finalY][finalX].player) return 1;
        if (finalX == movingPiece.startX-1 && finalY == movingPiece.startY-1 && movingPiece.startX != 0 && !stonerBoard[movingPiece.startY-1][movingPiece.startX-1].player) return 1;
        if (finalX == movingPiece.startX-2 && finalY == movingPiece.startY-2 && movingPiece.startX-2 >= 0 && movingPiece.startY-2 <= GRID_LENGTH-1 && stonerBoard[movingPiece.startY-1][movingPiece.startX-1].player && stonerBoard[movingPiece.startY-1][movingPiece.startX-1].player != currentPlayer && !stonerBoard[finalY][finalX].player) return 1;
        if (finalX == movingPiece.startX+1 && finalY == movingPiece.startY-1 && movingPiece.startX != GRID_LENGTH-1 && !stonerBoard[movingPiece.startY-1][movingPiece.startX+1].player) return 1;
        if (finalX == movingPiece.startX+2 && finalY == movingPiece.startY-2 && movingPiece.startX+2 <= GRID_LENGTH-1 && movingPiece.startY-2 <= GRID_LENGTH-1 && stonerBoard[movingPiece.startY-1][movingPiece.startX+1].player && stonerBoard[movingPiece.startY-1][movingPiece.startX+1].player != currentPlayer && !stonerBoard[finalY][finalX].player) return 1;
    }
    return 0;
}

int CheckLogic(stoner stonerBoard[GRID_LENGTH][GRID_LENGTH], mover* movingPiece, int locationX, int locationY) {
    int gridPointX = locationX / segmentCheckerX;
    int gridPointY = locationY / segmentCheckerY;

    if (!movingPiece->moveMode) {
        if (stonerBoard[gridPointY][gridPointX].player == currentPlayer) {
            movingPiece->moveMode = 1;
            movingPiece->startX = gridPointX;
            movingPiece->startY = gridPointY;
            return 0;
        }
    } else {
        if (stonerBoard[gridPointY][gridPointX].player == currentPlayer) {
            movingPiece->moveMode = 1;
            movingPiece->startX = gridPointX;
            movingPiece->startY = gridPointY;
            return 0;
        }
        if (isLegalMove2(stonerBoard, *movingPiece, gridPointX, gridPointY)) {
            int distance = abs(movingPiece->startX - gridPointX);
            if (distance == 1) {
                moveStone(stonerBoard, movingPiece->startX, movingPiece->startY, gridPointX, gridPointY);
                movingPiece->moveMode = 0;
            } else if (distance == 2) {
                moveStone(stonerBoard, movingPiece->startX, movingPiece->startY, gridPointX, gridPointY);
                stonerBoard[(movingPiece->startY+gridPointY)/2][(movingPiece->startX+gridPointX)/2].is_promoted = 0;
                stonerBoard[(movingPiece->startY+gridPointY)/2][(movingPiece->startX+gridPointX)/2].player = 0;
                movingPiece->moveMode = 0;
            }
            
            if (distance == 1 || distance == 2) {
                if (currentPlayer == BLUE_SPACE) {
                    if (gridPointY == GRID_LENGTH-1) {
                        stonerBoard[gridPointY][gridPointX].is_promoted = 1;
                    }
                    currentPlayer = RED_SPACE;
                } else {
                    if (gridPointY == 0) {
                        stonerBoard[gridPointY][gridPointX].is_promoted = 1;
                    }
                    currentPlayer = BLUE_SPACE;
                }
            }
        }
    }

    return 0;
}

int TouchCheckHandling(stoner stonerBoard[GRID_LENGTH][GRID_LENGTH], mover* movingPiece, TS_State_t* touching, int* gameTracker) {
    BSP_TS_GetState(0, touching);
    if (touching->TouchDetected) {
        CheckLogic(stonerBoard, movingPiece, touching->TouchX, touching->TouchY);
        StoneBoardDraw(stonerBoard, *movingPiece);
        int endState = IsCheckover(stonerBoard);
        if (endState) {
            *gameTracker = 1;
            UTIL_LCD_SetFont(&Font24);
            UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
            if (endState == BLUE_SPACE) {
                UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);
                UTIL_LCD_DisplayStringAt(0, 250, (uint8_t *) "BLUE WINS", CENTER_MODE);
            } else {
                UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKRED);
                UTIL_LCD_DisplayStringAt(0, 250, (uint8_t *) "RED WINS", CENTER_MODE);
            }
        }
    }

    return 0;
}

void StoneWash(stoner stonerBoard[GRID_LENGTH][GRID_LENGTH]) {
    for (int i = 0; i < GRID_LENGTH; i++) {
        for (int j = 0; j < GRID_LENGTH; j++) {
            stonerBoard[i][j].player = 0;
            if (i < 3 && (j+i) % 2 == 1) {
                stonerBoard[i][j].player = 1;
            } else if (i >= 5 && (j+i) % 2 == 1) {
                stonerBoard[i][j].player = 2;
            }

            stonerBoard[i][j].is_promoted = 0;
        }
    }
}

void Checkering(int* gameTracker) {
    segmentCheckerX = xLength / GRID_LENGTH;
    segmentCheckerY = yLength / GRID_LENGTH;

    TS_State_t state;

    stoner stonerBoard[GRID_LENGTH][GRID_LENGTH];
    mover movingPiece = {0};

    StoneWash(stonerBoard);
    StoneBoardDraw(stonerBoard, movingPiece);

    while (1) {
        TouchCheckHandling(stonerBoard, &movingPiece, &state, gameTracker);
        if (*gameTracker) {
            HAL_Delay(5000);
            break;
        } 
    }

    return;
}
