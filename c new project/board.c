/*
 * board.c
 * -------
 * Board-level operations: init, mine placement, neighbour counting,
 * recursive reveal, and mine exposure on game-over.
 *
 * Part of: Minesweeper (Raylib) — Refactored Academic Version
 * Author : [Student Name]
 * Date   : 2025
 */

#include "board.h"
#include "raylib.h"
#include <stdlib.h>

/* Returns true when (row, col) is inside the board. */
static bool IsValidCell(int row, int col)
{
    return (row >= 0 && row < ROWS && col >= 0 && col < COLS);
}

void Board_Init(Cell board[ROWS][COLS])
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
            board[r][c].revealed    = false;
            board[r][c].hasMine     = false;
            board[r][c].flagged     = false;
            board[r][c].nearbyMines = 0;
        }
}

void Board_PlaceMines(Cell board[ROWS][COLS])
{
    int placed = 0;
    while (placed < MINES) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (!board[r][c].hasMine) {
            board[r][c].hasMine = true;
            placed++;
        }
    }
}

void Board_CountNearbyMines(Cell board[ROWS][COLS])
{
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (board[r][c].hasMine) continue;
            int count = 0;
            for (int dr = -1; dr <= 1; dr++)
                for (int dc = -1; dc <= 1; dc++) {
                    int nr = r + dr, nc = c + dc;
                    if (IsValidCell(nr, nc) && board[nr][nc].hasMine)
                        count++;
                }
            board[r][c].nearbyMines = count;
        }
    }
}

/* Recursive flood-fill. Plays number sound on every reveal (Option B). */
void Board_RevealEmpty(Cell board[ROWS][COLS], int row, int col, void *sNumber)
{
    Sound *sound = (Sound *)sNumber;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            int nr = row + dr, nc = col + dc;
            if (!IsValidCell(nr, nc))   continue;
            if (board[nr][nc].revealed) continue;
            if (board[nr][nc].hasMine)  continue;

            board[nr][nc].revealed = true;
            PlaySound(*sound);

            if (board[nr][nc].nearbyMines == 0)
                Board_RevealEmpty(board, nr, nc, sNumber);
        }
    }
}

void Board_RevealAllMines(Cell board[ROWS][COLS])
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (board[r][c].hasMine)
                board[r][c].revealed = true;
}

int Board_CountRevealedSafe(Cell board[ROWS][COLS])
{
    int count = 0;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (board[r][c].revealed && !board[r][c].hasMine)
                count++;
    return count;
}
