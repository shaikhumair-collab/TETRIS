#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GRID_HEIGHT 20
#define GRID_WIDTH 10
#define CELL_SIZE 30
#define BOARD_X 70
#define BOARD_Y 45
#define WINDOW_WIDTH 650
#define WINDOW_HEIGHT 700
#define MAX_SCORES 100

typedef enum {
    SCREEN_MENU,
    SCREEN_PLAYING,
    SCREEN_SCORES,
    SCREEN_GAME_OVER
} GameScreen;

typedef struct {
    int shape[4][4];
    int x;
    int y;
    int type;
} Piece;

static int grid[GRID_HEIGHT][GRID_WIDTH];
static int points = 0;
static Piece present;
static GameScreen screen = SCREEN_MENU;
static bool paused = false;
static bool exitRequested = false;
static float fallTimer = 0.0f;
static const float fallInterval = 0.70f;

static const int blocks[7][4][4] = {
    {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
    {{1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,0,1,1}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
    {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}
};

static const Color pieceColors[8] = {
    BLANK,
    {  31, 244, 255, 255 },  /* neon cyan */
    { 255, 238,  46, 255 },  /* electric yellow */
    { 210,  55, 255, 255 },  /* neon purple */
    {  57, 255, 115, 255 },  /* laser green */
    { 255,  45, 100, 255 },  /* hot pink */
    {  64, 105, 255, 255 },  /* electric blue */
    { 255, 132,  34, 255 }   /* neon orange */
};

static const Color neonCyan = {31, 244, 255, 255};
static const Color neonPurple = {210, 55, 255, 255};
static const Color neonPink = {255, 45, 100, 255};
static const Color darkSpace = {7, 5, 20, 255};
static const Color darkPanel = {15, 10, 38, 255};

static void ResetGrid(void)
{
    memset(grid, 0, sizeof(grid));
}

static void NewPiece(void)
{
    present.type = rand() % 7;
    memcpy(present.shape, blocks[present.type], sizeof(present.shape));
    present.x = GRID_WIDTH / 2 - 2;
    present.y = 0;
}

static bool HasCollision(int offsetX, int offsetY)
{
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (!present.shape[row][col]) continue;

            int newX = present.x + col + offsetX;
            int newY = present.y + row + offsetY;

            if (newX < 0 || newX >= GRID_WIDTH || newY >= GRID_HEIGHT) return true;
            if (newY >= 0 && grid[newY][newX] != 0) return true;
        }
    }
    return false;
}

static void FixPiece(void)
{
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (present.shape[row][col]) {
                int x = present.x + col;
                int y = present.y + row;
                if (y >= 0 && y < GRID_HEIGHT && x >= 0 && x < GRID_WIDTH) {
                    grid[y][x] = present.type + 1;
                }
            }
        }
    }
    points++;
}

static void ClearLines(void)
{
    int cleared = 0;

    for (int row = GRID_HEIGHT - 1; row >= 0; row--) {
        bool full = true;
        for (int col = 0; col < GRID_WIDTH; col++) {
            if (grid[row][col] == 0) {
                full = false;
                break;
            }
        }

        if (full) {
            for (int moveRow = row; moveRow > 0; moveRow--) {
                for (int col = 0; col < GRID_WIDTH; col++) {
                    grid[moveRow][col] = grid[moveRow - 1][col];
                }
            }
            for (int col = 0; col < GRID_WIDTH; col++) grid[0][col] = 0;
            cleared++;
            row++;
        }
    }

    points += cleared * 10;
}

static void RotatePiece(void)
{
    int oldShape[4][4];
    int rotated[4][4] = {0};
    memcpy(oldShape, present.shape, sizeof(oldShape));

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            rotated[col][3 - row] = present.shape[row][col];
        }
    }

    memcpy(present.shape, rotated, sizeof(rotated));

    const int kicks[] = {0, -1, 1, -2, 2};
    for (int i = 0; i < 5; i++) {
        if (!HasCollision(kicks[i], 0)) {
            present.x += kicks[i];
            return;
        }
    }

    memcpy(present.shape, oldShape, sizeof(oldShape));
}

static void SaveScore(int score)
{
    FILE *file = fopen("highscore.txt", "a");
    if (file == NULL) return;
    fprintf(file, "%d\n", score);
    fclose(file);
}

