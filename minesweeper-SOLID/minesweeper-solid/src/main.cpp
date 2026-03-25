/*
 * main.cpp
 * --------
 * Program entry point.
 * Opens the window, runs the game loop, and closes everything.
 *
 * All Raylib draw calls are kept here exactly as in the original
 * C code — same colours, same positions, same font sizes.
 * No game logic and no board data are modified from this file.
 *
 * Drawing is split into three small helper functions so main()
 * stays short and readable (SRP applied at function level).
 *
 * HOW TO COMPILE — Windows (VS Code Terminal, from project root):
 *
 *   g++ -std=c++17 src/main.cpp src/Game.cpp src/Board.cpp
 *       -IC:\raylib\include -LC:\raylib\lib
 *       -lraylib -lopengl32 -lgdi32 -lwinmm
 *       -o build/minesweeper.exe
 *
 * HOW TO COMPILE — Linux / Cloud:
 *
 *   g++ -std=c++17 src/main.cpp src/Game.cpp src/Board.cpp
 *       -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
 *       -o build/minesweeper
 *
 * HOW TO RUN — Windows : .\build\minesweeper.exe
 * HOW TO RUN — Linux   : ./build/minesweeper
 *
 * NOTE: Always run from the project root so sounds/ and images/ resolve.
 *
 * FILES NEEDED (relative to project root):
 *   sounds/number.mp3  sounds/boom.mp3  sounds/flag.mp3
 *   sounds/over.mp3    sounds/win.mp3   images/boomm.png
 *
 * Part of: Minesweeper — C++ Refactor (University Submission)
 * Author : CLO4 Submission
 * Date   : 2025
 */

#include "raylib.h"
#include "Game.h"
#include "Board.h"

/* ============================================================
   COLOUR CONSTANTS
   Named constants replace all magic colour literals.
   Values are identical to the original C code.
   ============================================================ */

constexpr Color COLOR_BACKGROUND  = {  48,  99,  47, 255 }; /* dark green  */
constexpr Color COLOR_UNREV_LIGHT = { 190, 224, 145, 255 }; /* light green */
constexpr Color COLOR_UNREV_DARK  = { 170, 214, 135, 255 }; /* mid green   */
constexpr Color COLOR_REV_LIGHT   = { 240, 210, 170, 255 }; /* beige light */
constexpr Color COLOR_REV_DARK    = { 225, 195, 150, 255 }; /* beige dark  */
constexpr Color COLOR_BORDER      = { 110, 110, 110, 255 }; /* grey        */

/* ============================================================
   drawCell
   Draws one cell at grid position (row, col).
   Handles all three visual states:
     - revealed mine     → explosion image
     - revealed safe     → nearby mine count number (or blank)
     - unrevealed + flag → red triangle flag
     - unrevealed plain  → green background only
   All Raylib calls and pixel offsets match the original exactly.
   ============================================================ */

