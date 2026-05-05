#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

/*
 * GameController.h  —  CONTROLLER layer
 * ---------------------------------------
 * Declares the GameController class which processes user input
 * and coordinates communication between GameModel and GameView.
 *
 * MVC Role  : Controller
 *   - Reads mouse and keyboard input from Raylib each frame.
 *   - Translates raw input into Model method calls.
 *   - Contains ZERO rendering logic (no Draw* calls).
 *   - Contains ZERO game data (no Cell arrays, no state flags).
 *   - Acts as the wiring between Model and View.
 *
 * Responsibilities per frame (handleInput):
 *   1. Left-click  → ask Model to reveal the clicked cell;
 *                    if mine → call handleGameOver;
 *                    if safe + empty → trigger flood-fill.
 *   2. Right-click → ask Model to toggle a flag on the cell.
 *   3. After any   → ask Model to re-check win condition.
 *   4. Always      → check for pending end-game sounds via Model.
 *
 * SOLID alignment:
 *   SRP — Controller handles input coordination only.
 *   OCP — New input actions (keyboard shortcuts, touch) are added
 *         here without touching Model or View.
 *   DIP — Controller calls GameModel and GameView through their
 *         public interfaces, not internal implementation details.
 *
 * Part of: Minesweeper — C++ MVC Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "raylib.h"
#include "../model/GameModel.h"
#include "../view/GameView.h"

/* ============================================================
   GameController
   Wires user input to Model state changes each frame.
   ============================================================ */

class GameController {
public:

    /* Constructor — stores references to Model and View */
    GameController(GameModel& model, GameView& view);

    /* Process one frame of user input.
       Called each frame in the game loop (before draw). */
    void handleInput();

private:

    /* References to Model and View — Controller does not own them */
    GameModel& model;
    GameView&  view;

    /* ---- Private input helpers ---- */

    /* Convert mouse pixel position to board (row, col).
       Returns true if the mouse is inside the grid. */
    bool getMouseCell(int& rowOut, int& colOut) const;

    /* Handle a left-click at board position (row, col) */
    void processLeftClick(int row, int col);

    /* Handle a right-click at board position (row, col) */
    void processRightClick(int row, int col);
};

#endif /* GAME_CONTROLLER_H */