static int ReadScores(int scores[MAX_SCORES])
{
    FILE *file = fopen("highscore.txt", "r");
    if (file == NULL) return 0;

    int count = 0;
    while (count < MAX_SCORES && fscanf(file, "%d", &scores[count]) == 1) count++;
    fclose(file);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                int temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
    return count;
}

static void StartGame(void)
{
    points = 0;
    paused = false;
    fallTimer = 0.0f;
    ResetGrid();
    NewPiece();
    screen = SCREEN_PLAYING;
}

static void LockAndContinue(void)
{
    FixPiece();
    ClearLines();
    NewPiece();

    if (HasCollision(0, 0)) {
        SaveScore(points);
        screen = SCREEN_GAME_OVER;
    }
}

static bool GuiButton(Rectangle bounds, const char *text)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    Color fill = hovered ? (Color){82, 24, 128, 255} : (Color){28, 14, 67, 255};

    DrawRectangleRounded(bounds, 0.20f, 8, fill);
    DrawRectangleRoundedLinesEx(bounds, 0.20f, 8, hovered ? 3.0f : 2.0f,
                                hovered ? neonCyan : neonPurple);

    int fontSize = 24;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (int)(bounds.x + (bounds.width - textWidth) / 2),
             (int)(bounds.y + (bounds.height - fontSize) / 2), fontSize, RAYWHITE);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void DrawCell(int gridX, int gridY, Color color)
{
    int x = BOARD_X + gridX * CELL_SIZE;
    int y = BOARD_Y + gridY * CELL_SIZE;

    DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, Fade(color, 0.18f));
    DrawRectangleRounded((Rectangle){x + 3, y + 3, CELL_SIZE - 6, CELL_SIZE - 6},
                         0.22f, 6, color);
    DrawRectangleRoundedLinesEx((Rectangle){x + 3, y + 3, CELL_SIZE - 6, CELL_SIZE - 6},
                                0.22f, 6, 2.0f, ColorBrightness(color, 0.35f));
}

static void DrawGame(void)
{
    DrawRectangle(BOARD_X - 5, BOARD_Y - 5,
                  GRID_WIDTH * CELL_SIZE + 10, GRID_HEIGHT * CELL_SIZE + 10,
                  neonCyan);
    DrawRectangle(BOARD_X, BOARD_Y, GRID_WIDTH * CELL_SIZE,
                  GRID_HEIGHT * CELL_SIZE, darkPanel);

    for (int row = 0; row < GRID_HEIGHT; row++) {
        for (int col = 0; col < GRID_WIDTH; col++) {
            int x = BOARD_X + col * CELL_SIZE;
            int y = BOARD_Y + row * CELL_SIZE;
            DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, (Color){45, 25, 82, 255});
            if (grid[row][col] != 0) DrawCell(col, row, pieceColors[grid[row][col]]);
        }
    }

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (present.shape[row][col]) {
                DrawCell(present.x + col, present.y + row, pieceColors[present.type + 1]);
            }
        }
    }

    DrawText("NEON", 425, 55, 28, neonPink);
    DrawText("TETRIS", 425, 88, 42, neonCyan);
    DrawText(TextFormat("SCORE  %d", points), 425, 135, 26, RAYWHITE);
    DrawText("CONTROLS", 425, 225, 22, neonPurple);
    DrawText("A / Left: move", 425, 265, 18, LIGHTGRAY);
    DrawText("D / Right: move", 425, 295, 18, LIGHTGRAY);
    DrawText("S / Down: drop", 425, 325, 18, LIGHTGRAY);
    DrawText("W / Up: rotate", 425, 355, 18, LIGHTGRAY);
    DrawText("Space: hard drop", 425, 385, 18, LIGHTGRAY);
    DrawText("P: pause", 425, 415, 18, LIGHTGRAY);
    DrawText("Esc: menu", 425, 445, 18, LIGHTGRAY);

    if (paused) {
        DrawRectangle(BOARD_X, BOARD_Y, GRID_WIDTH * CELL_SIZE,
                      GRID_HEIGHT * CELL_SIZE, Fade(BLACK, 0.72f));
        int labelWidth = MeasureText("PAUSED", 42);
        DrawText("PAUSED", BOARD_X + (GRID_WIDTH * CELL_SIZE - labelWidth) / 2,
                 WINDOW_HEIGHT / 2 - 30, 42, RAYWHITE);
    }
}

