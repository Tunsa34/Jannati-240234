#ifndef GAME_VIEW_H
#define GAME_VIEW_H

/*
 * GameView.h  —  VIEW layer
 * --------------------------
 * Declares the GameView class responsible for all rendering.
 *
 * MVC Role  : View
 *   - ALL Raylib Draw* calls in the entire project live here.
 *   - Receives read-only access to GameModel and renders it.
 *   - Has NO game logic.  Has NO input handling.
 *   - Never modifies the Model's state.
 *
 * SOLID alignment:
 *   SRP — GameView does one thing: turn Model state into pixels.
 *   OCP — New visual elements (animations, overlays, counters) are
 *         added by extending GameView without touching Model or
 *         Controller.
 *   DIP — GameView depends on the public read-only API of GameModel,
 *         not on internal implementation details.
 *
 * Part of: Minesweeper — C++ MVC Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "raylib.h"
#include "../model/GameModel.h"

/* ============================================================
   GameView
   Owns all rendering logic for the Minesweeper game.
   Constructed once in main(); draw() is called every frame.
   ============================================================ */

class GameView {
public:

    /* Constructor — stores a const reference to the Model */
    explicit GameView(const GameModel& model);

    /* Draw the complete frame: background, board, status bar.
       Called inside BeginDrawing() / EndDrawing() in main(). */
    void draw() const;

private:

    /* Reference to the Model — read-only, never modified here */
    const GameModel& model;

    /* ---- Private drawing helpers ---- */

    /* Draw a single cell at grid position (row, col) */
    void drawCell(int row, int col) const;

    /* Draw all 81 cells */
    void drawBoard() const;

    /* Draw the status bar at the bottom (game-over / win / instructions) */
    void drawStatusBar() const;

    /* ---- Colour constants (named, not magic literals) ---- */

    static constexpr Color COLOR_BACKGROUND  = {  48,  99,  47, 255 };
    static constexpr Color COLOR_UNREV_LIGHT = { 190, 224, 145, 255 };
    static constexpr Color COLOR_UNREV_DARK  = { 170, 214, 135, 255 };
    static constexpr Color COLOR_REV_LIGHT   = { 240, 210, 170, 255 };
    static constexpr Color COLOR_REV_DARK    = { 225, 195, 150, 255 };
    static constexpr Color COLOR_BORDER      = { 110, 110, 110, 255 };
};

#endif /* GAME_VIEW_H */
