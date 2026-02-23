/*
 * game.c
 * ------
 * Game lifecycle and per-frame input/state logic.
 * No drawing code here — all graphics stay in main.c exactly as original.
 *
 * Part of: Minesweeper (Raylib) — Refactored Academic Version
 * Author : [Student Name]
 * Date   : 2025
 */

#include "game.h"
#include "board.h"
#include <stdlib.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/*  Static helpers                                                      */
/* ------------------------------------------------------------------ */

/* Converts mouse position to board cell. Returns false if out of bounds. */
static bool GetMouseCell(int *row, int *col)
{
    Vector2 mouse = GetMousePosition();
    int c = (int)(mouse.x / CELL_SIZE);
    int r = (int)(mouse.y / CELL_SIZE);

    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return false;
    *row = r;
    *col = c;
    return true;
}

/* Left-click: reveal cell, trigger boom or flood-fill. */
static void HandleLeftClick(Game *g, int row, int col)
{
    Cell *cell = &g->board[row][col];
    if (cell->flagged || cell->revealed) return;

    cell->revealed = true;

    if (cell->hasMine) {
        if (!g->playedBoomSound) {
            PlaySound(g->sBoom);
            g->playedBoomSound = true;
        }
        g->gameOver = true;
        Board_RevealAllMines(g->board);
        if (!g->playedGameOverSound) {
            PlaySound(g->sOver);
            g->playedGameOverSound = true;
        }
    } else {
        PlaySound(g->sNumber);
        if (cell->nearbyMines == 0)
            Board_RevealEmpty(g->board, row, col, &g->sNumber);
    }
}

/* Right-click: toggle flag. */
static void HandleRightClick(Game *g, int row, int col)
{
    Cell *cell = &g->board[row][col];
    if (cell->revealed) return;
    cell->flagged = !cell->flagged;
    PlaySound(g->sFlag);
}

/* Check if all safe cells are revealed. */
static void CheckWinCondition(Game *g)
{
    if (Board_CountRevealedSafe(g->board) == ROWS * COLS - MINES) {
        g->win = true;
        if (!g->playedWinSound) {
            PlaySound(g->sWin);
            g->playedWinSound = true;
        }
    }
}

/* Safety net: plays deferred sounds if state was set indirectly. */
static void HandleDeferredSounds(Game *g)
{
    if (g->gameOver && !g->playedGameOverSound) {
        PlaySound(g->sOver);
        g->playedGameOverSound = true;
    }
    if (g->win && !g->playedWinSound) {
        PlaySound(g->sWin);
        g->playedWinSound = true;
    }
}

/* ------------------------------------------------------------------ */
/*  Public lifecycle functions                                          */
/* ------------------------------------------------------------------ */

void Game_Init(Game *g)
{
    srand((unsigned)time(NULL));

    Board_Init(g->board);
    Board_PlaceMines(g->board);
    Board_CountNearbyMines(g->board);

    g->gameOver            = false;
    g->win                 = false;
    g->playedBoomSound     = false;
    g->playedGameOverSound = false;
    g->playedWinSound      = false;

    g->sNumber  = LoadSound("number.mp3");
    g->sBoom    = LoadSound("boom.mp3");
    g->sFlag    = LoadSound("flag.mp3");
    g->sOver    = LoadSound("over.mp3");
    g->sWin     = LoadSound("win.mp3");
    g->texBoom  = LoadTexture("boomm.png");
}

void Game_Update(Game *g)
{
    if (!g->gameOver && !g->win) {
        int row, col;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && GetMouseCell(&row, &col))
            HandleLeftClick(g, row, col);

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && GetMouseCell(&row, &col))
            HandleRightClick(g, row, col);

        CheckWinCondition(g);
    } else {
        HandleDeferredSounds(g);
    }
}

void Game_Cleanup(Game *g)
{
    UnloadSound(g->sNumber);
    UnloadSound(g->sBoom);
    UnloadSound(g->sFlag);
    UnloadSound(g->sOver);
    UnloadSound(g->sWin);
    UnloadTexture(g->texBoom);
}
