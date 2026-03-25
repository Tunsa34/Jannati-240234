#ifndef BOARD_H
#define BOARD_H

/*
 * Board.h
 * -------
 * Declares the Cell struct and the Board class.
 *
 * SRP : Board only manages the grid — mine placement, counting,
 *       revealing, and flood-fill. It knows nothing about sounds,
 *       textures, or window drawing.
 *
 * OCP : New reveal strategies or board sizes can be added by
 *       extending Board without touching existing logic.
 *
 * Part of: Minesweeper — C++ Refactor (University Submission)
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
   Represents one square on the Minesweeper grid.
   Plain data struct — no behaviour, just state.
   Default member initialisation used (C++11 and later).
   ============================================================ */

struct Cell {
    bool isRevealed      = false;  /* player has opened this cell      */
    bool hasMine         = false;  /* a mine is hiding inside          */
    bool isFlagged       = false;  /* player marked with a flag        */
    int  nearbyMineCount = 0;      /* mines in the 8 surrounding cells */
};

/* ============================================================
   Board
   Owns and manages the 9x9 grid of cells.

   Responsibilities (SRP):
     - reset / initialise cells
     - place mines randomly
     - calculate nearby mine counts
     - reveal a single cell
     - flood-fill reveal connected empty cells
     - expose all mine positions on game over
     - count how many safe cells are revealed (win check)

   Everything else (sounds, drawing, input) lives elsewhere.
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
       Returns true if the cell contained a mine (caller handles game-over).
       Returns false if it was safe. */
    bool revealCell(int row, int col);

    /* Flood-fill: reveal all connected empty cells from (row, col).
       soundCallback() is called for every cell revealed here so the
       caller can play a sound without Board depending on Raylib. */
    void revealConnectedEmptyCells(int row, int col,
                                   void (*soundCallback)());

    /* Mark every mine cell as revealed (called on game-over) */
    void revealAllMines();

    /* Returns how many safe (non-mine) cells are currently revealed */
    int countRevealedSafeCells() const;

    /* Read-only access to a single cell — used by main.cpp for drawing */
    const Cell& getCell(int row, int col) const;

    /* Toggle the flag on an unrevealed cell */
    void toggleFlag(int row, int col);

    /* Returns true if (row, col) is a valid grid position */
    static bool isInsideBoard(int row, int col);

private:

    /* The 9x9 grid of cells */
    Cell grid[TOTAL_ROWS][TOTAL_COLS];
};

#endif /* BOARD_H */
