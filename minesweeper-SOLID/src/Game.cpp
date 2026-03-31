/*
 * Game.cpp
 * --------
 * Implements all Game class methods declared in Game.h.
 *
 * Handles:
 *   - Resource loading and unloading (constructor / destructor)
 *   - Mouse input processing each frame
 *   - State transitions: game-over and win
 *   - Sound management with play-once guards
 *
 * No Raylib draw calls live here — all drawing is in main.cpp.
 * Asset paths are relative to the project root (sounds/, images/).
 *
 * Part of: Minesweeper — C++ Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "Game.h"
#include <cstdlib>   /* srand()  */
#include <ctime>     /* time()   */

/* ============================================================
   Static member initialisation
   ============================================================ */

Game* Game::activeInstance = nullptr;

/* ============================================================
   Constructor
   Loads all resources, sets up the board, resets all flags.
   Called after InitWindow() and InitAudioDevice().
   ============================================================ */

Game::Game()
{
    /* seed random so mine positions differ every game */
    srand(static_cast<unsigned int>(time(nullptr)));

    /* set up the board */
    board.reset();
    board.placeMines();
    board.calculateNearbyMineCounts();

    /* reset state flags */
    gameOver  = false;
    playerWin = false;

    /* reset play-once sound guards */
    alreadyPlayedExplosionSound = false;
    alreadyPlayedGameOverSound  = false;
    alreadyPlayedWinSound       = false;

    /* load sound files from the sounds/ folder */
    safeCellClickSound   = LoadSound("sounds/number.mp3");
    mineExplosionSound   = LoadSound("sounds/boom.mp3");
    flagToggleSound      = LoadSound("sounds/flag.mp3");
    gameOverJingleSound  = LoadSound("sounds/over.mp3");
    winJingleSound       = LoadSound("sounds/win.mp3");

    /* load explosion image from the images/ folder */
    mineExplosionImage = LoadTexture("images/boomm.png");

    /* register this instance so the static callback can reach it */
    activeInstance = this;
}

/* ============================================================
   Destructor
   Unloads all Raylib audio and texture resources automatically
   when the Game object goes out of scope in main.cpp.
   ============================================================ */

Game::~Game()
{
    UnloadSound(safeCellClickSound);
    UnloadSound(mineExplosionSound);
    UnloadSound(flagToggleSound);
    UnloadSound(gameOverJingleSound);
    UnloadSound(winJingleSound);
    UnloadTexture(mineExplosionImage);
}

/* ============================================================
   update
   Called once every frame from the game loop in main.cpp.
   Reads mouse input and updates the game state accordingly.
   No drawing happens here.
   ============================================================ */

void Game::update()
{
    if (!gameOver && !playerWin)
    {
        int row = 0;
        int col = 0;

        /* left click — reveal a cell */
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (getMouseCell(row, col))
                handleLeftClick(row, col);
        }

        /* right click — place or remove a flag */
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            if (getMouseCell(row, col))
                handleRightClick(row, col);
        }

        /* check win condition after every interaction */
        checkWinCondition();
    }
    else
    {
        /* game has ended — ensure end-game sounds play once */
        playPendingSounds();
    }
}

/* ============================================================
   getBoard  (read-only accessor)
   Returns a const reference to the Board.
   Used by main.cpp to read cell states for drawing.
   ============================================================ */

const Board& Game::getBoard() const
{
    return board;
}

/* ============================================================
   getMineExplosionImage  (read-only accessor)
   Returns the explosion texture for main.cpp to draw on mines.
   ============================================================ */

const Texture2D& Game::getMineExplosionImage() const
{
    return mineExplosionImage;
}

/* ============================================================
   isGameOver / isPlayerWin  (read-only accessors)
   Used by main.cpp to decide which status bar message to draw.
   ============================================================ */

bool Game::isGameOver()  const { return gameOver;  }
bool Game::isPlayerWin() const { return playerWin; }

/* ============================================================
   getMouseCell  (private)
   Converts the current mouse pixel position to a board cell.
   Writes the result into rowOut and colOut.
   Returns true if the position is valid on the board.
   ============================================================ */

bool Game::getMouseCell(int& rowOut, int& colOut) const
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
   handleLeftClick  (private)
   Called when the player left-clicks a cell.
   - Mine hit  → play explosion + game-over sounds, reveal all mines.
   - Safe cell → play click sound, flood-fill if 0 nearby mines.
   ============================================================ */

void Game::handleLeftClick(int row, int col)
{
    const Cell& cell = board.getCell(row, col);

    /* ignore already-revealed or flagged cells */
    if (cell.isRevealed || cell.isFlagged) return;

    /* revealCell returns true if it was a mine */
    bool hitMine = board.revealCell(row, col);

    if (hitMine)
    {
        /* play boom sound once */
        if (!alreadyPlayedExplosionSound)
        {
            PlaySound(mineExplosionSound);
            alreadyPlayedExplosionSound = true;
        }

        gameOver = true;
        board.revealAllMines();

        /* play game-over jingle once */
        if (!alreadyPlayedGameOverSound)
        {
            PlaySound(gameOverJingleSound);
            alreadyPlayedGameOverSound = true;
        }
    }
    else
    {
        /* safe cell — play click sound */
        PlaySound(safeCellClickSound);

        /* if no nearby mines, flood-fill the empty area */
        if (board.getCell(row, col).nearbyMineCount == 0)
        {
            board.revealConnectedEmptyCells(row, col,
                                            playSafeCellSoundCallback);
        }
    }
}

/* ============================================================
   handleRightClick  (private)
   Toggles a flag on the clicked unrevealed cell and plays
   the flag sound.
   ============================================================ */

void Game::handleRightClick(int row, int col)
{
    const Cell& cell = board.getCell(row, col);
    if (cell.isRevealed) return;

    board.toggleFlag(row, col);
    PlaySound(flagToggleSound);
}

/* ============================================================
   checkWinCondition  (private)
   The player wins when every safe cell has been opened.
   Total safe cells = TOTAL_ROWS * TOTAL_COLS - TOTAL_MINES
   ============================================================ */

void Game::checkWinCondition()
{
    int totalSafeCells    = TOTAL_ROWS * TOTAL_COLS - TOTAL_MINES;
    int revealedSafeCells = board.countRevealedSafeCells();

    if (revealedSafeCells == totalSafeCells)
    {
        playerWin = true;

        if (!alreadyPlayedWinSound)
        {
            PlaySound(winJingleSound);
            alreadyPlayedWinSound = true;
        }
    }
}

/* ============================================================
   playPendingSounds  (private)
   Safety net that runs every frame after the game ends.
   Ensures end-game sounds play exactly once even if state
   was set through an indirect code path.
   ============================================================ */

void Game::playPendingSounds()
{
    if (gameOver && !alreadyPlayedGameOverSound)
    {
        PlaySound(gameOverJingleSound);
        alreadyPlayedGameOverSound = true;
    }

    if (playerWin && !alreadyPlayedWinSound)
    {
        PlaySound(winJingleSound);
        alreadyPlayedWinSound = true;
    }
}

/* ============================================================
   playSafeCellSoundCallback  (static)
   Passed as a function pointer to Board::revealConnectedEmptyCells.
   Called once for every cell revealed during flood-fill.
   This preserves Option B behaviour from the original C code:
   a click sound plays for every auto-revealed cell.
   ============================================================ */

void Game::playSafeCellSoundCallback()
{
    if (activeInstance != nullptr)
        PlaySound(activeInstance->safeCellClickSound);
}
