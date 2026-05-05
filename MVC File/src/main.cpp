/*
 * main.cpp
 * --------
 * Program entry point for the MVC-refactored Minesweeper.
 *
 * Responsibilities at this level (SRP applied at function level):
 *   1. Initialise Raylib (window + audio device).
 *   2. Construct the three MVC objects: Model → View → Controller.
 *   3. Run the game loop:  Controller::handleInput() then View::draw().
 *   4. Shut down Raylib cleanly when the window is closed.
 *
 * Nothing else. No game logic. No drawing. No direct input.
 * All of that lives in its respective MVC layer.
 *
 * MVC Object Lifetimes:
 *   GameModel      — owns Board, audio, and game state.
 *   GameView       — holds a const-ref to GameModel.
 *   GameController — holds refs to GameModel and GameView.
 *   All three are stack-allocated; destructors run at function exit,
 *   which correctly unloads audio before CloseAudioDevice().
 *
 * HOW TO COMPILE — Windows (VS Code Terminal, from project root):
 *
 *   g++ -std=c++17
 *       src/main.cpp
 *       src/model/Board.cpp  src/model/GameModel.cpp
 *       src/view/GameView.cpp
 *       src/controller/GameController.cpp
 *       -IC:\raylib\include -LC:\raylib\lib
 *       -lraylib -lopengl32 -lgdi32 -lwinmm
 *       -o build/minesweeper.exe
 *
 * HOW TO COMPILE — Linux:
 *
 *   g++ -std=c++17
 *       src/main.cpp
 *       src/model/Board.cpp  src/model/GameModel.cpp
 *       src/view/GameView.cpp
 *       src/controller/GameController.cpp
 *       -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
 *       -o build/minesweeper
 *
 * HOW TO RUN:
 *   Windows : .\build\minesweeper.exe    (from project root)
 *   Linux   : ./build/minesweeper        (from project root)
 *
 * NOTE: Always run from the project root so sounds/ and images/ resolve.
 *
 * FILES NEEDED (relative to project root):
 *   sounds/number.mp3   sounds/boom.mp3   sounds/flag.mp3
 *   sounds/over.mp3     sounds/win.mp3    images/boomm.png
 *
 * Part of: Minesweeper — C++ MVC Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "raylib.h"
#include "model/GameModel.h"
#include "view/GameView.h"
#include "controller/GameController.h"

int main()
{
    /* Window dimensions derived from board constants */
    const int windowWidth  = TOTAL_COLS * CELL_PIXEL_SIZE;
    const int windowHeight = TOTAL_ROWS * CELL_PIXEL_SIZE + STATUS_BAR_HEIGHT;

    /* ---- Initialise Raylib ---- */
    InitWindow(windowWidth, windowHeight, "Minesweeper - MVC Architecture");
    SetTargetFPS(60);
    InitAudioDevice();

    /* ---- Construct MVC triple ---- */
    /*
     * Order matters:
     *   1. GameModel first  — loads audio/texture, sets up board.
     *   2. GameView second  — takes a const-ref to Model.
     *   3. GameController   — takes refs to both Model and View.
     */
    GameModel      model;
    GameView       view(model);
    GameController controller(model, view);

    /* ---- Game loop ---- */
    while (!WindowShouldClose())
    {
        /* CONTROLLER: process input → update Model */
        controller.handleInput();

        /* VIEW: read Model state → render to screen */
        BeginDrawing();
            view.draw();
        EndDrawing();
    }

    /* ---- Shutdown ---- */
    /*
     * model destructor runs here (stack unwind), unloading all
     * audio/textures before CloseAudioDevice() is called.
     */
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
