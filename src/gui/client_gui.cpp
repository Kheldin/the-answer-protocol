#include "raylib.h"

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED 1

int main(void)
{
    const int screenWidth =800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED);
    InitWindow(screenWidth, screenHeight, "Borderless fullscreen");
    int monitor = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    ToggleFullscreen();

    Texture2D guy = LoadTexture("src/assets/normal_guy.png");

    Vector2 position = { 15.0f, 40.0f };
    Rectangle frameRec = { 0.0f, 0.0f, (float)guy.width/9, (float)guy.height/2 };
    int currentFrame {0};
    
    int framesCounter {0};
    int framesSpeed {8};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // UPDATE
        framesCounter++;

        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;
            currentFrame++;

            if (currentFrame > 8) currentFrame = 0;

            frameRec.x = (float)currentFrame * (float)guy.width/9;
        }

        // DRAW
        BeginDrawing();

            ClearBackground(WHITE);
            // DrawTexture(guy, 15, 40, WHITE);

            DrawTextureRec(guy, frameRec, position, WHITE);

        EndDrawing();
    }
    UnloadTexture(guy);
    CloseWindow();

    return 0;
}