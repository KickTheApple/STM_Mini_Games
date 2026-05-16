#include "main.h"

#define X_AMOUNT 10
#define Y_AMOUNT 25

struct SuperCell{
    int isOccupied;
} typedef GameCell;

extern uint32_t xLength, yLength;

int celica_drawer(GameCell tetris[Y_AMOUNT][X_AMOUNT]) {
    
}

int tetra_drawer(GameCell tetris[Y_AMOUNT][X_AMOUNT]) {

}

int tetra_builder(GameCell tetris[Y_AMOUNT][X_AMOUNT]) {
    for (int i = 0; i < Y_AMOUNT; i++) {
        for (int j = 0; j < X_AMOUNT; j++) {
            tetris[i][j].isOccupied = 0;
        }
    }
}

int tetralnis(int* gameTracker) {

    GameCell tetris[Y_AMOUNT][X_AMOUNT];
    tetra_builder(tetris);



}