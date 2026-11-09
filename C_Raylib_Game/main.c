#include "raylib.h"

int main(void) {
    InitWindow(400, 400, "Green Box");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(150, 150, 100, 100, GREEN);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}