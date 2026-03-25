/*
 * Board.cpp
 * ---------
 * Implements all Board class methods declared in Board.h.
 *
 * Every method does exactly one thing (SRP).
 * No sound, no drawing — pure board logic only.
 * All recursion behaviour is identical to the original C code.
 *
 * Part of: Minesweeper — C++ Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "Board.h"
#include <cstdlib>   /* rand() */

/* ============================================================
   Constructor
   Calls reset() so the grid starts in a clean state.
   ============================================================ */

Board::Board()
{
    reset();
}

/* ============================================================
   reset
   Sets every cell back to its default empty starting state.
   Called once at the beginning of every new game.
   ============================================================ */

void Board::reset()
{
    for (int row = 0; row < TOTAL_ROWS; row++)
    {
        for (int col = 0; col < TOTAL_COLS; col++)
        {
            grid[row][col].isRevealed      = false;
            grid[row][col].hasMine         = false;
            grid[row][col].isFlagged       = false;
            grid[row][col].nearbyMineCount = 0;
        }
    }
}

/* ============================================================
   placeMines
   Randomly picks cells until all TOTAL_MINES mines are placed.
   Skips a cell if it already has a mine.
   ============================================================ */

void Board::placeMines()
{
    int minesPlacedSoFar = 0;

    while (minesPlacedSoFar < TOTAL_MINES)
    {
        int randomRow = rand() % TOTAL_ROWS;
        int randomCol = rand() % TOTAL_COLS;

        if (grid[randomRow][randomCol].hasMine == false)
        {
            grid[randomRow][randomCol].hasMine = true;
            minesPlacedSoFar++;
        }
    }
}

/* ============================================================
   calculateNearbyMineCounts
   For every non-mine cell, checks all 8 surrounding neighbors
   and counts how many of them contain a mine.
   Stores the result in nearbyMineCount.
   ============================================================ */

void Board::calculateNearbyMineCounts()
{
    for (int row = 0; row < TOTAL_ROWS; row++)
    {
        for (int col = 0; col < TOTAL_COLS; col++)
        {
            /* mine cells themselves do not need a count */
            if (grid[row][col].hasMine) continue;

            int mineCount = 0;

            /* check all 8 directions: up, down, left, right, 4 diagonals */
            for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
            {
                for (int colOffset = -1; colOffset <= 1; colOffset++)
                {
                    int neighborRow = row + rowOffset;
                    int neighborCol = col + colOffset;

                    if (isInsideBoard(neighborRow, neighborCol))
                    {
                        if (grid[neighborRow][neighborCol].hasMine)
                            mineCount++;
                    }
                }
            }

            grid[row][col].nearbyMineCount = mineCount;
        }
    }
}

/* ============================================================
   revealCell
   Opens the cell at (row, col).
   Returns true  → cell was a mine  (caller triggers game-over).
   Returns false → cell was safe.
   Does nothing and returns false for invalid / flagged cells.
   ============================================================ */

bool Board::revealCell(int row, int col)
{
    if (!isInsideBoard(row, col))     return false;
    if (grid[row][col].isRevealed)    return false;
    if (grid[row][col].isFlagged)     return false;

    grid[row][col].isRevealed = true;

    /* tell the caller whether this was a mine */
    return grid[row][col].hasMine;
}

/* ============================================================
   revealConnectedEmptyCells
   Recursive flood-fill identical to original RevealEmpty().
   Reveals all connected non-mine cells starting from (row, col).

   soundCallback — function pointer provided by Game so we can
   play the click sound for every auto-revealed cell (Option B)
   without Board depending directly on Raylib.
   ============================================================ */

void Board::revealConnectedEmptyCells(int row, int col,
                                      void (*soundCallback)())
{
    for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
    {
        for (int colOffset = -1; colOffset <= 1; colOffset++)
        {
            int neighborRow = row + rowOffset;
            int neighborCol = col + colOffset;

            if (!isInsideBoard(neighborRow, neighborCol))    continue;
            if (grid[neighborRow][neighborCol].isRevealed)   continue;
            if (grid[neighborRow][neighborCol].hasMine)      continue;

            /* reveal this neighbor */
            grid[neighborRow][neighborCol].isRevealed = true;

            /* play click sound for every auto-revealed cell (Option B) */
            if (soundCallback != nullptr)
                soundCallback();

            /* if this neighbor also has 0 nearby mines, keep spreading */
            if (grid[neighborRow][neighborCol].nearbyMineCount == 0)
                revealConnectedEmptyCells(neighborRow, neighborCol,
                                          soundCallback);
        }
    }
}

/* ============================================================
   revealAllMines
   Sets isRevealed = true on every mine cell.
   Called when the player hits a mine so they can see all
   the mine locations on the board.
   ============================================================ */

void Board::revealAllMines()
{
    for (int row = 0; row < TOTAL_ROWS; row++)
        for (int col = 0; col < TOTAL_COLS; col++)
            if (grid[row][col].hasMine)
                grid[row][col].isRevealed = true;
}

/* ============================================================
   countRevealedSafeCells
   Returns the number of non-mine cells the player has opened.
   Used by Game to check the win condition.
   ============================================================ */

int Board::countRevealedSafeCells() const
{
    int count = 0;

    for (int row = 0; row < TOTAL_ROWS; row++)
        for (int col = 0; col < TOTAL_COLS; col++)
            if (grid[row][col].isRevealed && !grid[row][col].hasMine)
                count++;

    return count;
}

/* ============================================================
   getCell  (read-only)
   Returns a const reference to the cell at (row, col).
   Used by main.cpp to read cell state for drawing.
   ============================================================ */

const Cell& Board::getCell(int row, int col) const
{
    return grid[row][col];
}

/* ============================================================
   toggleFlag
   Adds a flag if there is none; removes it if there is one.
   Ignores cells that are already revealed.
   ============================================================ */

void Board::toggleFlag(int row, int col)
{
    if (!isInsideBoard(row, col))    return;
    if (grid[row][col].isRevealed)   return;

    grid[row][col].isFlagged = !grid[row][col].isFlagged;
}

/* ============================================================
   isInsideBoard  (static)
   Returns true if (row, col) is within the valid grid bounds.
   Used everywhere neighbors are checked to prevent crashes.
   ============================================================ */

bool Board::isInsideBoard(int row, int col)
{
    return (row >= 0 && row < TOTAL_ROWS &&
            col >= 0 && col < TOTAL_COLS);
}
