# Minesweeper — C++ Refactoring Documentation
**Language:** C++17  
**Library:** Raylib  
**Project Folder:** `minesweeper-solid`  
**Date:** 2025

---

## 1. Original Task

Refactor a working C Minesweeper project into **industrial-level C++** using **SOLID object-oriented design**, while keeping exactly the same logic, graphics, UI, gameplay, and sound.

**Key constraints given:**
- Do NOT change gameplay logic, UI rendering, Raylib graphics calls, board size, mine count, or sounds
- Use C++ classes, structs, and OOD principles
- Apply all 5 SOLID principles
- Split into: `Board.h/Board.cpp` → `Game.h/Game.cpp` → `main.cpp`
- UI rendering stays in `main.cpp` using Raylib functions exactly as in the original
- Target C++17 or later
- Keep recursion and all logic exactly as in the original C project

---

## 2. Folder Structure

```
minesweeper-solid/
│
├── Board.h          ← Cell struct + Board class declaration
├── Board.cpp        ← Board class implementation
│
├── Game.h           ← Game class declaration
├── Game.cpp         ← Game class implementation
│
├── main.cpp         ← Entry point + all Raylib drawing code
│
├── number.mp3       ← Sound: safe cell clicked
├── boom.mp3         ← Sound: mine explodes
├── flag.mp3         ← Sound: flag placed or removed
├── over.mp3         ← Sound: game over screen
├── win.mp3          ← Sound: player wins
└── boomm.png        ← Image: explosion on mine cells
```

---

## 3. Header Files (.h)

### Board.h

Declares the `Cell` struct and the `Board` class.

```
constexpr constants:
  TOTAL_ROWS        = 9     (rows on the grid)
  TOTAL_COLS        = 9     (columns on the grid)
  TOTAL_MINES       = 10    (mines hidden on the board)
  CELL_PIXEL_SIZE   = 60    (pixel size of each cell)

struct Cell:
  bool isRevealed       = false   → has the player opened this cell?
  bool hasMine          = false   → is a mine hiding inside?
  bool isFlagged        = false   → did the player put a flag here?
  int  nearbyMineCount  = 0       → mines in the 8 surrounding cells

class Board — public methods:
  Board()                           → constructor, calls reset()
  reset()                           → clears all cells to default
  placeMines()                      → randomly places 10 mines
  calculateNearbyMineCounts()       → fills nearbyMineCount for every safe cell
  revealCell(row, col)              → opens a cell, returns true if mine
  revealConnectedEmptyCells(...)    → recursive flood fill
  revealAllMines()                  → exposes all mines on game over
  countRevealedSafeCells()          → counts opened safe cells (win check)
  getCell(row, col)                 → const read-only access to a cell
  toggleFlag(row, col)              → adds or removes a flag
  isInsideBoard(row, col)           → static boundary check
```

---

### Game.h

Declares the `Game` class which owns the Board, audio resources, and state.

```
constexpr constant:
  STATUS_BAR_HEIGHT = 50    (height of bottom text bar in pixels)

class Game — public methods:
  Game()                   → constructor: loads sounds, sets up board
  ~Game()                  → destructor: unloads all sounds and textures
  update()                 → processes one frame of input and state logic
  getBoard()               → const reference to Board (for drawing)
  getMineExplosionImage()  → const reference to explosion texture
  isGameOver()             → returns true if player hit a mine
  isPlayerWin()            → returns true if all safe cells revealed

class Game — private members:
  Board board                      → the 9x9 grid (composition)
  bool gameOver, playerWin         → high-level state flags
  Sound safeCellClickSound         → played when safe cell opened
  Sound mineExplosionSound         → played when mine is hit
  Sound flagToggleSound            → played when flag placed/removed
  Sound gameOverJingleSound        → played on game over screen
  Sound winJingleSound             → played on win screen
  Texture2D mineExplosionImage     → explosion image for mine cells
  bool alreadyPlayed...            → play-once guards for each sound
  static Game* activeInstance      → lets static callback reach sounds
```

---

## 4. Source Files (.cpp)

### Board.cpp

