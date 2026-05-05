#ifndef BOARD_H
#define BOARD_H

/*
 * Board.h  —  MODEL layer
 * -----------------------
 * Declares the Cell struct and the Board class.
 *
 * MVC Role  : Model
 *   - Owns the 9×9 grid of cells (pure game data).
 *   - Implements all game logic: mine placement, counting,
 *     revealing, flood-fill, flagging, and win detection.
 *   - Contains ZERO rendering code and ZERO input handling.
 *   - Has NO dependency on Raylib.
 *
 * SOLID alignment:
 *   SRP — Board does one thing: manage the grid.
 *   OCP — New board sizes or reveal strategies extend Board
 *         without modifying existing methods.
 *   DIP — Board communicates sound events through a plain
 *         function-pointer callback so it never touches audio.
 *
 * Part of: Minesweeper — C++ MVC Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

/* ============================================================
   BOARD CONSTANTS
   Change these to resize the game board.
   ============================================================ */

constexpr int TOTAL_ROWS      = 9;    /* rows on the grid              */
constexpr int TOTAL_COLS      = 9;    /* columns on the grid           */
constexpr int TOTAL_MINES     = 10;   /* mines hidden on the board     */
constexpr int CELL_PIXEL_SIZE = 60;   /* pixel size of each cell       */

/* ============================================================
   Cell
   Plain-data struct representing one square on the grid.
   No behaviour — just state. Default member initialisation
   used (C++11 and later).
   ============================================================ */

struct Cell {
    bool isRevealed      = false;  /* player has opened this cell      */
    bool hasMine         = false;  /* a mine is hiding inside          */
    bool isFlagged       = false;  /* player marked with a flag        */
    int  nearbyMineCount = 0;      /* mines in the 8 surrounding cells */
};

/* ============================================================
   Board
   Owns and manages the 9×9 grid of cells.

   Public interface (Model API — consumed by Controller):
     reset()                      — clear the grid
     placeMines()                 — scatter mines
     calculateNearbyMineCounts()  — precompute neighbour counts
     revealCell(row, col)         — open one cell; returns true on mine
     revealConnectedEmptyCells()  — flood-fill from an empty cell
     revealAllMines()             — expose all mines (game-over)
     countRevealedSafeCells()     — used for win-condition check
     getCell(row, col)            — read-only cell access (for View)
     toggleFlag(row, col)         — place / remove a flag
     isInsideBoard(row, col)      — bounds check (static)
   ============================================================ */

class Board {
public:

    /* Constructor — resets the grid but does NOT place mines yet */
    Board();

    /* Reset every cell to its default empty state */
    void reset();

    /* Randomly scatter TOTAL_MINES mines across the grid */
    void placeMines();

    /* Fill nearbyMineCount for every non-mine cell */
    void calculateNearbyMineCounts();

    /* Open the cell at (row, col).
       Returns true if the cell contained a mine (Controller handles game-over).
       Returns false if it was safe. */
    bool revealCell(int row, int col);

    /* Flood-fill: reveal all connected empty cells from (row, col).
       soundCallback() is called for every cell revealed so the
       Controller can play a sound without Board depending on Raylib. */
    void revealConnectedEmptyCells(int row, int col,
                                   void (*soundCallback)());

    /* Mark every mine cell as revealed (called on game-over) */
    void revealAllMines();

    /* Returns how many safe (non-mine) cells are currently revealed */
    int countRevealedSafeCells() const;

    /* Read-only access to a single cell — used by View for drawing */
    const Cell& getCell(int row, int col) const;

    /* Toggle the flag on an unrevealed cell */
    void toggleFlag(int row, int col);

    /* Returns true if (row, col) is a valid grid position */
    static bool isInsideBoard(int row, int col);

private:

    /* The 9×9 grid of cells */
    Cell grid[TOTAL_ROWS][TOTAL_COLS];
};

#endif /* BOARD_H */
