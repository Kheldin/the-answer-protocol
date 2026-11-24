#include "raylib.h"
#include "gui/game_app.hpp"

int main(void)
{
    GameApp app;
    app.Init();

    while (!WindowShouldClose())
    {
        app.Update(GetFrameTime());

        BeginDrawing();
        ClearBackground(WHITE);
        app.Draw();
        EndDrawing();
    }

    app.Shutdown();
    return 0;
}