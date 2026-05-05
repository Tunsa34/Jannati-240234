/*
 * GameView.cpp  —  VIEW layer
 * ----------------------------
 * Implements all GameView class methods declared in GameView.h.
 *
 * MVC Role  : View
 *   This file is the ONLY place in the entire project where
 *   Raylib Draw* functions are called. All visual rendering
 *   logic — cell backgrounds, borders, numbers, flags, mine
 *   explosions, status messages — lives exclusively here.
 *
 *   GameView reads Model state through const accessors and
 *   converts it to pixels. It does not modify the Model and
 *   does not process input.
 *
 * Part of: Minesweeper — C++ MVC Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "GameView.h"

/* ============================================================
   Constructor
   Stores a const reference to the Model for read-only access.
   ============================================================ */

GameView::GameView(const GameModel& model)
    : model(model)
{
}

/* ============================================================
   draw
   Called once per frame from main().
   Must be called between BeginDrawing() and EndDrawing().
   Clears the background then delegates to helper draw functions.
   ============================================================ */

void GameView::draw() const
{
    ClearBackground(COLOR_BACKGROUND);
    drawBoard();
    drawStatusBar();
}

/* ============================================================
   drawBoard
   Iterates through all TOTAL_ROWS × TOTAL_COLS cells and
   delegates each one to drawCell().
   ============================================================ */

void GameView::drawBoard() const
{
    for (int row = 0; row < TOTAL_ROWS; row++)
        for (int col = 0; col < TOTAL_COLS; col++)
            drawCell(row, col);
}

/* ============================================================
   drawCell
   Draws one cell at grid position (row, col).

   Visual states:
     - revealed mine     → explosion texture scaled to fill cell
     - revealed, count>0 → nearby mine count number centred
     - revealed, count=0 → beige background only (blank)
     - unrevealed, flag  → red triangle flag symbol
     - unrevealed, plain → green background only

   All pixel offsets and colours match the original C code exactly.
   ============================================================ */

void GameView::drawCell(int row, int col) const
{
    const Cell& cell = model.getBoard().getCell(row, col);

    int cellLeft = col * CELL_PIXEL_SIZE;
    int cellTop  = row * CELL_PIXEL_SIZE;

    Rectangle cellRect = {
        static_cast<float>(cellLeft),
        static_cast<float>(cellTop),
        static_cast<float>(CELL_PIXEL_SIZE),
        static_cast<float>(CELL_PIXEL_SIZE)
    };

    /* --- 1. Background fill — checkerboard pattern --- */
    bool isEvenSquare = ((row + col) % 2 == 0);

    if (cell.isRevealed)
    {
        DrawRectangleRec(cellRect,
                         isEvenSquare ? COLOR_REV_LIGHT : COLOR_REV_DARK);
    }
    else
    {
        DrawRectangleRec(cellRect,
                         isEvenSquare ? COLOR_UNREV_LIGHT : COLOR_UNREV_DARK);
    }

    /* --- 2. Grey border line --- */
    DrawRectangleLinesEx(cellRect, 1, COLOR_BORDER);

    /* --- 3. Cell content --- */
    if (cell.isRevealed)
    {
        if (cell.hasMine)
        {
            /* Draw explosion image scaled to fill the cell */
            const Texture2D& tex = model.getMineExplosionImage();

            Rectangle imageSrc  = { 0.0f, 0.0f,
                                    static_cast<float>(tex.width),
                                    static_cast<float>(tex.height) };
            Rectangle imageDest = { static_cast<float>(cellLeft),
                                    static_cast<float>(cellTop),
                                    static_cast<float>(CELL_PIXEL_SIZE),
                                    static_cast<float>(CELL_PIXEL_SIZE) };

            DrawTexturePro(tex, imageSrc, imageDest,
                           { 0.0f, 0.0f }, 0.0f, WHITE);
        }
        else if (cell.nearbyMineCount > 0)
        {
            /* Draw nearby mine count number centred in the cell */
            int textX = cellLeft + CELL_PIXEL_SIZE / 2 - 8;
            int textY = cellTop  + CELL_PIXEL_SIZE / 2 - 12;

            DrawText(TextFormat("%d", cell.nearbyMineCount),
                     textX, textY, 25, BLUE);
        }
        /* nearbyMineCount == 0 and not a mine → blank, draw nothing */
    }
    else if (cell.isFlagged)
    {
        /* Draw red triangle as flag — exact same vertices as original C code */
        float cx = static_cast<float>(cellLeft) + CELL_PIXEL_SIZE / 2.0f;
        float cy = static_cast<float>(cellTop)  + CELL_PIXEL_SIZE / 2.0f;

        DrawTriangle(
            { cx - 8.0f, cy + 8.0f  },   /* bottom-left  */
            { cx - 8.0f, cy - 12.0f },   /* top-left     */
            { cx + 8.0f, cy - 2.0f  },   /* right-middle */
            RED
        );
    }
}

/* ============================================================
   drawStatusBar
   Draws the status message in the bar at the bottom:
     - RED   "GAME OVER!" when player hit a mine
     - GREEN "YOU WIN!"   when all safe cells are revealed
     - WHITE instructions during normal gameplay

   Text positions match the original C code exactly.
   ============================================================ */

void GameView::drawStatusBar() const
{
    int messageY = TOTAL_ROWS * CELL_PIXEL_SIZE + 10;

    if (model.isGameOver())
        DrawText("💥 GAME OVER!", 10, messageY, 30, RED);
    else if (model.isPlayerWin())
        DrawText("🎉 YOU WIN!", 10, messageY, 30, GREEN);
    else
        DrawText("Left-click: Reveal | Right-click: Flag",
                 10, messageY + 5, 20, RAYWHITE);
}
