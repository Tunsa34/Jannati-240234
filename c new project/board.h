#ifndef BOARD_H
#define BOARD_H

/*
 * board.h
 * -------
 * Cell structure and all board-level operations.
 *
 * Part of: Minesweeper (Raylib) — Refactored Academic Version
 * Author : [Student Name]
 * Date   : 2025
 */

#include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  Board constants                                                     */
/* ------------------------------------------------------------------ */

#define ROWS    9
#define COLS    9
#define MINES   10

/* ------------------------------------------------------------------ */
/*  Cell structure                                                      */
/* ------------------------------------------------------------------ */

typedef struct {
    bool revealed;
    bool hasMine;
    bool flagged;
    int  nearbyMines;
} Cell;

/* ------------------------------------------------------------------ */
/*  Function prototypes                                                 */
/* ------------------------------------------------------------------ */

void Board_Init(Cell board[ROWS][COLS]);
void Board_PlaceMines(Cell board[ROWS][COLS]);
void Board_CountNearbyMines(Cell board[ROWS][COLS]);
void Board_RevealEmpty(Cell board[ROWS][COLS], int row, int col, void *sNumber);
void Board_RevealAllMines(Cell board[ROWS][COLS]);
int  Board_CountRevealedSafe(Cell board[ROWS][COLS]);

#endif /* BOARD_H */
