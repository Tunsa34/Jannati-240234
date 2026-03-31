#ifndef GAME_H
#define GAME_H

/*
 * Game.h
 * ------
 * Declares the Game class which manages the overall game session:
 * audio resources, state flags, and per-frame update logic.
 *
 * SRP : Game manages state and audio only — no drawing here.
 *       All Raylib draw calls stay in main.cpp.
 *
 * DIP : Game owns Board through composition and exposes it via
 *       a const accessor so main.cpp never touches raw grid data.
 *
 * ISP : Only the methods that main.cpp actually needs are public.
 *
 * Part of: Minesweeper — C++ Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "raylib.h"
#include "Board.h"

/* Height of the status bar drawn at the bottom of the window */
constexpr int STATUS_BAR_HEIGHT = 50;

/* ============================================================
   Game
   Owns the Board, all audio/texture resources, and the
   high-level game state (playing / game-over / win).
   ============================================================ */

class Game {
public:

    /* Constructor — call after InitWindow() and InitAudioDevice() */
    Game();

    /* Destructor — automatically unloads all audio and texture */
    ~Game();

    /* Process one frame of input + state logic (no drawing) */
    void update();

    /* ---- Read-only accessors used by main.cpp for drawing ---- */

    /* Returns a const reference to the Board */
    const Board& getBoard() const;

    /* Returns the explosion texture drawn on revealed mine cells */
    const Texture2D& getMineExplosionImage() const;

    /* Returns true if the player hit a mine */
    bool isGameOver() const;

    /* Returns true if the player revealed all safe cells */
    bool isPlayerWin() const;

private:

    /* The game board — Game owns it (composition over inheritance) */
    Board board;

    /* High-level state flags */
    bool gameOver;
    bool playerWin;

    /* ---- Audio resources ---- */
    Sound safeCellClickSound;    /* played when a safe cell is opened   */
    Sound mineExplosionSound;    /* played the moment a mine is clicked */
    Sound flagToggleSound;       /* played when flag is placed/removed  */
    Sound gameOverJingleSound;   /* played on the game-over screen      */
    Sound winJingleSound;        /* played on the win screen            */

    /* Explosion image rendered on top of revealed mine cells */
    Texture2D mineExplosionImage;

    /* Play-once guards — stop each sound from replaying every frame */
    bool alreadyPlayedExplosionSound;
    bool alreadyPlayedGameOverSound;
    bool alreadyPlayedWinSound;

    /* ---- Private helper methods ---- */

    /* Convert mouse pixel position to board (row, col).
       Returns true if the mouse is on the board. */
    bool getMouseCell(int& rowOut, int& colOut) const;

    /* Reveal cell on left-click; triggers boom or flood-fill */
    void handleLeftClick(int row, int col);

    /* Toggle flag on right-click */
    void handleRightClick(int row, int col);

    /* Check if all safe cells are revealed (win condition) */
    void checkWinCondition();

    /* Safety net: ensures end-game sounds play exactly once */
    void playPendingSounds();

    /* Static callback passed to Board::revealConnectedEmptyCells
       so it can play the click sound without depending on Raylib */
    static void playSafeCellSoundCallback();

    /* Pointer to the active Game instance used by the callback */
    static Game* activeInstance;
};

#endif /* GAME_H */