Refactoring notes for this file:

- All logic is identical to the original C functions `InitializeBoard`, `PlaceMines`, `CountNearbyMines`, `RevealEmpty`, `RevealAllMines`
- Converted from free functions to class methods — same internal logic, better encapsulation
- `revealConnectedEmptyCells` accepts a `void(*)()` function pointer so it can play sounds without depending on Raylib directly (DIP)
- `isInsideBoard` made `static` — it does not need any instance data
- `getCell` returns `const Cell&` — callers that only need to read cannot accidentally modify state (ISP)

| Method | What it does |
|--------|-------------|
| `Board()` | Calls reset() to initialise all cells |
| `reset()` | Sets isRevealed, hasMine, isFlagged to false; nearbyMineCount to 0 |
| `placeMines()` | Picks random cells until 10 mines placed, skips duplicates |
| `calculateNearbyMineCounts()` | For each non-mine cell, counts mines in 8 directions |
| `revealCell()` | Opens a cell, returns true if it was a mine |
| `revealConnectedEmptyCells()` | Recursive flood fill, calls soundCallback per revealed cell |
| `revealAllMines()` | Sets isRevealed = true on every mine cell |
| `countRevealedSafeCells()` | Returns count of revealed non-mine cells |
| `getCell()` | Returns const reference for read-only access |
| `toggleFlag()` | Flips isFlagged on an unrevealed cell |
| `isInsideBoard()` | Static: row 0–8 and col 0–8 check |

---

### Game.cpp

Refactoring notes for this file:

- All logic is identical to the original C `main()` game loop — extracted into named private methods
- Constructor replaces the original setup code at the top of `main()`
- Destructor automatically unloads all sounds — no manual cleanup needed in `main.cpp`
- `playSafeCellSoundCallback` is a static method used as a function pointer — lets Board play sounds without Game depending on Board knowing about Raylib (DIP)
- `activeInstance` static pointer is the standard C++ pattern for bridging static callbacks to instance data

| Method | Visible | What it does |
|--------|---------|-------------|
| `Game()` | Public | Loads sounds, sets up board, resets all flags |
| `~Game()` | Public | Unloads all sounds and textures |
| `update()` | Public | Reads mouse input, calls click handlers, checks win |
| `getBoard()` | Public | Returns const Board reference for main.cpp drawing |
| `getMineExplosionImage()` | Public | Returns const Texture2D for main.cpp drawing |
| `isGameOver()` | Public | Returns gameOver flag |
| `isPlayerWin()` | Public | Returns playerWin flag |
| `getMouseCell()` | Private | Converts mouse pixels to board row/col |
| `handleLeftClick()` | Private | Reveals cell or triggers game over + flood fill |
| `handleRightClick()` | Private | Toggles flag, plays flag sound |
| `checkWinCondition()` | Private | Checks if all safe cells are revealed |
| `playPendingSounds()` | Private | Safety net for end-game sounds |
| `playSafeCellSoundCallback()` | Static | Callback for flood-fill sound playback |

---

### main.cpp

Refactoring notes for this file:

- All Raylib draw calls kept exactly as in the original C code — same colours, same positions, same font sizes, same triangle vertices
- Drawing split into 3 static helper functions: `drawCell()`, `drawBoard()`, `drawStatusBar()` — keeps `main()` short and readable
- All magic colour literals replaced with named `constexpr Color` constants at the top
- `main()` only does 3 things: initialise Raylib, run the loop, close Raylib — no game logic, no raw board data access
- Game destructor handles `UnloadSound` / `UnloadTexture` automatically

| Function | What it does |
|----------|-------------|
| `drawCell()` | Draws background, border, and content for one cell |
| `drawBoard()` | Calls drawCell for all 81 cells |
| `drawStatusBar()` | Draws GAME OVER / YOU WIN / instructions text |
| `main()` | Init → game loop (update + draw) → cleanup |

**Game loop structure:**
```cpp
while (!WindowShouldClose()) {
    game.update();          // logic only — no drawing

    BeginDrawing();
        ClearBackground(COLOR_BACKGROUND);
        drawBoard(game);
        drawStatusBar(game);
    EndDrawing();
}
```

