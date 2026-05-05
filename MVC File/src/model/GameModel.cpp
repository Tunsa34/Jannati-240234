/*
 * GameModel.cpp  —  MODEL layer
 * ------------------------------
 * Implements all GameModel class methods declared in GameModel.h.
 *
 * MVC Role  : Model
 *   Handles all game-state transitions and audio side-effects.
 *   No drawing occurs here. No direct input is read here.
 *   The Controller drives every state change through named methods.
 *
 * Part of: Minesweeper — C++ MVC Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "GameModel.h"
#include <cstdlib>   /* srand()  */
#include <ctime>     /* time()   */

/* ============================================================
   Static member initialisation
   ============================================================ */

GameModel* GameModel::activeInstance = nullptr;

/* ============================================================
   Constructor
   Loads all resources, initialises the board, resets all flags.
   Must be called after InitWindow() and InitAudioDevice().
   ============================================================ */

GameModel::GameModel()
{
    /* Seed random so mine positions differ every game */
    srand(static_cast<unsigned int>(time(nullptr)));

    /* Register this instance for the static callback */
    activeInstance = this;

    /* Load audio assets (paths relative to project root) */
    safeCellClickSound  = LoadSound("sounds/number.mp3");
    mineExplosionSound  = LoadSound("sounds/boom.mp3");
    flagToggleSound     = LoadSound("sounds/flag.mp3");
    gameOverJingleSound = LoadSound("sounds/over.mp3");
    winJingleSound      = LoadSound("sounds/win.mp3");

    /* Load visual asset */
    mineExplosionImage  = LoadTexture("images/boomm.png");

    /* Begin a fresh game */
    newGame();
}

/* ============================================================
   Destructor
   Unloads all Raylib audio and texture resources automatically
   when the GameModel object goes out of scope.
   ============================================================ */

GameModel::~GameModel()
{
    UnloadSound(safeCellClickSound);
    UnloadSound(mineExplosionSound);
    UnloadSound(flagToggleSound);
    UnloadSound(gameOverJingleSound);
    UnloadSound(winJingleSound);
    UnloadTexture(mineExplosionImage);
}

/* ============================================================
   newGame
   Resets all state flags and reinitialises the board.
   Called by the Constructor and by the Controller on restart.
   ============================================================ */

void GameModel::newGame()
{
    gameOver  = false;
    playerWin = false;

    alreadyPlayedExplosionSound = false;
    alreadyPlayedGameOverSound  = false;
    alreadyPlayedWinSound       = false;

    board.reset();
    board.placeMines();
    board.calculateNearbyMineCounts();
}

/* ============================================================
   revealCell
   Delegates to Board::revealCell.
   Returns true if the cell was a mine — Controller decides what
   to do next (call handleGameOver, update View, etc.).
   ============================================================ */

bool GameModel::revealCell(int row, int col)
{
    return board.revealCell(row, col);
}

/* ============================================================
   toggleFlag
   Delegates to Board::toggleFlag and plays the flag sound.
   ============================================================ */

void GameModel::toggleFlag(int row, int col)
{
    board.toggleFlag(row, col);
    PlaySound(flagToggleSound);
}

/* ============================================================
   handleGameOver
   Reveals all mines, sets the gameOver flag, and plays the
   explosion + game-over jingle exactly once.
   ============================================================ */

void GameModel::handleGameOver()
{
    board.revealAllMines();
    gameOver = true;

    playExplosionSound();
    playGameOverSound();
}

/* ============================================================
   checkWinCondition
   The player wins when every safe cell has been opened.
   Total safe cells = TOTAL_ROWS * TOTAL_COLS - TOTAL_MINES.
   Sets playerWin and plays the win jingle exactly once.
   ============================================================ */

void GameModel::checkWinCondition()
{
    int totalSafeCells    = TOTAL_ROWS * TOTAL_COLS - TOTAL_MINES;
    int revealedSafeCells = board.countRevealedSafeCells();

    if (revealedSafeCells == totalSafeCells)
    {
        playerWin = true;
        playWinSound();
    }
}

/* ============================================================
   revealConnectedEmpty
   Passes the static sound callback to Board's flood-fill so
   a click sound plays for every auto-revealed cell.
   ============================================================ */

void GameModel::revealConnectedEmpty(int row, int col)
{
    board.revealConnectedEmptyCells(row, col, safeCellSoundCallback);
}

/* ============================================================
   Sound methods — each plays exactly once using a guard flag.
   ============================================================ */

void GameModel::playSafeCellSound()
{
    PlaySound(safeCellClickSound);
}

void GameModel::playExplosionSound()
{
    if (!alreadyPlayedExplosionSound)
    {
        PlaySound(mineExplosionSound);
        alreadyPlayedExplosionSound = true;
    }
}

void GameModel::playGameOverSound()
{
    if (!alreadyPlayedGameOverSound)
    {
        PlaySound(gameOverJingleSound);
        alreadyPlayedGameOverSound = true;
    }
}

void GameModel::playWinSound()
{
    if (!alreadyPlayedWinSound)
    {
        PlaySound(winJingleSound);
        alreadyPlayedWinSound = true;
    }
}

/* ============================================================
   Read-only accessors
   ============================================================ */

const Board& GameModel::getBoard() const
{
    return board;
}

const Texture2D& GameModel::getMineExplosionImage() const
{
    return mineExplosionImage;
}

bool GameModel::isGameOver()  const { return gameOver;  }
bool GameModel::isPlayerWin() const { return playerWin; }

/* ============================================================
   safeCellSoundCallback  (static)
   Passed as a function pointer to Board::revealConnectedEmptyCells.
   Called once for every cell revealed during flood-fill.
   Decouples Board (Model) from Raylib audio.
   ============================================================ */

void GameModel::safeCellSoundCallback()
{
    if (activeInstance != nullptr)
        activeInstance->playSafeCellSound();
}
