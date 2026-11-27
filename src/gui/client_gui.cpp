#include "raylib.h"
#include "gui/game_app.hpp"

int main(int argc, char** argv)
{
    std::string ip = (argc >= 2) ? argv[1] : "127.0.0.1";
    int port = (argc >= 3) ? std::stoi(argv[2]) : 8080;
    std::string username = (argc >= 4) ? argv[3] : "Antoine";

    GameApp app;
    if (!app.Init(ip, port, username)) {
        return 1;
    }

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