---

## 5. SOLID Principles Applied

### SRP — Single Responsibility Principle
| Class / File | Single Responsibility |
|---|---|
| `Board` | Manages the grid: mines, counts, reveal, flood-fill |
| `Game` | Manages game session: input, state, audio |
| `main.cpp` | Opens window, runs loop, draws everything |
| `drawCell()` | Draws exactly one cell |
| `drawBoard()` | Iterates and draws all cells |
| `drawStatusBar()` | Draws the bottom status message |

### OCP — Open/Closed Principle
- `Board` can be extended (e.g. larger grid, custom mine patterns) without modifying existing methods
- `Game` can be extended (e.g. timer, high scores) without changing Board or main.cpp

### LSP — Liskov Substitution Principle
- `const Board&` and `const Game&` passed by reference — any subclass could be substituted without breaking callers
- `getCell()` returns `const Cell&` — read-only contract preserved for any Board subclass

### ISP — Interface Segregation Principle
- `Game` public interface is minimal: only 5 methods that main.cpp actually needs
- `Board::getCell()` is `const` — callers that only read cannot accidentally write
- Drawing code reads game state only through public const accessors, never through internal data

### DIP — Dependency Inversion Principle
- `main.cpp` depends on `Game` and `Board` abstractions, not on raw internal data
- `Board::revealConnectedEmptyCells` receives a `void(*)()` callback instead of depending on Raylib Sound directly
- `Game` reads cell states through `board.getCell()` rather than accessing the grid array directly

---

## 6. Before vs After Summary

| Item | Before — Original C | After — Refactored C++ |
|------|---------------------|------------------------|
| Files | 1 `.c` file (~230 lines) | 3 `.cpp` + 2 `.h` files |
| `main()` size | ~150 lines | ~25 lines |
| Global variables | 10+ scattered globals | 0 globals — Game owns everything |
| Magic numbers | `60`, `9`, `10`, `50` inline | `constexpr` named constants |
| Magic colours | `(Color){48,99,47,255}` inline | Named `constexpr Color` constants |
| Variable names | `r`, `c`, `sBoom`, `sWin` | `row`, `col`, `mineExplosionSound`, `winJingleSound` |
| Resource cleanup | Manual `UnloadSound()` in main | `~Game()` destructor handles it |
| Board access | Direct array `board[r][c]` | `board.getCell(row, col)` |
| Sound in flood fill | `void*` pointer cast | Clean static callback method |
| Organisation | Everything in main() | Classes with clear responsibilities |

### What was NOT changed
- Board size: **9 × 9**
- Mine count: **10**
- Flood fill recursion (Option B — sound plays for every auto-revealed cell)
- All 5 sounds and when they play
- Win condition: all safe cells revealed
- Game over: all mines revealed at once
- All colours, font sizes, text positions
- Checkerboard pattern
- Flag triangle shape and vertices
- Explosion image scaling and rendering

---

## 7. Compilation Instructions

### Windows — VS Code Terminal (from inside `minesweeper-solid/`)

```bash
g++ -std=c++17 main.cpp Game.cpp Board.cpp -IC:\raylib\include -LC:\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm -o minesweeper.exe
```

Run:
```bash
.\minesweeper.exe
```

### Linux / Cloud

```bash
g++ -std=c++17 main.cpp Game.cpp Board.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o minesweeper
```

Run:
```bash
./minesweeper
```

### Compilation Command Breakdown

| Part | Meaning |
|------|---------|
| `g++` | C++ compiler |
| `-std=c++17` | use C++17 standard |
| `main.cpp Game.cpp Board.cpp` | all 3 source files compiled together |
| `-IC:\raylib\include` | folder where `raylib.h` is located |
| `-LC:\raylib\lib` | folder where the Raylib library file is located |
| `-lraylib` | link the Raylib library |
| `-lopengl32 -lgdi32 -lwinmm` | Windows system libraries that Raylib needs |
| `-o minesweeper.exe` | name of the output executable file |
