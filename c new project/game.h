#ifndef GAME_H
#define GAME_H

/*
 * game.h
 * ------
 * Top-level Game structure: board, audio, texture, and state flags.
 *
 * Part of: Minesweeper (Raylib) — Refactored Academic Version
 * Author : [Student Name]
 * Date   : 2025
 */

#include "raylib.h"
#include "board.h"
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  Window / layout constants (used in main.c for drawing)             */
/* ------------------------------------------------------------------ */

#define CELL_SIZE   60
#define STATUS_BAR_H 50

/* ------------------------------------------------------------------ */
/*  Game structure                                                      */
/* ------------------------------------------------------------------ */

typedef struct {
    /* Board */
    Cell board[ROWS][COLS];

    /* State */
    bool gameOver;
    bool win;

    /* Audio */
    Sound sNumber;
    Sound sBoom;
    Sound sFlag;
    Sound sOver;
    Sound sWin;

    /* Texture */
    Texture2D texBoom;

    /* Play-once guards */
    bool playedBoomSound;
    bool playedGameOverSound;
    bool playedWinSound;
} Game;

/* ------------------------------------------------------------------ */
/*  Function prototypes                                                 */
/* ------------------------------------------------------------------ */

void Game_Init(Game *g);
void Game_Update(Game *g);
void Game_Cleanup(Game *g);

#endif /* GAME_H */