static void UpdatePlaying(void)
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen = SCREEN_MENU;
        return;
    }
    if (IsKeyPressed(KEY_P)) paused = !paused;
    if (paused) return;

    if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && !HasCollision(-1, 0)) present.x--;
    if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && !HasCollision(1, 0)) present.x++;
    if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))) RotatePiece();

    if (IsKeyPressed(KEY_SPACE)) {
        while (!HasCollision(0, 1)) present.y++;
        LockAndContinue();
        fallTimer = 0.0f;
        return;
    }

    bool softDrop = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    fallTimer += GetFrameTime() * (softDrop ? 6.0f : 1.0f);

    if (fallTimer >= fallInterval) {
        fallTimer = 0.0f;
        if (!HasCollision(0, 1)) present.y++;
        else LockAndContinue();
    }
}

static void DrawMenu(void)
{
    DrawText("NEON TETRIS", WINDOW_WIDTH / 2 - MeasureText("NEON TETRIS", 62) / 2,
             105, 62, neonCyan);
    DrawText("Arcade Edition", WINDOW_WIDTH / 2 - MeasureText("Arcade Edition", 22) / 2,
             185, 22, LIGHTGRAY);

    Rectangle start = {185, 275, 280, 62};
    Rectangle scores = {185, 360, 280, 62};
    Rectangle exit = {185, 445, 280, 62};

    if (GuiButton(start, "START GAME") || IsKeyPressed(KEY_ENTER)) StartGame();
    if (GuiButton(scores, "HIGH SCORES")) screen = SCREEN_SCORES;
    if (GuiButton(exit, "EXIT")) exitRequested = true;
}

static void DrawScores(void)
{
    int scores[MAX_SCORES];
    int count = ReadScores(scores);

    DrawText("HIGH SCORES", WINDOW_WIDTH / 2 - MeasureText("HIGH SCORES", 44) / 2,
             65, 44, neonPurple);

    if (count == 0) {
        DrawText("No scores saved yet", WINDOW_WIDTH / 2 - MeasureText("No scores saved yet", 24) / 2,
                 190, 24, LIGHTGRAY);
    } else {
        int shown = count < 10 ? count : 10;
        for (int i = 0; i < shown; i++) {
            const char *line = TextFormat("%2d.     %d", i + 1, scores[i]);
            DrawText(line, WINDOW_WIDTH / 2 - MeasureText(line, 26) / 2,
                     145 + i * 39, 26, RAYWHITE);
        }
    }

    if (GuiButton((Rectangle){205, 580, 240, 58}, "BACK") || IsKeyPressed(KEY_ESCAPE)) {
        screen = SCREEN_MENU;
    }
}

static void DrawGameOver(void)
{
    DrawText("GAME OVER", WINDOW_WIDTH / 2 - MeasureText("GAME OVER", 58) / 2,
             125, 58, neonPink);
    const char *scoreText = TextFormat("Final score: %d", points);
    DrawText(scoreText, WINDOW_WIDTH / 2 - MeasureText(scoreText, 30) / 2,
             225, 30, RAYWHITE);

    if (GuiButton((Rectangle){185, 330, 280, 62}, "PLAY AGAIN") || IsKeyPressed(KEY_ENTER)) {
        StartGame();
    }
    if (GuiButton((Rectangle){185, 415, 280, 62}, "MAIN MENU") || IsKeyPressed(KEY_ESCAPE)) {
        screen = SCREEN_MENU;
    }
}

int main(void)
{
    srand((unsigned int)time(NULL));
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris - Graphical Edition");
    SetTargetFPS(60);

    while (!WindowShouldClose() && !exitRequested) {
        if (screen == SCREEN_PLAYING) UpdatePlaying();

        BeginDrawing();
        ClearBackground(darkSpace);

        switch (screen) {
            case SCREEN_MENU: DrawMenu(); break;
            case SCREEN_PLAYING: DrawGame(); break;
            case SCREEN_SCORES: DrawScores(); break;
            case SCREEN_GAME_OVER: DrawGameOver(); break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
