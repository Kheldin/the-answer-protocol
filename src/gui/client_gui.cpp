#include "raylib.h"
#include "gui/asset_manager.hpp"
#include "gui/room.hpp"
#include <unordered_map>
#include <string>
#include <vector>

int main(void)
{
    int screenWidth {800};
    int screenHeight {450};

    SetConfigFlags(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED);
    InitWindow(screenWidth, screenHeight, "Borderless fullscreen");
    int monitor = GetCurrentMonitor();
    screenWidth = GetMonitorWidth(monitor);
    screenHeight = GetMonitorHeight(monitor);

    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();

    AssetManager manager {};
    manager.LoadAll();

    std::unordered_map<std::string, Texture2D> player_textures = manager.getPlayerTextures();
    Texture2D current_guy = player_textures.at("guy_front");

    // Configuration du bouton interactif
    Texture2D buttonsTexture = manager.getButtonsTextures();
    float bframeWidth = (float)buttonsTexture.width / 1.9f;
    float bframeHeight = (float)buttonsTexture.height / 3.0f;

    Rectangle buttonPos = { 1650.0f, 570.0f, bframeWidth * 0.2f, bframeHeight * 0.2f };
    Rectangle interactionZone = { buttonPos.x - 250, buttonPos.y + 100, buttonPos.width + 310, buttonPos.height + 60 };
    bool isTalking = false;

    Room Reception(manager.getReceptionTextures(), manager.getReceptionTexturesPositions(), manager.getReceptionHitboxes());
    Room LockerRoom(manager.getLockerRoomTextures(), manager.getLockerRoomTexturesPositions(), manager.getLockerRoomHitboxes());
    Room Muscu(manager.getMuscuTextures(), manager.getMuscuTexturesPositions(), manager.getMuscuHitboxes());
    Room Cardio(manager.getCardioTextures(), manager.getCardioTexturesPositions(), manager.getCardioHitboxes());

    enum class Rooms {
        Reception,
        LockerRoom,
        Muscu,
        Cardio
    };
    
    const float HITBOX_OFFSET_X = 5.0f;
    const float HITBOX_OFFSET_Y = 172.0f;
    const float HITBOX_WIDTH = 70.0f;  
    const float HITBOX_HEIGHT = 20.0f;

    auto get_feet_hitbox = [=](float x, float y) -> Rectangle {
        return {
            x + HITBOX_OFFSET_X,
            y + HITBOX_OFFSET_Y,
            HITBOX_WIDTH,
            HITBOX_HEIGHT
        };
    };
    Rooms currentRoom {Rooms::Reception};

    Rectangle frameRec_faces = { 0.0f, 0.0f, (float)current_guy.width/2, (float)current_guy.height };
    bool frame_faces = true;
    int currentFrame = 0;
    Rectangle frameRec = { 0.0f, 0.0f, 0.0f, 0.0f };

    const float speed = 5.0f;
    const float scale = 0.6f;

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
        framesCounter++;

        Rectangle playerHitbox = get_feet_hitbox(destRec.x, destRec.y);

        // Détection de proximité avec le bouton
        bool nearButton = (currentRoom == Rooms::Reception) && CheckCollisionRecs(playerHitbox, interactionZone);

        // Interaction
        if (nearButton) {
            if (IsKeyPressed(KEY_E)) {
                isTalking = !isTalking;
            }
        } else {
            isTalking = false; // Ferme la boîte de dialogue si on s'éloigne
        }

        // Logique de l'image du bouton
        // Si on est proche : position 4 (index 3). Sinon : position 6 (index 5)
        int buttonFrameIndex = nearButton ? 3 : 5;
        int buttonCols = 2;
        int col = buttonFrameIndex % buttonCols;
        int row = buttonFrameIndex / buttonCols;

        // Décalage pour couper le bord gauche qui bave
        float cropOffset = 3.0f; // Augmente ce chiffre si le trait apparaît toujours

        Rectangle buttonSourceRec = {
            (col * bframeWidth) + cropOffset,
            row * bframeHeight,
            bframeWidth - cropOffset,
            bframeHeight
        };

        if (!(framesCounter % 5) && (IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_RIGHT)
            or IsKeyDown(KEY_DOWN) or IsKeyDown(KEY_UP)))
            currentFrame++;

        std::vector<Rectangle> current_hitboxes;
        switch (currentRoom) {
            case Rooms::Reception:  current_hitboxes = Reception.getHitboxes(); break;
            case Rooms::LockerRoom: current_hitboxes = LockerRoom.getHitboxes(); break;
            case Rooms::Muscu:      current_hitboxes = Muscu.getHitboxes(); break;
            case Rooms::Cardio:     current_hitboxes = Cardio.getHitboxes(); break;
        }

        if (!isTalking)
        {
            if (IsKeyDown(KEY_RIGHT))
            {
                Rectangle future_feet = get_feet_hitbox(destRec.x + speed, destRec.y);
                bool collision = false;
                for (Rectangle wall : current_hitboxes) {
                    if (CheckCollisionRecs(future_feet, wall)) collision = true;
                }
                
                if (!collision && destRec.x < screenWidth - 50)
                    destRec.x += speed;

                if (currentFrame % 3 == 1) current_guy = player_textures.at("guy_right1");
                else if (currentFrame % 3 == 2) current_guy = player_textures.at("guy_right2");
                else current_guy = player_textures.at("guy_right3");
                frame_faces = false;
            }
            
            if (IsKeyDown(KEY_LEFT))
            {
                Rectangle future_feet = get_feet_hitbox(destRec.x - speed, destRec.y);
                bool collision = false;
                for (Rectangle wall : current_hitboxes) {
                    if (CheckCollisionRecs(future_feet, wall)) collision = true;
                }
                
                if (!collision && destRec.x > 3) 
                    destRec.x -= speed;

                if (currentFrame % 3 == 1) current_guy = player_textures.at("guy_left1");
                else if (currentFrame % 3 == 2) current_guy = player_textures.at("guy_left2");
                else current_guy = player_textures.at("guy_left3");
                frame_faces = false;
            }
            
            if (IsKeyDown(KEY_DOWN))
            {
                Rectangle future_feet = get_feet_hitbox(destRec.x, destRec.y + speed);
                bool collision = false;
                for (Rectangle wall : current_hitboxes) {
                    if (CheckCollisionRecs(future_feet, wall)) collision = true;
                }
                
                if (!collision && destRec.y < screenHeight - 170) 
                    destRec.y += speed;

                current_guy = player_textures.at("guy_front");
                frame_faces = true;
            }
            
            if (IsKeyDown(KEY_UP))
            {
                Rectangle future_feet = get_feet_hitbox(destRec.x, destRec.y - speed);
                bool collision = false;
                for (Rectangle wall : current_hitboxes) {
                    if (CheckCollisionRecs(future_feet, wall)) collision = true;
                }
                
                if (!collision && destRec.y > 10) 
                    destRec.y -= speed;

                current_guy = player_textures.at("guy_back");
                frame_faces = true;
            }
        }

        switch (currentRoom)
        {
            case Rooms::Reception:
                if (destRec.y < 160 && destRec.x > 160 && destRec.x < 270)
                {
                    currentRoom = Rooms::LockerRoom;
                    destRec.x = 1650;
                    destRec.y = 150;
                }
                else if (destRec.y < 160 && destRec.x > 900 && destRec.x < 1070)
                {
                    currentRoom = Rooms::Muscu;
                    destRec.y = screenHeight - 180;
                }
                break;
            case Rooms::LockerRoom:
                if (destRec.x > screenWidth - 51)
                {
                    currentRoom = Rooms::Reception;
                    destRec.x = 5;
                }
                break;
            case Rooms::Muscu:
                if (destRec.y > screenHeight - 172)
                {
                    currentRoom = Rooms::Reception;
                    destRec.y = 180;
                }
                else if (destRec.x < 10)
                {
                    currentRoom = Rooms::Cardio;
                    destRec.x = screenWidth - 52;
                }
                break;
            case Rooms::Cardio:
                if (destRec.x > screenWidth - 51)
                {
                    currentRoom = Rooms::Muscu;
                    destRec.x = 5;
                }
                break;
        }

        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;
            int max_frames = frame_faces ? 2 : 3;
            if (currentFrame >= max_frames) currentFrame = 0;
        }

        int max_frames = frame_faces ? 2 : 1;
        frameRec.width = (float)current_guy.width / max_frames;
        frameRec.height = (float)current_guy.height;
        frameRec.x = (float)currentFrame * frameRec.width;
        
        destRec.width = frameRec.width * scale;
        destRec.height = frameRec.height * scale;

        BeginDrawing();
        ClearBackground(WHITE);

        switch (currentRoom)
        {
            case Rooms::Reception:
                DrawText("RECEPTION", screenWidth/2, screenHeight/2, 50, BLACK);
                Reception.draw();
                DrawTexturePro(buttonsTexture, buttonSourceRec, buttonPos, {0.0f, 0.0f}, 0.0f, WHITE);
                break;
                
            case Rooms::LockerRoom:
                DrawText("LockerRoom", screenWidth/2, screenHeight/2, 50, BLACK);
                LockerRoom.draw();
                break;
                
            case Rooms::Muscu:
                DrawText("Muscu", screenWidth/2, screenHeight/2, 50, BLACK);
                Muscu.draw();
                break;
                
            case Rooms::Cardio:
                DrawText("Cardio", screenWidth/2, screenHeight/2, 50, BLACK);
                Cardio.draw();
                break;
        }
        
        for (Rectangle wall : current_hitboxes) {
            DrawRectangleLinesEx(wall, 2.0f, GREEN);
        }
        
        DrawText(TextFormat("X: %d Y: %d", GetMouseX(), GetMouseY()), 10, 10, 20, WHITE);
        DrawTexturePro(current_guy, frameRec, destRec, origin, 0.0f, WHITE);
        DrawRectangleLinesEx(playerHitbox, 2.0f, RED);

        if (isTalking) {
            DrawRectangle(50, screenHeight - 120, screenWidth - 100, 100, LIGHTGRAY);
            DrawRectangleLines(50, screenHeight - 120, screenWidth - 100, 100, BLACK);
            DrawText("Hi, welcome to the gym !\nWhat can I do for you today.", 70, screenHeight - 100, 20, BLACK);
        }

        EndDrawing();
    }
    
    manager.UnloadAll();
    CloseWindow();

    return 0;
}