static void drawCell(const Game& game, int row, int col)
{
    const Cell& cell = game.getBoard().getCell(row, col);

    int cellLeft = col * CELL_PIXEL_SIZE;
    int cellTop  = row * CELL_PIXEL_SIZE;

    Rectangle cellRect = {
        static_cast<float>(cellLeft),
        static_cast<float>(cellTop),
        static_cast<float>(CELL_PIXEL_SIZE),
        static_cast<float>(CELL_PIXEL_SIZE)
    };

    /* --- 1. Background fill — checkerboard pattern --- */
    bool isEvenSquare = ((row + col) % 2 == 0);

    if (cell.isRevealed)
    {
        DrawRectangleRec(cellRect,
                         isEvenSquare ? COLOR_REV_LIGHT : COLOR_REV_DARK);
    }
    else
    {
        DrawRectangleRec(cellRect,
                         isEvenSquare ? COLOR_UNREV_LIGHT : COLOR_UNREV_DARK);
    }

    /* --- 2. Grey border line --- */
    DrawRectangleLinesEx(cellRect, 1, COLOR_BORDER);

    /* --- 3. Cell content --- */
    if (cell.isRevealed)
    {
        if (cell.hasMine)
        {
            /* draw explosion image scaled to fill the cell */
            const Texture2D& tex = game.getMineExplosionImage();

            Rectangle imageSrc  = { 0.0f, 0.0f,
                                    static_cast<float>(tex.width),
                                    static_cast<float>(tex.height) };
            Rectangle imageDest = { static_cast<float>(cellLeft),
                                    static_cast<float>(cellTop),
                                    static_cast<float>(CELL_PIXEL_SIZE),
                                    static_cast<float>(CELL_PIXEL_SIZE) };

            DrawTexturePro(tex, imageSrc, imageDest,
                           { 0.0f, 0.0f }, 0.0f, WHITE);
        }
        else if (cell.nearbyMineCount > 0)
        {
            /* draw nearby mine count number centred in the cell */
            int textX = cellLeft + CELL_PIXEL_SIZE / 2 - 8;
            int textY = cellTop  + CELL_PIXEL_SIZE / 2 - 12;

            DrawText(TextFormat("%d", cell.nearbyMineCount),
                     textX, textY, 25, BLUE);
        }
        /* nearbyMineCount == 0 and not a mine → blank, draw nothing */
    }
    else if (cell.isFlagged)
    {
        /* draw red triangle as flag — exact same vertices as original */
        float cx = static_cast<float>(cellLeft) + CELL_PIXEL_SIZE / 2.0f;
        float cy = static_cast<float>(cellTop)  + CELL_PIXEL_SIZE / 2.0f;

        DrawTriangle(
            { cx - 8.0f, cy + 8.0f  },   /* bottom-left  */
            { cx - 8.0f, cy - 12.0f },   /* top-left     */
            { cx + 8.0f, cy - 2.0f  },   /* right-middle */
            RED
        );
    }
}

/* ============================================================
   drawBoard
   Iterates through all 81 cells and calls drawCell for each.
   ============================================================ */

static void drawBoard(const Game& game)
{
    for (int row = 0; row < TOTAL_ROWS; row++)
        for (int col = 0; col < TOTAL_COLS; col++)
            drawCell(game, row, col);
}

/* ============================================================
   drawStatusBar
   Draws the message in the status bar at the bottom:
     - RED   "GAME OVER!" when player hit a mine
     - GREEN "YOU WIN!"   when all safe cells revealed
     - WHITE instructions during normal play
   Text positions match the original C code exactly.
   ============================================================ */

static void drawStatusBar(const Game& game)
{
    int messageY = TOTAL_ROWS * CELL_PIXEL_SIZE + 10;

    if (game.isGameOver())
        DrawText("💥 GAME OVER!", 10, messageY, 30, RED);
    else if (game.isPlayerWin())
        DrawText("🎉 YOU WIN!", 10, messageY, 30, GREEN);
    else
        DrawText("Left-click: Reveal | Right-click: Flag",
                 10, messageY + 5, 20, RAYWHITE);
}

/* ============================================================
   main
   Opens the window, creates the game, runs the loop, closes.
   Exactly 3 responsibilities at this level:
     1. initialise and close Raylib
     2. call game.update() each frame (logic)
     3. call draw functions each frame (presentation)
   ============================================================ */

int main()
{
    const int windowWidth  = TOTAL_COLS * CELL_PIXEL_SIZE;
    const int windowHeight = TOTAL_ROWS * CELL_PIXEL_SIZE + STATUS_BAR_HEIGHT;

    /* open window and audio */
    InitWindow(windowWidth, windowHeight, "Minesweeper - Raylib Styled");
    SetTargetFPS(60);
    InitAudioDevice();

    /* create the game — loads all sounds and sets up the board */
    Game game;

    /* game loop */
    while (!WindowShouldClose())
    {
        /* --- logic layer: no drawing --- */
        game.update();

        /* --- drawing layer: no logic --- */
        BeginDrawing();
            ClearBackground(COLOR_BACKGROUND);
            drawBoard(game);
            drawStatusBar(game);
        EndDrawing();
    }

    /* Game destructor handles UnloadSound / UnloadTexture */
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
