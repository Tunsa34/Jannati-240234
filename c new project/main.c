/*
 * main.c
 * ------
 * Entry point. Contains the original, unmodified drawing code exactly
 * as written. Only the logic and state management have been moved out
 * to game.c and board.c.
 *
 * Build:
 *   gcc -std=c99 src/main.c src/game.c src/board.c \
 *       -Iinclude -lraylib -lm -o minesweeper
 *
 * Assets required alongside the executable:
 *   number.mp3  boom.mp3  flag.mp3  over.mp3  win.mp3  boomm.png
 *
 * Part of: Minesweeper (Raylib) — Refactored Academic Version
 * Author : [Student Name]
 * Date   : 2025
 */

#include "raylib.h"
#include "game.h"
#include "board.h"

int main(void)
{
    InitWindow(COLS * CELL_SIZE, ROWS * CELL_SIZE + STATUS_BAR_H, "Minesweeper - Raylib Styled");
    SetTargetFPS(60);
    InitAudioDevice();

    Game g;
    Game_Init(&g);

    while (!WindowShouldClose()) {

        /* --- Update (logic only) --- */
        Game_Update(&g);

        /* --- Draw (original graphics, unchanged) --- */
        BeginDrawing();
        ClearBackground((Color){ 48, 99, 47, 255 });

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                Rectangle cell = { c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE };

                Color baseColor = ((r + c) % 2 == 0)
                    ? (Color){ 190, 224, 145, 255 }
                    : (Color){ 170, 214, 135, 255 };

                Color revealedColor = ((r + c) % 2 == 0)
                    ? (Color){ 240, 210, 170, 255 }
                    : (Color){ 225, 195, 150, 255 };

                if (g.board[r][c].revealed)
                    DrawRectangleRec(cell, revealedColor);
                else
                    DrawRectangleRec(cell, baseColor);

                DrawRectangleLinesEx(cell, 1, (Color){ 110, 110, 110, 255 });

                if (g.board[r][c].revealed) {
                    if (g.board[r][c].hasMine) {
                        Rectangle src  = { 0.0f, 0.0f, (float)g.texBoom.width, (float)g.texBoom.height };
                        Rectangle dest = { cell.x, cell.y, CELL_SIZE, CELL_SIZE };
                        Vector2 origin = { 0.0f, 0.0f };
                        DrawTexturePro(g.texBoom, src, dest, origin, 0.0f, WHITE);
                    } else if (g.board[r][c].nearbyMines > 0) {
                        DrawText(TextFormat("%d", g.board[r][c].nearbyMines),
                                 cell.x + CELL_SIZE / 2 - 8, cell.y + CELL_SIZE / 2 - 12, 25, BLUE);
                    }
                } else if (g.board[r][c].flagged) {
                    DrawTriangle(
                        (Vector2){ cell.x + CELL_SIZE / 2 - 8, cell.y + CELL_SIZE / 2 + 8 },
                        (Vector2){ cell.x + CELL_SIZE / 2 - 8, cell.y + CELL_SIZE / 2 - 12 },
                        (Vector2){ cell.x + CELL_SIZE / 2 + 8, cell.y + CELL_SIZE / 2 - 2 },
                        RED
                    );
                }
            }
        }

        if (g.gameOver)
            DrawText("💥 GAME OVER!", 10, ROWS * CELL_SIZE + 10, 30, RED);
        else if (g.win)
            DrawText("🎉 YOU WIN!", 10, ROWS * CELL_SIZE + 10, 30, GREEN);
        else
            DrawText("Left-click: Reveal | Right-click: Flag", 10, ROWS * CELL_SIZE + 15, 20, RAYWHITE);

        EndDrawing();
    }

    Game_Cleanup(&g);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
