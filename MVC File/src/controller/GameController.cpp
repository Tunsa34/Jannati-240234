/*
 * GameController.cpp  —  CONTROLLER layer
 * -----------------------------------------
 * Implements all GameController class methods declared in GameController.h.
 *
 * MVC Role  : Controller
 *   This file reads Raylib input (mouse buttons, pixel positions)
 *   and translates every event into calls on GameModel.
 *   It never draws anything. It never stores game-state data.
 *   It is the sole point of communication between input and Model.
 *
 * Data flow each frame:
 *   Raylib input → GameController::handleInput()
 *                      → GameModel::revealCell()       (left-click)
 *                      → GameModel::handleGameOver()   (if mine hit)
 *                      → GameModel::revealConnectedEmpty() (flood-fill)
 *                      → GameModel::toggleFlag()       (right-click)
 *                      → GameModel::checkWinCondition()
 *
 * Part of: Minesweeper — C++ MVC Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "GameController.h"

/* ============================================================
   Constructor
   Stores non-owning references to Model and View.
   The Controller does not create or destroy Model or View.
   ============================================================ */

GameController::GameController(GameModel& model, GameView& view)
    : model(model), view(view)
{
}

/* ============================================================
   handleInput
   Called once per frame from the game loop in main().
   Reads input only if the game is still in progress.
   After each interaction, re-checks the win condition.
   ============================================================ */

void GameController::handleInput()
{
    /* Do nothing if the game has already ended */
    if (model.isGameOver() || model.isPlayerWin())
        return;

    int row = 0;
    int col = 0;

    /* Left-click: reveal a cell */
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (getMouseCell(row, col))
            processLeftClick(row, col);
    }

    /* Right-click: place or remove a flag */
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        if (getMouseCell(row, col))
            processRightClick(row, col);
    }

    /* Re-evaluate the win condition after every interaction */
    model.checkWinCondition();
}

/* ============================================================
   getMouseCell  (private)
   Converts the current mouse pixel position to a board cell.
   Writes the result into rowOut and colOut.
   Returns true if the position maps to a valid cell.
   ============================================================ */

bool GameController::getMouseCell(int& rowOut, int& colOut) const
{
    int mousePixelX = static_cast<int>(GetMouseX());
    int mousePixelY = static_cast<int>(GetMouseY());

    int col = mousePixelX / CELL_PIXEL_SIZE;
    int row = mousePixelY / CELL_PIXEL_SIZE;

    if (!Board::isInsideBoard(row, col)) return false;

    rowOut = row;
    colOut = col;
    return true;
}

/* ============================================================
   processLeftClick  (private)
   Called when the player left-clicks inside the grid.

   Workflow:
     1. Skip already-revealed or flagged cells (no-op).
     2. Ask the Model to reveal the cell.
     3. If it was a mine → tell the Model to handle game-over.
     4. If it was a safe empty cell → trigger flood-fill.
     5. If it was a safe numbered cell → play click sound.
   ============================================================ */

void GameController::processLeftClick(int row, int col)
{
    const Cell& cell = model.getBoard().getCell(row, col);

    /* Ignore already-revealed or flagged cells */
    if (cell.isRevealed || cell.isFlagged) return;

    /* Ask the Model to reveal the cell; returns true on mine */
    bool hitMine = model.revealCell(row, col);

    if (hitMine)
    {
        /* Mine hit: Model handles sounds and exposes all mines */
        model.handleGameOver();
    }
    else
    {
        /* Safe cell: play click sound */
        model.playSafeCellSound();

        /* If no nearby mines, flood-fill the connected empty region */
        if (model.getBoard().getCell(row, col).nearbyMineCount == 0)
        {
            model.revealConnectedEmpty(row, col);
        }
    }
}

/* ============================================================
   processRightClick  (private)
   Called when the player right-clicks inside the grid.
   Delegates flag toggle and flag sound to the Model.
   ============================================================ */

void GameController::processRightClick(int row, int col)
{
    const Cell& cell = model.getBoard().getCell(row, col);

    /* Cannot flag an already-revealed cell */
    if (cell.isRevealed) return;

    /* Model toggles the flag and plays the flag sound */
    model.toggleFlag(row, col);
}
