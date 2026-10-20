#include "raylib.h"

typedef enum Rooms { Reception = 0, CARDIO, Lockerroom } Rooms;

int main(void)
{
    int screenWidth {800};
    int screenHeight = {450};

    SetConfigFlags(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED);
    InitWindow(screenWidth, screenHeight, "Borderless fullscreen");
    int monitor = GetCurrentMonitor();
    screenWidth = GetMonitorWidth(monitor);
    screenHeight = GetMonitorHeight(monitor);

    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();

    Rooms currentRoom {Reception};

    Texture2D guy_left = LoadTexture("assets/normal_walk_left.png");
    Texture2D guy_right = LoadTexture("assets/normal_walk_right.png");
    Texture2D guy_front = LoadTexture("assets/normal_front.png");
    Texture2D guy_back = LoadTexture("assets/normal_back.png");
    Texture2D current_guy = guy_front;

    Rectangle frameRec_sides = { 0.0f, 0.0f, (float)current_guy.width/3, (float)current_guy.height };
    Rectangle frameRec_faces = { 0.0f, 0.0f, (float)current_guy.width/2, (float)current_guy.height };
    bool frame_faces = true;
    int currentFrame = 0;
    Rectangle frameRec = { 0.0f, 0.0f, 0.0f, 0.0f };

    const float speed = 10.0f;
    const float scale = 0.5f;

    Rectangle destRec = {
        screenWidth / 2.0f,
        screenHeight / 2.0f,
        frameRec_faces.width * scale,
        frameRec_faces.height * scale
    };
    Vector2 origin = { 0.0f, 0.0f };

    int framesCounter {0};
    int framesSpeed {8};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        // UPDATE
        framesCounter++;
        
        if (!(framesCounter % 5) && (IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_RIGHT)
            or IsKeyDown(KEY_DOWN) or IsKeyDown(KEY_UP)))
            currentFrame++;
        if (IsKeyDown(KEY_RIGHT))
        {
            if (destRec.x < screenWidth - 50)
                destRec.x += speed;
            current_guy = guy_right;
            frame_faces = false;
        }
        if (IsKeyDown(KEY_LEFT))
        {
            if (destRec.x > 3)
                destRec.x -= speed;
            current_guy = guy_left;
            frame_faces = false;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            if (destRec.y < screenHeight - 170)
                destRec.y += speed;
            current_guy = guy_front;
            frame_faces = true;
        }
        if (IsKeyDown(KEY_UP))
        {
            if (destRec.y > 10)
                destRec.y -= speed;
            current_guy = guy_back;
            frame_faces = true;
        }

        switch (currentRoom)
        {
            case Rooms::Reception:
                if (destRec.x < 10)
                {
                    currentRoom = Lockerroom;
                    destRec.x = screenWidth - 52;
                }
                break;
            case Rooms::Lockerroom:
                if (destRec.x > screenWidth - 51)
                {
                    currentRoom = Reception;
                    destRec.x = 5;
                }
                break;
            // case Rooms::CARDIO:

        }

        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;

            int max_frames = frame_faces ? 2 : 3;
            if (currentFrame >= max_frames) currentFrame = 0;
        }

        int max_frames = frame_faces ? 2 : 3;
        frameRec.width = (float)current_guy.width / max_frames;
        frameRec.height = (float)current_guy.height;
        frameRec.x = (float)currentFrame * frameRec.width;
        
        destRec.width = frameRec.width * scale;
        destRec.height = frameRec.height * scale;

        // DRAW
        BeginDrawing();
        ClearBackground(WHITE);

        switch (currentRoom)
        {
            case Rooms::Reception:
                DrawText("Reception", screenWidth/2, screenHeight/2, 50, BLACK);
                break;
                
            case Rooms::Lockerroom:
                DrawText("Lockerroom", screenWidth/2, screenHeight/2, 50, BLACK);
                break;
                
        }

        DrawTexturePro(current_guy, frameRec, destRec, origin, 0.0f, WHITE);

        EndDrawing();
    }
    UnloadTexture(current_guy);
    UnloadTexture(guy_front);
    UnloadTexture(guy_left);
    UnloadTexture(guy_right);
    CloseWindow();

    return 0;
}