#include <gtk/gtk.h>
#include "Game.h"

Game *createGame() {
    Game *game = g_malloc(sizeof(Game));
    game->currentPlayer = FirstPlayer;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            Cell *cell = g_malloc(sizeof(Cell));
            cell->value = Empty;
            cell->row = row;
            cell->col = col;
            cell->color.red = 0;
            cell->color.green = 0;
            cell->color.blue = 0;
            cell->color.alpha = 1;
            game->grid[row][col] = cell;
        }
    }

    game->moveCount = 0;
    game->state = Running;

    return game;
}

void destroyGame(Game *game) {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            g_free(game->grid[row][col]);
        }
    }
    g_free(game);
}

void endGame(Game *game) {
    game->state = Finished;
    g_print("Game is over\n");
}

void restartGame(Game *game) {
    destroyGame(game);
    game = createGame();
    redrawGridCells(game);
}

void redrawGridCells(Game *game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            gtk_widget_queue_draw(game->grid[i][j]->widget);
        }
    }
}

void makeGameMove(Game *game, struct CellStruct *cell) {
    if (game->currentPlayer == FirstPlayer) {
        cell->value = X;
    } else {
        cell->value = O;
    }

    game->moveCount++;

    game->currentPlayer =
            game->currentPlayer == FirstPlayer ? SecondPlayer : FirstPlayer;
}

gboolean doWeHaveWinner(Game *game, struct CellStruct *lastUpdatedCell, struct CellStruct *winning_cells[3]) {
    int n = 3;

    // Check row
    for (int i = 0; i < n; i++) {
        Cell *actual_cell = game->grid[lastUpdatedCell->row][i];
        winning_cells[i] = actual_cell;
        if (actual_cell->value != lastUpdatedCell->value)
            break;
        if (i == n - 1) {
            return TRUE;
        }
    }

    // Check column
    for (int i = 0; i < n; i++) {
        Cell *actual_cell = game->grid[i][lastUpdatedCell->col];
        winning_cells[i] = actual_cell;
        if (actual_cell->value != lastUpdatedCell->value)
            break;
        if (i == n - 1) {
            return TRUE;
        }
    }

    // Check diagonal
    if (lastUpdatedCell->row == lastUpdatedCell->col) {
        for (int i = 0; i < n; i++) {
            Cell *actualCell = game->grid[i][i];
            winning_cells[i] = actualCell;
            if (actualCell->value != lastUpdatedCell->value)
                break;
            if (i == n - 1) {
                return TRUE;
            }
        }
    }

    // Check opposite diagonal
    if (lastUpdatedCell->row + lastUpdatedCell->col == n - 1) {
        for (int i = 0; i < n; i++) {
            Cell *actual_cell = game->grid[i][(n - 1) - i];
            winning_cells[i] = actual_cell;
            if (actual_cell->value != lastUpdatedCell->value)
                break;
            if (i == n - 1) {
                return TRUE;
            }
        }
    }

    return FALSE;
}