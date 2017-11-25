#ifndef GUX2_GAME_H
#define GUX2_GAME_H

#include "Cell.h"
#include "Player.h"

enum GameStateEnum {
    Running,
    Finished
} typedef GameState;

struct GameStruct {
    Cell *grid[3][3];
    Player currentPlayer;
    int moveCount;
    GameState state;
} typedef Game;

Game *createGame();

void destroyGame(struct GameStruct *game);

void endGame(struct GameStruct *game);

void restartGame(struct GameStruct *game);

void redrawGridCells(struct GameStruct *game);

void makeGameMove(struct GameStruct *game, struct CellStruct *cell);

gboolean
doWeHaveWinner(struct GameStruct *game, struct CellStruct *lastUpdatedCell, struct CellStruct *winning_cells[3]);

#endif //GUX2_GAME_H
