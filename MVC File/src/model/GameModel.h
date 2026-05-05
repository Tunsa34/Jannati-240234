#ifndef GAME_MODEL_H
#define GAME_MODEL_H

/*
 * GameModel.h  —  MODEL layer
 * ----------------------------
 * Declares the GameModel class which owns the Board, all audio
 * resources, and high-level game state (playing / game-over / win).
 *
 * MVC Role  : Model
 *   - Owns the Board (game data) and audio assets.
 *   - Manages state flags: gameOver, playerWin, play-once guards.
 *   - Exposes a clean read-only API consumed by View and Controller.
 *   - Contains NO rendering code and NO direct input handling.
 *
 * Why audio lives in the Model (not the View):
 *   Audio is a game-state side-effect (mine explodes → boom plays).
 *   Keeping it in the Model ensures the Controller can trigger sounds
 *   through well-defined Model methods, and the View never needs to
 *   know about sound state. Raylib audio types (Sound, Texture2D) are
 *   limited to this layer and the View layer — never the Controller.
 *
 * SOLID alignment:
 *   SRP — GameModel owns data and audio; input and rendering elsewhere.
 *   DIP — Board is accessed through its own class interface; audio
 *         events are triggered via named GameModel methods.
 *   ISP — Controller only calls input-response methods; View only
 *         calls read-only accessors.
 *
 * Part of: Minesweeper — C++ MVC Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "raylib.h"
#include "Board.h"

/* Height of the status bar drawn at the bottom of the window */
constexpr int STATUS_BAR_HEIGHT = 50;

/* ============================================================
   GameModel
   The single source of truth for the current game session.
   ============================================================ */

class GameModel {
public:

    /* Constructor — call after InitWindow() and InitAudioDevice() */
    GameModel();

    /* Destructor — automatically unloads all audio and textures */
    ~GameModel();

    /* ---- State mutators called by Controller ---- */

    /* Reset board and all state flags for a new game */
    void newGame();

    /* Reveal the cell at (row, col).
       Returns true if it was a mine (Controller should call handleGameOver). */
    bool revealCell(int row, int col);

    /* Toggle flag at (row, col) and play the flag sound */
    void toggleFlag(int row, int col);

    /* Expose all mines and mark game as over — called by Controller */
    void handleGameOver();

    /* Check and set win state if all safe cells are revealed */
    void checkWinCondition();

    /* ---- Sound methods called by Controller ---- */

    /* Play the safe-cell click sound */
    void playSafeCellSound();

    /* Play the explosion sound (once) */
    void playExplosionSound();

    /* Play the game-over jingle (once) */
    void playGameOverSound();

    /* Play the win jingle (once) */
    void playWinSound();

    /* Flood-fill from (row, col), playing click sound per cell */
    void revealConnectedEmpty(int row, int col);

    /* ---- Read-only accessors used by View and Controller ---- */

    const Board&     getBoard()              const;
    const Texture2D& getMineExplosionImage() const;
    bool             isGameOver()            const;
    bool             isPlayerWin()           const;

    /* Static callback used by Board's flood-fill to reach the active instance */
    static void safeCellSoundCallback();

private:

    Board     board;
    bool      gameOver;
    bool      playerWin;

    /* Audio resources */
    Sound     safeCellClickSound;
    Sound     mineExplosionSound;
    Sound     flagToggleSound;
    Sound     gameOverJingleSound;
    Sound     winJingleSound;

    /* Explosion image rendered on revealed mine cells */
    Texture2D mineExplosionImage;

    /* Play-once guards — prevent sounds from replaying every frame */
    bool      alreadyPlayedExplosionSound;
    bool      alreadyPlayedGameOverSound;
    bool      alreadyPlayedWinSound;

    /* Pointer to the active instance used by the static callback */
    static GameModel* activeInstance;
};

#endif /* GAME_MODEL_H */
